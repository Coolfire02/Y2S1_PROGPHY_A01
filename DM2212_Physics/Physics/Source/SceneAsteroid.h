#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "Enemy.h"
#include "Bullet.h"
#include <queue>

struct LaggingPosition {
	float time;
	Vector3 pos;
	LaggingPosition(Vector3 pos, float currentTime) : pos(pos), time(currentTime) {};
};

class SceneAsteroid : public SceneBase
{
	static const int MAX_SPEED = 6;
	static const int MAX_ANGULAR_SPEED = 1;

	static const int SHIP_BASE_HEALTH = 50;
	static const int SMALL_ASTEROID_HEALTH = 3;
	static const int BIG_ASTEROID_HEALTH = 10;

	static const int MAX_ENEMIES = 10;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	bool isLevelCompleted();

	Bullet* FetchBulletGO();
	Enemy* FetchEnemyGO();
	GameObject* FetchGO();
protected:

	//Physics
	std::vector<Bullet*> m_bulletList;
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;

	GameObject *m_ship;

	//Level Animation
	bool levelStartAnimation;
	double levelStartAnimationTimeLeft;

	bool levelCompleted;

	//Enemy Logic
	Enemy* enemies[MAX_ENEMIES];
	double posTrackingTime;
	double posTrackingCD;
	std::queue<LaggingPosition> m_posTracking;
	float freezeTrackingTill;
	float freezeTrackingCD;

	Vector3 m_force;
	Vector3 m_torque;
	
	int m_objectCount;
	int m_lives;
	int m_score;

	//Game Logic
	void LoadDefaultGameValues();

	bool tryShoot(GameObject*);

	//Enemy Ship Spawning

	void HandleCollision(GameObject*, double dt);
	void HandleBounceOffCollision(GameObject* go, GameObject* coll, bool damageGO, bool damageCOLL, float minImpactForDamage, float impactMultiplier);

	bool SpawnAsteroid(GameObject::GAMEOBJECT_TYPE type); //Only accepts asteroid and papa asteroid

	void ShowHealthBar(GameObject*);
	bool CanDamage(GameObject*, GameObject*);
	bool Collided(GameObject*, GameObject*);
	bool CollidedInFuture(GameObject*, GameObject*);
	void WrapObjectOnScreen(GameObject*);
	bool ObjectOffScreen(GameObject*);

	float elapsed;
};

#endif