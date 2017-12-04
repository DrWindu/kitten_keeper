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

#include <functional>

#include "widget.h"

#include "gui.h"

using namespace lair;

Gui::Gui(SysModule* sys,
         AssetManager* assets,
         LoaderManager* loader,
         SpriteRenderer* spriteRenderer)
    : _sys(sys)
    , _assets(assets)
    , _loader(loader)
    , _spriteRenderer(spriteRenderer)
    , _mouseWidget(nullptr)
    , _mouseGrabWidget(nullptr)
    , _logicScreenSize(Vector2(1920, 1080))
    , _realScreenSize(Vector2(1920, 1080))
{
	using std::placeholders::_1;
	_sys->onMouseMove    = std::bind(&Gui::dispatchMouseMoveEvent,   this, _1);
	_sys->onMousePress   = std::bind(&Gui::dispatchMouseButtonEvent, this, _1);
	_sys->onMouseRelease = std::bind(&Gui::dispatchMouseButtonEvent, this, _1);
	_sys->onMouseWheel   = std::bind(&Gui::dispatchMouseWheelEvent,  this, _1);
}

Gui::~Gui() {
	for(Widget* widget: _widgets)
		delete widget;
}

void Gui::addWidget(Widget* widget) {
	_widgets.push_back(widget);
}

void Gui::removeWidget(Widget* widget) {
	auto it = std::find(_widgets.begin(), _widgets.end(), widget);
	lairAssert(it != _widgets.end());
	_widgets.erase(it);
}

void Gui::preRender() {
	for(Widget* widget: _widgets) {
		widget->preRender(_spriteRenderer);
	}
}

void Gui::render(lair::RenderPass& renderPass, const Matrix4& transform) {
	for(Widget* widget: _widgets) {
		widget->render(renderPass, _spriteRenderer, transform);
	}
}

Widget* Gui::widgetAt(const Vector2& position) const {
	Widget* found = nullptr;

	// Keep the last one, i.e. the topmost.
	for(Widget* widget: _widgets) {
		if(widget->enabled()) {
			Widget* f = widget->widgetAt(position);
			if(f) found = f;
		}
	}

	return found;
}

lair::Vector2 Gui::logicScreenSize() const {
	return _logicScreenSize;
}

void Gui::setLogicScreenSize(const Vector2& logicSize) {
	_logicScreenSize = logicSize;
}

void Gui::setRealScreenSize(const Vector2& realSize) {
	_realScreenSize = realSize;
}

Vector2 Gui::screenFromReal(int rx, int ry) const {
	return Vector2(rx, _realScreenSize(1) - ry - 1)
	        .cwiseProduct(_logicScreenSize)
	        .cwiseQuotient(_realScreenSize);
}

lair::Vector2 Gui::lastMousePosition() const {
	return _lastMousePos;
}

void Gui::setMouseGrabWidget(Widget* widget) {
	_mouseGrabWidget = widget;
}

void Gui::dispatchEvent(Event& event) {
	switch(event.type()) {
	case EVENT_MOUSE: {
		auto mEvent = static_cast<MouseEvent&>(event);
		const Vector2& position = mEvent.position();

		for(Widget* w: _widgets) {
			dispatchHoverEvent(w, position);
		}

		Widget* widget = _mouseGrabWidget;
		if(widget) {
			widget->processEvent(event);
		}
		else {
			Widget* widget = widgetAt(position);
			if(widget) {
				Widget* w = widget;
				while(w && !event.isAccepted()) {
					w->processEvent(event);
					w = w->parent();
				}
			}
		}

		_lastMousePos = position;
		_mouseWidget = widget;
		break;
	}
	case EVENT_RESIZE:
		break;
	}
}

void Gui::dispatchHoverEvent(Widget* widget, const lair::Vector2& position) {
	if(!widget->enabled())
		return;

	Box2 box = widget->absoluteBox();
	bool wasInside = box.contains(_lastMousePos);
	bool isInside  = box.contains(position);

	if(wasInside != isInside) {
		HoverEvent hover(isInside? HoverEvent::ENTER: HoverEvent::LEAVE);
		widget->processEvent(hover);
	}

	for(Widget* child: widget->children())
		dispatchHoverEvent(child, position);
}

void Gui::dispatchMouseMoveEvent(const SDL_MouseMotionEvent& event) {
	MouseEvent e(MouseEvent::MOUSE_MOVE, screenFromReal(event.x, event.y), 0);
	dispatchEvent(e);
}

void Gui::dispatchMouseButtonEvent(const SDL_MouseButtonEvent& event) {
	unsigned button = 0;
	switch(event.button) {
	case SDL_BUTTON_LEFT:   button = MOUSE_LEFT;   break;
	case SDL_BUTTON_MIDDLE: button = MOUSE_MIDDLE; break;
	case SDL_BUTTON_RIGHT:  button = MOUSE_RIGHT;  break;
	default: break;
	}

	MouseEvent::MouseType type = (event.type == SDL_MOUSEBUTTONDOWN)?
	                                 MouseEvent::MOUSE_DOWN: MouseEvent::MOUSE_UP;

	MouseEvent e(type, screenFromReal(event.x, event.y), button);
	dispatchEvent(e);
}

void Gui::dispatchMouseWheelEvent(const SDL_MouseWheelEvent& /*event*/) {
}

AssetManager* Gui::assets() {
	return _assets;
}

LoaderManager* Gui::loader() {
	return _loader;
}

SpriteRenderer* Gui::spriteRenderer() {
	return _spriteRenderer;
}
