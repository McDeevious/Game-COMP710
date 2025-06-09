#ifndef GAME_OVER_MENU_H
#define GAME_OVER_MENU_H

#include "sprite.h"
#include "scene.h"
#include <fmod.hpp>

enum GameOverState {
	GAME_OVER_WAITING,
	GAME_OVER_RETRY,
	GAME_OVER_EXIT
};

enum GameOverFocus {
	GAMEOVER_FOCUS_NONE,
	GAMEOVER_FOCUS_RETRY,
	GAMEOVER_FOCUS_EXIT
};

class Sprite;
class Renderer;
class InputSystem;

class GameOverMenu
{
public:
	GameOverMenu(); 
	~GameOverMenu(); 

	bool Initialise(Renderer& renderer);
	void ProcessInput(InputSystem& inputSystem); 
	void Draw(Renderer& renderer);
	void Reset();

	GameOverState GetState() const;


private:
	Sprite* m_gameOverBack;
	Sprite* m_gameOverRetry;
	Sprite* m_gameOverExit;

	bool hoverRetry;
	bool hoverExit;

	GameOverState m_state;
	GameOverFocus m_focus; 

	float m_navCooldown = 0.0f;
	const float m_navDelay = 0.2f;

	FMOD::Sound* m_hoverSound;
	float m_buttonVolume;
	bool m_hoverRetryButton;
	bool m_hoverExitButton;
};

#endif // GAME_OVER_MENU_H
