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
#define KIT_FPT 0.01
#define KIT_BPT 0.05
#define KIT_HPT 0.02
#define KIT_NPT 0.02

#define KIT_LOW 25.0
#define KIT_BAD 75.0
#define KIT_MAX 100.0

#define KIT_WALK 0.02

#define KIT_REST 0.05
#define KIT_PLAY 0.5
#define KIT_FEED 0.4
#define KIT_PISS 0.6

KittenComponent::KittenComponent(Manager* manager, _Entity* entity) :
	Component(manager, entity),
	sick(0),
	tired(2),
	bored(2),
	hungry(2),
	needy(2),
	s(WANDERING),
	t(0),
	dst(0,0)
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


void KittenComponent::seek(ToyType t, bool now)
{
	if (now)
		(void) t;
	dbgLogger.info("Beuleubeuleuh !");
}

KittenComponentManager::KittenComponentManager(MainState* ms)
    : DenseComponentManager<KittenComponent>("kitten", 128),
    _ms(ms)
{
}

void KittenComponentManager::update() {
	// Some garbage collection...
	compactArray();

	for(unsigned k = 0 ; k < nComponents() ; ++k) {
		KittenComponent& kitten = _components[k];
		EntityRef entity = kitten.entity();

		if(!entity.isEnabledRec() || !kitten.isEnabled())
			continue;

		// Basal metabolism.
		if (kitten.sick)
			kitten.sick += kitten.sick * 0.01;
		else if (!(rand()%(60*TICKS_PER_SEC)))
			kitten.sick = KIT_LOW;
		kitten.tired  += KIT_FPT;
		kitten.bored  += KIT_BPT;
		kitten.hungry += KIT_HPT;
		kitten.needy  += KIT_NPT;

		// Current activity.
		Vector2 npos = entity.position2();
		switch (kitten.s) {
			case kitten.WANDERING:
				do { npos = entity.position2() + Vector2(rand()%5-2, rand()%5-2); }
				while (_ms->_level->inSolid(npos));
				break;
			case kitten.WALKING:
				npos = lerp(KIT_WALK, entity.position2(), kitten.dst);
				break;
			case kitten.SLEEPING:
				kitten.tired -= KIT_REST;
				break;
			case kitten.PLAYING:
				kitten.bored -= KIT_PLAY;
				break;
			case kitten.EATING:
				kitten.hungry -= KIT_FEED;
				break;
			case kitten.PEEING:
				kitten.needy -= KIT_PISS;
				break;
		};

		entity.moveTo(npos);

		kitten.t -= TICK_LENGTH_IN_SEC;

		// What kitty steps on.
		std::deque<EntityRef> hits;
		AlignedBox2 box = _ms->_collisions.get(entity)->shapes()[0].transformed(entity.worldTransform().matrix()).asAlignedBox();
		_ms->_collisions.hitTest(hits, box, HIT_TOY, entity);
		unsigned options = 0x00;
		for (EntityRef e: hits) {
			ToyComponent* t = _ms->_toys.get(e);
			if      (t->type ==  TOY_FEED)  { options |= 0x01; }
			else if (t->type ==  TOY_PLAY)  { options |= 0x02; }
			else if (t->type ==  TOY_PISS)  { options |= 0x04; }
			else if (t->type ==  TOY_HEAL)  { options |= 0x08; }
			else if (t->type ==  TOY_SLEEP) { options |= 0x10; }
		}

		hits.clear();
		_ms->_collisions.hitTest(hits, box, HIT_KITTEN, entity);
		if (!hits.empty()) { options |= 0x20; }

		/* Kitty is pondering things.
		 *
		 * stat: LOW, BAD, MAX (priority)
		 * SICK:   (6)seek/use, (6)seek/use, (1)die
		 * NEEDY:  (B)use,      (7)seek/use, (2)make a mess
		 * HUNGRY: (C)use,      (8)seek/use, (3)sick
		 * TIRED:  (D)use,      (9)seek/use, (4)sleep
		 * BORED:  (E)use/play, (A)seek/use, (5)sleep
		 */
		if (kitten.sick > KIT_MAX) { // 1
			dbgLogger.warning("Kit iz ded.");
			continue;
		} else if (kitten.needy > KIT_MAX) { // 2
			kitten.s = kitten.PEEING;
			dbgLogger.warning("Oop kitty made a mess.");
			continue;
		} else if (kitten.hungry > KIT_MAX) { // 3
			kitten.hungry = KIT_LOW;
			kitten.sick = KIT_LOW;
			dbgLogger.warning("Got sick from lack of food.");
			continue;
		} else if (kitten.tired > KIT_MAX) { // 4
			kitten.s = kitten.SLEEPING;
			dbgLogger.warning("I sleep now.");
			continue;
		} else if (kitten.bored > KIT_MAX) { // 5
			kitten.s = kitten.SLEEPING;
			kitten.bored = KIT_LOW;
			dbgLogger.warning("Sooooo boooooooZZZZzzz...");
			continue;
		} else if (kitten.sick > KIT_LOW) { // 6
			kitten.bored = KIT_BAD - TICKS_PER_SEC * KIT_BPT;
			if (options & 0x08) {
				kitten.sick = 0;
				kitten.hungry = KIT_LOW;
				kitten.tired = KIT_BAD;
				dbgLogger.warning("Pillz here !");
				continue;
			} else {
				kitten.seek(TOY_HEAL, true);
				dbgLogger.warning("Need a med-kit !");
				continue;
			}
		}

		for (KittenComponent::stat threshold: {KIT_BAD, KIT_LOW})
		{
			if (kitten.needy > threshold) { // 7/B
				if (options & 0x04) {
					kitten.s = kitten.PEEING;
					dbgLogger.warning("Found litter. Now peeing right next to it.");
				} else {
					kitten.seek(TOY_PISS, threshold == KIT_BAD);
					dbgLogger.warning("Woopsie gotta go !");
				}
				continue;
			} else if (kitten.hungry > threshold) { // 8/C
				if (options & 0x01) {
					kitten.s = kitten.EATING;
					dbgLogger.warning("OM NOM NOM NOM !");
				} else {
					kitten.seek(TOY_FEED, threshold == KIT_BAD);
					dbgLogger.warning("Today's fish is trout à la crème. Enjoy your meal !");
				}
				continue;
			} else if (kitten.tired > threshold) { // 9/D
				if (options & 0x10) {
					kitten.s = kitten.SLEEPING;
					dbgLogger.warning("ZZZZzzzzz...");
				} else {
					kitten.seek(TOY_SLEEP, threshold == KIT_BAD);
					dbgLogger.warning("Kitty is sleepy, hollow.");
				}
				continue;
			} else if (kitten.bored > threshold) { // A/E
				if (options & 0x22) {
					kitten.s = kitten.PLAYING;
					dbgLogger.warning("Kitteh is entertaining itself with some thing it found.");
				} else {
					kitten.seek(TOY_PLAY, threshold == KIT_BAD);
					dbgLogger.warning("Soooo booored...");
				}
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
    , state(NONE)
{
}


const PropertyList& ToyComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("type", toyTypeInfo(), &ToyComponent::type);
		props.addProperty("size", &ToyComponent::size);
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
