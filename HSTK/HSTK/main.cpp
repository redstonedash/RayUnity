#include <iostream>
#include "raylib.h"
#include "raymath.h"
//#include "GetPointerToSharedMemory.h"
#include "flecs.h"
#include "Systems.h"
#include "EditorGUI.h"

//void * data;
//Matrix * matrixData;
//int matrixDataLength;

int width, height;
int main(int argc, char *argv[]) {
	//loadSharedMemory();
	//data = getSharedMemory();
	//matrixData = (Matrix*)((int*)data + 1);
	//matrixDataLength = *(int*)data;
	InitWindow(1280, 720, "HSTK");

	c.fovy = 90;
	c.position = { 5,2,0 };
	c.target = { 0,0,0 };
	c.up = { 0,1,0 };
	c.type = CAMERA_PERSPECTIVE;
	gameState = ecs_init();
	registerAllComponents(gameState);
	RegisterAllSystems();
	RegisterInitialEntities();
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		//UPDATE
		UpdateInput();
		ecs_progress(gameState, GetFrameTime());
		UpdateEditor();
		BeginDrawing();
		ClearBackground(BLACK);
		Render(); //was a system but turned out that was a bad idea, it calls other systems though
		DrawEditor();
		EndDrawing();
	}
	ecs_fini(gameState);
	return 0;
}
