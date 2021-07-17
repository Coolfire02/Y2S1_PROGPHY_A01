#pragma once

#include "SceneAsteroid.h"

class Scene_GameL_One_One : public SceneAsteroid
{
public:
	Scene_GameL_One_One();
	~Scene_GameL_One_One();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	virtual void LoadDefaultGameValues();

	virtual void afterLevelAnimation();
	virtual std::string getObjective();
	virtual std::string getBossBarMessage();

	void SpawnGameEnemy();


private:
	int enemiesKilled;
	EnemyTracker enemy_trackingToPos[MAX_ENEMIES];
};

