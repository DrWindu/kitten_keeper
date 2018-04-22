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
    : _gui      (gui)
    , _parent   (nullptr)
    , _enabled  (true)
    , _box      (Vector2(0, 0), Vector2(128, 128))
    , _marginMin(Vector2(0, 0))
    , _marginMax(Vector2(0, 0))
    , _children ()
{
	lairAssert(_gui);

	if(parent) {
		parent->addChild(this);
	}
}

Widget::~Widget() {
	while(_children.size())
		delete _children.back();

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

const String Widget::name() const {
	return _name;
}

bool Widget::enabled() const {
	return _enabled;
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

TextureSetCSP Widget::frameTextureSet() const {
	return _frame.textureSet();
}

TextureAspectSP Widget::frameTexture() const {
	const TextureBinding* binding = _frame.textureSet()->get(TexColor);
	return binding? binding->texture: TextureAspectSP();
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

const WidgetVector& Widget::children() const {
	return _children;
}

void Widget::setName(const String& name) {
	_name = name;
}

void Widget::setEnabled(bool enabled) {
	_enabled = enabled;
}

void Widget::place(const lair::Vector2& position) {
	_box.max() = position + _box.sizes();
	_box.min() = position;
}

void Widget::resize(const Vector2& size) {
	_box.max() = _box.min() + size;
}

void Widget::setMargin(float margin) {
	_marginMin = Vector2(margin, margin);
	_marginMax = _marginMin;
}

void Widget::setMargin(float hMargin, float vMargin) {
	_marginMin = Vector2(hMargin, vMargin);
	_marginMax = _marginMin;
}

void Widget::setMargin(float top, float right, float bottom, float left) {
	_marginMin = Vector2(left, bottom);
	_marginMax = Vector2(right, top);
}

void Widget::setFrameTextureSet(TextureSetCSP textureSet) {
	_frame.setTextureSet(textureSet);
}

void Widget::setFrameTexture(lair::TextureAspectSP texture) {
	setFrameTextureSet(gui()->spriteRenderer()->getTextureSet(
	                       TexColor, texture, gui()->spriteRenderer()->defaultSampler()));
}

void Widget::setFrameTexture(AssetSP texture) {
	if(texture) {
		setFrameTexture(gui()->spriteRenderer()->createTexture(texture));
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

Box2 Widget::absoluteBox() const {
	Vector2 pos = absolutePosition();
	return Box2(pos, pos + _box.sizes());
}

bool Widget::isInside(const lair::Vector2& position) const {
	return absoluteBox().contains(position);
}

Widget* Widget::widgetAt(const Vector2& position) {
	Widget* found = nullptr;
	for(Widget* child: _children) {
		if(child->enabled()) {
			Widget* f = child->widgetAt(position);
			if(f) found = f;
		}
	}

	if(!found && isInside(position)) {
		found = this;
	}

	return found;
}

void Widget::grabMouse() {
	_gui->setMouseGrabWidget(this);
}

void Widget::releaseMouse() {
	_gui->setMouseGrabWidget(nullptr);
}

void Widget::processEvent(Event& event) {
	if(!_enabled)
		return;

//	dbgLogger.info(name(), ": ", event);

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

	case EVENT_HOVER: {
		HoverEvent& hEvent = static_cast<HoverEvent&>(event);
		switch(hEvent.hoverType()) {
		case HoverEvent::ENTER:
			mouseEnterEvent(hEvent);
			break;
		case HoverEvent::LEAVE:
			mouseLeaveEvent(hEvent);
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
	if(onMouseDown)
		onMouseDown(this, event);
	else
		event.reject();
}

void Widget::mouseReleaseEvent(MouseEvent& event) {
	if(onMouseUp)
		onMouseUp(this, event);
	else
		event.reject();
}

void Widget::mouseMoveEvent(MouseEvent& event) {
	if(onMouseMove)
		onMouseMove(this, event);
	else
		event.reject();
}

void Widget::mouseEnterEvent(HoverEvent& event) {
	if(onMouseEnter)
		onMouseEnter(this, event);
	else
		event.reject();
}

void Widget::mouseLeaveEvent(HoverEvent& event) {
	if(onMouseLeave)
		onMouseLeave(this, event);
	else
		event.reject();
}

void Widget::resizeEvent(ResizeEvent& event) {
	if(onResize)
		onResize(this, event);
	else
		event.reject();
}

void Widget::preRender(lair::SpriteRenderer* renderer) {
	for(Widget* child: _children)
		child->preRender(renderer);
}

float Widget::render(lair::RenderPass& renderPass, SpriteRenderer* renderer,
                    const Matrix4& transform, float depth) {
	if(!_enabled)
		return depth;

	depth = renderFrame(renderPass, renderer, transform, depth);
	return renderChildren(renderPass, renderer, transform, depth);
}

float Widget::renderFrame(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
                          const lair::Matrix4& transform, float depth) {
	_frame.render(renderPass, renderer, transform, absoluteBox(), depth);

	return depth + 1e-5;
}

float Widget::renderChildren(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
                             const lair::Matrix4& transform, float depth) {
	for(Widget* child: _children)
		depth = child->render(renderPass, renderer, transform, depth);

	return depth + 1e-5;
}
