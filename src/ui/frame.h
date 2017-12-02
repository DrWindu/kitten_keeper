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


#ifndef LD_40_UI_FRAME_H_
#define LD_40_UI_FRAME_H_

#include <vector>

#include <lair/core/lair.h>
#include <lair/core/shapes.h>

#include <lair/render_gl2/texture.h>

#include <lair/ec/sprite_renderer.h>

class Frame;

class Frame {
public:
	Frame(lair::TextureAspectSP texture = nullptr,
	      const lair::Vector4& color = lair::Vector4::Constant(1));

	lair::TextureAspectSP texture() const;
	const lair::Vector4& color() const;

	void setTexture(lair::TextureAspectSP texture);
	void setColor(const lair::Vector4& color);

	virtual void render(lair::RenderPass& renderPass, lair::SpriteRenderer* renderer,
	                    const lair::Matrix4& transform, const lair::Box2& box, int depth);

protected:
	lair::TextureAspectWP _texture;
	lair::Vector4         _color;
	lair::RenderPass::DrawStates _states;
};


#endif
