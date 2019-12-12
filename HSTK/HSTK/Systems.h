#pragma once
#include <iostream>
#ifndef  RAYLIB_H
#include "raylib.h"
#endif // ! RAYLIB_H
#ifndef FLECS_H
#include"flecs.h"
#endif // !FLECS_H
#include "raymath.h"
#include <fstream>
Camera c;
ecs_world * gameState = nullptr;
template<typename T> T *addrOf(T &&v) { return &v; }
#undef ecs_set
#define ecs_set(world, entity, component, ...)\
    _ecs_set_ptr(world, entity, ecs_entity(component), sizeof(component), addrOf(component __VA_ARGS__))

void Render(ecs_rows_t *rows) {
	ECS_COLUMN(rows, Model, model, 1);
	BeginDrawing();
	BeginMode3D(c);
	for (int i = 0; i < rows->count; i++) {
		//DrawSphere(Vector3Transform({ 0,0,0 }, kube.transform), 1, GREEN);
		DrawModel(model[i], { 0,0,0 }, 1, RED);
	}
	EndDrawing();
}


void RegisterAll() {
	ECS_TAG(gameState, Renderable);
	ECS_COMPONENT(gameState, Model);
	ECS_COMPONENT(gameState, ModelAnimation);
	ECS_SYSTEM(gameState, Render, EcsPostUpdate, Model, Renderable);
}

void RegisterInitialEntities() {
	std::fstream myfile;
	myfile.open("scene.txt");
	ECS_ENTITY();
	while(!myfile.eof){
		//ecs_entity_t temp = ecs_new_entity(world, #id, #__VA_ARGS__);
		myfile.getline(;
	}
	myfile.close();

}