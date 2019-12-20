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
#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#endif // !RAYGUI_H

#define GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
#include "gui_textbox_extended.h"
#ifndef FLECS_H
#include"flecs.h"
#endif // !FLECS_H
#include <fstream>
#include <list>
#include <iterator>
#include <cstdarg>
//SYSTEM SIGNATURES HERE
#define SYSTEMS \
SYSTEM(DrawTransformInspector, EcsManual, Transform, selected);\
SYSTEM(RenderModels, EcsManual, Model, renderable);\
SYSTEM(RecalculateModelMatrix, EcsOnUpdate, Model, Transform); //TODO hay it would be really nice if you added a "Transform changed" flag

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

#define ecs_set_ref(world, entity, component, ...)\
    _ecs_set_ptr(world, entity, ecs_entity(component), sizeof(component), &__VA_ARGS__)

void Render() {
	//ECS_COLUMN(rows, Model, model, 1);
	BeginMode3D(c);
	ecs_run(gameState, FRenderModels, GetFrameTime(), nullptr);
	//TODO call system to render 3d objects

	EndMode3D();
}

void RecalculateModelMatrix(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Model, mdl, 1);
	ECS_COLUMN(rows, Transform, trs, 2);
	for (int i = 0; i < rows->count; i++)
	{
		auto rot = QuaternionToMatrix(trs[i].rotation);
		auto trans = MatrixTranslate(trs[i].translation.x, trs[i].translation.y, trs[i].translation.z);
		auto scale = MatrixScale(trs[i].scale.x, trs[i].scale.y, trs[i].scale.z);
		mdl[i].transform = MatrixMultiply(MatrixMultiply(scale, rot), trans);
	}
}

void RenderModels(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Model, mdl, 1);
	for (int i = 0; i < rows->count; i++) {
		DrawModel(mdl[i], { 0 }, 1.0f, PURPLE);
	}
}

const size_t SMALL_TEXT_FIELD_LENGTH = 32;
char smallTextFieldBuffer[32]; //move this to gui file
char smallTextFieldInputBuffer[32]; //move this to gui file
int textFieldCounter = 0;
int selectedTextField = -1;
template <typename T>
void DrawTextField(Rectangle rect, const char* formatOptions, T ptrToThing) {
	textFieldCounter++;
	if (inputManager.leftMouseButton && CheckCollisionPointRec(inputManager.mousePos, rect)) {
		inputManager.leftMouseButton = false;
		selectedTextField = textFieldCounter;
		snprintf(smallTextFieldInputBuffer, SMALL_TEXT_FIELD_LENGTH, formatOptions, *ptrToThing);
	}
	else if (inputManager.leftMouseButton && textFieldCounter == selectedTextField) {
		inputManager.leftMouseButton = false;
		selectedTextField = -1;
	}

	if (textFieldCounter == selectedTextField) {
		GuiTextBox(rect, smallTextFieldInputBuffer, SMALL_TEXT_FIELD_LENGTH, true);
		if (inputManager.enter) {
			sscanf(smallTextFieldInputBuffer, formatOptions, ptrToThing);
			inputManager.enter = false;
			selectedTextField = -1;
		}
	} else {
		snprintf(smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, formatOptions, *ptrToThing);
		GuiTextBox(rect, smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, false);
	}
	
}

void DrawTransformInspector(ecs_rows_t * rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	textFieldCounter = 0;
	
	((Vector2*)rows->param)->y += 24.0f;
	DrawTextField({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y, 100,24 }, "%f", &trs[0].translation.x);
	((Vector2*)rows->param)->x += 100.0f;
	snprintf(smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, "%f", trs[0].translation.y);
	GuiTextBox({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y, 100,24 }, smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, true);
	((Vector2*)rows->param)->x += 100.0f;
	snprintf(smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, "%f", trs[0].translation.z);
	GuiTextBox({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y, 100,24 }, smallTextFieldBuffer, SMALL_TEXT_FIELD_LENGTH, true);
	((Vector2*)rows->param)->x -= 200.0f; //return
}

#define SYSTEM(id, kind, ...) F##id = ecs_new_system(gameState, #id, kind, #__VA_ARGS__, id);\
    ecs_type(id) = ecs_type_from_entity(gameState, F##id);\

void RegisterAllSystems() {

	SYSTEMS

}



void UpdateInput() { //FIRST CLASS CITIZEN INPUT SYSTEM
	inputManager.enter = IsKeyPressed(KEY_ENTER);
	inputManager.space = IsKeyPressed(KEY_SPACE);
	inputManager.leftMouseButton = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	inputManager.lastMousePos = inputManager.mousePos;
	inputManager.mousePos = GetMousePosition();
	inputManager.mouseDelta = { inputManager.mousePos.x - inputManager.lastMousePos.x,inputManager.mousePos.y - inputManager.lastMousePos.y };
}
void RegisterInitialEntities() {
	ECS_ENTITY(gameState, testObject, Transform, renderable, Model, selected);
	auto temp = LoadModelFromMesh(GenMeshCube(1, 1, 1));
	ecs_set_ref(gameState, testObject, Model, temp);
	ecs_set(gameState, testObject, Transform, { {1.0f,0.0f,0.0f}, QuaternionIdentity(), {1.0f,1.0f,1.0f} });
}