#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();

	//Elapsed time init
	elapsed = 0.0f;

	levelCompleted = false;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_NONE and add into m_goList
	for (int i = 0; i < 100; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_NONE));
	}

	for (int i = 0; i < 100; ++i)
	{
		m_bulletList.push_back(new Bullet());
	}

	//Init ship for first time
	m_ship = new GameObject(GameObject::GO_SHIP);

	for (int i = 0; i < MAX_ENEMIES; ++i) {
		enemies[i] = new Enemy(GameObject::GO_ENEMY);
	}

	LoadDefaultGameValues();
}

//Game Logic Functions

void SceneAsteroid::LoadDefaultGameValues() {
	
	//Disable all Game Objects
	for (auto go : m_goList) {
		go->setActive(false);
	}
	for (auto go : m_bulletList) {
		go->setActive(false);
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		enemies[i]->setActive(false);
	}
	
	//Game Logic
	m_speed = 1.f;
	m_lives = 3;
	m_score = 0;
	
	//Default values for ship upon level start
	posTrackingTime = 0.05f;
	posTrackingCD = posTrackingTime;

	freezeTrackingTill = elapsed;
	freezeTrackingCD = 0.f;

	//Flush any existing weapons
	m_ship->flushWeapons();

	//Set primary weapon with 0.15s cooldown as the Default weapon
	m_ship->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_QUADGUN, 0.15f);
	m_ship->setCurrentWeapon(PRIMARY);
	m_ship->getCurrentWeapon()->setLastBulletShot(elapsed);
	m_ship->getCurrentWeapon()->setAllTargettable(true);
	m_ship->getCurrentWeapon()->setTargettable(TARGET_PLAYER, false);
	m_ship->getCurrentWeapon()->setBulletSpeed(50.f);

	m_ship->setActive(true);
	m_ship->setHealth(SHIP_BASE_HEALTH);
	m_ship->mass = 1.f;
	m_ship->scale = Vector3(5.0, 5.0, 5.0);
	m_ship->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f);
	m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	m_ship->angularVelocity = 0.f;

}

Enemy* SceneAsteroid::FetchEnemyGO() {
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if (!enemies[i]->isActive()) {
			enemies[i]->flushWeapons();
			return enemies[i];
		}
	}
	return nullptr;
}

Bullet* SceneAsteroid::FetchBulletGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (auto go : m_bulletList)
	{
		if (!go->isActive()) {
			return go;
		}
	}
	//Allocate more memory for more objects
	for (int i = 0; i < 10; ++i)
	{
		m_bulletList.push_back(new Bullet());
	}
	return m_bulletList.at(m_goList.size() - 10);
}

GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (auto go : m_goList)
	{
		if (!go->isActive()) {
			return go;
		}
	}
	//Allocate more memory for more objects
	for (int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_NONE));
	}
	return m_goList.at(m_goList.size() - 10);
}

void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);
	elapsed += dt;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	if(Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if(Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}
	m_force.SetZero();
	m_torque.SetZero();
	//Exercise 6: set m_force values based on WASD
	if(Application::IsKeyPressed('W'))
	{
		m_force += 50 * m_ship->dir;
		m_torque = Vector3(0, -1, 0).Cross(Vector3(0,50,0));
	}
	if(Application::IsKeyPressed('S'))
	{
		m_force += -50 * m_ship->dir;
		m_torque = Vector3(0, 1, 0).Cross(Vector3(0, -50, 0));
	}
	if (Application::IsKeyPressed('A'))
	{
		m_force += 20 * m_ship->dir;
		m_torque = Vector3(1, -1, 0).Cross(Vector3(20, 0, 0));
	}
	if(Application::IsKeyPressed('D'))
	{
		m_force += 20 * m_ship->dir;
		m_torque = Vector3(-1, -1, 0).Cross(Vector3(-20, 0, 0));
	}
	//Exercise 8: use 2 keys to increase and decrease mass of ship
	if (Application::IsKeyPressed('7'))
	{
		m_ship->mass -= 1 * dt;
		m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	}
	if (Application::IsKeyPressed('8'))
	{
		m_ship->mass += 1 * dt;
		m_ship->momentOfInertia = m_ship->mass * m_ship->scale.x * m_ship->scale.x;
	}


	//Exercise 11: use a key to spawn some asteroids
	if (Application::IsKeyPressed('V'))
	{
		GameObject* obj = FetchGO();
		obj->setActive(true);
		obj->scale = Vector3(3,3,3);
		obj->mass = 1.f;
		obj->angularVelocity = Math::RandFloatMinMax(-0.5f, 0.5f);
		obj->setHealth(SMALL_ASTEROID_HEALTH);
		obj->type = GameObject::GO_ASTEROID;
		obj->pos = Vector3(Math::RandFloatMinMax(0.f,m_worldWidth) , Math::RandFloatMinMax(0.f, m_worldHeight),0);
		obj->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f));
	}

	//Exercise 14: use a key to spawn a bullet
		//Exercise 15: limit the spawn rate of bullets

	if (Application::IsKeyPressed(VK_SPACE))
	{
		tryShoot(m_ship);
	}

	//Mouse Section
	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if(bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if(!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if(bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

	//Physics Simulation Section

	//Ship Movement Handling [Linear Motion]
	Vector3 acc = Vector3(m_force.x / m_ship->mass, m_force.y / m_ship->mass, m_force.z / m_ship->mass);
	Vector3 oldVel = m_ship->vel;
	m_ship->vel += acc * dt * m_speed;
	if (m_ship->vel.LengthSquared() > MAX_SPEED * MAX_SPEED)
		m_ship->vel = m_ship->vel.Normalize() * MAX_SPEED;

	//Move Ship
	m_ship->pos += m_ship->vel * dt * m_speed;
	this->WrapObjectOnScreen(m_ship);

	//Rotate Ship [Angular Motion]

	//Let alpha be angular acceleration use m_torque.z cause in 2D, u can only turn in z
	float alpha = m_torque.z / m_ship->momentOfInertia;

	m_ship->angularVelocity += alpha * dt;

	m_ship->angularVelocity = Math::Clamp(m_ship->angularVelocity, -(float)MAX_ANGULAR_SPEED, (float)MAX_ANGULAR_SPEED);//Zhi Wei
	float dTheta = m_ship->angularVelocity * dt;

	m_ship->dir.Set(m_ship->dir.x * cosf(dTheta) - m_ship->dir.y * sinf(dTheta),
		m_ship->dir.x * sinf(dTheta) + m_ship->dir.y * cosf(dTheta));
	
	m_ship->angle = Math::RadianToDegree(atan2f(m_ship->dir.y, m_ship->dir.x));

	for (auto& bullet : m_bulletList) {
		if (bullet->bulletType == BULLET_HOMING)
		{
			Vector3 dir = bullet->targetGO->pos - bullet->pos;
			if (!dir.IsZero())
			{
				bullet->vel += dir;
				bullet->vel = bullet->vel.Normalized() * bullet->weaponUsed->getBulletSpeed();
			}
			
		}
	}

	//Position Tracking for m_ship
	posTrackingCD -= dt;
	if (posTrackingCD <= 0) {
		posTrackingCD = posTrackingTime;
		m_posTracking.push(LaggingPosition(Vector3(m_ship->pos), elapsed));
	}

	//Move Enemy
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if(enemies[i]->isActive())
			enemies[i]->pos += enemies[i]->vel * dt * m_speed;
	}

	//Make Enemy Track Player
	//FOr all enemies, then find longest time to track, if found, pop front
	if (m_posTracking.size() > 0 && elapsed-m_posTracking.front().time > 4.0f) {
		Vector3 oldPos = m_posTracking.front().pos;
		m_posTracking.pop();

		if (freezeTrackingTill < elapsed) {
			if (freezeTrackingCD <= 0) {
				freezeTrackingCD = 0 + Math::RandFloatMinMax(1.0f, 2.0f);

				freezeTrackingTill = elapsed + 10.0f;
			}
			else {
				freezeTrackingCD -= dt;
			}

			for (int i = 0; i < MAX_ENEMIES; ++i) {
				if (enemies[i]->isActive())
				{
					Vector3 dir = (m_ship->pos - enemies[i]->pos);
					if (!dir.IsZero())
					{
						enemies[i]->dir = dir.Normalized();
					}

				}
			}
		}
	}

	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if(enemies[i]->isActive())
			tryShoot(enemies[i]);
	}

	for (auto& bullet : m_bulletList) {
		HandleCollision(bullet, dt);
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		HandleCollision(enemies[i], dt);
	}
	for (auto& go : m_goList) {
		HandleCollision(go, dt);
	}
	
	if (m_ship->getHealth() <= 0) {
		m_lives--;
		m_ship->setHealth(SHIP_BASE_HEALTH);
	}

	if (m_lives <= 0)
		LoadDefaultGameValues();
}

bool SceneAsteroid::SpawnAsteroid(GameObject::GAMEOBJECT_TYPE type) {
	GameObject* obj = FetchGO();
	switch (type) {
	case GameObject::GO_ASTEROID:
		obj->scale = Vector3(3, 3, 3);
		obj->mass = 1.f;
		obj->angularVelocity = Math::RandFloatMinMax(-0.5f, 0.5f);
		obj->setHealth(SMALL_ASTEROID_HEALTH);
		obj->type = GameObject::GO_ASTEROID;
		obj->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f));

		
		break;
	}

	//Finding a spot for the Asteroid
	bool freeSpot = true;
	for (int i = 0; i < 2; ++i)
	{
		obj->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		for (auto& go : m_goList) {
			if (go->isActive() && Collided(obj, go))
			{
				freeSpot = false;
				break;
			}
		}
		if (freeSpot && Collided(obj, m_ship))
		{
			freeSpot = false;
		}
		if (freeSpot)
		{
			for (int i = 0; i < MAX_ENEMIES; ++i)
			{
				if (enemies[i]->isActive() && Collided(enemies[i], obj))
				{
					freeSpot = false;
					break;
				}
			}
		}
		if (freeSpot == true)
		{
			break;
		}

	}
	if (freeSpot == true) {
		obj->setActive(true);
		return true;
	}

	return false;
}

void SceneAsteroid::HandleCollision(GameObject* go, double dt)
{
	if (!go->isActive()) return;
	//Bullet Collision
	if (go->type == GameObject::GO_BULLET)
	{
		Bullet* bullet = dynamic_cast<Bullet*>(go);
		if (bullet)
		{
			bullet->pos += bullet->vel * dt * m_speed;
			if (this->ObjectOffScreen(bullet)) {
				bullet->setActive(false);
			}

			for (auto& coll : m_goList) {
				if (coll->isActive())
				{
					//Bullet to ASteroid collision
					if (coll->type == GameObject::GO_ASTEROID && CanDamage(bullet->parentGO, coll)) {
						if (Collided(bullet, coll)) {
							coll->subtractHealth(bullet->weaponUsed->getDefaultBulletDMG());
							bullet->setActive(false);
							if (coll->getHealth() <= 0) {
								//Game Logic: Delete Aesteroid & Bullet, add 2 to score
								coll->setActive(false);
								m_score += 2;
							}

							break;
						}
					}
				}
			}
			//Go object should be able to know what its parent (Aka weapon for the bullet)
			if (Collided(bullet, m_ship) && CanDamage(bullet->parentGO, m_ship)) {
				m_ship->subtractHealth(bullet->weaponUsed->getDefaultBulletDMG());
				bullet->setActive(false); //destroy bullet
			}


			for (int i = 0; i < MAX_ENEMIES; ++i) {
				if (enemies[i]->isActive()) {
					if (Collided(bullet, enemies[i])) {
						if (CanDamage(bullet->parentGO, enemies[i])) {
							bullet->setActive(false);
							enemies[i]->subtractHealth(bullet->weaponUsed->getDefaultBulletDMG());
							if (enemies[i]->getHealth() <= 0) {
								enemies[i]->setActive(false);
								m_score += 100;
							}
						}
					}
				}
			}

		}
	}

	else if (go->type == GameObject::GO_ASTEROID)
	{
		//Asteroid Handling		
		go->pos += go->vel * dt * m_speed;

		//Ship Collision
		if (Collided(go, m_ship))
			HandleBounceOffCollision(go, m_ship, false, true, 1.f, 1.0f);
		for (int i = 0; i < 10; ++i)
		{
			if (enemies[i]->isActive() && Collided(go, enemies[i]))
				HandleBounceOffCollision(go, enemies[i], false, false, 1.f, 1.0f);
		}
		for (auto& coll : m_goList) {
			if (go != coll && coll->isActive() && Collided(go, coll))
				HandleBounceOffCollision(go, coll, false, false, 1.f, 1.0f);
		}
		

		this->WrapObjectOnScreen(go);
	}

	else if (go->type == GameObject::GO_ENEMY) 
	{
		if (Collided(go, m_ship))
			HandleBounceOffCollision(go, m_ship, true, true, 1.0f, 1.0f);
		Enemy* enemy = dynamic_cast<Enemy*>(go);
		if(enemy->arrivedOnScreen)
			this->WrapObjectOnScreen(go);
	}
		
	
}

/**
 * This Method helps to handle "Bouncing off" collisions between objects in the game. Damage can then be dealt based on the impact of collision
 * 
 * \param go
 * \param coll
 * \param damageGO
 * \param damageCOLL
 * \param minImpactForDamage
 */
void SceneAsteroid::HandleBounceOffCollision(GameObject* go, GameObject* coll, bool damageGO, bool damageCOLL, float minImpactForDamage, float impactMultiplier)
{
	Vector3 dir = go->vel - coll->vel;
	float impact = 0.f;
	if (!dir.IsZero()) {
		impact = dir.Length();
		dir.Normalize();

		float a1 = go->vel.Dot(dir);
		float a2 = coll->vel.Dot(dir);

		float mag = (2.0 * (a1 - a2)) / (go->mass + coll->mass);
		go->vel = go->vel - mag * dir;
		coll->vel = coll->vel + mag * dir;

		impact *= impactMultiplier;
		if (impact > minImpactForDamage)
		{
			a1 = Math::FAbs(a1);
			a2 = Math::FAbs(a2);
			if (damageGO)
			{
				go->subtractHealth(impact * ((a2 + 1) / (a1 + a2 + 2.0)));
				if (go->getHealth() <= 0) {
					go->setActive(false);
				}
			}
			if (damageCOLL)
			{
				coll->subtractHealth(impact * ( (a1 + 1) / (a1 + a2 + 2.0)));
				if (coll->getHealth() <= 0) {
					coll->setActive(false);
				}
			}
		}
	}
}

bool SceneAsteroid::tryShoot(GameObject* go) {
	if (elapsed - go->getCurrentWeapon()->getLastBulletShot() > go->getCurrentWeapon()->getBulletCD())
	{
		go->getCurrentWeapon()->setLastBulletShot(elapsed);

		BULLET_TYPE bType = BULLET_COUNT;
		switch (go->type) {
		case GameObject::GO_ENEMY:
			bType = BULLET_RED;
			break;
		case GameObject::GO_SHIP:
			bType = BULLET_BLUE;
			break;
		}
		if (bType == BULLET_COUNT) return false; //Cant shoot if not ship or enemy
		
		switch (go->getCurrentWeapon()->getWeaponType()) {
		case WEAPON_DEFAULT:
		{ //Scope
			Bullet* unused = FetchBulletGO();

			unused->setActive(true);
			unused->type = GameObject::GO_BULLET;
			unused->scale = Vector3(0.5f, 0.5f, 0.5f);
			unused->parentGO = go;
			unused->bulletType = bType;
			//unused->targetGO = m_ship; Only needed for homing missile
			unused->weaponUsed = go->getCurrentWeapon();
			unused->pos = go->pos + go->dir * (go->scale.x * 0.63f);
			unused->vel = go->dir * go->getCurrentWeapon()->getBulletSpeed();
			break;
		}

		case WEAPON_DOUBLEGUN:
		{ //Scope
			for (int i = 0; i < 2; ++i) {
				Bullet* unused = FetchBulletGO();
				unused->setActive(true);
				unused->type = GameObject::GO_BULLET;
				unused->scale = Vector3(0.5f, 0.5f, 0.5f);
				unused->parentGO = go;
				unused->bulletType = bType;
				unused->weaponUsed = go->getCurrentWeapon();
				Vector3 offset = go->dir * (go->scale.x * 0.63f);
				Vector3 rightV = offset.Cross(Vector3(0, 0, 1));
				offset += -rightV * 0.2 + rightV * 0.2 * i * 2;

				unused->pos = go->pos + offset;
				unused->vel = go->dir * go->getCurrentWeapon()->getBulletSpeed();
			}
			break;
		}

		case WEAPON_QUADGUN:
		{ //Scope
			for (int i = 0; i < 4; ++i) {
				Bullet* unused = FetchBulletGO();
				unused->setActive(true);
				unused->type = GameObject::GO_BULLET;
				unused->scale = Vector3(0.5f, 0.5f, 0.5f);
				unused->parentGO = go;
				unused->bulletType = bType;
				unused->weaponUsed = go->getCurrentWeapon();
				Vector3 offset = go->dir * (go->scale.x * 0.63f);
				Vector3 rightV = offset.Cross(Vector3(0, 0, 1));
				offset += -rightV * 0.6 + rightV * 0.4 * i;

				unused->pos = go->pos + offset;
				unused->vel = go->dir * go->getCurrentWeapon()->getBulletSpeed();
			}
			break;
		}

		}
		return true;
	}
	return false;
}

bool SceneAsteroid::CanDamage(GameObject* attacker, GameObject* victim) {

	switch (attacker->type) {
	case GameObject::GO_SHIP:
		switch (victim->type) {
		case GameObject::GO_ENEMY:
		case GameObject::GO_ASTEROID:
			return true;
		}
		break;

	case GameObject::GO_ENEMY:
		switch (victim->type) {
		case GameObject::GO_SHIP:
		case GameObject::GO_ASTEROID:
			return true;
		}
		break;

	}
	return false;
}

bool SceneAsteroid::Collided(GameObject* go, GameObject* coll) {
	return ((go->pos - coll->pos).Length() < go->scale.x + coll->scale.x);
}

bool SceneAsteroid::CollidedInFuture(GameObject* go, GameObject* coll) {
	return ((go->pos + go->vel * Application::GetFrameTime() - coll->pos + coll->vel * Application::GetFrameTime()).Length() < go->scale.x + coll->scale.x);
}

bool SceneAsteroid::ObjectOffScreen(GameObject* go) 
{
	if (go->pos.x > (m_worldWidth + go->scale.x)) {
		return true;
	}
	else if (go->pos.x < (0 - go->scale.x)) {
		return true;
	}
	else if (go->pos.y > (m_worldHeight + go->scale.y)) {
		return true;
	}
	else if (go->pos.y < (0 - go->scale.y)) {
		return true;
	}
	return false;
}

void SceneAsteroid::WrapObjectOnScreen(GameObject *go)
{
	go->pos.x = Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f);
	go->pos.y = Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f);
}

void SceneAsteroid::RenderGO(GameObject *go)
{

	if (go->isDamaged() && 
		go->type != GameObject::GO_BULLET) {

		float healthPercen = go->getHealth() / go->getMaxHealth();

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y - go->scale.y * 1.5, go->pos.z);
		modelStack.Scale(1.f, 0.2f, 1.f);
		RenderMesh(meshList[GEO_HEALTHBAR_OUTER], false);

			modelStack.PushMatrix();
				modelStack.Translate(-2.5*(1-healthPercen),0.f,0.f);
				modelStack.Scale(healthPercen, 1.f, 1.f);
				RenderMesh(meshList[GEO_HEALTHBAR], false);
			modelStack.PopMatrix();
		modelStack.PopMatrix();
	}

	modelStack.PushMatrix();
	modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	modelStack.Rotate(Math::RadianToDegree(atan2f(go->dir.y, go->dir.x)), 0.0f, 0.0f, 1.f);
	modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
	switch(go->type)
	{ 
	case GameObject::GO_SHIP:
		//Exercise 4a: render a sphere with radius 1
		RenderMesh(meshList[GEO_SHIP], false);

		//Exercise 17a: render a ship texture or 3D ship model
		//Exercise 17b:	re-orientate the ship with velocity
		break;
	case GameObject::GO_ASTEROID:
		//Exercise 4b: render a cube with length 2
		RenderMesh(meshList[GEO_ASTEROID], false);

		break;
	case GameObject::GO_BULLET:
	{
		Bullet* bullet = dynamic_cast<Bullet*>(go);
		if (bullet) {
			switch (bullet->bulletType) {
			case BULLET_BLUE:
				RenderMesh(meshList[GEO_BULLET], false);
				break;
			case BULLET_RED:
			case BULLET_HOMING:
				RenderMesh(meshList[GEO_BULLET_RED], false);
				break;
			}
		}
		break;
	}
	case GameObject::GO_ENEMY:
		RenderMesh(meshList[GEO_ENEMY_1], false);
		break;
		
	}
	modelStack.PopMatrix();
}

void SceneAsteroid::ShowHealthBar(GameObject* go) {

}

void SceneAsteroid::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	modelStack.PushMatrix();
		modelStack.Translate(m_worldWidth*0.5,m_worldHeight*0.5, -1.0);
		RenderMesh(meshList[GEO_SPACEBG], false);
	modelStack.PopMatrix();

	RenderGO(m_ship);
	for (auto& bullet : m_bulletList) {
		if (bullet->isActive()) {
			RenderGO(bullet);
		}
	}
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->isActive())
		{
			RenderGO(go);
		}
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if(enemies[i]->isActive())
			RenderGO(enemies[i]);
	}
	

	//On screen text
	std::ostringstream ss;

	//Exercise 5a: Render m_lives, m_score
	ss.str("");
	ss.precision(3);
	ss << "Health: " << m_ship->getHealth();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 27);

	ss.str("");
	ss.precision(3);
	ss << "Ship Rotation: " << m_ship->angle;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 24);

	//Exercise 5a: Render m_lives, m_score
	ss.str("");
	ss.precision(1);
	ss << "Lives: " << m_lives;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 21);

	ss.str("");
	ss.precision(3);
	ss << "Score: " << m_score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 18);

	//Exercise 5b: Render position, velocity & mass of ship
	ss.str("");
	ss.precision(5);
	ss << "Ship Pos: " << m_ship->pos;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 15);

	ss.str("");
	ss.precision(3);
	ss << "Ship Velocity: " << m_ship->vel;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str("");
	ss.precision(3);
	ss << "Mass of Ship: " << m_ship->mass;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str("");
	ss.precision(3);
	ss << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 6);
	
	ss.str("");
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);
	
	RenderTextOnScreen(meshList[GEO_TEXT], "Asteroid", Color(0, 1, 0), 3, 0, 0);
}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i] != nullptr) {
			delete enemies[i];
		}
	}
}
