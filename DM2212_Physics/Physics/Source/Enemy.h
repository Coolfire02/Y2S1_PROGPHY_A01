#pragma once

#include "GameObject.h"

enum ENEMY_TYPE {
	ENEMY_1,
	ENEMY_2,
	ENEMY_BOSS,
	ENEMY_COUNT
};

struct Enemy : GameObject
{
	float damagePercent; /*0-100% of Gun Selected Damage*/
	float trackingDelay; /*Tracking delay of enemy*/
	bool arrivedOnScreen;
	ENEMY_TYPE enemy_type;

	Enemy(GAMEOBJECT_TYPE type) : enemy_type(ENEMY_1), damagePercent(50.f), trackingDelay(4.0), arrivedOnScreen(false), GameObject(type) {
	}
	~Enemy() {}
};

