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

#include "label.h"


using namespace lair;

Label::Label(Gui* gui, Widget* parent)
    : Widget(gui, parent)
{
}

const lair::String& Label::text() const
{
	return _text;
}

const Text& Label::textInfo() const
{
	return _textInfo;
}

Text& Label::textInfo() {
	return _textInfo;
}

void Label::setText(const String& text) {
	_text = text;
}

void Label::setFont(lair::BitmapFontAspectSP font) {
	_textInfo.setFont(font);
}

void Label::setFont(lair::AssetSP font) {
	BitmapFontAspectSP fa;
	if(font) {
		fa = font->aspect<BitmapFontAspect>();
	}
	setFont(fa);
}

void Label::setFont(const lair::Path& logicPath)
{
	AssetSP asset = gui()->loader()->loadAsset<BitmapFontLoader>(logicPath);
	setFont(asset);
}

void Label::preRender(lair::SpriteRenderer* renderer) {
	_textInfo.preRender(renderer);

	Widget::preRender(renderer);
}

float Label::render(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
                    const lair::Matrix4& transform, float depth) {
	if(!_enabled)
		return depth;

	depth = renderFrame(renderPass, renderer, transform, depth);

	Vector2 position = absolutePosition();
	position(1) += size()(1);
	_textInfo.render(renderPass, renderer, _text, size()(0), position,
	                 depth, transform);

	depth += 1e-5;
	return renderChildren(renderPass, renderer, transform, depth);
}
