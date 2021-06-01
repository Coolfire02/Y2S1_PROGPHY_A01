#pragma once

#include "GameObject.h"

enum BULLET_TYPE {
	BULLET_BLUE,
	BULLET_RED,
	BULLET_HOMING,
	BULLET_COUNT
};

struct Bullet : public GameObject
{
	Weapon* weaponUsed;
	GameObject* targetGO;
	GameObject* parentGO;
	BULLET_TYPE bulletType;
	Bullet() : GameObject(GameObject::GO_BULLET), weaponUsed(nullptr), targetGO(nullptr), parentGO(nullptr), bulletType(BULLET_BLUE) {}
	~Bullet() {};
};

