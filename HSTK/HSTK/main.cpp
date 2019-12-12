#include <iostream>
#include "raylib.h"
#include "raymath.h"
//#include "GetPointerToSharedMemory.h"
#include "flecs.h"
#include "Systems.h"


//void * data;
//Matrix * matrixData;
//int matrixDataLength;
int width, height;

int main(int argc, char *argv[]) {
	
	
	
	//loadSharedMemory();
	//data = getSharedMemory();
	//matrixData = (Matrix*)((int*)data + 1);
	//matrixDataLength = *(int*)data;
	width = GetMonitorHeight(0);
	height = GetMonitorWidth(0);
	InitWindow(width, height, "RayUnity");
	c.fovy = 90;
	c.position = { 5,2,0 };
	c.target = { 0,0,0 };
	c.up = { 0,1,0 };
	c.type = CAMERA_PERSPECTIVE;
	gameState = ecs_init();
	while (!WindowShouldClose()) {
		//UPDATE
		ecs_progress(gameState, GetFrameTime());
		//DRAW
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(0, 20);
		BeginMode3D(c);
		DrawGrid(10, 1);
		EndMode3D();
		EndDrawing();
	}
	ecs_fini(gameState);
	return 0;
}
