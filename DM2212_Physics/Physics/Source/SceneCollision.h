#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

class SceneCollision : public SceneBase
{

	enum DIFFICULTY
	{
		EASY,
		REGULAR
	};

	enum GAME_STATE
	{
		PLAYING,
		GAMEOVER
	};


public:
	SceneCollision();
	~SceneCollision();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject* go);

	void BuildThickWall(Vector3 scale, Vector3 normal, Vector3 pos, GameObject::GAMEOBJECT_TYPE wallType);

	GameObject* FetchGO();
	void ReturnBall(GameObject* go);
	bool CheckCollision(GameObject* go1, GameObject* go2);
	void CollisionResponse(GameObject* go1, GameObject* go2, double dt);

	void UpdateFilpper(GameObject* flipper, double dt);

	void activatePowerup(int powerupType);
	void updatePowerups(double dt);

	void switchGameState(GAME_STATE state);


	bool spawnBall();
	bool spawnPoint();

protected:

	//Physics
	std::vector<GameObject*> m_goList;
	std::vector<GameObject*> balls;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	Vector3 m_worldGravity;
	double elapsed;

	double flipper_speed; //Period of one rotation

	GameObject* m_ghost;
	int m_ballCount;

	GameObject* left_flipper;
	GameObject* right_flipper;
	GameObject* spring;

	GameObject* powerup[3];

	//Game

	DIFFICULTY difficulty;
	GAME_STATE gameState;

	int score;
	int ballsRemaining;
	bool canSpawnBall;
	int points;

	bool ballOnSpring;

	float coefFriction = 0.05;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;
};

#endif