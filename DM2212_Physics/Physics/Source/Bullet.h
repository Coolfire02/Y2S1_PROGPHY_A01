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
	float damageMultiplier;
	Bullet() : GameObject(GameObject::GO_BULLET), damageMultiplier(1.f),weaponUsed(nullptr), targetGO(nullptr), parentGO(nullptr), bulletType(BULLET_BLUE) {}
	~Bullet() {};
};

