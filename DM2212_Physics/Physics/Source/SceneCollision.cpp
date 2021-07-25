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
	m_objectCount = 0;
	elapsed = 0.0;

	m_ghost = new GameObject(GameObject::GO_BALL);

	flipper_speed = 2.0;
	m_worldGravity = Vector3(0, -20, 0);

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

	//Left Flipper
	left_flipper = FetchGO();
	left_flipper->type = GameObject::GO_FLIPPER;
	left_flipper->setActive(true);
	left_flipper->scale.Set(1, 6, 1);
	left_flipper->minAngle = Math::DegreeToRadian(-55);
	left_flipper->maxAngle = Math::DegreeToRadian(0);
	
	left_flipper->pivot = Vector3(m_worldWidth * 0.7, m_worldHeight * 0.4, 0);
	left_flipper->pivotDir = Vector3(cosf(left_flipper->minAngle), sinf(left_flipper->minAngle), 0);
	left_flipper->angle = atan2f(left_flipper->pivotDir.y, left_flipper->pivotDir.x);
	left_flipper->pos = left_flipper->pivot + left_flipper->pivotDir * left_flipper->scale.y;
	left_flipper->normal = Vector3(left_flipper->pivotDir.y, -left_flipper->pivotDir.x); //PivotDir Perpendicular


	//Right Flipper
	right_flipper = FetchGO();
	right_flipper->type = GameObject::GO_FLIPPER;
	right_flipper->setActive(true);
	right_flipper->scale.Set(1, 6, 1);
	right_flipper->minAngle = Math::DegreeToRadian(-180);
	right_flipper->maxAngle = Math::DegreeToRadian(-135);

	right_flipper->pivot = Vector3(m_worldWidth * 0.85, m_worldHeight * 0.4, 0);
	right_flipper->pivotDir = Vector3(cosf(right_flipper->minAngle), sinf(right_flipper->minAngle), 0);
	right_flipper->angle = atan2f(right_flipper->pivotDir.y, right_flipper->pivotDir.x);
	right_flipper->pos = right_flipper->pivot + right_flipper->pivotDir * right_flipper->scale.y;
	right_flipper->normal = Vector3(right_flipper->pivotDir.y, right_flipper->pivotDir.x); //PivotDir Perpendicular


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
	go->pos = Vector3(pos.x + NP.Dot(scale.x), pos.y + normal.Dot(scale.y), 0);

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

void SceneCollision::ReturnGO(GameObject* go)
{
	//Exercise 3: implement ReturnGO()
	go->setActive(false);
	m_objectCount--;
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
		if (right_flipper->angle <= right_flipper->minAngle
			&& right_flipper->angularVelocity == 0)
		{
			right_flipper->cooldown = 3.0f;
			right_flipper->angularVelocity = 2 * Math::PI / flipper_speed;
		}
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

	int physicsSplit = 1;
	dt = dt / physicsSplit;
	for (int ps = 0; ps < physicsSplit; ++ps)
	{


		for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject* go = (GameObject*)*it;
			if (go->isActive())
			{
				//Gravity Implementation
				if (go->gravity && (Math::FAbs(go->vel.x) > Math::EPSILON))
					go->vel += m_worldGravity * dt;
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
								CollisionResponse(go2, go, dt);
								continue;
							}
						}
						if (CheckCollision(go, go2)) {
							CollisionResponse(go, go2, dt);
						}
					}
				}

				//Exercise 8b: store values in auditing variables

				//Exercise 10: handle collision using momentum swap instead

				//Exercise 12: improve inner loop to prevent double collision

				//Exercise 13: improve collision detection algorithm [solution to be given later] 
			}
		}
		//Flipper Update

		//Update Flipper direction
		UpdateFilpper(left_flipper, dt);
		UpdateFilpper(right_flipper, dt);
		
		
	}

	
}

void SceneCollision::UpdateFilpper(GameObject* flipper, double dt)
{
	float angle = atan2f(flipper->pivotDir.y, flipper->pivotDir.x);

	angle += flipper->angularVelocity * dt;


	if (angle <= flipper->minAngle)
	{
		angle = flipper->minAngle;
		flipper->angularVelocity = 0.0; //Stop Transfer of energy to ball
	}
	else if (angle >= flipper->maxAngle)
	{
		angle = flipper->maxAngle;
		flipper->angularVelocity = -flipper_speed; //Stop Transfer of energy to ball

	}
	flipper->pivotDir = Vector3(cosf(angle), sinf(angle), 0);

	//Update Flipper position
	flipper->pos = flipper->pivot + flipper->pivotDir * flipper->scale.y;
	flipper->normal = Vector3(flipper->pivotDir.y, -flipper->pivotDir.x); //PivotDir Perpendicular

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

	if (go2->type == GameObject::GO_BALL)
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

	else if (go2->type == GameObject::GO_WALL)
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

			//&& N.Dot(go1->vel) > 0
			)
		{
			std::cout << "Collided " << elapsed << std::endl;
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

			&& N.Dot(go1->vel) > 0)
		{
			std::cout << "Collided " << elapsed << std::endl;
			return true;
		}
	}
	

	return false;
}

void SceneCollision::CollisionResponse(GameObject* go1, GameObject* go2, double dt)
{
	static Vector3 FRICTION_K = Vector3(10, 10, 10);
	if (go2->type == GameObject::GO_BALL)
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
		go1->vel = go1->vel - (2 * go1->vel).Dot(N) * N
			;
	}
	else if (go2->type == GameObject::GO_WALL)
	{
		go1->vel = go1->vel - (2 * go1->vel.Dot(go2->normal)) * go2->normal;

		//Prevent internal clipping
		
		Vector3 N = go2->normal;
		Vector3 displacement = go2->pos - go1->pos;
		if (displacement.Dot(N) > 0)
			N = -N; // Make sure N is the inward normal
		
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
		std::cout << "length: " << lengthFromPivot << std::endl;
		Vector3 returnVec = (2 * go1->vel.Dot(go2->normal)) * go2->normal;
		go1->vel = go1->vel - returnVec - returnVec.Normalized() * lengthFromPivot * go2->angularVelocity;
		std::cout << go1->vel.Length() << std::endl;
	}
}