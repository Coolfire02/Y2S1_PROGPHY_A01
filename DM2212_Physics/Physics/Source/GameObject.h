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
		
		GO_ASTEROID,
		
		GO_SHIP,
		GO_BULLET,

		GO_ENEMY,
		GO_ENEMY_BULLET,
		GO_MISSILE,
		GO_POWERUP,

		GO_TOTAL, //must be last
	};

	GAMEOBJECT_TYPE type;
	Vector3 pos;
	Vector3 vel;
	Vector3 scale;
	
	float mass;
	
	GameObject* parentGO;

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

	float angle;
	Vector3 dir; //direction / orientation
	float momentOfInertia;
	float angularVelocity; //in radians

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	~GameObject();
};

#endif