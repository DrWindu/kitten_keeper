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


#include "event.h"


Event::Event(unsigned type)
    : _type    (type)
    , _accepted(false)
{
}

Event::~Event() {
}

unsigned Event::type() const {
	return _type;
}

bool Event::isAccepted() const {
	return _accepted;
}

void Event::accept() {
	_accepted = true;
}

void Event::reject() {
	_accepted = false;
}


MouseEvent::MouseEvent(MouseType mouseType, const lair::Vector2& position, MouseButton button)
    : Event     (EVENT_MOUSE)
    , _mouseType(mouseType)
    , _position (position)
    , _button   (button)
{
}

MouseEvent::MouseType MouseEvent::mouseType() const {
	return _mouseType;
}

const lair::Vector2& MouseEvent::position() const {
	return _position;
}

MouseButton MouseEvent::button() const {
	return _button;
}


ResizeEvent::ResizeEvent(const lair::Vector2& size)
    : Event(EVENT_RESIZE)
    , _size(size)
{
}

const lair::Vector2& ResizeEvent::size() const {
	return _size;
}
