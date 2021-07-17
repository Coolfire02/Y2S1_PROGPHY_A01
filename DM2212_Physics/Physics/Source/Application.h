
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "Scene.h"

enum GAME_STATE {
	GAMESTATE_PAUSED,
	GAMESTATE_MAINMENU,
	GAMESTATE_PLAYING,
	GAMESTATE_QUIT
};

enum SCENE_TYPE {
	SCENE_MENU,
	SCENE_LEVEL_ONE_ONE,
	SCENE_LEVEL_ONE_TWO,
	SCENE_LEVEL_ONE_BOSS,
	SCENE_COUNT
};

class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double *xpos, double *ypos);
	static int GetWindowWidth();
	static int GetWindowHeight();
	static float GetFrameTime();

	//Can all be moved into a Game State Manager. Do if have time
	static bool quit;
	static int currentScene;
	static int prevGameScene;
	static int ship_lives;
	static int gameScore;
	static void resetGame();
	static void resumeGame();
	static void setCurrentScene(SCENE_TYPE type);


private:
	Application();
	~Application();

	Scene* scenes[SCENE_COUNT];

	//Declare a window object
	StopWatch m_timer;
};

#endif