#pragma once

#include "SceneAsteroid.h"

class Scene_GameL_One_One : public SceneAsteroid
{
public:
	Scene_GameL_One_One();
	~Scene_GameL_One_One();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
private:

};

