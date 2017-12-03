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


#include "widget.h"

#include "gui.h"

using namespace lair;

Gui::Gui(AssetManager* assets,
         LoaderManager* loader,
         SpriteRenderer* spriteRenderer)
    : _assets(assets)
    , _loader(loader)
    , _spriteRenderer(spriteRenderer)
{
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

AssetManager* Gui::assets() {
	return _assets;
}

LoaderManager* Gui::loader() {
	return _loader;
}

SpriteRenderer* Gui::spriteRenderer() {
	return _spriteRenderer;
}
