#pragma once
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
#ifndef FLECS_H
#include"flecs.h"
#endif // !FLECS_H
#include <fstream>
#ifndef RAYGUI_H
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_RICONS
#include "raygui.h"
#endif // !RAYGUI_H
#include <list>
#include <iterator>
#define SYSTEMS \
SYSTEM(UpdateWindows, EcsOnUpdate, Window)\
SYSTEM(RenderWindows, EcsManual, Window)\
SYSTEM(InitializeWindows, EcsOnAdd, Window)

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
	ecs_run(gameState, FRenderWindows, GetFrameTime(), nullptr);
	BeginMode3D(c);

	//TODO call system to render 3d objects

	EndMode3D();
}

std::list<size_t> windowQue = std::list<size_t>();
void UpdateWindows(ecs_rows_t *rows) {
	std::list<size_t>::reverse_iterator j;
	ECS_COLUMN(rows, Window, window, 1);
	for (j = windowQue.rbegin();
		j != windowQue.rend();) {
		int k = 0;
		for (; k < rows->count; k++) {
			if (rows->entities[k] == *j) {
				break;
			}
		}
		if ((CheckCollisionPointRec(inputManager.mousePos, { window[k].rect.x,window[k].rect.y,window[k].rect.width,24 }) && inputManager.leftMouseButton) ||
			(CheckCollisionPointRec(inputManager.lastMousePos, { window[k].rect.x,window[k].rect.y,window[k].rect.width,24 }) && inputManager.leftMouseButton)) {
			window[k].rect.x += inputManager.mouseDelta.x;
			window[k].rect.y += inputManager.mouseDelta.y;
			inputManager.leftMouseButton = false; //consume mouse input
			if (j != windowQue.rbegin()) {
				size_t temp = *j;

				auto tempI = j;
				tempI++;

				std::list<size_t>::iterator test;
				for (test = windowQue.begin(); test != windowQue.end(); test++) {
					printf("%d, ", *test);
				}
				printf("\n");
				windowQue.erase(j.base()--);
				j = tempI;
				for (test = windowQue.begin(); test != windowQue.end(); test++) {
					printf("%d, ", *test);
				}
				printf("\n");
				windowQue.push_back(temp);
				for (test = windowQue.begin(); test != windowQue.end(); test++) {
					printf("%d, ", *test);
				}
				printf("\n");
				printf("\n");
			}
		}
		if (j == windowQue.rend()) break;
		j++;
		//if(temp)
	}
}

void RenderWindows(ecs_rows_t *rows) {
	std::list<size_t>::iterator j; 
	ECS_COLUMN(rows, Window, window, 1);
	for (j = windowQue.begin(); 
		j != windowQue.end();) {
		int k = 0;
		for (; k < rows->count; k++) {
			if (rows->entities[k] == *j) {
				break;
			}
		}
		if (GuiWindowBox(window[k].rect, window[k].title)) {
			ecs_delete(rows->world, rows->entities[k]);
		}
		if (j == windowQue.end()) break;
		j++;
		//if(temp)
	}
}

void InitializeWindows(ecs_rows_t *rows)
{
	ECS_COLUMN(rows, Window, w, 1);
	for(int i = 0; i < rows->count; i++){
		windowQue.push_back(rows->entities[i]);
		char * testdeletethis = new char[8];
		sprintf(testdeletethis, "%d", rows->entities[i]);
		w->title = testdeletethis;
		w->rect = { inputManager.mousePos.x,inputManager.mousePos.y, 250, 24 };
	}
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
	if (inputManager.space) { //TODO handle this AFTER update so if entities consume inputs we don't make editor actions off of them

		ecs_entity_t window  = ecs_new(gameState, Window);
		//ecs_set(gameState, window, Window, { {inputManager.mousePos.x,inputManager.mousePos.y, 250, 24}, "DEFAULT WINDOW" });
	}
}
void RegisterInitialEntities() {

}