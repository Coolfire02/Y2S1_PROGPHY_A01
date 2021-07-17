#pragma once
#include "Vector3.h"
#include "SceneBase.h"

class BackgroundManager
{
	float worldHeight;
	float worldWidth;
	Vector3 BGmotion;
	Vector3 gridCenter;
	Vector3 grid[3][3];
public:
	void setBGMotion(Vector3 motion);
	void Render(SceneBase* scene, MS modelStack);

	BackgroundManager(float worldWidth, float worldHeight) : worldHeight(worldHeight), worldWidth(worldWidth), BGmotion(Vector3(0, 0, 0)), gridCenter(Vector3(0, 0, 0)) {}
	~BackgroundManager();
};

