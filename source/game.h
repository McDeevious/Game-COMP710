// COMP710 GP Framework
#ifndef GAME_H

#include "scenesplash.h"
#include "SceneGame.h"
#include "SceneMenu.h"

#include <fmod.hpp>

#define GAME_H
#define USE_LAG
// Forward declarations:
class Renderer;
class InputSystem;
class Game
{
	// Member methods:
public:
	static Game& GetInstance();
	static void DestroyInstance();
	bool Initialise();
	bool DoGameLoop();
	void Quit();
	void ToggleDebugWindow();
	FMOD::System* GetFMODSystem() const { return m_fmodSystem; }
protected:
	void Process(float deltaTime);
	void Draw(Renderer& renderer);
	void ProcessFrameCounting(float deltaTime);
private:
	Game();
	~Game();
	Game(const Game& game);
	Game& operator=(const Game& game);

	// Member data:
public:
protected:
	static Game* sm_pInstance;
	Renderer* m_pRenderer;
	InputSystem* m_pInputSystem;

	SceneSplash* m_pSceneSplash;
	SceneMenu* m_pSceneMenu;
	SceneGame* m_pSceneGame;

	bool m_splashComplete;
	bool m_menuComplete;

	Sprite* m_pTextSprite;  
	_int64 m_iLastTime; 
	float m_fExecutionTime;
	float m_fElapsedSeconds;
	int m_iFrameCount;
	int m_iFPS;
#ifdef USE_LAG
	float m_fLag;
	int m_iUpdateCount;
#endif // USE_LAG
	bool m_bLooping;
	bool m_bDebugWindowEnabled;
private:
	FMOD::System* m_fmodSystem;

};
#endif // GAME_H