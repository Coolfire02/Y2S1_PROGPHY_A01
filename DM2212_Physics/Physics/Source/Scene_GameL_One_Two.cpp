#include "Scene_GameL_One_Two.h"
#include "Application.h"
#include "GL\glew.h"

Scene_GameL_One_Two::Scene_GameL_One_Two()
{
}

Scene_GameL_One_Two::~Scene_GameL_One_Two()
{
}

void Scene_GameL_One_Two::Init()
{
	SceneBase::Init();
	SceneAsteroid::Init();

	LoadDefaultGameValues();
}

void Scene_GameL_One_Two::SpawnGameEnemy() {
	Enemy* enemy = FetchEnemyGO();

	enemy->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_DOUBLEGUN, 2.0f);
	enemy->setWeapon(SECONDARY, WEAPON_TYPE::WEAPON_HOMING, 30.f);
	enemy->setCurrentWeapon(SECONDARY);
	enemy->getCurrentWeapon()->setLastBulletShot(elapsed-30.f);
	enemy->getCurrentWeapon()->setAllTargettable(true);
	enemy->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
	enemy->getCurrentWeapon()->setBulletSpeed(17.f);
	enemy->setCurrentWeapon(PRIMARY);
	enemy->getCurrentWeapon()->setLastBulletShot(elapsed);
	enemy->getCurrentWeapon()->setAllTargettable(true);
	enemy->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
	enemy->getCurrentWeapon()->setBulletSpeed(20.f);
	enemy->enemy_type = ENEMY_2;
	enemy->setActive(true);
	enemy->scale = Vector3(5, 10, 1);
	enemy->mass = 1.f;
	enemy->setHealth(25);
	enemy->pos = getRandomOutOfBoundsPosition(16);

	enemy->dir = (m_ship->pos - enemy->pos).Normalized();

	enemy->damagePercent = 50.0f;
	enemy->trackingDelay = 2.0f;
}

void Scene_GameL_One_Two::LoadDefaultGameValues()
{
	enemiesKilled = 0;
	SceneAsteroid::LoadDefaultGameValues();
	//Spawn Asteroids
	for (int i = 0; i < 20; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_ASTEROID)) i--;
	}

	for (int i = 0; i < 5; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_PAPA_ASTEROID)) i--;
	}


}

void Scene_GameL_One_Two::afterLevelAnimation() {
	//Spawning one enemy
	SpawnGameEnemy();
}


void Scene_GameL_One_Two::Update(double dt)
{
	int enemyCount = getActiveEnemyCount();
	SceneBase::Update(dt);
	SceneAsteroid::Update(dt);
	if (getActiveEnemyCount() < enemyCount)
	{
		for (int i = 0; i < enemyCount - getActiveEnemyCount(); ++i)
		{
			enemiesKilled++;
			if (enemiesKilled >= 2 && !key_spawned)
			{
				if (Math::RandIntMinMax(0, 100) <= 10 + 10 * enemiesKilled)
				{
					key_spawned = true;
					GameObject* go = FetchGO();
					go->setActive(true);
					go->type = GameObject::GO_KEYCARD;
					go->scale = Vector3(2.5f, 1.8425f, 1);
					go->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
					go->mass = 1.f;
				}
			}
			SpawnGameEnemy();
		}
	}

	for (int i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i]->isActive())
		{
			
			if (enemies[i]->getCurrentWeaponsOrder() == PRIMARY)
			{
				Weapon* secondary = enemies[i]->getWeapon(SECONDARY);
				if (secondary != nullptr)
				{
					if (elapsed - secondary->getLastBulletShot() > secondary->getBulletCD())
					{
						enemies[i]->setCurrentWeapon(SECONDARY);
					}
				}
			}
			else if (enemies[i]->getCurrentWeaponsOrder() == SECONDARY) {
				Weapon* secondary = enemies[i]->getWeapon(SECONDARY);
				if (secondary != nullptr)
				{
					if (elapsed - secondary->getLastBulletShot() < secondary->getBulletCD())
					{
						enemies[i]->setCurrentWeapon(PRIMARY);
					}
				}
			}

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

std::string Scene_GameL_One_Two::getObjective() {
	return "Kill 2-5 Enemies to Advance";
}

std::string Scene_GameL_One_Two::getBossBarMessage() {
	if (levelEndAnimationTimeLeft > 0.f)
		return "Cygnus Infiltration 1-2 COMPLETE";

	return "";
}

void Scene_GameL_One_Two::Render()
{
	SceneAsteroid::Render();
}

void Scene_GameL_One_Two::Exit()
{
	SceneAsteroid::Exit();
}