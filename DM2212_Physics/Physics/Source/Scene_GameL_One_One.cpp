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

	LoadDefaultGameValues();
}

void Scene_GameL_One_One::SpawnGameEnemy() {
	Enemy* enemy = FetchEnemyGO();

	enemy->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_DEFAULT, 2.5f);
	enemy->setCurrentWeapon(PRIMARY);
	enemy->getCurrentWeapon()->setLastBulletShot(elapsed);
	enemy->getCurrentWeapon()->setAllTargettable(true);
	enemy->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
	enemy->getCurrentWeapon()->setBulletSpeed(20.f);

	enemy->setActive(true);
	enemy->scale = Vector3(4, 8, 1);
	enemy->mass = 1.f;
	enemy->setHealth(15);
	enemy->pos = getRandomOutOfBoundsPosition(16);

	enemy->dir = (m_ship->pos - enemy->pos).Normalized();

	enemy->damagePercent = 50.0f;
	enemy->trackingDelay = 2.0f;
}

void Scene_GameL_One_One::LoadDefaultGameValues() 
{
	enemiesKilled = 0;
	SceneAsteroid::LoadDefaultGameValues();
	//Spawn Asteroids
	for (int i = 0; i < 30; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_ASTEROID)) i--;
	}

	for (int i = 0; i < 5; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_PAPA_ASTEROID)) i--;
	}


}

std::string Scene_GameL_One_One::getObjective() {
	return "Kill 2 Enemies to Advance";
}

std::string Scene_GameL_One_One::getBossBarMessage() {
	if(levelEndAnimationTimeLeft > 0.f)
		return "Cygnus Infiltration 1-1 COMPLETE";

	return "";
}


void Scene_GameL_One_One::afterLevelAnimation() {
	//Spawning one enemy
	SpawnGameEnemy();
}


void Scene_GameL_One_One::Update(double dt)
{
	int enemyCount = getActiveEnemyCount();
	SceneBase::Update(dt);
	SceneAsteroid::Update(dt);
	if (getActiveEnemyCount() < enemyCount)
	{
		for (int i = 0; i < enemyCount - getActiveEnemyCount(); ++i)
		{
			enemiesKilled++;
			SpawnGameEnemy();
		}
	}

	if (enemiesKilled >= 2 && !key_spawned)
	{
		key_spawned = true;
		GameObject* go = FetchGO();
		go->setActive(true);
		go->type = GameObject::GO_KEYCARD;
		go->scale = Vector3(2.5f, 1.8425f, 1);
		go->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
		go->mass = 1.f;
	}

	for (int i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i]->isActive())
		{ 
			if (!ObjectIsWithinScreen(enemies[i]))
			{
				Vector3 dirV = m_ship->pos - enemies[i]->pos;
				enemies[i]->vel = dirV.Normalized() * MAX_SPEED_ENEMY;
			}
			else 
			{
				enemies[i]->vel = Vector3(0, 0, 0);
				enemies[i]->arrivedOnScreen = true;
				if (!enemy_trackingToPos[i].active && elapsed - enemy_trackingToPos[i].lastTrackingChange > 5.0f)
				{
					enemy_trackingToPos[i].active = true;
					enemy_trackingToPos[i].lastTrackingChange = elapsed;

					Vector3 dirV = m_ship->pos - enemies[i]->pos;
					float angle = Math::RadianToDegree(atan2f(dirV.y, dirV.x));
					angle += Math::RandFloatMinMax(-80.f, 80.f);

					Vector3 toPos = m_ship->pos + Vector3(cosf(Math::DegreeToRadian(angle)), sinf(Math::DegreeToRadian(angle)), 0) * 30;

					enemy_trackingToPos[i].toPos = toPos;
				}
				else if (enemy_trackingToPos[i].active)
				{
					Vector3 dirV = enemy_trackingToPos[i].toPos - enemies[i]->pos;
					enemies[i]->vel = dirV.Normalized() * MAX_SPEED_ENEMY;
					Vector3 closeEnough = enemy_trackingToPos[i].toPos - enemies[i]->pos;
					if (closeEnough.LengthSquared() < 3 * 3)
					{
						enemy_trackingToPos[i].active = false;
					}
					
				}
			}
		}
	}


}

void Scene_GameL_One_One::Render()
{
	SceneAsteroid::Render();
}

void Scene_GameL_One_One::Exit()
{
	SceneAsteroid::Exit();
}


