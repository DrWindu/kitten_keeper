/*
 *  Copyright (C) 2016 the authors (see AUTHORS)
 *
 *  This file is part of ld36.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
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


KittenComponent::KittenComponent(Manager* manager, _Entity* entity) :
	Component(manager, entity),
	sick(42),
	tired(42),
	bored(42),
	hungry(42),
	t(0)
{
}


const PropertyList& KittenComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		props.addProperty("sick",   &KittenComponent::sick);
		props.addProperty("tired",  &KittenComponent::tired);
		props.addProperty("bored",  &KittenComponent::bored);
		props.addProperty("hungry", &KittenComponent::hungry);
		props.addProperty("t",      &KittenComponent::t);
	}
	return props;
}


KittenComponentManager::KittenComponentManager(MainState* ms)
    : DenseComponentManager<KittenComponent>("kitten", 128),
    _ms(ms)
{
}

// Kitten constants : fatigue/boredom/hunger per tick.
#define KIT_FPT 0.01
#define KIT_BPT 0.05
#define KIT_HPT 0.02

void KittenComponentManager::update() {
	// Some garbage collection...
	compactArray();

	for(unsigned k = 0 ; k < nComponents() ; ++k) {
		KittenComponent& kitten = _components[k];
		EntityRef entity = kitten.entity();

		kitten.sick   += 0;
		kitten.tired  += KIT_FPT;
		kitten.bored  += KIT_BPT;
		kitten.hungry += KIT_HPT;

		Vector2 npos;
		do { npos = entity.position2() + Vector2(rand()%5-2, rand()%5-2); }
		while (_ms->_level->inSolid(npos));
		entity.moveTo(npos);

		kitten.t += TICK_LENGTH_IN_SEC;
	}
}


//---------------------------------------------------------------------------//


ToyComponent::ToyComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity)
    // TODO: Default toy states
{
}


const PropertyList& ToyComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		// TODO: Toys properties declarations.
	}
	return props;
}


ToyComponentManager::ToyComponentManager(MainState* ms)
    : DenseComponentManager<ToyComponent>("toy", 128),
    _ms(ms)
{
}
