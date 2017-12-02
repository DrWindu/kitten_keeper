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

#include "frame.h"


using namespace lair;


Frame::Frame(lair::TextureAspectSP texture, const Vector4& color)
    : _texture(texture)
    , _color(color)
{
}

TextureAspectSP Frame::texture() const {
	return _texture.lock();
}

const Vector4& Frame::color() const {
	return _color;
}

void Frame::setTexture(TextureAspectSP texture) {
	_texture = texture;
}

void Frame::setColor(const Vector4& color) {
	_color = color;
}

void Frame::render(RenderPass& renderPass, SpriteRenderer* renderer,
                   const Matrix4& transform, const Box2& box, int depth) {
	TextureAspectSP texAspect = texture();
	if(texAspect) {
		if(!texAspect->isValid()) {
			// FIXME:
//			texAspect->warnIfInvalid(gui()->loader()->log());
			texAspect = renderer->defaultTexture();
		}

		const Texture& tex = texAspect->get();
		Vector2 tileSize = Vector2(tex.width(), tex.height()) / 3;

		Eigen::Array<bool, 2, 1> collapse = box.sizes().array() < tileSize.array() * 2;
		Vector2  offset(collapse(0)? box.sizes()(0): tileSize(0),
		                collapse(1)? box.sizes()(1): tileSize(1));

		Vector2 corner[4] = {
		    box.min(),
		    box.min() + offset,
		    box.max() - offset,
		    box.max(),
		};

		for(unsigned i = 0; i < 2; ++i) {
			if(collapse(i))
				corner[2](i) = corner[1](i);
		}

		unsigned firstVertex = renderer->vertexCount();
		for(unsigned y = 0; y < 4; ++y) {
			for(unsigned x = 0; x < 4; ++x) {
				Vector4 p = Vector4(corner[x](0), corner[y](1), 0, 1);
				Vector2 t(float(x) / 3.0f, float(y) / 3.0f);
				renderer->addVertex(p, _color, t);
			}
		}

		unsigned firstIndex = renderer->indexCount();
		for(unsigned y = 0; y < 3; ++y) {
			for(unsigned x = 0; x < 3; ++x) {
				unsigned i = firstVertex + 4 * y + x;
				renderer->addIndex(i + 0);
				renderer->addIndex(i + 1);
				renderer->addIndex(i + 4);
				renderer->addIndex(i + 4);
				renderer->addIndex(i + 1);
				renderer->addIndex(i + 5);
			}
		}
		unsigned indexCount = renderer->indexCount() - firstIndex;

		_states.shader       = renderer->shader().shader;
		_states.buffer       = renderer->buffer();
		_states.format       = renderer->format();
		_states.texture      = &texAspect->_get();
		_states.textureFlags = lair::Texture::BILINEAR_NO_MIPMAP;
		_states.blendingMode = lair::BLEND_ALPHA;

		Vector4i tileInfo;
		tileInfo << 1, 1, tex.width(), tex.height();
		const ShaderParameter* params = renderer->addShaderParameters(
		            renderer->shader(), transform, 0, tileInfo);

		renderPass.addDrawCall(_states, params, 1 - (depth / 1e6f), firstIndex, indexCount);
	}
}
