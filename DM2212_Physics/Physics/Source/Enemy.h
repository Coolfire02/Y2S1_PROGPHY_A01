#pragma once

#include "GameObject.h"

enum ENEMY_TYPE {
	SMALL_SHIP,
	MEDIUM_SHIP,
	BOSS_SHIP,
};

struct Enemy : GameObject
{
	float damagePercent; /*0-100% of Gun Selected Damage*/
	float trackingDelay; /*Tracking delay of enemy*/
	bool arrivedOnScreen;
	Enemy(GAMEOBJECT_TYPE type) : damagePercent(50.f), trackingDelay(4.0), arrivedOnScreen(false), GameObject(type) {
	}
	~Enemy() {}
};

