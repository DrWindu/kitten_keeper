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


#include <lair/sys_sdl2/image_loader.h>

#include "gui.h"

#include "widget.h"


using namespace lair;

Widget::Widget(Gui* gui, Widget* parent)
    : _gui     (gui)
    , _parent  (nullptr)
    , _box     (Vector2(0, 0), Vector2(128, 128))
    , _children()
{
	lairAssert(_gui);

	if(parent) {
		parent->addChild(this);
	}
}

Widget::~Widget() {
	for(Widget* child: _children)
		delete child;

	if(_parent) {
		_parent->removeChild(this);
	}
}

Gui* Widget::gui() const {
	return _gui;
}

Widget* Widget::parent() const {
	return _parent;
}

Vector2 Widget::position() const {
	return _box.min();
}

Vector2 Widget::absolutePosition() const {
	if(_parent)
		return _parent->absolutePosition() + position();
	return position();
}

Vector2 Widget::size() const {
	return _box.sizes();
}

TextureAspectSP Widget::frameTexture() const {
	return _frame.texture();
}

Vector4 Widget::frameColor() const {
	return _frame.color();
}

unsigned Widget::nChildren() const {
	return _children.size();
}

Widget* Widget::child(unsigned i) const {
	return _children[i];
}

void Widget::place(const lair::Vector2& position) {
	_box.max() = position + _box.sizes();
	_box.min() = position;
}

void Widget::resize(const Vector2& size) {
	_box.max() = _box.min() + size;
}

void Widget::setFrameTexture(TextureAspectSP texture) {
	_frame.setTexture(texture);
}

void Widget::setFrameTexture(AssetSP texture) {
	if(texture) {
		TextureAspectSP ta = texture->aspect<TextureAspect>();
		if(!ta) {
			ta = gui()->spriteRenderer()->createTexture(texture);
		}
		setFrameTexture(ta);
	}
	else {
		setFrameTexture(TextureAspectSP());
	}
}

void Widget::setFrameTexture(const Path& logicPath) {
	AssetSP asset = gui()->loader()->loadAsset<ImageLoader>(logicPath);
	setFrameTexture(asset);
}

void Widget::setFrameColor(const Vector4& color) {
	_frame.setColor(color);
}

void Widget::addChild(Widget* child) {
	if(child->parent()) {
		child->parent()->removeChild(child);
	}

	child->_parent = this;
	_children.push_back(child);
}

void Widget::removeChild(Widget* child) {
	lairAssert(child->parent() == this);

	child->_parent = nullptr;
	auto it = std::find(_children.begin(), _children.end(), child);
	lairAssert(it != _children.end());
	_children.erase(it);
}

void Widget::processEvent(Event& event) {
	switch(event.type()) {
	case EVENT_MOUSE: {
		MouseEvent& mEvent = static_cast<MouseEvent&>(event);
		switch(mEvent.mouseType()) {
		case MouseEvent::MOUSE_DOWN:
			mousePressEvent(mEvent);
			break;
		case MouseEvent::MOUSE_UP:
			mouseReleaseEvent(mEvent);
			break;
		case MouseEvent::MOUSE_MOVE:
			mouseMoveEvent(mEvent);
			break;
		}
		break;
	}
	case EVENT_RESIZE:
		resizeEvent(static_cast<ResizeEvent&>(event));
		break;
	}
}

void Widget::mousePressEvent(MouseEvent& event) {
	event.reject();
}

void Widget::mouseReleaseEvent(MouseEvent& event) {
	event.reject();
}

void Widget::mouseMoveEvent(MouseEvent& event) {
	event.reject();
}

void Widget::resizeEvent(ResizeEvent& event) {
	event.reject();
}

int Widget::render(lair::RenderPass& renderPass, SpriteRenderer* renderer,
                    const Matrix4& transform, int depth) {
	Vector2 pos = absolutePosition();
	Box2 absBox(pos, pos + _box.sizes());
	_frame.render(renderPass, renderer, transform, absBox, depth);

	depth += 1;
	for(Widget* child: _children)
		depth = child->render(renderPass, renderer, transform, depth);

	return depth;
}
