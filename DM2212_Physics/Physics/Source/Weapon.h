#pragma once



enum WEAPON_TARGET {
	TARGET_PLAYER,
	TARGET_ENEMIES,
	TARGET_ASTEROIDS,
	TARGET_COUNT,
};

enum WEAPON_TYPE {
	WEAPON_DEFAULT,
	WEAPON_DOUBLEGUN,
	WEAPON_QUADGUN,

	WEAPON_LASER,

	WEAPON_HOMING,

	WEAPON_COUNT,
};

enum WEAPON {
	PRIMARY,
	SECONDARY,
	THIRD,
	FOURTH,
	FIFTH,
	MAX_WEAPONS,
};

class Weapon {
	float bulletDMG;
	float bulletCD;
	float bulletSpeed;
	float lastBulletShot;
	WEAPON_TYPE type;

	bool weaponTargets[TARGET_COUNT];

public:
	Weapon(WEAPON_TYPE, float cd);
	~Weapon();
	WEAPON_TYPE getWeaponType();

	bool isTargettable(WEAPON_TARGET target);
	void setTargettable(WEAPON_TARGET target, bool targettable);
	void setAllTargettable(bool targettable);

	void setLastBulletShot(float time);
	float getLastBulletShot();

	float getBulletSpeed();
	void setBulletSpeed(float speed);

	float getDefaultBulletDMG();
	float getBulletCD();
	void setBulletCD(float cd);
};

