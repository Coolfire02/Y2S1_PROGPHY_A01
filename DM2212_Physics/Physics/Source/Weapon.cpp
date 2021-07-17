#include "Weapon.h"

Weapon::Weapon(WEAPON_TYPE type, float cd) : type(type), bulletCD(cd), lastBulletShot(0.f), bulletSpeed(50.f){
	for (int i = 0; i < TARGET_COUNT; ++i) {
		weaponTargets[i] = false;
	}
	switch (type) {
	case WEAPON_DEFAULT:
		bulletDMG = 1.0f;
		break;

	case WEAPON_DOUBLEGUN:
		bulletDMG = 0.75f;
		break;

	case WEAPON_QUADGUN:
		bulletDMG = 0.6f;
		break;

	case WEAPON_LASERGUN:
		bulletDMG = 0.25f;
		break;

	case WEAPON_HOMING:
		bulletDMG = 10.f;
		break;

	}

}
Weapon::~Weapon() {};


bool Weapon::isTargettable(WEAPON_TARGET target) {
	return weaponTargets[target];
}

void Weapon::setAllTargettable(bool targettable) {
	for (int i = 0; i < TARGET_COUNT; ++i) {
		weaponTargets[i] = targettable;
	}
}

float Weapon::getBulletSpeed() {
	return bulletSpeed;
}

void Weapon::setType(WEAPON_TYPE type) {
	this->type = type;
}

void Weapon::setBulletSpeed(float speed) {
	bulletSpeed = speed;
}

void Weapon::setTargettable(WEAPON_TARGET target, bool targettable) {
	weaponTargets[target] = targettable;
}

void Weapon::setBulletCD(float cd) {
	bulletCD = cd;
}

float Weapon::getBulletCD() {
	return bulletCD;
}

float Weapon::getDefaultBulletDMG() {
	return bulletDMG;
}

WEAPON_TYPE Weapon::getWeaponType() {
	return type;
}

void Weapon::setLastBulletShot(float t) {
	lastBulletShot = t;
}

float Weapon::getLastBulletShot() {
	return lastBulletShot;
}

