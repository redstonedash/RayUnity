#pragma once
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
#ifndef SYSTEMS_H
#include "Systems.h"
#endif // !SYSTEMS_H
#include <iostream>
#include <list>
ecs_entity_t selectedEntity;
void UpdateInspectorWindow();
void DrawInspectorWindow();
void TranslateGizmo();
struct Gizmo {
	void* data = nullptr; //not type safe but whatever
	void(*drawGizmo)() = nullptr;
	void(*updateGizmo)() = nullptr;
};

std::list<Gizmo>::iterator i;

struct Inspector {
	Rectangle rect = { 0,0 };
};

std::list<Gizmo> gizmos = std::list<Gizmo>();

void MakeNewInspector(float x, float y) {
	Inspector * insp = new Inspector{ { x,y,250,250 } };
	gizmos.push_back({ insp, DrawInspectorWindow, UpdateInspectorWindow });
}
void UpdateEditor() {
	if (!gizmos.empty()) {
		i = --gizmos.end();
		while(true)
		{		
			if ((*i).updateGizmo != nullptr) {
				(*i).updateGizmo(); //this is how the windows are drawn, assigning different functions to window behaviour will give you different values
			}
			if (i != gizmos.begin() && !gizmos.empty()) {
				i--;
			}
			else {
				break;
			}
		}
	}
	if (inputManager.space) {
		inputManager.space = false;
		MakeNewInspector(inputManager.mousePos.x, inputManager.mousePos.y);
		
	}
}

void DrawEditor() {
	for (i = gizmos.begin(); i !=gizmos.end(); i++) {
		if ((*i).drawGizmo != nullptr) {
			(*i).drawGizmo(); //this is how the windows are drawn, assigning different functions to window behaviour will give you different values
		}
		if (i == gizmos.end()) {
			break;
		}
	}
}


void UpdateInspectorWindow() {
	if (inputManager.leftMouseButton) {
		if (CheckCollisionPointRec(inputManager.mousePos, ((Inspector *)(*i).data)->rect) || CheckCollisionPointRec(inputManager.lastMousePos, ((Inspector *)(*i).data)->rect))
		{
			inputManager.leftMouseButton = false; //consume input;
			float difx, dify;
			difx = ((Inspector *)(*i).data)->rect.x - inputManager.lastMousePos.x;
			dify = ((Inspector *)(*i).data)->rect.y - inputManager.lastMousePos.y;
			((Inspector *)(*i).data)->rect.x = inputManager.mousePos.x+difx;
			((Inspector *)(*i).data)->rect.y = inputManager.mousePos.y+dify;
			auto tempI = i;
			gizmos.splice(gizmos.end(), gizmos, tempI);
			
		}
		
	}
}
void DrawInspectorWindow() {

	if (GuiWindowBox(((Inspector *)(*i).data)->rect, "Inspector")) { //unfortunatly this needs to be inside of draw
		delete((*i).data); //dealocate data cause inspector windows use new data
		gizmos.erase(i++); //remove from list
	}
	ecs_run(gameState, FDrawTransformInspector, GetFrameTime(), addrOf(Vector2{ ((Inspector *)(*i).data)->rect.x, ((Inspector *)(*i).data)->rect.y }));
}

inline void TranslateGizmo()
{

}

