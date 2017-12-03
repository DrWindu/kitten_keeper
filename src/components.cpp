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


KittenComponent::KittenComponent(Manager* manager, _Entity* entity)
    : Component(manager, entity)
    // TODO[Doc]: Default kitten states
    , test1(128, 128)
    , test2(1500, 1000)
    , t(0)
{
}


const PropertyList& KittenComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		// TODO: Kitten properties declarations. This allows to set properties
		// in ldl files AND to clone component automagically.
		props.addProperty("test1", &KittenComponent::test1);
		props.addProperty("test2", &KittenComponent::test2);
		props.addProperty("n", &KittenComponent::t);
	}
	return props;
}


KittenComponentManager::KittenComponentManager()
    : DenseComponentManager<KittenComponent>("kitten", 128)
{
}


void KittenComponentManager::update() {
	// Some garbage collection...
	compactArray();

	// TODO[Doc]: Update kittens here (note: this function might need extra arguments).
	for(unsigned ki = 0; ki < nComponents(); ++ki) {
		KittenComponent& kitten = _components[ki];
		EntityRef entity = kitten.entity();

		if(kitten.t > 2)
			kitten.t -= 2;
		float t = fabs(kitten.t - 1);
		entity.moveTo(lerp(t, kitten.test1, kitten.test2));

		kitten.t += 0.003;
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


ToyComponentManager::ToyComponentManager()
    : DenseComponentManager<ToyComponent>("toy", 128)
{
}
