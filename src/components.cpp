/*
 *  Copyright (C) 2017 the authors (see AUTHORS)
 *
 *  This file is part of Kitten Keeper.
 *
 *  Kitten Keeper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Kitten Keeper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kitten Keeper.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "main_state.h"
#include "level.h"

#include "components.h"


TriggerComponent::TriggerComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity)
    , prevInside(false)
    , inside(false)
{
}


const PropertyList& TriggerComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("on_enter", &TriggerComponent::onEnter);
		props.addProperty("on_exit",  &TriggerComponent::onExit);
		props.addProperty("on_use",   &TriggerComponent::onUse);
	}
	return props;
}


TriggerComponentManager::TriggerComponentManager()
    : DenseComponentManager<TriggerComponent>("trigger", 128)
{
}


//---------------------------------------------------------------------------//

// Kitten constants : fatigue/boredom/hunger/need-to-go per tick.
#define KIT_FPT 0.01f
#define KIT_BPT 0.05f
#define KIT_HPT 0.02f
#define KIT_NPT 0.02f

#define KIT_LOW 25.0f
#define KIT_BAD 75.0f
#define KIT_MAX 100.0f

#define KIT_WALK 0.02f

#define KIT_REST 0.05f
#define KIT_PLAY 0.5f
#define KIT_FEED 0.4f
#define KIT_PISS 0.6f

#define KIT_ANIM_LEN 0.4f

KittenComponent::KittenComponent(Manager* manager, _Entity* entity) :
	Component(manager, entity),
	sick(0),
	tired(2),
	bored(2),
	hungry(2),
	needy(2),
	s(SITTING),
	t(0),
    dst(0,0),
    anim(ANIM_IDLE),
    animTime(0)
{
}


const PropertyList& KittenComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("sick",   &KittenComponent::sick);
		props.addProperty("tired",  &KittenComponent::tired);
		props.addProperty("bored",  &KittenComponent::bored);
		props.addProperty("hungry", &KittenComponent::hungry);
		props.addProperty("needy",  &KittenComponent::needy);
		props.addProperty("status", &KittenComponent::s);
		props.addProperty("t",      &KittenComponent::t);
		props.addProperty("dst",    &KittenComponent::dst);
	}
	return props;
}


KittenComponentManager::KittenComponentManager(MainState* ms)
    : DenseComponentManager<KittenComponent>("kitten", 128),
    _ms(ms)
{
}


void KittenComponentManager::setBubble(EntityRef kitten, BubbleType bubbleType) {
	EntityRef bubble = kitten.firstChild();
	if(!bubble.isValid()) {
		dbgLogger.error("Kitten with no bubble ?");
		return;
	}

	SpriteComponent* sprite = _ms->_sprites.get(bubble);
	if(sprite) {
		bubble.setEnabled(true);
		if (bubbleType == BUBBLE_NONE)
			bubble.setEnabled(false);
		else
			sprite->setTileIndex(bubbleType);
	}
}


void KittenComponentManager::setAnim(KittenComponent& kitten, KittenAnim anim) {
	if(kitten.anim == anim)
		return;

	SpriteComponent* sprite = _ms->_sprites.get(kitten.entity());
	if(!sprite) {
		dbgLogger.error("Kitten without sprite ?");
		return;
	}

	kitten.anim = anim;
	kitten.animTime = 0;
	switch(anim) {
	case ANIM_IDLE:  sprite->setTileIndex(10); break;
	case ANIM_UP:   sprite->setTileIndex(6); break;
	case ANIM_RIGHT: sprite->setTileIndex(0); break;
	case ANIM_DOWN:  sprite->setTileIndex(4); break;
	case ANIM_LEFT:  sprite->setTileIndex(2); break;
	case ANIM_SLEEP: sprite->setTileIndex(8); break;
	case ANIM_PLAY:  sprite->setTileIndex(9); break;
	case ANIM_DEAD:  sprite->setTileIndex(11); break;
	}
}

void KittenComponentManager::updateAnim(KittenComponent& kitten) {
	SpriteComponent* sprite = _ms->_sprites.get(kitten.entity());
	if(!sprite) {
		dbgLogger.error("Kitten without sprite ?");
		return;
	}

	if(sprite->tileIndex() < 8 && kitten.animTime >= KIT_ANIM_LEN) {
		kitten.animTime -= KIT_ANIM_LEN;
		sprite->setTileIndex(sprite->tileIndex() ^ 1);
	}

	kitten.animTime += TICK_LENGTH_IN_SEC;
}

void KittenComponentManager::seek(KittenComponent& k, ToyType tt, bool now)
{
	if (tt != TOY_HEAL && !now && k.s != SITTING)
		return;

	float range = now ? 800 : 200 ;
	for (ToyComponent& t: _ms->_toys)
	{
		float dist = (k.entity().position2() - t.entity().position2()).norm();
		if (t.type == tt && dist < range && t.state == t.PLACED) {
			k.s = WALKING;
			k.dst = t.entity().position2();
			return;
		}
	}
}

Vector2 KittenComponentManager::findRandomDest(const Vector2& p, float radius) {
	int tryCount = 0;
	AlignedBox2 box;
	Vector2 dest;
	do {
		dest = p + Vector2::Random() * radius;
		box = AlignedBox2(dest - Vector2(16, 32), dest + Vector2(16, 0));
		++tryCount;
	} while(tryCount < 10 && _ms->_level->hitTest(box));

	return (tryCount < 10)? dest: p;
}

/* stat: LOW, BAD, MAX (priority)
 * SICK:   (6)seek/use, (6)seek/use, (1)die
 * NEEDY:  (B)use,      (7)seek/use, (2)make a mess
 * HUNGRY: (C)use,      (8)seek/use, (3)sick
 * TIRED:  (D)use,      (9)seek/use, (4)sleep
 * BORED:  (E)use/play, (A)seek/use, (5)sleep
 */

void KittenComponentManager::update() {
	// Some garbage collection...
	compactArray();

	int nDir = 8;
	Eigen::Rotation2D<float> rotL( M_PI / double(nDir));
	Eigen::Rotation2D<float> rotR(-M_PI / double(nDir));

	for(unsigned k = 0 ; k < nComponents() ; ++k) {
		KittenComponent& kitten = _components[k];
		EntityRef entity = kitten.entity();

		if(!entity.isEnabledRec() || !kitten.isEnabled())
			continue;

		// Basal metabolism.
		if (kitten.sick)
			kitten.sick += kitten.sick * 0.003;
		else if (rand()%(180*TICKS_PER_SEC) == 0)
			kitten.sick = KIT_LOW;

		kitten.tired  += KIT_FPT;
		kitten.bored  += KIT_BPT;
		kitten.hungry += KIT_HPT;
		kitten.needy  += KIT_NPT;

		// Animation setting.
		switch(kitten.s) {
		case SITTING:
		case EATING:
		case PEEING:
			setAnim(kitten, ANIM_IDLE);
			break;
		case WALKING: {
			Vector2 v = kitten.dst - entity.position2();
			int axis;
			v.cwiseAbs().maxCoeff(&axis);
			if(axis == 0 && v(axis) <  0) setAnim(kitten, ANIM_LEFT);
			if(axis == 0 && v(axis) >= 0) setAnim(kitten, ANIM_RIGHT);
			if(axis == 1 && v(axis) <  0) setAnim(kitten, ANIM_DOWN);
			if(axis == 1 && v(axis) >= 0) setAnim(kitten, ANIM_UP);
			break;
		}
		case SLEEPING:
			setAnim(kitten, ANIM_SLEEP);
			break;
		case PLAYING:
			setAnim(kitten, ANIM_PLAY);
			break;
		}
		updateAnim(kitten);

		// Bubble setting.
		setBubble(entity, BUBBLE_NONE);
		if (kitten.bored  > KIT_LOW) { setBubble(entity, BUBBLE_TOY  ); }
		if (kitten.tired  > KIT_LOW) { setBubble(entity, BUBBLE_SLEEP); }
		if (kitten.hungry > KIT_LOW) { setBubble(entity, BUBBLE_FOOD ); }
		if (kitten.needy  > KIT_LOW) { setBubble(entity, BUBBLE_PEE  ); }
		if (kitten.bored  > KIT_BAD) { setBubble(entity, BUBBLE_TOY  ); }
		if (kitten.tired  > KIT_BAD) { setBubble(entity, BUBBLE_SLEEP); }
		if (kitten.hungry > KIT_BAD) { setBubble(entity, BUBBLE_FOOD ); }
		if (kitten.needy  > KIT_BAD) { setBubble(entity, BUBBLE_PEE  ); }
		if (kitten.sick   > KIT_LOW) { setBubble(entity, BUBBLE_PILL ); }

		// Current activity.
		kitten.t -= TICK_LENGTH_IN_SEC;
		Vector2 npos = entity.position2();
		switch (kitten.s) {
			case SITTING:
				if (rand()%(8*TICKS_PER_SEC) == 0) {
					kitten.bored += KIT_BPT;
					kitten.s = WALKING;
					kitten.bypass = BYPASS_NONE;
					kitten.dst = findRandomDest(entity.position2(), 400);
				}
				break;
		    case WALKING: {
			    Vector2 v = kitten.dst - entity.position2();
				float dist = v.norm();
				float walkDist = 100.0f * TICK_LENGTH_IN_SEC;
				if(dist >= walkDist) v = (v / dist) * walkDist;

				Vector2 vl = v;
				Vector2 vr = v;
				npos = entity.position2() + v;
				AlignedBox2 box(npos - Vector2(16, 32), npos + Vector2(16, 0));
				int tryCount = 0;
				int nTries = (kitten.bypass == BYPASS_NONE)? (nDir - 1) * 2: nDir - 1;
				BypassDir nextBypass = BYPASS_NONE;
				while(_ms->_level->hitTest(box)) {
					// Rotate v
					if(kitten.bypass == BYPASS_LEFT ||
					        (kitten.bypass == BYPASS_NONE && (tryCount & 1))) {
						vl = rotL * vl;
						v = vl;
						nextBypass = BYPASS_LEFT;
					}
					if(kitten.bypass == BYPASS_RIGHT ||
					        (kitten.bypass == BYPASS_NONE && !(tryCount & 1))) {
						vr = rotR * vr;
						v = vr;
						nextBypass = BYPASS_RIGHT;
					}

					npos = entity.position2() + v;
					box = AlignedBox2(npos - Vector2(16, 32), npos + Vector2(16, 0));

					if(tryCount > nTries) {
						// Stuck, should change target.
						npos = entity.position2();
						break;
					}
					++tryCount;
				}

				kitten.bypass = nextBypass;

				if(npos == entity.position2()) {
					setAnim(kitten, ANIM_IDLE);
					kitten.s = SITTING;
				}

				break;
		    }
			case SLEEPING:
				if (kitten.tired > 0) {
					kitten.tired -= KIT_REST;
					kitten.bored += KIT_BPT;
					kitten.bored = std::min(kitten.bored, KIT_LOW);
					kitten.hungry = std::min(kitten.hungry, KIT_BAD);
					kitten.needy = std::min(kitten.needy, KIT_BAD);
				}
				else
					kitten.s = SITTING;
				break;
			case PLAYING:
				if (kitten.bored > 0) {
					kitten.bored -= KIT_PLAY;
					kitten.tired += KIT_FPT;
				}
				else
					kitten.s = SITTING;
				break;
			case EATING:
				if (kitten.hungry > 0) {
					kitten.hungry -= KIT_FEED;
					kitten.bored -= KIT_BPT;
					kitten.needy += KIT_NPT;
				}
				else
					kitten.s = SITTING;
				break;
			case PEEING:
				if (kitten.needy > 0)
					kitten.needy -= KIT_PISS;
				else
					kitten.s = SITTING;
				break;
		};
		entity.moveTo(npos);

		// Shit happens to kitty.
		if (kitten.sick > KIT_MAX) { // 1
			kitten.s = DECOMPOSING;
			_ms->setSpawnDeath(_ms->_spawnCount, _ms->_deathCount + 1);
			setAnim(kitten, ANIM_DEAD);
			setBubble(entity, BUBBLE_NONE);
			kitten.setEnabled(false);
			dbgLogger.warning("Kit iz ded.");
			continue;
		} else if (kitten.needy > KIT_MAX) { // 2
			kitten.s = PEEING;
			kitten.t = 2;
			_ms->setHappiness(_ms->_happiness - 0.1);
			dbgLogger.warning("Oop kitty made a mess.");
			continue;
		} else if (kitten.hungry > KIT_MAX) { // 3
			kitten.hungry = KIT_LOW;
			kitten.sick = KIT_LOW;
			_ms->setHappiness(_ms->_happiness - 0.08);
			dbgLogger.warning("Got sick from lack of food.");
			continue;
		} else if (kitten.tired > KIT_MAX) { // 4
			kitten.s = SLEEPING;
			kitten.t = 5;
			_ms->setHappiness(_ms->_happiness - 0.05);
			dbgLogger.warning("I sleep now.");
			continue;
		} else if (kitten.bored > KIT_MAX) { // 5
			kitten.s = SLEEPING;
			kitten.t = 2;
			kitten.bored = KIT_LOW;
			_ms->setHappiness(_ms->_happiness - 0.05);
			dbgLogger.warning("Sooooo boooooooZZZZzzz...");
			continue;
		}

		// What kitty steps on.
		std::deque<EntityRef> hits;
		AlignedBox2 box = _ms->_collisions.get(entity)->shapes()[0].transformed(entity.worldTransform().matrix()).asAlignedBox();
		_ms->_collisions.hitTest(hits, box, HIT_TOY, entity);
		unsigned options = 0x00;
		for (EntityRef e: hits) { // Toys ?
			ToyComponent* t = _ms->_toys.get(e);
			if (t->state != t->PLACED) { continue; }
			if      (t->type == TOY_FEED)  { options |= 0x01; }
			else if (t->type == TOY_PLAY)  { options |= 0x02; }
			else if (t->type == TOY_PISS)  { options |= 0x04; }
			else if (t->type == TOY_HEAL)  { options |= 0x08; }
			else if (t->type == TOY_SLEEP) { options |= 0x10; }
		}
		hits.clear(); // Other kit ?
		_ms->_collisions.hitTest(hits, box, HIT_KITTEN, entity);
		if (!hits.empty()) { options |= 0x20; }

		// Kitty is pondering things.
		if (kitten.s > WALKING && kitten.t > 0)
			continue;

		if (kitten.sick > KIT_LOW) { // 6
			kitten.bored = KIT_BAD - TICKS_PER_SEC * KIT_BPT;
			if (options & 0x08) {
				kitten.sick = 0;
				kitten.hungry = KIT_LOW;
				kitten.tired = KIT_BAD;
			} else
				seek(kitten,TOY_HEAL, true);
			continue;
		}

		for (KittenComponent::stat threshold: {KIT_BAD, KIT_LOW})
		{
			if (kitten.needy > threshold) { // 7/B
				if (options & 0x04) {
					kitten.s = PEEING;
					kitten.t = 1;
				} else
					seek(kitten,TOY_PISS, threshold == KIT_BAD);
				continue;
			} else if (kitten.hungry > threshold) { // 8/C
				if (options & 0x01) {
					kitten.s = EATING;
					kitten.t = 2;
				} else
					seek(kitten,TOY_FEED, threshold == KIT_BAD);
				continue;
			} else if (kitten.tired > threshold) { // 9/D
				if (options & 0x10) {
					kitten.s = SLEEPING;
					kitten.t = 5;
				} else
					seek(kitten,TOY_SLEEP, threshold == KIT_BAD);
				continue;
			} else if (kitten.bored > threshold) { // A/E
				if (options & 0x22) {
					kitten.s = PLAYING;
					kitten.t = 1;
				} else
					seek(kitten,TOY_PLAY, threshold == KIT_BAD);
				continue;
			}
		}
	}
}

//---------------------------------------------------------------------------//


const lair::EnumInfo* toyTypeInfo() {
	static EnumInfo info("ToyType");
	if(!info.nFields()) {
		info.add(TOY_FEED,  "eat");
		info.add(TOY_PLAY,  "play");
		info.add(TOY_PISS,  "piss");
		info.add(TOY_HEAL,  "heal");
		info.add(TOY_SLEEP, "sleep");
	}
	return &info;
}


ToyComponent::ToyComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity)
    , type()
    , size(1, 1)
    , cost(10)
    , state(NONE)
{
}


const PropertyList& ToyComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("type", toyTypeInfo(), &ToyComponent::type);
		props.addProperty("size", &ToyComponent::size);
		props.addProperty("cost", &ToyComponent::cost);
	}
	return props;
}


ToyComponentManager::ToyComponentManager(MainState* ms)
    : DenseComponentManager<ToyComponent>("toy", 128),
    _ms(ms)
{
}

void ToyComponentManager::update() {
	compactArray();

	for(unsigned ti = 0; ti < nComponents(); ++ti) {
		ToyComponent& toy = _components[ti];
		EntityRef entity = toy.entity();

		if(!entity.isEnabledRec() || !toy.isEnabled())
			continue;

		// TODO: update toy.
	}
}
