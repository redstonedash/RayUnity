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
SYSTEM(RecalculateModelMatrix, EcsOnUpdate, Model, Transform);\
SYSTEM(DrawTranslationGizmo3D, EcsManual, Transform, selected);\
SYSTEM(DrawTranslationGizmo2D, EcsManual, Transform, selected);\
SYSTEM(UpdateTranslationGizmo2D, EcsOnUpdate, Transform, selected);\
//SYSTEM(Spin, EcsOnUpdate, Transform);\



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
	ecs_run(gameState, FDrawTranslationGizmo2D, GetFrameTime(), nullptr);
	BeginMode3D(c);
	ecs_run(gameState, FRenderModels, GetFrameTime(), nullptr);
	ecs_run(gameState, FDrawTranslationGizmo3D, GetFrameTime(), nullptr);
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

void Spin(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	trs[0].rotation = QuaternionMultiply(QuaternionFromEuler(2.0f*GetFrameTime(), 0, 0), trs[0].rotation);
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
	textFieldCounter++;
	
}

void DrawTransformInspector(ecs_rows_t * rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	textFieldCounter = 0;
	
	((Vector2*)rows->param)->y += 24.0f;
	DrawTextField({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y, 100,24 }, "%f", &trs[0].translation.x);
	((Vector2*)rows->param)->x += 100.0f;
	DrawTextField({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y, 100,24 }, "%f", &trs[0].translation.y);
	((Vector2*)rows->param)->x += 100.0f;
	DrawTextField({ ((Vector2*)rows->param)->x, ((Vector2*)rows->param)->y, 100,24 }, "%f", &trs[0].translation.z);
	((Vector2*)rows->param)->x -= 200.0f; //return
}

void DrawTranslationGizmo3D(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	Vector3 upPos = Vector3Add(Vector3Multiply(Vector3Transform({ 0,1,0 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 rightPos = Vector3Add(Vector3Multiply(Vector3Transform({ 1,0,0 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 forwardPos = Vector3Add(Vector3Multiply(Vector3Transform({ 0,0,1 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	DrawLine3D(upPos, trs[0].translation, GREEN);
	DrawLine3D(rightPos, trs[0].translation, RED);
	DrawLine3D(forwardPos, trs[0].translation, BLUE);
	//DrawRay({ Vector3Multiply(Vector3RotateByQuaternion({ 0,1,0 }, trs[0].rotation), -1), trs[0].translation }, GREEN);
}
enum SelectedGizmo { Null, Up, Right, Forward };
SelectedGizmo sg = Null;
void DrawTranslationGizmo2D(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	Vector3 upPos = Vector3Add(Vector3Multiply(Vector3Transform({ 0,1,0 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 rightPos = Vector3Add(Vector3Multiply(Vector3Transform({ 1,0,0 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 forwardPos = Vector3Add(Vector3Multiply(Vector3Transform({ 0,0,1 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);

	if (sg == Up) {
		DrawCircle(
			GetWorldToScreen(upPos, c).x,
			GetWorldToScreen(upPos, c).y, 15, WHITE);
	}
	else {
		DrawCircle(
			GetWorldToScreen(upPos, c).x,
			GetWorldToScreen(upPos, c).y, 15, GREEN);
	}

	if (sg == Right) {
		DrawCircle(
			GetWorldToScreen(rightPos, c).x,
			GetWorldToScreen(rightPos, c).y, 15, WHITE);
	}
	else {
		DrawCircle(
			GetWorldToScreen(rightPos, c).x,
			GetWorldToScreen(rightPos, c).y, 15, RED);
	}

	if (sg == Forward) {
		DrawCircle(
			GetWorldToScreen(forwardPos, c).x,
			GetWorldToScreen(forwardPos, c).y, 15, WHITE);
	}
	else {
		DrawCircle(
			GetWorldToScreen(forwardPos, c).x,
			GetWorldToScreen(forwardPos, c).y, 15, BLUE);
	}

	//DrawRay({ Vector3Multiply(Vector3RotateByQuaternion({ 0,1,0 }, trs[0].rotation), -1), trs[0].translation }, GREEN);
}
void UpdateTranslationGizmo2D(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Transform, trs, 1);
	Vector3 upPos		=	Vector3Add(Vector3Multiply(Vector3Transform({ 0,1,0 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 rightPos	=	Vector3Add(Vector3Multiply(Vector3Transform({ 1,0,0 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 forwardPos	=	Vector3Add(Vector3Multiply(Vector3Transform({ 0,0,1 }, QuaternionToMatrix(trs[0].rotation)), 2), trs[0].translation);
	Vector3 up		=	Vector3Transform({ 0,1,0 }, QuaternionToMatrix(trs[0].rotation));
	Vector3 right	=	Vector3Transform({ 1,0,0 }, QuaternionToMatrix(trs[0].rotation));
	Vector3 forward =	Vector3Transform({ 0,0,1 }, QuaternionToMatrix(trs[0].rotation));
	Vector2 up2d = Vector2Subtract(GetWorldToScreen(upPos, c), GetWorldToScreen(trs[0].translation, c));
	Vector2 right2d = Vector2Subtract(GetWorldToScreen(rightPos, c), GetWorldToScreen(trs[0].translation, c));
	Vector2 forward2d = Vector2Subtract(GetWorldToScreen(forwardPos, c), GetWorldToScreen(trs[0].translation, c));
	if (sg != Null && !inputManager.leftMouseButton) {
		sg = Null;
	}
	switch (sg)
	{
	case Up:
		trs[0].translation=Vector3Add(trs[0].translation, Vector3Multiply(up, Vector2DotProduct(up2d, inputManager.mouseDelta)/10000));
		break;
	case Right:
		trs[0].translation = Vector3Add(trs[0].translation, Vector3Multiply(right, Vector2DotProduct(right2d, inputManager.mouseDelta) / 10000));
		break;
	case Forward:
		trs[0].translation = Vector3Add(trs[0].translation, Vector3Multiply(forward, Vector2DotProduct(forward2d, inputManager.mouseDelta) / 10000));
		break;
	case Null:
		if (CheckCollisionPointCircle(inputManager.mousePos, GetWorldToScreen(upPos, c), 15)) {
			if (inputManager.leftMouseButton) {
				sg = Up;
				inputManager.leftMouseButton = false;
			}
		}
		if (CheckCollisionPointCircle(inputManager.mousePos, GetWorldToScreen(rightPos, c), 15)) {
			if (inputManager.leftMouseButton) {
				sg = Right;
				inputManager.leftMouseButton = false;
			}
		}

		if (CheckCollisionPointCircle(inputManager.mousePos, GetWorldToScreen(forwardPos, c), 15)) {
			if (inputManager.leftMouseButton) {
				sg = Forward;
				inputManager.leftMouseButton = false;
			}
		}
		break;
	}	
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
	
	ecs_set(gameState, testObject, Transform, { {0.0f,0.0f,0.0f}, QuaternionFromEuler(0,0,0), {1.0f,1.0f,1.0f} });
}