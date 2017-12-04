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


#ifndef KITTEN_KEEPER_COMPONENTS_H_
#define KITTEN_KEEPER_COMPONENTS_H_


#include <map>

#include <lair/core/lair.h>
#include <lair/core/metatype.h>
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

typedef enum {
	SITTING,
	WALKING,
	SLEEPING,
	PLAYING,
	EATING,
	PEEING,
	DECOMPOSING
} status;

enum BubbleType {
	BUBBLE_PEE,
	BUBBLE_TOY,
	BUBBLE_FOOD,
	BUBBLE_SLEEP,
	BUBBLE_PILL,
	BUBBLE_LOVE,
	BUBBLE_NONE
};

enum KittenAnim {
	ANIM_IDLE,
	ANIM_TOP,
	ANIM_RIGHT,
	ANIM_DOWN,
	ANIM_LEFT,
	ANIM_SLEEP,
	ANIM_PLAY,
	ANIM_DEAD,
};

enum ToyType {
	TOY_FEED,
	TOY_PLAY,
	TOY_PISS,
	TOY_HEAL,
	TOY_SLEEP,
};
const lair::EnumInfo* toyTypeInfo();

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
	typedef float stat;
	stat sick;
	stat tired;
	stat bored;
	stat hungry;
	stat needy;

	unsigned s; // Not "status s;" because fuck it, that's why.
	double t;
	Vector2 dst;

	KittenAnim anim;
	float      animTime;
};

class KittenComponentManager : public DenseComponentManager<KittenComponent> {
public:
	KittenComponentManager(MainState* ms);
	virtual ~KittenComponentManager() = default;


	void setBubble(EntityRef kitten, BubbleType bubbleType);
	void setAnim(KittenComponent& kitten, KittenAnim anim);
	void updateAnim(KittenComponent& kitten);
	void seek(KittenComponent& k, ToyType tt, bool now);
	void update();

public:
	MainState* _ms;
};

class ToyComponent : public Component {
public:
	typedef ToyComponentManager Manager;

	enum State {
		NONE,
		DRAGGED,
		PLACED,
	};

public:
	ToyComponent(Manager* manager, _Entity* entity);
	ToyComponent(const ToyComponent&)  = delete;
	ToyComponent(      ToyComponent&&) = default;
	~ToyComponent() = default;

	ToyComponent& operator=(const ToyComponent&)  = delete;
	ToyComponent& operator=(      ToyComponent&&) = default;

	static const PropertyList& properties();

public:
	ToyType        type;
	lair::Vector2i size;

	State         state;
	State         startState;
	lair::Vector2 startPos;
};

class ToyComponentManager : public DenseComponentManager<ToyComponent> {
public:
	ToyComponentManager(MainState* ms);
	virtual ~ToyComponentManager() = default;

	void update();

public:
	MainState* _ms;
};

LAIR_REGISTER_METATYPE(ToyType, "ToyType");


#endif
