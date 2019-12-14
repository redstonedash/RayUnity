#pragma region BoilderPlate
#pragma once
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
#define COMPONENTS_H
//DEFINE STRUCTS THAT WILL BE COMPONENTS
struct renderable {};

struct Input { //this exist so input can be consumed
	bool space;
	bool leftMouseButton;
	Vector2 mousePos;
	Vector2 mouseDelta;
	Vector2 lastMousePos;
};

__declspec(selectany) Input inputManager = {};

struct Window {
	Rectangle rect;
	char * title = "DEFAULT WINDOW";
};

#pragma endregion
//DEFINE COMPONENTS, syntax is COMPONENT(id);\
AND YES YOU DO NEED THE BACKSLASH
#define COMPONENTS COMPONENT(Model);\
COMPONENT(renderable);\
COMPONENT(ModelAnimation);\
COMPONENT(Window);

#pragma region MoreBoilerPlate
#define COMPONENT(id) __declspec(selectany) ecs_entity_t ecs_entity(id);
COMPONENTS;
#define COMPONENT(id) __declspec(selectany) ecs_type_t ecs_type(id);
COMPONENTS;
void registerAllComponents(ecs_world * gameState);
#pragma endregion