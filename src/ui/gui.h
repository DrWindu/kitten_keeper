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


#ifndef LD_40_UI_GUI_H_
#define LD_40_UI_GUI_H_

#include <lair/core/lair.h>
#include <lair/core/asset_manager.h>
#include <lair/core/loader.h>

#include <lair/render_gl2/render_pass.h>

#include <lair/ec/sprite_renderer.h>

#include "widget.h"

class Gui {
public:
	Gui(lair::AssetManager* assets,
	    lair::LoaderManager* loader,
	    lair::SpriteRenderer* spriteRenderer);
	Gui(const Gui& ) = delete;
	Gui(      Gui&&) = delete;
	~Gui();

	template<typename T>
	T* createWidget() {
		T* widget = new T(this);
		addWidget(widget);
		return widget;
	}

	void addWidget(Widget* widget);
	void removeWidget(Widget* widget);

	void preRender();
	void render(lair::RenderPass& renderPass, const lair::Matrix4& transform);

	lair::AssetManager* assets();
	lair::LoaderManager* loader();
	lair::SpriteRenderer* spriteRenderer();

protected:
	lair::AssetManager*    _assets;
	lair::LoaderManager*   _loader;
	lair::SpriteRenderer*  _spriteRenderer;

	WidgetVector _widgets;
};


#endif
