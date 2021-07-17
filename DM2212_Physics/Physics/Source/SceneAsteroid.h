#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"
#include "Enemy.h"
#include "Bullet.h"
#include <string>
#include <queue>

struct LaggingPosition {
	float time;
	Vector3 pos;
	LaggingPosition(Vector3 pos, float currentTime) : pos(pos), time(currentTime) {};
};

struct EnemyTracker
{
	double lastTrackingChange;
	bool active;
	Vector3 toPos;

	EnemyTracker() : lastTrackingChange(0.0), active(false), toPos(Vector3(0, 0, 0)) {}
	~EnemyTracker() {}
};

enum POWERUP_TYPE
{
	POWERUP_GUNDOUBLE,
	POWERUP_GUNQUAD,
	POWERUP_TIME,
	POWERUP_HEALTH,
	POWERUP_COUNT
};

struct PowerUp
{
	float durationRemaining;

	PowerUp() : durationRemaining(0.f) {}
};

enum ORB_TYPE {
	ORB_HEALTH,
	ORB_ATTACK,
	ORB_DEFENSE,
	ORB_COUNT
};

struct Orb : public GameObject
{
	GameObject* trackingGO;
	ORB_TYPE orbType;
	float angleFromShip;
	float displacement;
	float timeAliveRemaining;
	Orb(ORB_TYPE type, GameObject* trackingGO, float angleFromShip, float displacement, float timeAliveRemaining) : GameObject(GO_ORB), orbType(type), angleFromShip(angleFromShip), displacement(displacement), trackingGO(trackingGO), timeAliveRemaining(timeAliveRemaining) {
	}
};

class SceneAsteroid : public SceneBase
{
protected:
	static const int MAX_SPEED = 7;
	static const int MAX_ANGULAR_SPEED = 1;

	static const int SHIP_BASE_HEALTH = 50;
	static const int SMALL_ASTEROID_HEALTH = 3;
	static const int BIG_ASTEROID_HEALTH = 10;

	static const int MAX_SPEED_ENEMY = 3;
	
	static const int MAX_ENEMIES = 10;
	float POWERUP_SLOWTIME_SPEED = 0.3f;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	virtual std::string getObjective() = 0;
	virtual std::string getBossBarMessage() = 0;

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

	int healthOrbAngle, attackOrbAngle, defenseOrbAngle;
	std::vector<Orb*> m_shipOrbs;

	GameObject *m_ship;
	float shipMovementCD; //When collided with asteroid so vel does not hook into asteroid's
	PowerUp m_shipPowerUps[POWERUP_COUNT];

	//Level Animation
	GameObject* wormHole;
	bool levelAnimating;
	double levelStartAnimationTimeLeft;

	double levelEndAnimationTimeLeft;

	bool levelCompleted;
	bool levelCompletedOnce;

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

	bool key_spawned;
	bool key_collected;

	//Game Logic
	virtual void LoadDefaultGameValues();

	virtual void afterLevelAnimation();

	bool tryShoot(GameObject*);

	//Enemy Ship Spawning

	Vector3 getRandomOutOfBoundsPosition(float radius);
	int getActiveEnemyCount();

	void SpawnPowerUpAtLocation(Vector3 pos);

	bool ObjectIsWithinScreen(GameObject* go);

	void HandleCollision(GameObject*, double dt);
	void HandleBounceOffCollision(GameObject* go, GameObject* coll, bool damageGO, bool damageCOLL, float minImpactForDamage, float impactMultiplier, double dt);

	bool IsSpaceOccupied(GameObject* obj, Vector3 newPos);

	bool SpawnAsteroid(GameObject::GAMEOBJECT_TYPE type); //Only accepts asteroid and papa asteroid

	

	void ShowHealthBar(GameObject*);
	bool CanDamage(GameObject*, GameObject*);
	bool Collided(GameObject*, GameObject*);
	bool CollidedInFuture(GameObject*, GameObject*);
	void WrapObjectOnScreen(GameObject*, double dt);
	bool ObjectOffScreen(GameObject*);

	float elapsed;
};

#endif