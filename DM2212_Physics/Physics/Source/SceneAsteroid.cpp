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

	levelCompletedOnce = false;

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_NONE and add into m_goList
	for (int i = 0; i < 150; ++i)
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

	levelCompleted = false;
	levelAnimating = true;

	key_spawned = false;
	key_collected = false;

	shipMovementCD = 0;

	levelStartAnimationTimeLeft = 10.f;
	levelEndAnimationTimeLeft = 0.f;

	attackOrbAngle = 0.f;
	defenseOrbAngle = 0.f;
	healthOrbAngle = 0.f;

	wormHole = new GameObject(GameObject::GO_WORMHOLE);
	wormHole->setActive(true);
	wormHole->mass = 1.f;
	wormHole->scale = Vector3(0.1, 0.1, 0.1);
	wormHole->pos = Vector3(m_worldWidth * 0.5f, 10, 0);
	wormHole->momentOfInertia = wormHole->mass * wormHole->scale.x * wormHole->scale.x;
	wormHole->angularVelocity = 0.f;
	
	//Game Logic
	m_speed = 1.f;
	if(Application::ship_lives < 3) //One free life every stage completed
		Application::ship_lives++;
	
	//Default values for ship upon level start
	posTrackingTime = 0.05f;
	posTrackingCD = posTrackingTime;

	freezeTrackingTill = elapsed;
	freezeTrackingCD = 0.f;

	//Flush any existing weapons
	m_ship->flushWeapons();

	//Set primary weapon with 0.15s cooldown as the Default weapon
	m_ship->setWeapon(PRIMARY, WEAPON_TYPE::WEAPON_DEFAULT, 0.15f);
	m_ship->setCurrentWeapon(PRIMARY);
	m_ship->getCurrentWeapon()->setLastBulletShot(elapsed);
	m_ship->getCurrentWeapon()->setAllTargettable(true);
	m_ship->getCurrentWeapon()->setTargettable(TARGET_PLAYER, false);
	m_ship->getCurrentWeapon()->setBulletSpeed(50.f);
	m_ship->angle = 90;
	m_ship->dir = Vector3(cosf(Math::DegreeToRadian(m_ship->angle)), sinf(Math::DegreeToRadian(m_ship->angle)), 0);
	m_ship->vel = Vector3(0, 0, 0);
	m_ship->setActive(true);
	m_ship->setHealth(SHIP_BASE_HEALTH);
	m_ship->mass = 1.f;
	m_ship->scale = Vector3(5.0, 5.0, 5.0);
	if (levelAnimating)
	{
		m_ship->scale = Vector3(0.1, 0.1, 0.1);
		m_ship->pos = wormHole->pos;
	}
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
	return m_bulletList.at(m_bulletList.size() - 10);
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

	if (key_collected && !levelCompleted)
	{
		if (wormHole->isActive() == false)
		{
			wormHole->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight - 10, 0);
			wormHole->setActive(true);
		}
		if (wormHole->scale.LengthSquared() < 81)
		{
			wormHole->scale += Vector3(1.63333 * dt, 1.63333 * dt, 0);
		}
		wormHole->angle += 360 * dt;
		if (wormHole->angle > 360) wormHole->angle = 0;
		wormHole->dir = Vector3(cosf(Math::DegreeToRadian(wormHole->angle)), sinf(Math::DegreeToRadian(wormHole->angle)), 0);
	}

	if (levelAnimating)
	{
		if (levelStartAnimationTimeLeft > 0)
		{
			levelStartAnimationTimeLeft -= dt;
			if (levelStartAnimationTimeLeft > 7.5f)
			{
				wormHole->scale += Vector3(2.25 * dt, 2.25 * dt, 0);
			}
			wormHole->angle += 360 * dt;
			if (wormHole->angle > 360) wormHole->angle = 0;
			wormHole->dir = Vector3(cosf(Math::DegreeToRadian(wormHole->angle)), sinf(Math::DegreeToRadian(wormHole->angle)), 0);

			if (levelStartAnimationTimeLeft > 2.0f && levelStartAnimationTimeLeft < 5.0f)
			{
				m_ship->scale += Vector3(1.63333 * dt, 1.63333 * dt, 0);
			}

			if (levelStartAnimationTimeLeft <= 1.9f)
			{
				wormHole->scale -= Vector3(2.578f * dt, 2.578f * dt, 0);
			}

			if (levelStartAnimationTimeLeft <= 0 )
			{
				levelAnimating = false;
				wormHole->setActive(false);
				m_ship->scale = Vector3(5, 5, 5);
				this->afterLevelAnimation();
			}
		}
		else if (levelEndAnimationTimeLeft > 0)
		{
			m_ship->vel = Vector3(0, 0, 0);
			levelEndAnimationTimeLeft -= dt;
			if (levelEndAnimationTimeLeft > 0.0f)
			{
				Vector3 dir = Vector3(m_worldWidth * 0.5, m_worldHeight - 10, 0) - m_ship->pos;
				if (!dir.IsZero())
				{
					dir = dir.Normalized() * 6;
					m_ship->pos += dir * dt;
				}
			}

			if (levelEndAnimationTimeLeft < 2.0f)
			{
				m_ship->scale -= Vector3(2.45 * dt, 2.45 * dt, 0);
				wormHole->scale -= Vector3(2.45 * dt, 2.45 * dt, 0);
				if (levelEndAnimationTimeLeft < 0) {
					levelAnimating = false;
					wormHole->setActive(false);
					levelCompleted = true;
					levelCompletedOnce = true;

					switch (static_cast<SCENE_TYPE>(Application::currentScene))
					{
					case SCENE_LEVEL_ONE_ONE:
						Application::setCurrentScene(SCENE_LEVEL_ONE_TWO);
						break;
					case SCENE_LEVEL_ONE_TWO:
						Application::setCurrentScene(SCENE_LEVEL_ONE_BOSS);
						break;
					case SCENE_LEVEL_ONE_BOSS:
						Application::resetGame();
						
						break;
					}
					LoadDefaultGameValues();
				}

			}

		}
	}

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	m_force.SetZero();
	m_torque.SetZero();
	//Exercise 6: set m_force values based on WASD

	shipMovementCD -= 1 * dt;
	if (!levelAnimating && shipMovementCD <= 0)
	{
		if (Application::IsKeyPressed('W'))
		{
			m_force += 50 * m_ship->dir;
			m_torque = Vector3(0, -1, 0).Cross(Vector3(0, 50, 0));
		}
		if (Application::IsKeyPressed('S'))
		{
			m_force += -50 * m_ship->dir;
			m_torque = Vector3(0, 1, 0).Cross(Vector3(0, -50, 0));
		}
		if (Application::IsKeyPressed('A'))
		{
			m_force += 23 * m_ship->dir;
			m_torque = Vector3(1, -1, 0).Cross(Vector3(23, 0, 0));
		}
		if (Application::IsKeyPressed('D'))
		{
			m_force += 23 * m_ship->dir;
			m_torque = Vector3(-1, -1, 0).Cross(Vector3(-23, 0, 0));
		}
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
	this->WrapObjectOnScreen(m_ship, dt);

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
			Vector3 dir = (bullet->targetGO->pos)  - bullet->pos;

			if (!dir.IsZero())
			{
				float shipAngle = Math::RadianToDegree(atan2f(bullet->dir.y, bullet->dir.x));
				float dirAngle = Math::RadianToDegree(atan2f(dir.y, dir.x));
				float diff = dirAngle - shipAngle;
				float MAX_ANGLE = 20.f;
				if (dir.LengthSquared() < 25 * 25) MAX_ANGLE = 11.f;
				if (diff > MAX_ANGLE * dt) {
					diff = MAX_ANGLE * dt;
				}
				else if (diff < -MAX_ANGLE * dt) {
					diff = -MAX_ANGLE * dt;
				}
				dir.Set(cosf(Math::DegreeToRadian(shipAngle+diff)), sinf(Math::DegreeToRadian(shipAngle + diff)));
				bullet->dir = dir.Normalized() * bullet->weaponUsed->getBulletSpeed();
				bullet->dir.Normalize();
				bullet->vel = bullet->dir * bullet->weaponUsed->getBulletSpeed();
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
		if (enemies[i]->isActive())
		{
			float multi = 1.f;
			if (m_shipPowerUps[POWERUP_TIME].durationRemaining > 0)
				multi = POWERUP_SLOWTIME_SPEED;
			if(enemies[i]->isMoveable && enemies[i]->movementCooldown <= 0)
				enemies[i]->pos += enemies[i]->vel * dt * m_speed * multi;
		}
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

	std::vector<Orb*>::iterator mIterator;
	for (mIterator = m_shipOrbs.begin(); mIterator != m_shipOrbs.end(); ++mIterator) {
		(*mIterator)->timeAliveRemaining -= dt;
		(*mIterator)->angleFromShip += 20 * (*mIterator)->displacement * dt;
		
		Vector3 newDir = Vector3(cosf(Math::DegreeToRadian((*mIterator)->angleFromShip)),
			sinf(Math::DegreeToRadian((*mIterator)->angleFromShip)), 0);
		(*mIterator)->pos = m_ship->pos + newDir.Normalized() * (*mIterator)->displacement;

		if ((*mIterator)->timeAliveRemaining <= 0 || !(*mIterator)->isActive()) {
			delete* mIterator;
			*mIterator = nullptr;
		}
	}
	m_shipOrbs.erase(std::remove(m_shipOrbs.begin(), m_shipOrbs.end(), nullptr), m_shipOrbs.end());


	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if(enemies[i]->isActive())
			tryShoot(enemies[i]);
	}

	if (levelAnimating || key_collected)
		HandleCollision(wormHole, dt);

	for (auto& bullet : m_bulletList) {
		HandleCollision(bullet, dt);
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		HandleCollision(enemies[i], dt);
	}
	for (auto& go : m_goList) {
		HandleCollision(go, dt);

		//Passively subtract health from powerups if any are active
		if (go->type == GameObject::GO_POWERUP_GUNDOUBLE
			|| go->type == GameObject::GO_POWERUP_GUNQUAD ||
			go->type == GameObject::GO_POWERUP_HEALTH ||
			go->type == GameObject::GO_POWERUP_TIME)
		{
			go->subtractHealth(1 * dt);
			if (go->getHealth() <= 0)
			{
				go->setActive(false);
			}
		}
	}

	for (int i = 0; i < POWERUP_COUNT; ++i)
	{
		if (m_shipPowerUps[i].durationRemaining > 0.f)
		{
			m_shipPowerUps[i].durationRemaining -= dt;
			if (m_shipPowerUps[i].durationRemaining <= 0)
			{
				m_shipPowerUps[i].durationRemaining = 0;
				switch (static_cast<POWERUP_TYPE>(i))
				{
				case POWERUP_GUNDOUBLE:
				case POWERUP_GUNQUAD:
					m_ship->getCurrentWeapon()->setType(WEAPON_DEFAULT);
					break;

				case POWERUP_TIME:
					m_speed = 1.f;
					break;
				}
			}

			switch (static_cast<POWERUP_TYPE>(i))
			{
			case POWERUP_HEALTH:
			{
				if (m_ship->getHealth() < m_ship->getMaxHealth())
				{
					m_ship->addHealth(5.f * dt);
				}
			}
			break;
			}
		}	
	}		
	
	if (m_ship->getHealth() <= 0) {
		Application::ship_lives--;
		m_ship->setHealth(SHIP_BASE_HEALTH);
	}

	if (Application::ship_lives <= 0)
	{
		Application::resetGame();
	}
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
	case GameObject::GO_PAPA_ASTEROID:
		obj->scale = Vector3(6, 6, 6);
		obj->mass = 2.f;
		obj->angularVelocity = Math::RandFloatMinMax(-0.5f, 0.5f);
		obj->setHealth(BIG_ASTEROID_HEALTH);
		obj->type = GameObject::GO_PAPA_ASTEROID;
		obj->vel = Vector3(Math::RandFloatMinMax(-3.f, 3.f), Math::RandFloatMinMax(-3.f, 3.f));
		break;
	default:
		return false;
	}

	//Finding a spot for the Asteroid
	bool occupied = true;
	for (int i = 0; i < 2; ++i)
	{
		obj->pos = Vector3(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0.f, m_worldHeight), 0);
		occupied = IsSpaceOccupied(obj, obj->pos);
		if (occupied == false) break;
	}
	if (occupied == false) {
		obj->setActive(true);
		return true;
	}

	return false;
}


bool SceneAsteroid::IsSpaceOccupied(GameObject* obj, Vector3 newPos)
{
	Vector3 oldPos = obj->pos;
	obj->pos = newPos;
	bool freeSpot = true;
	for (auto& go : m_goList) {
		if (obj != go && go->isActive() && Collided(obj, go))
		{
			freeSpot = false;
			break;
		}
	}
	if (obj != m_ship && freeSpot && Collided(obj, m_ship))
	{
		freeSpot = false;
	}
	if (freeSpot)
	{
		for (int i = 0; i < MAX_ENEMIES; ++i)
		{
			if (obj != enemies[i] && enemies[i]->isActive() && Collided(enemies[i], obj))
			{
				freeSpot = false;
				break;
			}
		}
	}
	obj->pos = oldPos;
	return !freeSpot;
}

Vector3 SceneAsteroid::getRandomOutOfBoundsPosition(float radius)
{
	Vector3 pos = Vector3(0, 0, 0);
	if (Math::RandIntMinMax(0, 2) == 0)
	{
		switch (Math::RandIntMinMax(0, 2))
		{
		case 0:
			pos.x = Math::RandFloatMinMax(-radius, -radius * 0.5);
			pos.y = Math::RandFloatMinMax(0, m_worldHeight);
			break;
		case 1:
			pos.x = Math::RandFloatMinMax(radius * 0.5 + m_worldWidth, m_worldWidth + radius);
			pos.y = Math::RandFloatMinMax(0, m_worldHeight);
			break;
		}
	}
	else
	{
		switch (Math::RandIntMinMax(0, 2))
		{
		case 0:
			pos.y = Math::RandFloatMinMax(-radius, -radius * 0.5);
			pos.x = Math::RandFloatMinMax(0, m_worldWidth);
			break;
		case 1:
			pos.y = Math::RandFloatMinMax(radius * 0.5 + m_worldHeight, m_worldHeight + radius);
			pos.x = Math::RandFloatMinMax(0, m_worldWidth);
			break;
		}
	}

	return pos;
}

int SceneAsteroid::getActiveEnemyCount() {
	int a = 0;
	for (int i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i]->isActive())
			a++;
	}
	return a;
}

void SceneAsteroid::HandleCollision(GameObject* go, double dt)
{
	if (!go->isActive()) return;

	if (go->movementCooldown >= 0) {
		go->movementCooldown -= dt;
	}
	
	//WormHole Collision
	if (go->type == GameObject::GO_WORMHOLE && key_collected && go->isActive())
	{
		if (Collided(go, m_ship) && !levelAnimating)
		{
			levelEndAnimationTimeLeft = 5.0f;
			levelAnimating = true;
		}
	}

	//Bullet Collision
	if (go->type == GameObject::GO_BULLET)
	{
		Bullet* bullet = dynamic_cast<Bullet*>(go);
		if (bullet)
		{
			float multi = 1.f;
			if (m_shipPowerUps[POWERUP_TIME].durationRemaining > 0 && bullet->targetGO == m_ship)
				multi = POWERUP_SLOWTIME_SPEED;
			bullet->pos += bullet->vel * dt * m_speed * multi;
			if (this->ObjectOffScreen(bullet)) {
				bullet->setActive(false);
			}
			if (!bullet->isActive()) return;

			for (auto& orb : m_shipOrbs)
			{
				if (Collided(bullet, orb) && CanDamage(bullet->parentGO, m_ship))
				{
					orb->setActive(false);
					bullet->setActive(false);
				}
			}
			if (!bullet->isActive()) return;

			for (auto& coll : m_goList) {
				if (coll->isActive())
				{
					//Bullet to ASteroid collision
					if ( (coll->type == GameObject::GO_ASTEROID || coll->type == GameObject::GO_PAPA_ASTEROID) && CanDamage(bullet->parentGO, coll)) {
						if (Collided(bullet, coll)) {
							coll->subtractHealth(bullet->weaponUsed->getDefaultBulletDMG() * bullet->damageMultiplier);
							bullet->setActive(false);
							if (coll->getHealth() <= 0) {
								//Game Logic: Delete Aesteroid & Bullet, add 2 to score
								coll->setActive(false);
								Application::gameScore += 2;
								if (coll->type == GameObject::GO_ASTEROID && bullet->weaponUsed == m_ship->getCurrentWeapon()) {
									if (m_shipOrbs.size() < 25)
									{
										ORB_TYPE type = static_cast<ORB_TYPE>(Math::RandIntMinMax(0, ORB_COUNT - 1));
										float timeAlive = Math::RandFloatMinMax(5.0f, 10.f);
										Orb* orb = new Orb(type, m_ship, 0, 0, timeAlive);
										orb->setActive(true);
										orb->scale = m_ship->scale * 0.1;
										switch (type) 
										{
										case ORB_ATTACK:
											orb->displacement = 6.0f;
											orb->angleFromShip = attackOrbAngle++;
											break;
										case ORB_DEFENSE:
											orb->displacement = 7.2f;
											orb->angleFromShip = defenseOrbAngle++;
											break;
										case ORB_HEALTH:
											orb->displacement = 8.4f;
											orb->angleFromShip = healthOrbAngle++;
											break;
										}
        								m_shipOrbs.push_back(orb);								}
								}

								//Asteroid Splitting
								else if (coll->type == GameObject::GO_PAPA_ASTEROID)
								{
									Application::gameScore += 3;
									Vector3 totalMomentum = coll->vel * coll->mass;
									int numSplits = 3;
									GameObject* newAsteroid;
									for (int i = 0; i < numSplits; ++i)
									{
										newAsteroid = FetchGO();
										Vector3 newVel = Vector3(cosf(i / (double)(numSplits + 1) * 360.0), sinf(i / (double)(numSplits + 1) * 360.0), 0) * 4;
										newAsteroid->setActive(true);
										newAsteroid->scale = Vector3(3, 3, 3);
										newAsteroid->mass = 1.f;
										newAsteroid->angularVelocity = Math::RandFloatMinMax(-0.5f, 0.5f);
										newAsteroid->setHealth(SMALL_ASTEROID_HEALTH);
										newAsteroid->type = GameObject::GO_ASTEROID;
										totalMomentum += newVel * newAsteroid->mass;
										newAsteroid->pos = coll->pos + Vector3(cosf(i / (double) (numSplits+1) * 360.0) , sinf(i / (double)(numSplits + 1) * 360.0), 0) * newAsteroid->scale.x * 1.25;
										if (i == numSplits - 1)
											newAsteroid->vel = -totalMomentum;
										else
											newAsteroid->vel = newVel;
									}

									//Spawn Powerup
									if (Math::RandIntMinMax(0, 0) == 0)
									{
										SpawnPowerUpAtLocation(coll->pos);
									}
								}

							}

							break;
						}
					}
				}
			}
			if (!bullet->isActive()) return;

			//Go object should be able to know what its parent (Aka weapon for the bullet)
			if (Collided(bullet, m_ship) && CanDamage(bullet->parentGO, m_ship) && !levelAnimating) {
				m_ship->subtractHealth(bullet->weaponUsed->getDefaultBulletDMG() * bullet->damageMultiplier);
				bullet->setActive(false); //destroy bullet
			}

			if (!bullet->isActive()) return;
			for (int i = 0; i < MAX_ENEMIES; ++i) {
				if (enemies[i]->isActive()) {
					if (Collided(bullet, enemies[i])) {
						if (CanDamage(bullet->parentGO, enemies[i])) {
							bullet->setActive(false);
							enemies[i]->subtractHealth(bullet->weaponUsed->getDefaultBulletDMG() * bullet->damageMultiplier);
							if (enemies[i]->getHealth() <= 0) {
								enemies[i]->setActive(false);
								Application::gameScore += 100;
							}
						}
					}
				}
			}

		}
	}

	//Asteroid Collision Handling
	else if (go->type == GameObject::GO_ASTEROID || go->type == GameObject::GO_PAPA_ASTEROID)
	{
		//Asteroid Handling		
		float multi = 1.f;
		if (m_shipPowerUps[POWERUP_TIME].durationRemaining > 0)
			multi = POWERUP_SLOWTIME_SPEED;
		go->pos += go->vel * dt * m_speed * multi;

		//Ship Collision
		if (Collided(go, m_ship))
		{
			HandleBounceOffCollision(go, m_ship, false, true, 1.f, 1.0f, dt);
			shipMovementCD = 0.20f;
		}
		for (int i = 0; i < 10; ++i)
		{
			if (enemies[i]->isActive() && Collided(go, enemies[i]))
			{
				HandleBounceOffCollision(go, enemies[i], false, false, 1.f, 1.0f, dt);
				go->movementCooldown = 0.20f;
			}
		}
		for (auto& coll : m_goList) {
			if (go != coll && coll->isActive() && Collided(go, coll))
				HandleBounceOffCollision(go, coll, false, false, 1.f, 1.0f, dt);
		}
		

		this->WrapObjectOnScreen(go, dt);
	}

	// Enemy Collision Handling
	else if (go->type == GameObject::GO_ENEMY) 
	{
		if (Collided(go, m_ship))
		{
			HandleBounceOffCollision(go, m_ship, true, true, 1.0f, 1.0f, dt);
			go->movementCooldown = 0.20f;
		}
		
		Enemy* enemy = dynamic_cast<Enemy*>(go);
		if(enemy->arrivedOnScreen)
			this->WrapObjectOnScreen(go, dt);
	}

	// Gun Double Handling
	else if (go->type == GameObject::GO_POWERUP_GUNDOUBLE)
	{
		if (Collided(go, m_ship))
		{
			if (m_shipPowerUps[POWERUP_GUNQUAD].durationRemaining <= 0)
			{
				m_shipPowerUps[POWERUP_GUNDOUBLE].durationRemaining += 20.f;
				m_ship->getCurrentWeapon()->setType(WEAPON_DOUBLEGUN);
				go->setActive(false);
			}
			else
			{
				HandleBounceOffCollision(go, m_ship, false, false, 0.f, 1.f, dt);
			}
		}
		this->WrapObjectOnScreen(go, dt);
	}

	// Gun Quad Handling
	else if (go->type == GameObject::GO_POWERUP_GUNQUAD)
	{
		if (Collided(go, m_ship))
		{
			if (m_shipPowerUps[POWERUP_GUNDOUBLE].durationRemaining <= 0)
			{
				m_shipPowerUps[POWERUP_GUNQUAD].durationRemaining += 20.f;
				m_ship->getCurrentWeapon()->setType(WEAPON_QUADGUN);
				go->setActive(false);
			}
			else
			{
				HandleBounceOffCollision(go, m_ship, false, false, 0.f, 1.f, dt);
			}
		}
		this->WrapObjectOnScreen(go, dt);
	}

	// Health Handling
	else if (go->type == GameObject::GO_POWERUP_HEALTH)
	{
		if (Collided(go, m_ship))
		{
			m_shipPowerUps[POWERUP_HEALTH].durationRemaining += 3.f;
			go->setActive(false);
		}
		this->WrapObjectOnScreen(go, dt);
	}
	
	// Power Time
	else if (go->type == GameObject::GO_POWERUP_TIME)
	{
		if (Collided(go, m_ship))
		{
			m_shipPowerUps[POWERUP_TIME].durationRemaining += 6.f;
			go->setActive(false);
		}
		this->WrapObjectOnScreen(go, dt);
	}

	// Keycard Handling
	else if (go->type == GameObject::GO_KEYCARD)
	{
		if (Collided(go, m_ship))
		{
			go->setActive(false);
			key_collected = true;
		}
	}
}

bool SceneAsteroid::ObjectIsWithinScreen(GameObject* go)
{
	Vector3 pos = go->pos;
	if (go->pos.x > m_worldWidth + go->scale.x ||
		go->pos.x <= 0 - go->scale.x ||
		go->pos.y > m_worldHeight + go->scale.y ||
		go->pos.y <= 0 - go->scale.y)
		return false;
	return true;
}

void SceneAsteroid::SpawnPowerUpAtLocation(Vector3 pos)
{
	GameObject* pU = FetchGO();
	pU->setActive(true);
	pU->scale = Vector3(2.5, 2.5, 2.5);
	pU->mass = 1.f;
	pU->setHealth(60.f); //Claimable within 20 seconds
	pU->pos = pos;
	pU->type = static_cast<GameObject::GAMEOBJECT_TYPE>(Math::RandIntMinMax(0 + GameObject::GO_POWERUP_TIME, POWERUP_COUNT -1 + GameObject::GO_POWERUP_TIME));
}

void SceneAsteroid::afterLevelAnimation() {

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
void SceneAsteroid::HandleBounceOffCollision(GameObject* go, GameObject* coll, bool damageGO, bool damageCOLL, float minImpactForDamage, float impactMultiplier, double dt)
{	
	if ((coll == m_ship || coll->type == GameObject::GO_WORMHOLE) && levelAnimating)
	{
		go->vel *= -1;
		if (go->vel.LengthSquared() < 3 * 3)
			go->vel = go->vel.Normalized() * 3;
		go->pos += go->vel * 10 * dt;
		return;
	}
	else if((go == m_ship || go->type == GameObject::GO_WORMHOLE) && levelAnimating) 
	{
		coll->vel *= -1;
		coll->pos += coll->vel * 10 * dt;
		return;
	}

	Vector3 dir = go->vel - coll->vel;
	float impact = 0.f;
	if (dir.LengthSquared() > 1.0f) {

		static int collision_formula = 2;
		float a1 = go->vel.Dot(dir);
		float a2 = coll->vel.Dot(dir);

		float mag = (2.0 * (a1 - a2)) / (2.0);

		if (collision_formula == 0)
		{
			//Wikipedea formula for Linear, does not account for angular velocity
			Vector3 go_uVel = go->vel;
			Vector3 coll_uVel = coll->vel;
			go->vel = ((go->mass - coll->mass) / (go->mass + coll->mass)) * go_uVel
				+ ((2*coll->mass) / (go->mass + coll->mass)) * coll_uVel;

			coll->vel = ((2 * go->mass) / (go->mass + coll->mass)) * go_uVel +
				((coll->mass - go->mass) / (go->mass + coll->mass)) * coll_uVel;
		}
		else if (collision_formula == 1)
		{
			go->vel = go->vel - mag * dir;
			coll->vel = coll->vel + mag * dir;
		}
		else if (collision_formula == 2)
		{

			//Wikipedea forumula for Angular, similar to collision form 1 which was from stack overflow
			
			Vector3 u1 = go->vel;
			Vector3 u2 = coll->vel;
			float m1 = go->mass;
			float m2 = coll->mass;

			go->vel = u1 - (2 * m2 / (m1 + m2)) * (((u1 - u2).Dot(go->pos - coll->pos)) / (go->pos - coll->pos).LengthSquared()) * (go->pos - coll->pos);
			coll->vel = u2 - (2 * m1 / (m1 + m2)) * (((u2 - u1).Dot(coll->pos - go->pos)) / (coll->pos - go->pos).LengthSquared()) * (coll->pos - go->pos);
		}

		impact = dir.Length();
		dir.Normalize();


		impact *= impactMultiplier;
		if (impact > minImpactForDamage)
		{
			a1 = Math::FAbs(a1);
			a2 = Math::FAbs(a2);
			if (damageGO)
			{
				double health = impact * ((a2 + 1) / (a1 + a2 + 2.0));
				go->subtractHealth(health);
				if (go->getHealth() <= 0) {
					go->setActive(false);
				}
			}
			if (damageCOLL)
			{
				double health = impact * ((a1 + 1) / (a1 + a2 + 2.0));
				coll->subtractHealth(health);
				if (coll->getHealth() <= 0) {
					coll->setActive(false);
				}
			}
		}
	}
}

/**
 * Handles Shooting systems for both m_ship and Enemies.
 * 
 * \param go
 * \return 
 */
bool SceneAsteroid::tryShoot(GameObject* go) {
	if (!levelAnimating && elapsed - go->getCurrentWeapon()->getLastBulletShot() > go->getCurrentWeapon()->getBulletCD())
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

		float scaleMulti = 1.0f;
		float damageMulti = 1.0f;
		
		switch (go->getCurrentWeapon()->getWeaponType()) {
		case WEAPON_DEFAULT:
		{ //Scope
			Bullet* unused = FetchBulletGO();

			unused->setActive(true);
			unused->type = GameObject::GO_BULLET;
			unused->scale = Vector3(0.5f, 0.5f, 0.5f) * scaleMulti;
			unused->parentGO = go;
			unused->bulletType = bType;
			unused->damageMultiplier = damageMulti;
			unused->weaponUsed = go->getCurrentWeapon();
			unused->pos = go->pos + go->dir * (go->scale.x * 0.63f);
			unused->vel = go->dir * go->getCurrentWeapon()->getBulletSpeed();
			break;
		}

		case WEAPON_HOMING:
		{ //Scope
			Bullet* unused = FetchBulletGO();

			unused->setActive(true);
			unused->type = GameObject::GO_BULLET;
			unused->scale = Vector3(2.4f, 2.4f, 2.4f) * scaleMulti;
			unused->parentGO = go;
			unused->bulletType = BULLET_HOMING;
			unused->targetGO = m_ship;
			unused->damageMultiplier = damageMulti;
			unused->dir = go->dir;
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
				unused->scale = Vector3(0.5f, 0.5f, 0.5f) * scaleMulti;
				unused->parentGO = go;
				unused->bulletType = bType;
				unused->damageMultiplier = damageMulti;
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
				unused->scale = Vector3(0.5f, 0.5f, 0.5f) * scaleMulti;
				unused->parentGO = go;
				unused->bulletType = bType;
				unused->damageMultiplier = damageMulti;
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
		case GameObject::GO_PAPA_ASTEROID:
			return true;
		}
		break;

	case GameObject::GO_ENEMY:
		switch (victim->type) {
		case GameObject::GO_SHIP:
		case GameObject::GO_ASTEROID:
		case GameObject::GO_PAPA_ASTEROID:
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

void SceneAsteroid::WrapObjectOnScreen(GameObject *go, double dt)
{

	Vector3 futurePos = Vector3(Math::Wrap(go->pos.x, 0 - go->scale.x * 0.5f, m_worldWidth + go->scale.x * 0.5f),
		Math::Wrap(go->pos.y, 0 - go->scale.y * 0.5f, m_worldHeight + go->scale.y * 0.5f),
		go->pos.z);
	if (go->pos != futurePos) //Wrapping occured
	{
		if (IsSpaceOccupied(go, futurePos))
		{
			go->vel *= -1;
			go->pos += go->vel * dt * 5;
		}
		else
		{
			go->pos = futurePos;
		}
	}
	else
	{
		go->pos = futurePos;
	}
	
}

void SceneAsteroid::RenderGO(GameObject *go)
{

	if (go->isDamaged() && 
		go->type != GameObject::GO_BULLET) {

		float healthPercen = go->getHealth() / go->getMaxHealth();

		modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y - go->scale.x * 1.5, go->pos.z);
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
		RenderMesh(meshList[GEO_SHIP], false);
		break;
	case GameObject::GO_ASTEROID:
	case GameObject::GO_PAPA_ASTEROID:
		RenderMesh(meshList[GEO_ASTEROID], false);
		break;
	case GameObject::GO_KEYCARD:
		RenderMesh(meshList[GEO_KEYCARD], false);
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
				RenderMesh(meshList[GEO_BULLET_RED], false);
				break;
			case BULLET_HOMING:
				RenderMesh(meshList[GEO_BULLET_HOMING], false);
				break;
			}
		}
		break;
	}

	case GameObject::GO_WORMHOLE:
		RenderMesh(meshList[GEO_WORMHOLE], false);
		break;

	case GameObject::GO_ENEMY:
	{
		Enemy* enemy = dynamic_cast<Enemy*>(go);
		if (enemy)
		{
			switch (enemy->enemy_type)
			{
			case ENEMY_1:
				RenderMesh(meshList[GEO_ENEMY_1], false);
				break;
			case ENEMY_2:
				RenderMesh(meshList[GEO_ENEMY_2], false);
				break;
			case ENEMY_BOSS:
				RenderMesh(meshList[GEO_ENEMY_BOSS_1], false);
				break;
			}
		}

		break;
	}

	case GameObject::GO_ORB:
	{
		Orb* orb = dynamic_cast<Orb*>(go);
		switch (orb->orbType)
		{
		case ORB_ATTACK:
			RenderMesh(meshList[GEO_POWER_ORB], false);
			break;
		case ORB_DEFENSE:
			RenderMesh(meshList[GEO_DEFENSE_ORB], false);
			break;
		case ORB_HEALTH:
			RenderMesh(meshList[GEO_HEALTH_ORB], false);
			break;
		}
		break;
	}

	//Power UPs
	case GameObject::GO_POWERUP_GUNDOUBLE:
		RenderMesh(meshList[GEO_POWERUP_GUNDOUBLE], false);
		break;
	case GameObject::GO_POWERUP_GUNQUAD:
		RenderMesh(meshList[GEO_POWERUP_GUNQUAD], false);
		break;
	case GameObject::GO_POWERUP_HEALTH:
		RenderMesh(meshList[GEO_POWERUP_HEALTH], false);
		break;
	case GameObject::GO_POWERUP_TIME:
		RenderMesh(meshList[GEO_POWERUP_TIME], false);
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

	if(wormHole->isActive())
		RenderGO(wormHole);

	for (auto& orb : m_shipOrbs)
	{
		RenderGO(orb);
	}

	//On screen text
	std::ostringstream ss;

	//Exercise 5a: Render Application::ship_lives, Application::gameScore
	for (int i = 0; i < Application::ship_lives; ++i)
	{
		modelStack.PushMatrix();
			modelStack.Translate(10 + i * 12, 12, 1.0);
			modelStack.Scale(6.0f, 6.f, 6.f);
			RenderMesh(meshList[GEO_SHIP_LIVES], false);
		modelStack.PopMatrix();
	}
	int displayed = 0;
	for (int i = 0; i < POWERUP_COUNT; ++i) {
		if (m_shipPowerUps[i].durationRemaining > 0.f) {
			modelStack.PushMatrix();
			modelStack.Translate(m_worldWidth - 10, 12 + 16 * displayed, 1.0);
			modelStack.Scale(6.0f, 6.f, 6.f);
			
			
			switch (static_cast<POWERUP_TYPE>(i))
			{
			case POWERUP_GUNDOUBLE:
				RenderMesh(meshList[GEO_POWERUP_GUNDOUBLE], false);
				break;
			case POWERUP_GUNQUAD:
				RenderMesh(meshList[GEO_POWERUP_GUNQUAD], false);
				break;
			case POWERUP_TIME:
				RenderMesh(meshList[GEO_POWERUP_TIME], false);
				break;
			case POWERUP_HEALTH:
				RenderMesh(meshList[GEO_POWERUP_HEALTH], false);
				break;

			}
			modelStack.PopMatrix();

			ss.str("");
			ss.precision(3);
			ss << m_shipPowerUps[i].durationRemaining << "s";
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1.0f, 1.0f, 1.0f), 2, 73, 3.3 + displayed * 9.6f);
			displayed++;
		}
	}




	ss.str("");
	ss.precision(3);
	ss << "Score: " << Application::gameScore;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1.0f, 1.0f, 1.0f), 3, 0, 15);

	ss.str("");
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1.0f, 1.0f, 1.0f), 3, 0, 12);

	ss.str("");
	ss.precision(5);
	ss << "Objective: " << this->getObjective();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1.0f, 1.0f, 1.0f), 2, 0, 58);

	ss.str("");
	ss.precision(5);
	ss << this->getBossBarMessage();
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0.7f, 1.0f, 0.0f), 2, 23, 55);
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

	while (m_bulletList.size() > 0)
	{
		GameObject* go = m_bulletList.back();
		delete go;
		m_bulletList.pop_back();
	}

	while (m_shipOrbs.size() > 0)
	{
		Orb* orb = m_shipOrbs.back();
		delete orb;
		m_shipOrbs.pop_back();
	}

	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
	if (wormHole)
	{
		delete wormHole;
		wormHole = NULL;
	}
	for (int i = 0; i < MAX_ENEMIES; ++i) {
		if (enemies[i] != nullptr) {
			delete enemies[i];
		}
	}
}
