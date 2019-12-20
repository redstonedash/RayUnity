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
#include <string>
void UpdateInspectorWindow();
void DrawInspectorWindow();
void TranslateGizmo();
struct Gizmo {
	void* data = nullptr; //not type safe but whatever
	void(*drawGizmo)() = nullptr;
	void(*updateGizmo)() = nullptr;
};

std::list<Gizmo>::iterator i;

struct Inspector { //lmao this dosn't need to be a window
	Vector2 pos = { 0,0 };
};

std::list<Gizmo> gizmos = std::list<Gizmo>(); //lmao it's not the transform gizmo though
const float WINDOW_WIDTH = 75.0f, WINDOW_HEIGHT = 24.0f;
void MakeNewInspector(float x, float y) {
	Inspector * insp = new Inspector{ { x,y } };
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
		if (CheckCollisionPointRec(inputManager.mousePos, { ((Inspector *)(*i).data)->pos.x,((Inspector *)(*i).data)->pos.y, WINDOW_WIDTH, WINDOW_HEIGHT } )|| CheckCollisionPointRec(inputManager.lastMousePos, { ((Inspector *)(*i).data)->pos.x,((Inspector *)(*i).data)->pos.y, WINDOW_WIDTH, WINDOW_HEIGHT }))
		{
			inputManager.leftMouseButton = false; //consume input;
			float difx, dify;
			difx = ((Inspector *)(*i).data)->pos.x - inputManager.lastMousePos.x;
			dify = ((Inspector *)(*i).data)->pos.y - inputManager.lastMousePos.y;
			((Inspector *)(*i).data)->pos.x = inputManager.mousePos.x+difx;
			((Inspector *)(*i).data)->pos.y = inputManager.mousePos.y+dify;
			auto tempI = i;
			gizmos.splice(gizmos.end(), gizmos, tempI);
			
		}
		
	}
}
void DrawInspectorWindow() {

	if (GuiWindowBox({ ((Inspector *)(*i).data)->pos.x,((Inspector *)(*i).data)->pos.y, WINDOW_WIDTH, WINDOW_HEIGHT }, "Inspector")) { //unfortunatly this needs to be inside of draw
		delete((*i).data); //dealocate data cause inspector windows use new data
		gizmos.erase(i++); //remove from list	
	} else {
		Vector2 pos = ((Inspector *)(*i).data)->pos; //just done so the pointers arn't the same
		ecs_run(gameState, FDrawTransformInspector, GetFrameTime(), &pos);
	}
}

inline void TranslateGizmo()
{

}

