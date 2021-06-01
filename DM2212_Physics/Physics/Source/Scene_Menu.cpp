#include "Scene_Menu.h"
#include "Application.h"
#include "GL\glew.h"

Scene_Menu::Scene_Menu()
{
}

Scene_Menu::~Scene_Menu() 
{
}

void Scene_Menu::Init() 
{
	SceneBase::Init();
	state = MAIN_MENU;
	buttonIndex = 0;
	elapsed = 0.f;
	clickCooldown = 0.f;
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();
}

void Scene_Menu::setMenuState(MENU_STATE state) {
	this->state = state;
}

void Scene_Menu::Update(double dt) 
{
	elapsed += dt;
	if (clickCooldown >= 0) clickCooldown -= dt;
	SceneBase::Update(dt);
	if (Application::IsKeyPressed(VK_RETURN) && clickCooldown <= 0) { //Press Enter
		clickCooldown = 0.5f;
		switch (state) {
		case MAIN_MENU:
		{
			if (buttonIndex == 0) {
				Application::resumeGame();
			}
			else if (buttonIndex == 1) {
				//Quit Button
				Application::quit = true;
			}
		}
		break;

		case PAUSE_MENU:
		{
			if (buttonIndex == 0) {
				Application::resumeGame();
			}
			else if (buttonIndex == 1) {
				//Quit Button
				setMenuState(MAIN_MENU);
			}
		}

		}

	}

	if (Application::IsKeyPressed(VK_UP) || Application::IsKeyPressed('W'))
	{
		if (buttonIndex > 0) --buttonIndex;
	}
	else if (Application::IsKeyPressed(VK_DOWN) || Application::IsKeyPressed('S'))
	{
		if (buttonIndex < getMaxButtonIndex(state)) buttonIndex++;
	}
}

int Scene_Menu::getMaxButtonIndex(MENU_STATE state) {
	switch (state) {
	case MAIN_MENU:
		return 1;
	case PAUSE_MENU:
		return 1;
	}
}

void Scene_Menu::Render() 
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
		modelStack.Translate(m_worldWidth * 0.5, m_worldHeight * 0.5, -1.0);
		RenderMesh(meshList[GEO_SPACEBG], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		modelStack.Translate(m_worldWidth * 0.5, m_worldHeight * 0.5, 1.0);
		switch (state) {
		case MAIN_MENU:
			RenderMesh(meshList[GEO_MENU_MAIN], false);
			break;
		case PAUSE_MENU:
			RenderMesh(meshList[GEO_MENU_PAUSED], false);
			break;
		}
	modelStack.PopMatrix();

	modelStack.PushMatrix();
		modelStack.Translate(m_worldWidth * 0.5 - 0.35, m_worldHeight * 0.5 - 2.1 - (18.05 * buttonIndex), 1.0);
		modelStack.Scale(0.59f, 0.251f, 1.0f);
		RenderMesh(meshList[GEO_MENU_SELECTOR], false);
	modelStack.PopMatrix();
}

void Scene_Menu::Exit() 
{

}
