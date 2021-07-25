
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue)
	: type(typeValue),
	scale(1, 1, 1),
	angle(0.f),
	gravity(false),
	health(1.0f),
	customMesh(nullptr),
	isMoveable(true),
	justDamagedByCollision(false),
	active(false),
	weaponIndex(WEAPON::PRIMARY),
	mass(1.f),
	dir(1, 0, 0),
	momentOfInertia(1.f),
	angularVelocity(0.f),

	cooldown(0.0) //Move to flipper class?
{
	for (int i = 0; i < WEAPON::MAX_WEAPONS; ++i) {
		weapons[i] = nullptr;
	}
}

GameObject::~GameObject()
{
	flushWeapons();
}

void GameObject::flushWeapons() {
	for (int i = 0; i < WEAPON::MAX_WEAPONS; ++i) {
		if (weapons[i] != nullptr) {
			delete weapons[i];
			weapons[i] = nullptr;
		}
	}
}

Weapon* GameObject::getCurrentWeapon() {
	return weapons[weaponIndex];
}

void GameObject::setWeapon(WEAPON pos, WEAPON_TYPE type, float cd) {
	if (weapons[pos] != nullptr) {
		delete weapons[pos];
		weapons[pos] = nullptr;
	}
	weapons[pos] = new Weapon(type, cd);
}

bool GameObject::hasCustomMesh() {
	return (customMesh != nullptr);
}

void GameObject::setCustomMesh(Mesh* cM) {
	customMesh = cM;
}

void GameObject::setActive(bool active) {
	this->active = active;
	if (active == false) {
		customMesh = nullptr;
		maxHealth = 0;
		health = 0;
	}
}

bool GameObject::isActive() {
	return active;
}

Weapon* GameObject::getWeapon(WEAPON type)
{
	return weapons[type];
}

Mesh* GameObject::getCustomMesh() {
	return customMesh;
}

void GameObject::setCurrentWeapon(WEAPON pos) {
	weaponIndex = pos;
}

WEAPON GameObject::getCurrentWeaponsOrder() {
	return static_cast<WEAPON>(weaponIndex);
}

void GameObject::setHealth(float health) {
	if (health > this->maxHealth) this->maxHealth = health;
	this->health = health;
}

void GameObject::subtractHealth(float amt) {
	if (amt > 0)
	{
		health -= amt;
		if (health < 0) health = 0;
	}
	else {
		amt + 1;
	}
}

void GameObject::addHealth(float amt) {
	health += amt;
	if (health > maxHealth)
		maxHealth = health;
}

float GameObject::getHealth() {
	return health;
}

float GameObject::getMaxHealth() {
	return maxHealth;
}

bool GameObject::isDamaged() {
	return !(health > maxHealth - Math::EPSILON);
}

bool GameObject::setCurrentWeapon(WEAPON_TYPE type) {
	for (int i = 0; i < WEAPON::MAX_WEAPONS; ++i) {
		if (weapons[i]->getWeaponType() == type) {
			weaponIndex = i;
			return true;
			break;
		}
	}
	return false;
}


