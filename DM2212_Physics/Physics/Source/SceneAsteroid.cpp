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

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i < 100; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}

	//Exercise 2b: Initialize m_lives and m_score
	m_lives = 3;
	m_score = 0;

	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->mass = 1.f;
	m_ship->scale = Vector3(1, 1, 1);
	m_ship->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f);
}

GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (auto go : m_goList)
	{
		if (!go->active) {
			return go;
		}
	}
	//Allocate more memory for more objects
	for (int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}
	return m_goList.at(m_goList.size() - 10);
}

void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);

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
	//Exercise 6: set m_force values based on WASD
	if(Application::IsKeyPressed('W'))
	{
		m_force += Vector3(0, 100, 0);
	}
	if(Application::IsKeyPressed('A'))
	{
		m_force += Vector3(-100, 0, 0);
	}
	if(Application::IsKeyPressed('S'))
	{
		m_force += Vector3(0, -100, 0);
	}
	if(Application::IsKeyPressed('D'))
	{
		m_force += Vector3(100, 0, 0);
	}
	//Exercise 8: use 2 keys to increase and decrease mass of ship
	if (Application::IsKeyPressed('7'))
	{
		m_ship->mass -= 1 * dt;
	}
	if (Application::IsKeyPressed('8'))
	{
		m_ship->mass += 1 * dt;
	}


	//Exercise 11: use a key to spawn some asteroids
	if (Application::IsKeyPressed('V'))
	{
		GameObject* obj = FetchGO();
		obj->active = true;
		obj->scale = Vector3(1,1,1);
		obj->pos = Vector3(Math::RandFloatMinMax(0.f,m_worldWidth) , Math::RandFloatMinMax(0.f, m_worldHeight),0);
		obj->vel = Vector3(Math::RandFloatMinMax(-5.f, 5.f), Math::RandFloatMinMax(-5.f, 5.f));
	}

	//Exercise 14: use a key to spawn a bullet

	//Exercise 15: limit the spawn rate of bullets

	//Mouse Section
	static bool bLButtonState = false;
	if(!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		GameObject* unused = FetchGO();
		unused->active = true;

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

	//Ship Movement Handling
	Vector3 acc = Vector3(m_force.x / m_ship->mass, m_force.y / m_ship->mass, m_force.z / m_ship->mass);
	Vector3 oldVel = m_ship->vel;
	m_ship->vel += acc * dt * m_speed;
	if (m_ship->vel.LengthSquared() > MAX_SPEED * MAX_SPEED)
		m_ship->vel = m_ship->vel.Normalize() * MAX_SPEED;
	m_force = Vector3(0, 0, 0);

	//Move Ship
	m_ship->pos += m_ship->vel * dt * m_speed;
	this->WarpObjectOnScreen(m_ship);

	//Rotate Ship
	//Calculate Angle
	if (m_ship->vel.y != 0)
	{
		
		float newA = Math::RadianToDegree(atan2f(m_ship->vel.y, m_ship->vel.x));
		float aDiff = max(newA, m_ship->angle) - min(newA, m_ship->angle);
		if (aDiff > 180) aDiff = aDiff - 360;
		if (m_ship->angle < newA)
			m_ship->angle += (aDiff) * dt * m_speed;
		else if (m_ship->angle > newA)
			m_ship->angle -= (aDiff) * dt * m_speed;

		//float newA = atan(m_ship->vel.x / m_ship->vel.y);
		//if (newA < 0) newA = 360 + newA;
		//else if (newA > 360) newA = 0 + newA - 360;
		//float aDiff = max(newA, m_ship->angle) - min(newA, m_ship->angle);
		//if(newA != m_ship->angle)
		//	m_ship->angle += (m_ship->angle > newA ? -aDiff : aDiff) * dt * m_speed;
		//if (m_ship->angle < 0) m_ship->angle = 360 + m_ship->angle;
		//else if (m_ship->angle > 360) m_ship->angle = 0 + m_ship->angle - 360;

		//float newA = atan(m_ship->vel.x / m_ship->vel.y);
		//if (newA < 0) newA = 360 + newA;
		//else if (newA > 360) newA = 0 + newA - 360;
		//float aDiff = max(newA, m_ship->angle) - min(newA, m_ship->angle);
		//if (newA != m_ship->angle)
		//	m_ship->angle += (m_ship->angle > newA ? -aDiff : aDiff) * dt * m_speed;
		//if (m_ship->angle < 0) m_ship->angle = 360 + m_ship->angle;
		//else if (m_ship->angle > 360) m_ship->angle = 0 + m_ship->angle - 360;
	}



	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{

			if (go->type == GameObject::GO_ASTEROID)
			{

				Vector3 ds = Vector3(0, 0, 0);
				ds = go->vel * dt * m_speed;
				go->pos += ds;

				//Handle ship collision
				if ((go->pos - m_ship->pos).Length() < go->scale.x + m_ship->scale.x) {
					
					//Game Logic
					m_lives -= 1;
					go->active = false;
					m_ship->pos = Vector3(m_worldWidth * 0.5f, m_worldHeight * 0.5f);
					m_ship->vel = Vector3(0, 0, 0);

					if (m_lives <= 0)
						ResetGame();

					break;
				}

				this->WarpObjectOnScreen(go);
			}

			//Exercise 16: unspawn bullets when they leave screen

			//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
		}
	}
}

//Game Logic Functions

void SceneAsteroid::ResetGame() {
	for (auto go : m_goList) {
		go->active = false;
	}
	m_lives = 3;
	m_score = 0;

	m_ship->active = true;
	m_ship->mass = 1.f;
	m_ship->scale = Vector3(1, 1, 1);
}

bool SceneAsteroid::WarpObjectOnScreen(GameObject *go)
{
	if (go->pos.x > (m_worldWidth + go->scale.x)) {
		go->pos.x = 0;
		return true;
	}
	else if (go->pos.x < (0 - go->scale.x))
	{
		go->pos.x = m_worldWidth;
		return true;
	}
	else if (go->pos.y > (m_worldHeight + go->scale.y)) {
		go->pos.y = 0;
		return true;
	}
	else if (go->pos.y < (0 - go->scale.y)) {
		go->pos.y = m_worldHeight;
		return true;
	}
	return false;
}

void SceneAsteroid::RenderGO(GameObject *go)
{
	modelStack.PushMatrix();
	//modelStack.Translate(100, 100, 0);
	modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	modelStack.Rotate(go->angle, 0.0f, 0.0f, 1.f);
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
		RenderMesh(meshList[GEO_CUBE], false);
		break;
	case GameObject::GO_BULLET:
		RenderMesh(meshList[GEO_BALL], false);
		break;
	}
	modelStack.PopMatrix();
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
	
	RenderMesh(meshList[GEO_AXES], false);

	RenderGO(m_ship);
	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}
	

	//On screen text
	std::ostringstream ss;

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
}
