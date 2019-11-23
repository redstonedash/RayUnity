#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "GetPointerToSharedMemory.h"
int main() {
	
	void * data;
	Matrix * matrixData;
	loadSharedMemory();
	data = getSharedMemory();
	matrixData = (Matrix*)((int*)data + 1);
	int width, height;
	width = GetMonitorHeight(0);
	height = GetMonitorWidth(0);
	InitWindow(width, height, "RayUnity");
	Camera c;
	c.fovy = 90;
	c.position = { 5,2,0 };
	c.target = { 0,0,0 };
	c.up = { 0,1,0 };
	c.type = CAMERA_PERSPECTIVE;
	Model kube = LoadModelFromMesh(GenMeshCube(1, 1, 1));
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawFPS(0, 20);
		BeginMode3D(c);
		//c.position = *(Vector3*)data;
		//DrawCircle(((Vector2*)data)->x, ((Vector2*)data)->y,100,BLACK);
		//DrawCube(*(Vector3*)((char*)data + sizeof(Vector3)), 1, 1, 1, RED);
		//*(Matrix*)(data) = MatrixIdentity();
		for (int i = 0; i < *(int*)data; i++) {
			(*(matrixData + i)) = MatrixMultiply((*(matrixData+i)), MatrixTranslate(1 * GetFrameTime(), 0, 0));
			kube.transform = (*(matrixData + i));
			//kube.transform = MatrixMultiply(MatrixIdentity(),MatrixTranslate(2,0,0));
			Matrix * trans = &kube.transform;
			//DrawModel(kube, { 0,0,0 }, 1, RED);
			DrawModelWires(kube, { 0,0,0 }, 1, RED);
		}
		DrawGrid(10, 1);
		EndMode3D();
		EndDrawing();
	}
	return 0;
}
