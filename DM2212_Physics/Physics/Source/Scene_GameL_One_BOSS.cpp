#include "Scene_GameL_One_BOSS.h"
#include "Application.h"
#include "GL\glew.h"

Scene_GameL_One_BOSS::Scene_GameL_One_BOSS()
{
}

Scene_GameL_One_BOSS::~Scene_GameL_One_BOSS()
{
}

void Scene_GameL_One_BOSS::Init()
{
	SceneBase::Init();
	SceneAsteroid::Init();
	
	boss = FetchEnemyGO();
	boss->setActive(true);

	LoadDefaultGameValues();
}

void Scene_GameL_One_BOSS::SpawnGameEnemy() {
	Enemy* enemy = FetchEnemyGO();

	if (Math::RandIntMinMax(0, 1) == 0)
	{
		enemy->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_DOUBLEGUN, 2.f);
		enemy->setWeapon(SECONDARY, WEAPON_TYPE::WEAPON_HOMING, 30.f);
		enemy->setCurrentWeapon(SECONDARY);
		enemy->getCurrentWeapon()->setLastBulletShot(elapsed - 30.f);
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
		if (enemy->pos.y > m_worldHeight - 20)
			enemy->pos.y *= -1;

		enemy->dir = (m_ship->pos - enemy->pos).Normalized();

		enemy->damagePercent = 50.0f;
		enemy->trackingDelay = 2.0f;
	}
	else
	{
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
		enemy->enemy_type = ENEMY_1;

		enemy->dir = (m_ship->pos - enemy->pos).Normalized();

		enemy->damagePercent = 50.0f;
		enemy->trackingDelay = 2.0f;
	}
}

void Scene_GameL_One_BOSS::LoadDefaultGameValues()
{
	enemiesKilled = 0;
	SceneAsteroid::LoadDefaultGameValues();
	//Spawn Asteroids
	
	boss->setActive(true);
	boss->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_DOUBLEGUN, 1.0f);
	boss->setWeapon(SECONDARY, WEAPON_TYPE::WEAPON_HOMING, 20.f);
	boss->setCurrentWeapon(SECONDARY);
	boss->getCurrentWeapon()->setLastBulletShot(elapsed - 30.f);
	boss->getCurrentWeapon()->setAllTargettable(true);
	boss->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
	boss->getCurrentWeapon()->setBulletSpeed(17.f);
	boss->setCurrentWeapon(PRIMARY);
	boss->getCurrentWeapon()->setLastBulletShot(elapsed);
	boss->getCurrentWeapon()->setAllTargettable(true);
	boss->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
	boss->getCurrentWeapon()->setBulletSpeed(20.f);
	boss->enemy_type = ENEMY_BOSS;
	boss->getCurrentWeapon()->setTargettable(TARGET_ENEMIES, false);
	boss->getCurrentWeapon()->setBulletSpeed(20.f);
	boss->scale = Vector3(7, 22, 1);
	boss->mass = 1.f;
	boss->setHealth(100);
	boss->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight - 10, 0);
	boss->isMoveable = false;

	boss->dir = (m_ship->pos - boss->pos).Normalized();

	boss->damagePercent = 50.0f;
	boss->trackingDelay = 2.0f;

	for (int i = 0; i < 20; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_ASTEROID)) i--;
	}

	for (int i = 0; i < 5; ++i)
	{
		if (!SpawnAsteroid(GameObject::GO_PAPA_ASTEROID)) i--;
	}
	

}

std::string Scene_GameL_One_BOSS::getObjective() {
	return "Kill Cygnus Ship";
}

std::string Scene_GameL_One_BOSS::getBossBarMessage() {
	if (levelEndAnimationTimeLeft > 0.f)
		return "Cygnus Infiltration 1-3 COMPLETE";

	return "";
}

void Scene_GameL_One_BOSS::afterLevelAnimation() {
	//Spawning one enemy
	SpawnGameEnemy();
	SpawnGameEnemy();
}


void Scene_GameL_One_BOSS::Update(double dt)
{
	int enemyCount = getActiveEnemyCount();
	SceneBase::Update(dt);
	SceneAsteroid::Update(dt);

	//Boss Updates
	if (boss->getHealth() < boss->getMaxHealth() && boss->getCurrentWeaponsOrder() == PRIMARY)
	{
		float percent = boss->getHealth() / boss->getMaxHealth();
		if (percent < 0.3)
		{
			boss->getCurrentWeapon()->setBulletCD(0.5f);
			boss->getCurrentWeapon()->setType(WEAPON_QUADGUN);
			boss->getWeapon(SECONDARY)->setBulletCD(10.f);
			boss->addHealth(1.0 * dt);
		}
		else if (percent < 0.6)
		{
			boss->getCurrentWeapon()->setBulletCD(0.65f);
			boss->getCurrentWeapon()->setType(WEAPON_DOUBLEGUN);
			boss->getWeapon(SECONDARY)->setBulletCD(15.f);
			boss->addHealth(0.3 * dt);
		}
	}

	if (getActiveEnemyCount() < enemyCount)
	{
		for (int i = 0; i < enemyCount - getActiveEnemyCount(); ++i)
		{
			enemiesKilled++;
			SpawnGameEnemy();
		}
	}

	if ( (!boss->isActive() || boss->enemy_type != ENEMY_BOSS) && !key_spawned)
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

void Scene_GameL_One_BOSS::Render()
{
	SceneAsteroid::Render();
}

void Scene_GameL_One_BOSS::Exit()
{
	SceneAsteroid::Exit();
}
