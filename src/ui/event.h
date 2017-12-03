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
	EVENT_HOVER,
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

	virtual std::ostream& write(std::ostream& out) const = 0;

protected:
	unsigned _type;
	bool _accepted;
};

inline std::ostream& operator<<(std::ostream& out, const Event& event) {
	return event.write(out);
}


enum MouseButton {
	MOUSE_LEFT   = (1 << 0),
	MOUSE_MIDDLE = (1 << 1),
	MOUSE_RIGHT  = (1 << 2),
};


class MouseEvent : public Event {
public:
	enum MouseType {
		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
	};

public:
	MouseEvent(MouseType mouseType, const lair::Vector2& position, unsigned button);

	MouseType mouseType() const;
	const lair::Vector2& position() const;
	unsigned button() const;

	virtual std::ostream& write(std::ostream& out) const override;

protected:
	MouseType     _mouseType;
	lair::Vector2 _position;
	unsigned      _button;
};



class HoverEvent : public Event {
public:
	enum HoverType {
		ENTER,
		LEAVE,
	};

public:
	HoverEvent(HoverType hoverType);

	HoverType hoverType() const;

	virtual std::ostream& write(std::ostream& out) const override;

protected:
	HoverType     _hoverType;
};


class ResizeEvent : public Event {
public:
	ResizeEvent(const lair::Vector2& size);

	const lair::Vector2& size() const;

	virtual std::ostream& write(std::ostream& out) const override;

protected:
	lair::Vector2 _size;
};


#endif
