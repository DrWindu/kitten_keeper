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


#ifndef LD_40_UI_EVENT_H_
#define LD_40_UI_EVENT_H_

#include <lair/core/lair.h>


enum EventType {
	EVENT_MOUSE,
	EVENT_RESIZE,
};

class Event {
public:
	Event(unsigned type);
	virtual ~Event();

	unsigned type() const;
	bool isAccepted() const;

	void accept();
	void reject();

protected:
	unsigned _type;
	bool _accepted;
};


enum MouseButton {
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT,
};


class MouseEvent : public Event {
public:
	enum MouseType {
		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
	};

public:
	MouseEvent(MouseType mouseType, const lair::Vector2& position, MouseButton button);

	MouseType mouseType() const;
	const lair::Vector2& position() const;
	MouseButton button() const;

protected:
	MouseType     _mouseType;
	lair::Vector2 _position;
	MouseButton   _button;
};


class ResizeEvent : public Event {
public:
	ResizeEvent(const lair::Vector2& size);

	const lair::Vector2& size() const;

protected:
	lair::Vector2 _size;
};


#endif
