#include "BackgroundManager.h"


void BackgroundManager::Render(SceneBase* scene, MS modelStack)
{
	modelStack.PushMatrix();
	
	Vector3 pos = Vector3(worldWidth * 0.5, worldHeight * 0.5, -1.0) + gridCenter;
	modelStack.Translate(pos.x, pos.y, pos.z);
	scene->RenderMesh(scene->getMesh(SceneBase::GEO_SPACEBG), false);

	// - - -
	// X - - 
	// - - -
	if (pos.x > -worldWidth)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-1, 0, 0);
		scene->RenderMesh(scene->getMesh(SceneBase::GEO_SPACEBG), false);
		modelStack.PopMatrix();

		// - - -
		// - - - 
		// X - -
		if (pos.y > -worldHeight)
		{
			modelStack.PushMatrix();
			modelStack.Translate(0, -1, 0);
			scene->RenderMesh(scene->getMesh(SceneBase::GEO_SPACEBG), false);
			modelStack.PopMatrix();


		}

		// X - -
		// - - - 
		// - - -
		if (pos.y < worldHeight * 2)
		{
			modelStack.PushMatrix();
			modelStack.Translate(0, 1, 0);
			scene->RenderMesh(scene->getMesh(SceneBase::GEO_SPACEBG), false);
			modelStack.PopMatrix();


		}


	}

	modelStack.PopMatrix();
}
