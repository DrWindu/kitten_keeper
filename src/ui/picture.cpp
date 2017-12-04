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

#include "picture.h"


using namespace lair;

Picture::Picture(Gui* gui, Widget* parent)
    : Widget(gui, parent)
    , _color(Vector4::Constant(1))
    , _blendingMode(BLEND_ALPHA)
    , _textureFlags(Texture::BILINEAR_NO_MIPMAP)
{
}

void Picture::setPictureColor(const Vector4& color) {
	_color = color;
}

void Picture::setTexture(TextureAspectSP texture) {
	_texture = texture;
}

void Picture::setTexture(AssetSP texture) {
	if(texture) {
		TextureAspectSP ta = texture->aspect<TextureAspect>();
		if(!ta) {
			ta = _gui->spriteRenderer()->createTexture(texture);
		}
		setTexture(ta);
	}
	else {
		setTexture(TextureAspectSP());
	}
}

void Picture::setTexture(const Path& logicPath) {
	AssetSP asset = _gui->loader()->loadAsset<ImageLoader>(logicPath);
	setTexture(asset);
}

void Picture::setBlendingMode(BlendingMode blendingMode) {
	_blendingMode = blendingMode;
}

void Picture::setTextureFlags(unsigned flags) {
	_textureFlags = flags;
}

void Picture::resizeToPicture() {
	TextureAspectSP texture = _texture.lock();
	ImageAspectSP image;
	if(texture)
		image = texture->asset()->aspect<ImageAspect>();
	if(image) {
		resize(Vector2(image->get().width(), image->get().height())
		       + _marginMin + _marginMax);
	}
}

float Picture::render(RenderPass& renderPass, SpriteRenderer* renderer,
                    const Matrix4& transform, float depth) {
	if(!_enabled)
		return depth;

	depth = renderFrame(renderPass, renderer, transform, depth);

	TextureAspectSP texAspect = _texture.lock();
	if(texAspect) {
		const Texture& tex = texAspect->get();

		Matrix4 trans = Matrix4::Identity();

		Vector2 pos = absolutePosition();
		Box2 box(pos + _marginMin, pos + size() - _marginMax);

		unsigned index = renderer->indexCount();
		renderer->addSprite(trans, box, _color, Box2(Vector2(0, 0), Vector2(1, 1)));
		unsigned count = renderer->indexCount() - index;

		if(count) {
			RenderPass::DrawStates states;
			states.shader       = renderer->shader().shader;
			states.buffer       = renderer->buffer();
			states.format       = renderer->format();
			states.texture      = &texAspect->_get();
			states.textureFlags = _textureFlags;
			states.blendingMode = _blendingMode;

			Vector4i tileInfo(1, 1, tex.width(), tex.height());
			const ShaderParameter* params = renderer->addShaderParameters(
			            renderer->shader(), transform, 0, tileInfo);

			renderPass.addDrawCall(states, params, 1 - depth, index, count);
		}

		depth += 1e-5;
	}

	return renderChildren(renderPass, renderer, transform, depth);
}
