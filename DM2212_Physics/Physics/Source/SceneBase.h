#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "Scene.h"
#include "Mtx44.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "GameObject.h"
#include <vector>

class SceneBase : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_TEXT,
		GEO_BALL,
		GEO_BALL_GREYREDHINT,
		GEO_CUBE,
		GEO_GREENCUBE,
		GEO_YELLOWCUBE,
		GEO_REDCUBE,
		GEO_ONEWAYSOLIDWALL_LEFT,
		
		GEO_BLUECUBE,
		GEO_BALLHOLE,
		GEO_BALLHOLE_YELLOW,
		GEO_BALLHOLE_BLUE,
		GEO_BALLHOLE_PINK,

		GEO_POINT,

		GEO_MENU_MAIN,
		GEO_MENU_PAUSED,
		GEO_MENU_SELECTOR,

		GEO_HEALTHBAR_OUTER,
		GEO_HEALTHBAR,

		GEO_BULLET_RED,
		GEO_BULLET_HOMING,

		GEO_POWERUP_TIME,
		GEO_POWERUP_HEALTH,
		GEO_POWERUP_GUNDOUBLE,
		GEO_POWERUP_GUNQUAD,
		GEO_ABILITY_CLOAX,

		GEO_KEYCARD,

		GEO_ENEMY_1,
		GEO_ENEMY_2,
		GEO_ENEMY_BOSS_1,

		GEO_WORMHOLE,

		GEO_HEALTH_ORB,
		GEO_POWER_ORB,
		GEO_DEFENSE_ORB,

		GEO_SPACEBG,
		GEO_SHIP,
		GEO_SHIP_LIVES,
		GEO_SHIP_CLOAX,
		GEO_BULLET,
		GEO_ASTEROID,
		NUM_GEOMETRY,
	};
public:
	SceneBase();
	~SceneBase();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, int(&charWidth)[256], int arrSize);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMesh(Mesh *mesh, bool enableLight);
	void RenderGO(GameObject *go);
	Mesh* getMesh(GEOMETRY_TYPE type);
	GameObject* FetchGO();
protected:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera camera;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Light lights[1];

	bool bLightEnabled;

	float fps;
};

#endif