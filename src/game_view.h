/*
 *  Copyright (C) 2017 the authors (see AUTHORS)
 *
 *  This file is part o kitten keeper.
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


#ifndef LD_40_GAME_VIEW_H_
#define LD_40_GAME_VIEW_H_

#include <vector>

#include <lair/core/lair.h>

#include <lair/ec/entity.h>

#include "ui/widget.h"

class MainState;
class ToyComponent;

enum {
	PLACEMENT_TILE_SIZE = 16,
};

class GameView : public Widget {
public:
	GameView(Gui* gui, Widget* parent = nullptr);

	MainState* mainState() const;

	void setMainState(MainState* mainState);

	lair::Vector2 sceneFromScreen(lair::Vector2 screen) const;

	lair::Vector2 roundPlacement(const lair::Vector2& p) const;
	bool canPlaceToy(ToyComponent* toy, const lair::Vector2& scenePos);

	lair::EntityRef grabEntity();
	void beginGrab(lair::EntityRef& toy, const lair::Vector2& scenePos);
	void moveGrabbed(const lair::Vector2& scenePos);
	void endGrab();
	void cancelGrab();

	virtual void mousePressEvent(MouseEvent& event) override;
	virtual void mouseReleaseEvent(MouseEvent& event) override;
	virtual void mouseMoveEvent(MouseEvent& event) override;

protected:
	MainState* _mainState;

	lair::EntityRef _grabEntity;
};


#endif
