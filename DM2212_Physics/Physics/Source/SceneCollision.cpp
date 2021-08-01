#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

//My own physics implemented
//Prevent internal clipping with walls - see CollisionResponse for GO_BALL to GO_WALL
//

//TODO
//Friction
//Circular Motion

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_ballCount = 0;
	elapsed = 0.0;
	score = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	flipper_speed = 2.0;
	m_worldGravity = Vector3(0, -30, 0);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	ballsRemaining = 3;
	canSpawnBall = true;
	ballOnSpring = false;

	//Level Layout

	GameObject* go;

	//Outerwalls
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->setActive(true);
	go->scale.Set(1.4, 50, 1);
	go->normal.Set(cos(Math::DegreeToRadian(0)), sin(Math::DegreeToRadian(0)), 0);
	go->pos = Vector3(m_worldWidth * 0.703, m_worldHeight * 0.5, 0);

	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->setActive(true);
	go->scale.Set(1.4, 50, 1);
	go->normal.Set(cos(Math::DegreeToRadian(0)), sin(Math::DegreeToRadian(0)), 0);
	go->pos = Vector3(m_worldWidth * 0.297, m_worldHeight * 0.5, 0);

	//Ball Spawn Area
	this->BuildThickWall(
		Vector3(0.71, 20, 1)
		, Vector3(cos(0), sin(0), 0)
		, Vector3(m_worldWidth * 0.68, m_worldHeight * 0.20, 0)
	);

	spring = FetchGO();
	spring->type = GameObject::GO_SPRING;
	spring->setActive(true);
	spring->scale.Set(7, 1.4, 1);
	spring->normal.Set(cos(Math::DegreeToRadian(90)), sin(Math::DegreeToRadian(90)), 0);
	spring->pos = Vector3(m_worldWidth * 0.69, m_worldHeight * 0.07, 0);
	spring->maxSpringLength = 3;
	spring->springMagnitude = 6;
	spring->origin = spring->pos;

	int slices;
	int startAngle;
	double angleStepUp;

	//Ball Spawn Starting Curve
		//Inward arc at Bottom Right of map
	slices = 10;
	startAngle = 90;
	angleStepUp = -60 / (double)slices;
	for (int i = 0; i <= slices; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->setActive(true);
		go->scale.Set(1, 1, 1);
		go->normal.Set(cos(Math::DegreeToRadian(startAngle + angleStepUp * i)), sin(Math::DegreeToRadian(startAngle + angleStepUp * i)), 0);
		go->pos = Vector3(m_worldWidth * 0.667, m_worldHeight * 0.4, 0) + go->normal * 7 * go->scale.y;
	}

	//On Top of Ball Spawn Curved Wall
	this->BuildThickWall(
		Vector3(1.5, 3.15, 1)
		, Vector3(cos(30), sin(30), 0)
		, Vector3(m_worldWidth * 0.68, m_worldHeight * 0.475, 0)
	);
	//Trigger
	go = FetchGO();
	go->type = GameObject::GO_BALLEXIT;
	go->setActive(true);
	go->scale.Set(0.71, 3, 1);
	go->normal.Set(cos(Math::DegreeToRadian(0)), sin(Math::DegreeToRadian(0)), 0);
	go->pos = Vector3(m_worldWidth * 0.68, m_worldHeight * 0.43, 0);

	//Left Side Bottom Wall
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->setActive(true);
	go->scale.Set(2.5, 20, 1);
	go->normal.Set(cos(Math::DegreeToRadian(0)), sin(Math::DegreeToRadian(0)), 0);
	go->pos = Vector3(m_worldWidth * 0.316, m_worldHeight * 0.2, 0);

	//On Top of Left Side Bottom Wall
	this->BuildThickWall(
		Vector3(1.5, 2.6, 1)
		, Vector3(cos(-30), sin(-30), 0)
		, Vector3(m_worldWidth * 0.316, m_worldHeight * 0.4, 0)
	);

	//go = FetchGO();
	//go->type = GameObject::GO_WALL;
	//go->setActive(true);
	//go->scale.Set(0.5, 2, 1);
	//go->normal.Set(cos(Math::DegreeToRadian(90)), sin(Math::DegreeToRadian(90)), 0);
	//go->pos = Vector3(m_worldWidth * 0.69, m_worldHeight * 0.115, 0);

	//Bouncers

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.72, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.72, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.40, m_worldHeight * 0.45, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.56, m_worldHeight * 0.52, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.44, m_worldHeight * 0.52, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.63, m_worldHeight * 0.7, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.33, m_worldHeight * 0.65, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(2, 2, 1);
	go->pos = Vector3(m_worldWidth * 0.38, m_worldHeight * 0.55, 0);

	//Semi Circle at Top of map
	slices = 20;
	startAngle = -70;
	angleStepUp = -40/ (double) slices;
	for (int i = 0; i <= slices; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->setActive(true);
		go->scale.Set(1, 3.5, 1);
		go->normal.Set(cos(Math::DegreeToRadian(startAngle + angleStepUp * i)), sin(Math::DegreeToRadian(startAngle + angleStepUp * i)), 0);
		go->pos = Vector3(m_worldWidth * 0.50, m_worldHeight * 1.88, 0) + go->normal * 27 * go->scale.y;
	}

	//Powerup Entry Points at Top of Map
	this->BuildThickWall(
		Vector3(0.35, 5, 1)
		, Vector3(cos(0), sin(0), 0)
		, Vector3(m_worldWidth * 0.46, m_worldHeight * 0.895, 0)
	);

	this->BuildThickWall(
		Vector3(0.35, 5, 1)
		, Vector3(cos(0), sin(0), 0)
		, Vector3(m_worldWidth * 0.54, m_worldHeight * 0.895, 0)
	);

	this->BuildThickWall(
		Vector3(0.35, 4.5, 1)
		, Vector3(cos(0), sin(0), 0)
		, Vector3(m_worldWidth * 0.61, m_worldHeight * 0.92, 0)
	);

	this->BuildThickWall(
		Vector3(0.35, 4.5, 1)
		, Vector3(cos(0), sin(0), 0)
		, Vector3(m_worldWidth * 0.39, m_worldHeight * 0.92, 0)
	);

	//Triangle Ledges at the bottom

	/*GameObject* go;
	for (int i = 0; i < 8; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->setActive(true);
		go->scale.Set(2, 12, 1);
		go->normal.Set(cos(Math::DegreeToRadian(i * 45)), sin(Math::DegreeToRadian(i * 45)), 0);
		go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0) + (go->normal * 27);
	}
	this->BuildThickWall(Vector3(2, 12, 1), Vector3(0, 1, 0), Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5));*/

	//Left Flipper Walls
	this->BuildThickWall(
		Vector3(0.5, 5, 1)
		, Vector3(cos(Math::DegreeToRadian(25)), sin(Math::DegreeToRadian(25)), 0)
		, Vector3(m_worldWidth * 0.40, m_worldHeight * 0.225, 0)
	);

	this->BuildThickWall(
		Vector3(0.5, 3.6, 1)
		, Vector3(cos(Math::DegreeToRadian(70)), sin(Math::DegreeToRadian(70)), 0)
		, Vector3(m_worldWidth * 0.43, m_worldHeight * 0.17, 0)
	);

	//Inward arc at Bottom Left of map
	slices = 10;
	startAngle = 230;
	angleStepUp = 17 / (double)slices;
	for (int i = 0; i <= slices; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->setActive(true);
		go->scale.Set(1, 1.4, 1);
		go->normal.Set(cos(Math::DegreeToRadian(startAngle + angleStepUp * i)), sin(Math::DegreeToRadian(startAngle + angleStepUp * i)), 0);
		go->pos = Vector3(m_worldWidth * 0.468, m_worldHeight * 0.35, 0) + go->normal * 27 * go->scale.y;
	}

	//Left Flipper
	left_flipper = FetchGO();
	left_flipper->type = GameObject::GO_FLIPPER;
	left_flipper->setActive(true);
	left_flipper->scale.Set(0.5, 4, 1);
	left_flipper->minAngle = Math::DegreeToRadian(-30);
	left_flipper->maxAngle = Math::DegreeToRadian(0);
	
	left_flipper->pivot = Vector3(m_worldWidth * 0.448, m_worldHeight * 0.158, 0);
	left_flipper->pivotDir = Vector3(cosf(left_flipper->minAngle), sinf(left_flipper->minAngle), 0);
	left_flipper->angle = atan2f(left_flipper->pivotDir.y, left_flipper->pivotDir.x);
	left_flipper->pos = left_flipper->pivot + left_flipper->pivotDir * left_flipper->scale.y;
	left_flipper->normal = Vector3(left_flipper->pivotDir.y, -left_flipper->pivotDir.x); //PivotDir Perpendicular
	left_flipper->attachment = new Attachment
	(
		GameObject::GO_PILLAR
		, Vector3(0, left_flipper->scale.y, 0)
		, Vector3(left_flipper->scale.x, left_flipper->scale.x, 1)
	);

	//Right Flipper Walls
	this->BuildThickWall(
		Vector3(0.5, 5, 1)
		, Vector3(cos(Math::DegreeToRadian(155)), sin(Math::DegreeToRadian(155)), 0)
		, Vector3(m_worldWidth * 0.60, m_worldHeight * 0.225, 0)
	);

	this->BuildThickWall(
		Vector3(0.5, 3.6, 1)
		, Vector3(cos(Math::DegreeToRadian(110)), sin(Math::DegreeToRadian(110)), 0)
		, Vector3(m_worldWidth * 0.57, m_worldHeight * 0.17, 0)
	);

	//Inward arc at Bottom Right of map
	slices = 10;
	startAngle = -50;
	angleStepUp = -17 / (double)slices;
	for (int i = 0; i <= slices; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->setActive(true);
		go->scale.Set(1, 1.4, 1);
		go->normal.Set(cos(Math::DegreeToRadian(startAngle + angleStepUp * i)), sin(Math::DegreeToRadian(startAngle + angleStepUp * i)), 0);
		go->pos = Vector3(m_worldWidth * 0.538, m_worldHeight * 0.35, 0) + go->normal * 27 * go->scale.y;
	}

	//Right Flipper
	right_flipper = FetchGO();
	right_flipper->type = GameObject::GO_FLIPPER;
	right_flipper->setActive(true);
	right_flipper->scale.Set(0.5, 4, 1);
	right_flipper->minAngle = Math::DegreeToRadian(-150);
	right_flipper->maxAngle = Math::DegreeToRadian(-180);

	right_flipper->pivot = Vector3(m_worldWidth * 0.552, m_worldHeight * 0.158, 0);
	right_flipper->pivotDir = Vector3(cosf(right_flipper->minAngle), sinf(right_flipper->minAngle), 0);
	right_flipper->angle = atan2f(right_flipper->pivotDir.y, right_flipper->pivotDir.x);
	right_flipper->pos = right_flipper->pivot + right_flipper->pivotDir * right_flipper->scale.y;
	right_flipper->normal = Vector3(right_flipper->pivotDir.y, right_flipper->pivotDir.x); //PivotDir Perpendicular
	right_flipper->attachment = new Attachment
	(
		GameObject::GO_PILLAR
		, Vector3(0,right_flipper->scale.y,0)
		, Vector3(right_flipper->scale.x, right_flipper->scale.x, 1)
	);

	//Left Breakable wall
	go = FetchGO();
	go->type = GameObject::GO_BREAKABLEWALL;
	go->setActive(true);
	go->setHealth(5);
	go->scale.Set(0.5, 6.2, 1);
	go->normal.Set(cos(Math::DegreeToRadian(90)), sin(Math::DegreeToRadian(90)), 0);
	go->pos = Vector3(m_worldWidth * 0.358, m_worldHeight * 0.255, 0);

	//Right Breakable wall
	go = FetchGO();
	go->type = GameObject::GO_BREAKABLEWALL;
	go->setHealth(5);
	go->setActive(true);
	go->scale.Set(0.5, 6.2, 1);
	go->normal.Set(cos(Math::DegreeToRadian(90)), sin(Math::DegreeToRadian(90)), 0);
	go->pos = Vector3(m_worldWidth * 0.642, m_worldHeight * 0.255, 0);

	spawnBall();

	for (int i = 0; i < 10; ++i)
	{
		if (spawnPoint())
			std::cout << "Out" << std::endl;
	}
}

bool SceneCollision::spawnBall()
{
	if (ballsRemaining > 0)
	{
		if (!ballOnSpring)
		{
			ballOnSpring = true;
			ballsRemaining -= 1;
			m_ballCount += 1;
			GameObject* go;
			go = FetchGO();
			go->type = GameObject::GO_BALL;
			go->setActive(true);
			go->pos = spring->pos + Vector3(0,spring->scale.x * 1.1,0);
			go->scale = Vector3(1, 1, 1);
			go->mass = 2;
			go->gravity = true;
			return true;
		}
	}
	return false;
}

bool SceneCollision::spawnPoint()
{
	GameObject* go = FetchGO();
	go->type = GameObject::GO_POINT;
	go->scale.Set(0.5, 0.6, 1);
	go->normal.Set(cos(Math::DegreeToRadian(45)), sin(Math::DegreeToRadian(45)), 0);

	bool occupied = false;
	for (int i = 0; i < 4; ++i)
	{
		occupied = false;
		go->pos = Vector3(Math::RandFloatMinMax(m_worldWidth * 0.35, m_worldWidth * 0.65), Math::RandFloatMinMax(m_worldHeight * 0.4, m_worldHeight * 0.8), 0);
		
		for (auto& go2 : m_goList) {
			if (go != go2 && go2->isActive() && CheckCollision(go, go2))
			{
				occupied = true;
				break;
			}
		}
		if (occupied == false) break;
	}
	if (occupied == false) {
		go->setActive(true);
		return true;
	}
	go->type = GameObject::GO_NONE;
	go->scale.Set(1, 1, 1);
	go->normal.Set(cos(Math::DegreeToRadian(0)), sin(Math::DegreeToRadian(0)), 0);
	return false;
}

void SceneCollision::BuildThickWall(Vector3 scale, Vector3 normal, Vector3 pos)
{
	Vector3 NP = Vector3(normal.y, -normal.x, 0);

	//Horizontal
	GameObject* go;
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->setActive(true);
	go->scale.Set(scale.x, scale.y, 1);
	go->normal.Set(normal.x, normal.y, 0);
	go->pos = Vector3(pos.x, pos.y, pos.z);

	//Vertical
	go = FetchGO();
	go->type = GameObject::GO_WALL;
	go->setActive(true);
	go->scale.Set(scale.y, scale.x, 1);
	go->normal.Set(normal.y, -normal.x, 0);
	go->pos = Vector3(pos.x, pos.y, pos.z);

	//4 Corners
	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(0.01, 0.01, 0.01);
	go->pos = Vector3(pos.x, pos.y, pos.z) + scale.x * normal + scale.y * NP;

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(0.01, 0.01, 0.01);
	go->pos = Vector3(pos.x, pos.y, pos.z) + scale.x * normal - scale.y * NP;

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(0.01, 0.01, 0.01);
	go->pos = Vector3(pos.x, pos.y, pos.z) - scale.x * normal + scale.y * NP;

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(0.01, 0.01, 0.01);
	go->pos = Vector3(pos.x, pos.y, pos.z) - scale.x * normal - scale.y * NP;
}

GameObject* SceneCollision::FetchGO()
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

void SceneCollision::ReturnBall(GameObject* go)
{
	//Exercise 3: implement ReturnGO()
	go->setActive(false);
	m_ballCount--;
}

void SceneCollision::Update(double dt)
{
	elapsed += dt;
	SceneBase::Update(dt);
	double x, y;
	Application::GetCursorPos(&x, &y);
	int w = Application::GetWindowWidth();
	int h = Application::GetWindowHeight();
	float posX = x / w * m_worldWidth;
	float posY = (h - y) / h * m_worldHeight;

	if (Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	if (Application::IsKeyPressed('B'))
	{
		GameObject* temp = FetchGO();
		temp->setActive(true);
		temp->type = GameObject::GO_WALL;
		temp->pos = Vector3(posX, posY, 0);
		temp->vel = Vector3(0, 0, 0);
		temp->scale = Vector3(5, 5, 5);
		temp->mass = 50;
	}

	if (left_flipper->cooldown > 0.0)
	{
		left_flipper->cooldown -= dt;
	}
	if (right_flipper->cooldown > 0.0)
		right_flipper->cooldown -= dt;

	//Left Flipper
	if (Application::IsKeyPressed('G'))
	{
		if (left_flipper->angle <= left_flipper->minAngle 
			&& left_flipper->angularVelocity == 0)
		{
			left_flipper->cooldown = 3.0f;
			left_flipper->angularVelocity = 2 * Math::PI / flipper_speed;
		}
	}

	if (Application::IsKeyPressed('K'))
	{
		if (right_flipper->angle >= right_flipper->minAngle
			&& right_flipper->angularVelocity == 0)
		{
			right_flipper->cooldown = 3.0f;
			right_flipper->angularVelocity = 2 * Math::PI / flipper_speed;
		}
	}

	if (Application::IsKeyPressed('E'))
	{
		spawnBall();
	}

	float springMag = (spring->origin - spring->pos).LengthSquared();
	if (ballOnSpring)
	{
		if (Application::IsKeyPressed(VK_SPACE))
		{

			if (springMag < spring->maxSpringLength * spring->maxSpringLength)
			{
				spring->pos -= Vector3(0, spring->maxSpringLength, 0) * dt;
			}
			 
		}
		else
		{
			float minLength = spring->maxSpringLength * 0.7;
			if (springMag > minLength * minLength)
			{
				//Release spring
				spring->vel = springMag * spring->normal * spring->springMagnitude;
			}
		}
	}
	if (spring->pos.y > spring->origin.y)
	{
		spring->pos.y = spring->origin.y;
		spring->vel = Vector3(0, 0, 0);
	}



	//Mouse Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		m_ghost->setActive(true);
		m_ghost->pos = Vector3(posX, posY, 0);
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		//Exercise 6: spawn small GO_BALL
		GameObject* temp = FetchGO();
		temp->setActive(true);
		temp->type = GameObject::GO_BALL;
		temp->pos = m_ghost->pos;
		temp->vel = m_ghost->pos - Vector3(posX, posY, 0);
		temp->gravity = true;
		temp->vel *= 2;
		temp->scale = Vector3(1, 1, 1);
		temp->mass = 2;
		m_ghost->setActive(false);
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		m_ghost->pos = Vector3(posX, posY, 0);
		m_ghost->setActive(true);


	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//Exercise 10: spawn large GO_BALL
		GameObject* temp = FetchGO();
		temp->type = GameObject::GO_BALL;
		temp->setActive(true);
		temp->pos = m_ghost->pos;
		temp->vel = m_ghost->pos - Vector3(posX, posY, 0);
		temp->gravity = true;
		temp->scale = Vector3(5, 5, 5);
		temp->mass = 50;
		m_ghost->setActive(false);
	}



	//Physics Simulation Section


	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;

		if (go->isActive())
		{
			float GO_dt = dt;
			int physicsSplit = 1;
			GO_dt = dt / physicsSplit;
			for (int ps = 0; ps < physicsSplit; ++ps)
			{
				//Gravity Implementation
				if (go->gravity)
					go->vel += m_worldGravity * GO_dt;
				go->pos += go->vel * GO_dt;
				/*if (!go->vel.IsZero()) {
					go->vel -= coefFriction * go->mass * go->vel.Normalized();
				}*/
				//Exercise 7: handle out of bound game objects

				if (go->type == GameObject::GO_BALL)
				{
					if (go->pos.y < 0)
					{
						ReturnBall(go);
					}
				}
				
				//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap
				for (std::vector<GameObject*>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
				{
					GameObject* go2 = (GameObject*)*it2;
					if (go2->isActive() && go2 != go)
					{
						if (go->type != GameObject::GO_BALL && go2->type == GameObject::GO_BALL)
						{
							if (CheckCollision(go2, go)) {
								CollisionResponse(go2, go, GO_dt);
								continue;
							}
						}
						if (CheckCollision(go, go2)) {
							CollisionResponse(go, go2, GO_dt);
						}
					}
				}
			}
		}
	}
	//Flipper Update

	//Update Flipper direction
	UpdateFilpper(left_flipper, dt);
	UpdateFilpper(right_flipper, dt);
		
		
	

	
}

void SceneCollision::UpdateFilpper(GameObject* flipper, double dt)
{
	float angle = atan2f(flipper->pivotDir.y, flipper->pivotDir.x);


	//Min angle is bottom, Max is top, but max is less numerically than min
	if (flipper->minAngle > flipper->maxAngle)
	{
		if (angle > 0) { //atan2f flips -180 to 180
			angle = -angle;
		}
		angle -= flipper->angularVelocity * dt;
		if (angle >= flipper->minAngle)
		{
			angle = flipper->minAngle;
			flipper->angularVelocity = 0.0; //Stop Transfer of energy to ball
			flipper->vel = Vector3(0, 0, 0);
		}
		else if (angle <= flipper->maxAngle)
		{
			angle = flipper->maxAngle;
			flipper->angularVelocity = (flipper->angularVelocity > 0 ? 1 : -1) *  -flipper_speed; //Stop Transfer of energy to ball

		}
	}
	else
	{
		angle += flipper->angularVelocity * dt;
		if (angle <= flipper->minAngle)
		{
			angle = flipper->minAngle;
			flipper->angularVelocity = 0.0; //Stop Transfer of energy to ball
			flipper->vel = Vector3(0, 0, 0);
		}
		else if (angle >= flipper->maxAngle)
		{
			angle = flipper->maxAngle;
			flipper->angularVelocity = -flipper_speed; //Stop Transfer of energy to ball

		}
	}


	flipper->pivotDir = Vector3(cosf(angle), sinf(angle), 0);

	//Update Flipper position
	flipper->pos = flipper->pivot + flipper->pivotDir * flipper->scale.y;
	flipper->normal = Vector3(flipper->pivotDir.y, -flipper->pivotDir.x); //PivotDir Perpendicular


	flipper->attachment->worldPos = flipper->pos
		+ flipper->pivotDir * flipper->attachment->relativePos.x
		+ flipper->pivotDir * flipper->attachment->relativePos.y;

}

void SceneCollision::RenderGO(GameObject* go)
{
	switch (go->type)
	{
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		//Exercise 11: think of a way to give balls different colors
		break;

	case GameObject::GO_PILLAR:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		//Exercise 11: think of a way to give balls different colors
		break;

	case GameObject::GO_WALL:
	case GameObject::GO_SPRING:
	{
	
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		float angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();
		break;
	}

	case GameObject::GO_BREAKABLEWALL:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		float angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		float hP = go->getHealthPercentage();
		if(hP > 66)
			RenderMesh(meshList[GEO_GREENCUBE], false);
		else if (hP > 33)
			RenderMesh(meshList[GEO_YELLOWCUBE], false);
		else
			RenderMesh(meshList[GEO_REDCUBE], false);
		modelStack.PopMatrix();
		break;
	}

	case GameObject::GO_POINT:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		float angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_POINT], false);
		modelStack.PopMatrix();
		break;
	}

	

	case GameObject::GO_FLIPPER:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		float angle = atan2f(go->normal.y, go->normal.x);
		modelStack.Rotate(Math::RadianToDegree(angle), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();


		modelStack.PushMatrix();
		modelStack.Translate(go->pivot.x, go->pivot.y, go->pivot.z);
		modelStack.Scale(go->scale.x * 0.5, go->scale.x * 0.5, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		modelStack.Translate(go->attachment->worldPos.x, go->attachment->worldPos.y, go->attachment->worldPos.z);
		modelStack.Scale(go->attachment->scale.x, go->attachment->scale.y, go->attachment->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();

		break;
	}

	}
}

void SceneCollision::Render()
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

	RenderMesh(meshList[GEO_AXES], false);

	for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject* go = (GameObject*)*it;
		if (go->isActive())
		{
			RenderGO(go);
		}
	}
	if (m_ghost->isActive()) {
		RenderGO(m_ghost);
	}
	//On screen text

	//Exercise 5: Render m_objectCount

	//Exercise 8c: Render initial and final momentum

	std::ostringstream ss2;
	ss2.precision(3);
	ss2 << "Score ";
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(1, 0.8, 0), 4, 60, 50);

	ss2.str("");
	ss2.precision(3);
	ss2 << score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0.9, 0.9, 0.9), 3, 60, 45);

	ss2.str("");
	ss2.precision(3);
	ss2 << "Balls Left ";
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 0.8, 0), 2, 60, 38);

	ss2.str("");
	ss2.precision(3);
	ss2 << ballsRemaining;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0.9, 0.9, 0.9), 3, 60, 34);

	std::ostringstream ss;
	ss.precision(2);
	ss << "FPS ";
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0.6, 0.6, 0.6), 3, 4, 50);

	ss.str("");
	ss << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0.9, 0.9, 0.9), 3, 4, 46);
}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject* go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if (m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}

bool SceneCollision::CheckCollision(GameObject* go1, GameObject* go2) {

	bool regardInternalCollision = true;
	if (go1->type != GameObject::GO_BALL && go1->type != GameObject::GO_POINT)
		return false;
	if (go1->type == GameObject::GO_POINT)
		regardInternalCollision = false; //Only need to check if its physically colliding

	//Flip it in check
	regardInternalCollision = !regardInternalCollision;

	if (go2->type == GameObject::GO_BALL)
	{
		Vector3 displacement = go1->pos - go2->pos;
		float combinedRadii = go1->scale.x + go2->scale.x;
		if (displacement.LengthSquared() < combinedRadii * combinedRadii
			&& (regardInternalCollision || displacement.Dot(go2->vel - go1->vel) > 0)) {
			return true;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 displacement = go1->pos - go2->pos;
		float combinedRadii = go1->scale.x + go2->scale.x;
		if (displacement.LengthSquared() < combinedRadii * combinedRadii
			&& (regardInternalCollision || (go2->pos - go1->pos).Dot(go1->vel) > 0)) {
			return true;
		}

	}

	else if (go2->type == GameObject::GO_POINT)
	{
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;
		if (displacement.Dot(N) < 0)
			N = -N; // Make sure N is the outward normal
		Vector3 NP = Vector3(N.y, -N.x, 0);
		if (
			//Horizontal Wall
			displacement.Dot(N) < go1->scale.x + go2->scale.x &&
			Math::FAbs(displacement.Dot(NP)) < go2->scale.y
			)
		{
			return true;
		}
	}

	else if (go2->type == GameObject::GO_WALL 
		|| go2->type == GameObject::GO_BREAKABLEWALL)
	{
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;
		if (displacement.Dot(N) < 0)
			N = -N; // Make sure N is the outward normal

		Vector3 NP = Vector3(N.y, -N.x, 0);

		
		if(
			//Horizontal Wall
			displacement.Dot(N) < go1->scale.x + go2->scale.x &&
			Math::FAbs( displacement.Dot(NP)) < go2->scale.y

			&& (regardInternalCollision ||  N.Dot(go1->vel) > 0)
			)
		{
			if (go2->type == GameObject::GO_BREAKABLEWALL)
			{
				go2->subtractHealth(1.0);
				if(go2->getHealth() <= 0)
					go2->setActive(false);
			}
			return true;
		}
	}

	else if (go2->type == GameObject::GO_BALLEXIT)
	{
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;
		Vector3 NP = Vector3(N.y, -N.x, 0);
		if (displacement.Dot(N) < 0)
			N = -N; // Make sure N is the outward normal
		if (
			//Horizontal Wall
			displacement.Dot(N) < go1->scale.x + go2->scale.x &&
			Math::FAbs(displacement.Dot(NP)) < go2->scale.y

			&& (regardInternalCollision || N.Dot(go1->vel) > 0)
			)
		{
			if (displacement.Dot(go2->normal) < 0)
			{
				ballOnSpring = false;
				return false;
			}else
				return true;
		}




	}

	else if (go2->type == GameObject::GO_SPRING)
	{
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;
		if (displacement.Dot(N) < 0)
			N = -N; // Make sure N is the outward normal

		Vector3 NP = Vector3(N.y, -N.x, 0);

		if (
			//Horizontal Wall
			displacement.Dot(N) < go1->scale.x + go2->scale.x &&
			Math::FAbs(displacement.Dot(NP)) < go2->scale.y

			&& (regardInternalCollision || N.Dot(go1->vel) > 0)
			)
		{
			return true;
		}
	}


	else if (go2->type == GameObject::GO_FLIPPER)
	{
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;
		if (displacement.Dot(N) < 0)
			N = -N; // Make sure N is the outward normal

		Vector3 NP = Vector3(N.y, -N.x, 0);
		if (
			//Horizontal Wall
			displacement.Dot(N) < go1->scale.x + go2->scale.x &&
			Math::FAbs(displacement.Dot(NP)) < go2->scale.y

			&& (regardInternalCollision || N.Dot(go1->vel) > 0)
			)
		{
			return true;
		}

		if (go2->attachment->type == GameObject::GO_PILLAR)
		{
			Vector3 displacement = go1->pos - go2->attachment->worldPos;
			float combinedRadii = go1->scale.x + go2->attachment->scale.x;
			if (displacement.LengthSquared() < combinedRadii * combinedRadii
				&& (regardInternalCollision || (go2->attachment->worldPos - go1->pos).Dot(go1->vel) > 0)) {
				return true;
			}
		}
	}
	

	return false;
}

void SceneCollision::CollisionResponse(GameObject* go1, GameObject* go2, double dt)
{
	static float FRICTION_K = 80;
	
	if (go1->type != GameObject::GO_BALL)
		return;

	if (go2->type == GameObject::GO_POINT)
	{
		go2->setActive(false);
		score += 1;
		spawnPoint(); //Spawns another point in map
	}
	else if (go2->type == GameObject::GO_BALL)
	{
		Vector3 u1 = go1->vel;
		Vector3 u2 = go2->vel;
		float m1 = go1->mass;
		float m2 = go2->mass;

		go1->vel = u1 - (2 * m2 / (m1 + m2)) * (((u1 - u2).Dot(go1->pos - go2->pos)) / (go1->pos - go2->pos).LengthSquared()) * (go1->pos - go2->pos);
		go2->vel = u2 - (2 * m1 / (m1 + m2)) * (((u2 - u1).Dot(go2->pos - go1->pos)) / (go2->pos - go1->pos).LengthSquared()) * (go2->pos - go1->pos);
	
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 N = (go2->pos - go1->pos).Normalized();
		go1->vel = go1->vel - (2 * go1->vel).Dot(N) * N;

		//Friction
		Vector3 fric = (go1->vel).Normalized() * (FRICTION_K) * dt;
		go1->vel -= fric;
	}
	else if (go2->type == GameObject::GO_WALL 
		|| go2->type == GameObject::GO_BALLEXIT
		|| go2->type == GameObject::GO_BREAKABLEWALL)
	{
		//Prevent internal clipping
		Vector3 N = go2->normal;
		if (go1->vel.y < 0)
		{
			Vector3 displacement = go2->pos - go1->pos;
			if (displacement.Dot(N) > 0)
				N = -N; // Make sure N is the inward normal

			float angle = atan2f(go2->normal.y, go2->normal.x);
			angle = 90;
			
			if (angle > 90 - Math::EPSILON && angle < 90 + Math::EPSILON)
			{
				//Find Vector Projection to center of wall
				Vector3 w0_b1 = go2->pos - go1->pos;
				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //Projection onto the inward normal
				if (projection.LengthSquared() < (go2->scale.x + go1->scale.x) * (go2->scale.x + go1->scale.x))
				{
					Vector3 line = projection - w0_b1;
					go1->pos = go2->pos + line + N * (go2->scale.x + go1->scale.x)
						;

				}
			}
		}
		go1->vel = go1->vel - (2 * go1->vel.Dot(go2->normal)) * go2->normal;
		//Friction
		Vector3 fric = (go1->vel).Normalized() * (FRICTION_K)*dt;
		go1->vel -= fric;
	}

	else if (go2->type == GameObject::GO_SPRING)
	{
		//Prevent internal clipping
		Vector3 N = go2->normal;
		if (go1->vel.y < 0)
		{
			Vector3 displacement = go2->pos - go1->pos;
			if (displacement.Dot(N) > 0)
				N = -N; // Make sure N is the inward normal

			float angle = atan2f(go2->normal.y, go2->normal.x);
			angle = 90;

			if (angle > 90 - Math::EPSILON && angle < 90 + Math::EPSILON)
			{
				//Find Vector Projection to center of wall
				Vector3 w0_b1 = go2->pos - go1->pos;
				Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //Projection onto the inward normal
				if (projection.LengthSquared() < (go2->scale.x + go1->scale.x) * (go2->scale.x + go1->scale.x))
				{
					Vector3 line = projection - w0_b1;
					go1->pos = go2->pos + line + N * (go2->scale.x + go1->scale.x)
						;

				}
			}
		}
		go1->vel = go2->vel;
	}

	else if (go2->type == GameObject::GO_FLIPPER)
	{
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;

		
		if ((displacement).Dot(N) > 0)
			N = -N; // Make sure N is the inward normal

		//When we check which side is the flipping side that has the power, the wall prevent going in code should work.
		//

		//Find Vector Projection to center of wall
		Vector3 w0_b1 = go2->pos - go1->pos;
		Vector3 projection = (w0_b1.Dot(N) / N.Dot(N)) * N; //Projection onto the inward normal
		Vector3 line = projection - w0_b1;

		float lengthFromPivot = ((go2->pos + line) - go2->pivot).Length();

		go2->vel = N * lengthFromPivot * go2->angularVelocity;

		if (lengthFromPivot > go2->scale.y * 2) //Hitting the Pillar Attachment
		{
			Vector3 pillarNormal = (go2->attachment->worldPos - go1->pos).Normalized();
			go1->vel = go1->vel - (2 * (go1->vel - go2->vel)).Dot(pillarNormal) * pillarNormal;
		}
		else
		{
			Vector3 returnVec = (2 * (go1->vel - go2->vel)).Dot(go2->normal) * go2->normal;
			go1->vel = go1->vel - returnVec;
		}

		//Friction

		Vector3 fric = (go1->vel).Normalized() * (FRICTION_K)*dt;
		go1->vel -= fric;
	}
}