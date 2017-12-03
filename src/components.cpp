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
    // TODO: Default kitten states
{
}


const PropertyList& KittenComponent::properties() {
	static PropertyList props;
	if(props.nProperties() == 0) {
		// TODO: Kitten properties declarations
	}
	return props;
}


KittenComponentManager::KittenComponentManager()
    : DenseComponentManager<KittenComponent>("kitten", 128)
{
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
