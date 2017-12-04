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


#ifndef LD_40_TOY_BUTTON_H_
#define LD_40_TOY_BUTTON_H_

#include <lair/ec/entity.h>

#include "ui/widget.h"

class MainState;
class Picture;
class Label;

class ToyButton : public Widget {
public:
	ToyButton(Gui* gui, Widget* parent = nullptr);

	void setMainState(MainState* mainState);
	void setPicture(const lair::Path& logicPath);
	void setFont(const lair::Path& logicPath);
	void setModel(lair::EntityRef model);
	void setCost(int cost);
	void setToyName(const lair::String& name);
	void setDescription(const lair::String& description);

	void update();

	void layout();

	virtual void mouseReleaseEvent(MouseEvent& event) override;

	virtual void mouseEnterEvent(HoverEvent& event) override;
	virtual void mouseLeaveEvent(HoverEvent& event) override;

protected:
	MainState* _mainState;

	Picture* _picture;
	Label*  _tooltip;

	lair::EntityRef _model;
	int             _cost;
	lair::String    _toyName;
};


#endif
