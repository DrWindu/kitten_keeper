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


#ifndef LD_40_UI_WIDGET_H_
#define LD_40_UI_WIDGET_H_

#include <vector>
#include <functional>

#include <lair/core/lair.h>
#include <lair/core/shapes.h>

#include <lair/render_gl2/texture.h>

#include <lair/ec/sprite_renderer.h>

#include "event.h"
#include "frame.h"

class Gui;
class Widget;
typedef std::vector<Widget*> WidgetVector;

class Widget {
public:
	Widget(Gui* gui, Widget* parent = nullptr);
	Widget(const Widget& ) = delete;
	Widget(      Widget&&) = delete;
	virtual ~Widget();

	Gui*          gui() const;
	Widget*       parent() const;
	const lair::String name() const;
	bool enabled() const;
	lair::Vector2 position() const;
	lair::Vector2 absolutePosition() const;
	lair::Vector2 size() const;
	lair::Box2 absoluteBox() const;
	lair::TextureAspectSP frameTexture() const;
	lair::Vector4 frameColor() const;

	unsigned nChildren() const;
	Widget* child(unsigned i) const;
	const WidgetVector& children() const;

	void setName(const lair::String& name);
	void setEnabled(bool enabled);
	void place(const lair::Vector2& position);
	void resize(const lair::Vector2& size);

	void setFrameTexture(lair::TextureAspectSP texture);
	void setFrameTexture(lair::AssetSP texture);
	void setFrameTexture(const lair::Path& logicPath);
	void setFrameColor(const lair::Vector4& color);

	template<typename T>
	T* createChild() {
		return new T(_gui, this);
	}

	void addChild(Widget* child);
	void removeChild(Widget* child);

	bool isInside(const lair::Vector2& position) const;
	Widget* widgetAt(const lair::Vector2& position);

	virtual void processEvent(Event& event);

	virtual void mousePressEvent(MouseEvent& event);
	virtual void mouseReleaseEvent(MouseEvent& event);
	virtual void mouseMoveEvent(MouseEvent& event);

	virtual void mouseEnterEvent(HoverEvent& event);
	virtual void mouseLeaveEvent(HoverEvent& event);

	virtual void resizeEvent(ResizeEvent& event);

	virtual void preRender(lair::SpriteRenderer* renderer);
	virtual float render(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
	                   const lair::Matrix4& transform, float depth = 0);

public:
	std::function<void(MouseEvent&)> onMouseDown;
	std::function<void(MouseEvent&)> onMouseUp;
	std::function<void(MouseEvent&)> onMouseMove;

	std::function<void(HoverEvent&)> onMouseEnter;
	std::function<void(HoverEvent&)> onMouseLeave;

	std::function<void(ResizeEvent&)> onResize;

protected:
	float renderFrame(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
	                  const lair::Matrix4& transform, float depth);
	float renderChildren(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
	                     const lair::Matrix4& transform, float depth);

protected:
	Gui*              _gui;
	Widget*           _parent;
	lair::String      _name;
	bool              _enabled;
	lair::AlignedBox2 _box;
	WidgetVector      _children;

	Frame _frame;

//	bool              _layoutDirty;
//	LayoutSP          _layout;
};


#endif
