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

#include "text.h"


using namespace lair;


Text::Text(BitmapFontAspectSP font, const Vector4& color)
    : _font(font)
    , _color(color)
    , _layout()
    , _anchor(0, 1)
    , _blendingMode(BLEND_ALPHA)
    , _textureFlags(Texture::BILINEAR_NO_MIPMAP)
{
}

BitmapFontAspectSP Text::font() const {
	return _font.lock();
}

const Vector4& Text::color() const {
	return _color;
}

const Vector2& Text::anchor() const {
	return _anchor;
}

BlendingMode Text::blendingMode() const {
	return _blendingMode;
}

unsigned Text::textureFlags() const {
	return _textureFlags;
}

lair::Vector2 Text::textSize(const lair::String& text, int width) const {
	BitmapFontAspectSP fontAspect = this->font();
	if(!fontAspect)
		return Vector2(0, 0);
	const BitmapFont& font = fontAspect->get();

	TextLayout layout = font.layoutText(text, width);
	return layout.box().sizes();
}

void Text::setFont(lair::BitmapFontAspectSP font) {
	_font = font;
}

void Text::setColor(const Vector4& color) {
	_color = color;
}

void Text::setAnchor(const Vector2& anchor) {
	_anchor = anchor;
}

void Text::setBlendingMode(BlendingMode blendingMode) {
	_blendingMode = blendingMode;
}

void Text::setTextureFlags(unsigned textureFlags) {
	_textureFlags = textureFlags;
}

void Text::preRender(lair::SpriteRenderer* renderer) {
	BitmapFontAspectSP font = _font.lock();
	if(font && !_texture.lock() && font->isValid())
		_texture = renderer->createTexture(font->get().image());
}

void Text::render(RenderPass& renderPass, SpriteRenderer* renderer,
                  const lair::String& text, int width, const Vector2& position,
                  float depth, const Matrix4& viewTransform) {
	BitmapFontAspectSP fontAspect = this->font();
	if(!fontAspect)
		return;

	const BitmapFont& font = fontAspect->get();
	if(!font.image())
		return;

	TextureAspectSP texAspect = _texture.lock();
	if(!texAspect) {
		dbgLogger.warning("Texture ", font.image()->logicPath(), " not found.");
		texAspect = renderer->defaultTexture();
	}

	Matrix4 transform = Matrix4::Identity();
	transform.topRightCorner<2, 1>() = position;
	transform(1, 3) -= (font.height() - font.glyph('O').size(1)) / 2;
	transform(1, 3) = std::round(transform(1, 3));

	TextLayout layout = font.layoutText(text, width);

	renderBitmapText(&renderPass, renderer, font, &texAspect->_get(), transform, 1 - depth,
	                 layout, _anchor, _color, viewTransform, _textureFlags, _blendingMode);
}
