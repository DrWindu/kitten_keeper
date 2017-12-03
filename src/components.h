/*
 *  Copyright (C) 2015, 2017 the authors (see AUTHORS)
 *
 *  This file is part of Draklia's ld39.
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


#ifndef LD_39_COMPONENTS_H_
#define LD_39_COMPONENTS_H_


#include <map>

#include <lair/core/lair.h>
#include <lair/core/path.h>

#include <lair/ec/entity.h>
#include <lair/ec/component.h>
#include <lair/ec/dense_component_manager.h>
#include <lair/ec/collision_component.h>


using namespace lair;

class Level;
class MainState;

class TriggerComponentManager;
class KittenComponentManager;
class ToyComponentManager;

enum DirFlags {
	DIR_NONE  = 0x00,
	DIR_UP    = 1 << UP,
	DIR_LEFT  = 1 << LEFT,
	DIR_DOWN  = 1 << DOWN,
	DIR_RIGHT = 1 << RIGHT,
};

class TriggerComponent : public Component {
public:
	typedef TriggerComponentManager Manager;

public:
	TriggerComponent(Manager* manager, _Entity* entity);
	TriggerComponent(const TriggerComponent&)  = delete;
	TriggerComponent(      TriggerComponent&&) = default;
	~TriggerComponent() = default;

	TriggerComponent& operator=(const TriggerComponent&)  = delete;
	TriggerComponent& operator=(      TriggerComponent&&) = default;

	static const PropertyList& properties();

public:
	bool        prevInside;
	bool        inside;
	std::string onEnter;
	std::string onExit;
	std::string onUse;
};

class TriggerComponentManager : public DenseComponentManager<TriggerComponent> {
public:
	TriggerComponentManager();
	virtual ~TriggerComponentManager() = default;
};


class KittenComponent : public Component {
public:
	typedef KittenComponentManager Manager;

public:
	KittenComponent(Manager* manager, _Entity* entity);
	KittenComponent(const KittenComponent&)  = delete;
	KittenComponent(      KittenComponent&&) = default;
	~KittenComponent() = default;

	KittenComponent& operator=(const KittenComponent&)  = delete;
	KittenComponent& operator=(      KittenComponent&&) = default;

	static const PropertyList& properties();

public:
	// TODO[Doc]: Kitten states: put whatever you need here !
	Vector2 test1;
	Vector2 test2;
	double  t;
};

class KittenComponentManager : public DenseComponentManager<KittenComponent> {
public:
	KittenComponentManager();
	virtual ~KittenComponentManager() = default;

	void update();
};


class ToyComponent : public Component {
public:
	typedef ToyComponentManager Manager;

public:
	ToyComponent(Manager* manager, _Entity* entity);
	ToyComponent(const ToyComponent&)  = delete;
	ToyComponent(      ToyComponent&&) = default;
	~ToyComponent() = default;

	ToyComponent& operator=(const ToyComponent&)  = delete;
	ToyComponent& operator=(      ToyComponent&&) = default;

	static const PropertyList& properties();

public:
	// TODO: Toys states
};

class ToyComponentManager : public DenseComponentManager<ToyComponent> {
public:
	ToyComponentManager();
	virtual ~ToyComponentManager() = default;
};


#endif
