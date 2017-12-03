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


#include "main_state.h"
#include "level.h"

#include "game_view.h"


using namespace lair;

GameView::GameView(Gui* gui, Widget* parent)
    : Widget(gui, parent)
{
}

MainState* GameView::mainState() const {
	return _mainState;
}

void GameView::setMainState(MainState* mainState) {
	_mainState = mainState;
}

lair::Vector2 GameView::sceneFromScreen(lair::Vector2 screen) const {
	Vector4 normalized;
	normalized << screen.cwiseQuotient(_gui->logicScreenSize()) * 2 - Vector2(1, 1), 0, 1;
	Matrix4 viewTrans = _mainState->_camera.transform();
	return viewTrans.partialPivLu().solve(normalized).head<2>();
	return Vector2();
}

EntityRef GameView::grabEntity() {
	return _grabEntity;
}

void GameView::beginGrab(EntityRef& entity, const Vector2& scenePos) {
	if(_grabEntity.isValid()) {
		dbgLogger.error("Start dragging before the end of the previous drag ?");
		endGrab();
	}

	ToyComponent* toy = _mainState->_toys.get(entity);
	if(!toy) {
		dbgLogger.error("Try to drag an entity which is not a Toy.");
		return;
	}

	SpriteComponent* sprite = _mainState->_sprites.get(entity);
	if(!sprite) {
		dbgLogger.error("Try to drag an entity which is not a Sprite.");
		return;
	}

	_grabEntity = entity;

	toy->startState = toy->state;
	toy->startPos   = _grabEntity.position2();
	toy->state      = ToyComponent::DRAGGED;

	grabMouse();
	moveGrabbed(scenePos);
}

void GameView::moveGrabbed(const Vector2& scenePos) {
	if(!_grabEntity.isValid()) {
		dbgLogger.error("moveGrabbed: no grab entity");
		return;
	}

	ToyComponent* toy = _mainState->_toys.get(_grabEntity);
	SpriteComponent* sprite = _mainState->_sprites.get(_grabEntity);
	lairAssert(toy && sprite);

	float tileSize = 16;
	Vector2 size = toy->size.cast<float>() * tileSize;
	Vector2 pos = ((scenePos - size / 2) / tileSize).array().round().matrix() * tileSize;

	AlignedBox2 box(pos, pos + size);
	bool canPlace = !_mainState->_level->hitTest(box);

	sprite->setColor(canPlace? Vector4(.8, 1, .8, .5):
	                           Vector4(1.2, .8, .8, .5));

	_grabEntity.placeAt(pos);
}

void GameView::endGrab() {
	if(!_grabEntity.isValid()) {
		dbgLogger.error("endGrab: no grab entity");
		return;
	}

	ToyComponent* toy = _mainState->_toys.get(_grabEntity);
	SpriteComponent* sprite = _mainState->_sprites.get(_grabEntity);
	lairAssert(toy && sprite);

	float tileSize = 16;
	Vector2 size = toy->size.cast<float>() * tileSize;
	Vector2 pos = _grabEntity.position2();

	AlignedBox2 box(pos, pos + size);
	bool canPlace = !_mainState->_level->hitTest(box);

	if(canPlace) {
		toy->state = ToyComponent::PLACED;
		sprite->setColor(Vector4(1, 1, 1, 1));
	}
	else {
		// TODO: Some noise.
		return;
	}

	releaseMouse();
	_grabEntity.release();
}

void GameView::cancelGrab() {
	if(!_grabEntity.isValid()) {
		dbgLogger.error("cancelGrab: no grab entity");
		return;
	}

	ToyComponent* toy = _mainState->_toys.get(_grabEntity);
	SpriteComponent* sprite = _mainState->_sprites.get(_grabEntity);
	lairAssert(toy && sprite);

	if(toy->startState == ToyComponent::NONE) {
		_grabEntity.destroy();
	}
	else {
		toy->state = toy->startState;
		_grabEntity.placeAt(toy->startPos);
		sprite->setColor(Vector4(1, 1, 1, 1));
	}

	releaseMouse();
	_grabEntity.release();
}

void GameView::mousePressEvent(MouseEvent& event) {
	if(!_grabEntity.isValid()) {
		// TODO: picking.
	}

	event.reject();
}

void GameView::mouseReleaseEvent(MouseEvent& event) {
	if(_grabEntity.isValid()) {
		if(event.button() == MOUSE_LEFT) {
			endGrab();
			event.accept();
			return;
		}
		else if(event.button() == MOUSE_RIGHT) {
			cancelGrab();
			event.accept();
			return;
		}
	}
	event.reject();
}

void GameView::mouseMoveEvent(MouseEvent& event) {
	if(_grabEntity.isValid()) {
		moveGrabbed(sceneFromScreen(event.position()));
		event.accept();
		return;
	}

	event.reject();
}
