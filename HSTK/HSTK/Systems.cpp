#include "Systems.h"
void Render(ecs_rows_t *rows) {
	//ECS_COLUMN(rows, Model, model, 1);
	ecs_run(rows->world, FDrawWindows, rows->delta_time, nullptr);

	BeginMode3D(c);
	for (int i = 0; i < rows->count; i++) {
		//DrawSphere(Vector3Transform({ 0,0,0 }, kube.transform), 1, GREEN);
		//DrawModel(model[i], { 0,0,0 }, 1, RED);
	}
	EndMode3D();
}

void DrawWindows(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Window, window, 1);
	for (int i = 0; i < rows->count; i++) {
		if (CheckCollisionPointRec(inputManager.mousePos, { window[i].rect.x,window[i].rect.y,window[i].rect.width,24 }) && inputManager.leftMouseButton) {
			window[i].rect.x += inputManager.mouseDelta.x;
			window[i].rect.y += inputManager.mouseDelta.y;
		}

		if (GuiWindowBox(window[i].rect, window[i].title)) {
			ecs_delete(rows->world, rows->entities[i]);
		}
	}
}

#define SYSTEM(id, kind, ...) F##id = ecs_new_system(gameState, #id, kind, #__VA_ARGS__, id);\
    ecs_type(id) = ecs_type_from_entity(gameState, F##id);\

void RegisterAllSystems() {

	SYSTEMS

}

void UpdateInput() {

	inputManager.space = IsKeyPressed(KEY_SPACE);
	inputManager.leftMouseButton = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	inputManager.lastMousePos = inputManager.mousePos;
	inputManager.mousePos = GetMousePosition();
	inputManager.mouseDelta = { inputManager.mousePos.x - inputManager.lastMousePos.x,inputManager.mousePos.y - inputManager.lastMousePos.y };
	if (inputManager.space) { //TODO handle this AFTER update so if entities consume inputs we don't make editor actions off of them
		ECS_ENTITY(gameState, newWindow, Window, renderable);
		ecs_set(gameState, newWindow, Window, { {0,0,0,0}, "DEFAULT WINDOW" });
	}
}

void RegisterInitialEntities() {

}