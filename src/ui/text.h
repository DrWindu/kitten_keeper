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


#ifndef LD_40_UI_TEXT_H_
#define LD_40_UI_TEXT_H_

#include <vector>

#include <lair/core/lair.h>
#include <lair/core/shapes.h>
#include <lair/core/bitmap_font.h>

#include <lair/render_gl2/texture.h>

#include <lair/ec/sprite_renderer.h>
#include <lair/ec/bitmap_text_component.h>

class Text {
public:
	Text(lair::BitmapFontAspectSP font = nullptr,
	     const lair::Vector4& color = lair::Vector4::Constant(1));

	lair::BitmapFontAspectSP font() const;
	const lair::Vector4& color() const;
	const lair::Vector2& anchor() const;
	lair::BlendingMode blendingMode() const;
	unsigned textureFlags() const;

	void setFont(lair::BitmapFontAspectSP font);
	void setColor(const lair::Vector4& color);
	void setAnchor(const lair::Vector2& anchor);
	void setBlendingMode(lair::BlendingMode blendingMode);
	void setTextureFlags(unsigned textureFlags);

	void preRender(lair::SpriteRenderer* renderer);
	void render(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
	            const lair::String& text, int width, const lair::Vector2& position,
	            float depth, const lair::Matrix4& viewTransform);

protected:
	lair::BitmapFontAspectWP _font;
	lair::TextureAspectWP    _texture;
	lair::Vector4            _color;
	lair::TextLayout         _layout;
	lair::Vector2            _anchor;
	lair::BlendingMode       _blendingMode;
	unsigned                 _textureFlags;
};


#endif
