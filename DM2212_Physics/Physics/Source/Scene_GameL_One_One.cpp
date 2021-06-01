#include "Scene_GameL_One_One.h"
#include "Application.h"
#include "GL\glew.h"

Scene_GameL_One_One::Scene_GameL_One_One()
{
}

Scene_GameL_One_One::~Scene_GameL_One_One()
{
}

void Scene_GameL_One_One::Init()
{
	SceneBase::Init();
	SceneAsteroid::Init();
	
	//Spawning enemies
	for (int i = 0; i < 3; i++) {
		Enemy* enemy = FetchEnemyGO();

		enemy->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_DEFAULT, 1.0f);
		enemy->setCurrentWeapon(PRIMARY);
		enemy->getCurrentWeapon()->setLastBulletShot(elapsed);
		enemy->getCurrentWeapon()->setAllTargettable(true);
		enemy->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
		enemy->getCurrentWeapon()->setBulletSpeed(20.f);

		enemy->setActive(true);
		enemy->scale = Vector3(4, 8, 1);
		enemy->mass = 1.f;
		enemy->setHealth(30);
		enemy->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);

		enemy->damagePercent = 50.0f;
		enemy->trackingDelay = 2.0f;
	}

	//Spawn Asteroids
	for (int i = 0; i < 30; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_ASTEROID)) i--;
	}

}


void Scene_GameL_One_One::Update(double dt)
{
	SceneBase::Update(dt);
	SceneAsteroid::Update(dt);

}

void Scene_GameL_One_One::Render()
{
	SceneAsteroid::Render();
}

void Scene_GameL_One_One::Exit()
{
	SceneAsteroid::Exit();
}


