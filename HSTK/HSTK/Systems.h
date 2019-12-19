#pragma once
#define SYSTEMS_H
#ifndef COMPONENTS_H
#include "Components.h"
#endif // !COMPONENTS_H
#ifndef _IOSTREAM_
#include <iostream>
#endif
#ifndef  RAYLIB_H
#include "raylib.h"
#include "raymath.h"
#endif // ! RAYLIB_H
#ifndef RAYGUI_H
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_RICONS
#include "raygui.h"
#endif // !RAYGUI_H
#ifndef FLECS_H
#include"flecs.h"
#endif // !FLECS_H
#include <fstream>
#include <list>
#include <iterator>
//SYSTEM SIGNATURES HERE
#define SYSTEMS \
SYSTEM(DrawTransformInspector, EcsManual, Transform, selected);

#define SYSTEM(id, kind, ...) ecs_entity_t F##id;\

SYSTEMS

#define SYSTEM(id, kind, ...) ecs_type_t ecs_type(id);

SYSTEMS

Camera c;
ecs_world * gameState = nullptr;
template<typename T> T *addrOf(T &&v) { return &v; }
#undef ecs_set

#define ecs_set(world, entity, component, ...)\
    _ecs_set_ptr(world, entity, ecs_entity(component), sizeof(component), addrOf(component __VA_ARGS__))

void Render() {
	//ECS_COLUMN(rows, Model, model, 1);
	BeginMode3D(c);

	//TODO call system to render 3d objects

	EndMode3D();
}
const size_t SMALL_TEXT_FIELD_LENGTH = 32;
char smallTextFieldBuffer[32];
void DrawTransformInspector(ecs_rows_t * rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	snprintf(smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, "%f", trs[0].translation.x);
	GuiTextBox({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y+24.0f, 100,24 }, smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, true);
}

#define SYSTEM(id, kind, ...) F##id = ecs_new_system(gameState, #id, kind, #__VA_ARGS__, id);\
    ecs_type(id) = ecs_type_from_entity(gameState, F##id);\

void RegisterAllSystems() {

	SYSTEMS

}

void UpdateInput() { //FIRST CLASS CITIZEN INPUT SYSTEM

	inputManager.space = IsKeyPressed(KEY_SPACE);
	inputManager.leftMouseButton = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	inputManager.lastMousePos = inputManager.mousePos;
	inputManager.mousePos = GetMousePosition();
	inputManager.mouseDelta = { inputManager.mousePos.x - inputManager.lastMousePos.x,inputManager.mousePos.y - inputManager.lastMousePos.y };
}
void RegisterInitialEntities() {
	ECS_ENTITY(gameState, testObject, Transform, renderable, Model, selected);
	ecs_set(gameState, testObject, Transform, { {0.5f,0.1f,0.2f}, Quaternion(), Vector3() });
}