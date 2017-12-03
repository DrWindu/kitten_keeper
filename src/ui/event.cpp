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

using namespace lair;

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


MouseEvent::MouseEvent(MouseType mouseType, const lair::Vector2& position, unsigned button)
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

unsigned MouseEvent::button() const {
	return _button;
}

std::ostream& MouseEvent::write(std::ostream& out) const {
	switch(_mouseType) {
	case MOUSE_DOWN: out << "<Event MouseDown"; break;
	case MOUSE_UP:   out << "<Event MouseUp";   break;
	case MOUSE_MOVE: out << "<Event MouseMove"; break;
	}

	out << " " << fmt(_position) << " buttons: (";

	bool sep = false;
	if(_button & MOUSE_LEFT) {
		out << "left";
		sep = true;
	}
	if(_button & MOUSE_MIDDLE) {
		out << (sep? ", ": "") << "middle";
		sep = true;
	}
	if(_button & MOUSE_RIGHT) {
		out << (sep? ", ": "") << "right";
		sep = true;
	}

	out << ")>";

	return out;
}


HoverEvent::HoverEvent(HoverType hoverType)
    : Event(EVENT_HOVER)
    , _hoverType(hoverType)
{
}

HoverEvent::HoverType HoverEvent::hoverType() const {
	return _hoverType;
}

std::ostream& HoverEvent::write(std::ostream& out) const {
	out << "<Event MouseHover " << ((hoverType() == ENTER)? "enter": "leave") << ">";
	return out;
}


ResizeEvent::ResizeEvent(const lair::Vector2& size)
    : Event(EVENT_RESIZE)
    , _size(size)
{
}

const lair::Vector2& ResizeEvent::size() const {
	return _size;
}

std::ostream& ResizeEvent::write(std::ostream& out) const {
	out << "<Event Resize " << fmt(_size) << ">";
	return out;
}
