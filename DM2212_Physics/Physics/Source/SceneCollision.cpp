#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

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
	m_objectCount = 0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
	GameObject* go;
	for (int i = 0; i < 8; i++)
	{
		go = FetchGO();
		go->type = GameObject::GO_WALL;
		go->setActive(true);
		go->scale.Set(2, 12, 1);
		go->normal.Set(cos(Math::DegreeToRadian(i * 45)), sin(Math::DegreeToRadian(i * 45)), 0);
		go->pos = Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5, 0) + (go->normal * 27);

	}
	this->BuildThickWall(Vector3(2, 12, 1), Vector3(0, 1, 0), Vector3(m_worldWidth * 0.5, m_worldHeight * 0.5));

	/*for (int i = -1; i <= 1; i += 2) 
	{
		GameObject* go2 = FetchGO();
		go2->type = GameObject::GO_PILLAR;
		go2->setActive(true);
		go2->scale.Set(go->scale.x, go->scale.x, 1);
		go2->pos = Vector3(go->pos.x + i * go->scale.y, go->pos.y, 0);
	}*/
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
	go->scale.Set(1, 1, 1);
	go->pos = Vector3(pos.x + normal.x * scale.x, pos.y, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(1, 1, 1);
	go->pos = Vector3(pos.x + -scale.x, pos.y + scale.y, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(1, 1, 1);
	go->pos = Vector3(pos.x + scale.x, pos.y + scale.y, 0);

	go = FetchGO();
	go->type = GameObject::GO_PILLAR;
	go->setActive(true);
	go->scale.Set(1, 1, 1);
	go->pos = Vector3(pos.x + -scale.x, pos.y + -scale.y, 0);
}

GameObject* SceneCollision::FetchGO()
{
	//Exercise 2a: implement FetchGO()
	for (auto go : m_goList)
	{
		if (!go->isActive())
		{
			m_objectCount++;
			go->setActive(true);
			return go;
		}
	}
	for (int i = 0; i < 10; i++)
	{
		GameObject* newGO = new GameObject(GameObject::GO_BALL);
		m_goList.push_back(newGO);
	}
	//Exercise 2b: increase object count every time an object is set to active
	FetchGO();
}

void SceneCollision::ReturnGO(GameObject* go)
{
	//Exercise 3: implement ReturnGO()
	go->setActive(false);
	m_objectCount--;
}

void SceneCollision::Update(double dt)
{
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
		temp->pos = m_ghost->pos;
		temp->vel = m_ghost->pos - Vector3(posX, posY, 0);
		temp->vel *= 2;
		temp->scale = Vector3(2, 2, 2);
		temp->mass = 8;
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
		temp->pos = m_ghost->pos;
		temp->vel = m_ghost->pos - Vector3(posX, posY, 0);
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
			go->pos += go->vel * dt;
			/*if (!go->vel.IsZero()) {
				go->vel -= coefFriction * go->mass * go->vel.Normalized();
			}*/
			//Exercise 7: handle out of bound game objects

			if (go->pos.x > m_worldWidth && go->vel.x > 0 ||
				go->pos.x < go->scale.x && go->vel.x < 0) {
				go->vel.x *= -1;
			}
			if (go->pos.y > m_worldHeight && go->vel.y > 0 ||
				go->pos.y < go->scale.y && go->vel.y < 0) {
				go->vel.y *= -1;
			}

			if (go->pos.x + go->scale.x > m_worldWidth + 10 ||
				go->pos.x - go->scale.x < -10) {
				ReturnGO(go);
			}
			if (go->pos.y + go->scale.y > m_worldHeight + 10 ||
				go->pos.y - go->scale.y < -10) {
				ReturnGO(go);
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
							CollisionResponse(go2, go);
							continue;
						}
					}
					if (CheckCollision(go, go2)) {
						CollisionResponse(go, go2);
					}
				}
			}

			//Exercise 8b: store values in auditing variables

			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm [solution to be given later] 
		}
	}
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
	ss2 << "Object Count: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);

	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);

	RenderTextOnScreen(meshList[GEO_TEXT], "Collision", Color(0, 1, 0), 3, 0, 0);
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

	if (go1->type == GameObject::GO_BALL && (go2->type == GameObject::GO_BALL || go2->type == GameObject::GO_PILLAR))
	{
		Vector3 displacement = go1->pos - go2->pos;
		float combinedRadii = go1->scale.x + go2->scale.x;
		if (displacement.LengthSquared() < combinedRadii * combinedRadii
			&& displacement.Dot(go2->vel - go1->vel) > 0) {
			return true;
		}
	}
	else if (go2->type == GameObject::GO_PILLAR)
	{
		Vector3 displacement = go1->pos - go2->pos;
		float combinedRadii = go1->scale.x + go2->scale.x;
		if (displacement.LengthSquared() < combinedRadii * combinedRadii
			&& (go2->pos - go1->pos).Dot(go1->vel) > 0) {
			return true;
		}
	}

	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_WALL)
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

			&& N.Dot(go1->vel) > 0)
		{
			return true;
		}
	}

	return false;
}

void SceneCollision::CollisionResponse(GameObject* go1, GameObject* go2)
{
	if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_BALL)
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
	}
	else if (go1->type == GameObject::GO_BALL && go2->type == GameObject::GO_WALL)
	{
		go1->vel = go1->vel - (2 * go1->vel.Dot(go2->normal)) * go2->normal;
	}
}