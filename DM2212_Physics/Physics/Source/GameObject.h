#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include "Weapon.h"
#include "Mesh.h"

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{

		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,
		GO_PILLAR,
		GO_WALL,
		GO_FLIPPER,

		GO_ASTEROID,
		GO_PAPA_ASTEROID,
		
		GO_SHIP,
		GO_BULLET,

		GO_ENEMY,
		GO_ENEMY_BULLET,
		GO_HOMINGMISSILE,
		
		GO_KEYCARD,

		GO_POWERUP_TIME,
		GO_POWERUP_HEALTH,
		GO_POWERUP_GUNDOUBLE,
		GO_POWERUP_GUNQUAD,
		GO_ABILITY_CLOAX,

		GO_ORB,

		GO_WORMHOLE,

		GO_TOTAL, //must be last
	};

	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;

	//Circular Motion
	Vector3 pivot;
	Vector3 pivotDir;
	double cooldown;
	bool isOnGround;
	float minAngle;
	float maxAngle;
	bool flipperClockwise;

	bool gravity;
	
	float mass;

	bool justDamagedByCollision;
	
	GameObject* parentGO;

	bool isMoveable;

private:
	Mesh* customMesh;
	Weapon* weapons[WEAPON::MAX_WEAPONS];
	int weaponIndex;
	float health;
	float maxHealth;
	bool active;
public:
	void flushWeapons();
	Weapon* getCurrentWeapon();
	WEAPON getCurrentWeaponsOrder();
	Weapon* getWeapon(WEAPON type);
	void setCurrentWeapon(WEAPON weapon);
	bool setCurrentWeapon(WEAPON_TYPE);
	void setWeapon(WEAPON, WEAPON_TYPE, float cd);

	//Can change to GEO_TYPE instead. Mesh pointer shouldnt be accessed/stored like that
	bool hasCustomMesh();
	Mesh* getCustomMesh();
	void setCustomMesh(Mesh*);

	bool isActive();
	virtual void setActive(bool active);

	void setHealth(float health);
	void subtractHealth(float amt);
	void addHealth(float amt);
	float getHealth();
	float getMaxHealth();
	bool isDamaged();

	float movementCooldown; //Used when object has constantly modifying velocities to not get stuck on asteroids.
	float angle;
	Vector3 dir; //direction / orientation
	float momentOfInertia;
	float angularVelocity; //in radians

	Vector3 normal;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif