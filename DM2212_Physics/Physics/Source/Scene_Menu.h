#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include "SceneBase.h"

enum MENU_STATE {
	MAIN_MENU,
	PAUSE_MENU,
	MENU_COUNT
};

class Scene_Menu : public SceneBase
{
public:
	Scene_Menu();
	~Scene_Menu();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void setMenuState(MENU_STATE state);
protected:
	MENU_STATE state;
	int buttonIndex;
	float m_worldWidth;
	float m_worldHeight;

	float elapsed;
	float clickCooldown;

	int getMaxButtonIndex(MENU_STATE);
	
};

#endif
