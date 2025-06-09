#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include "sprite.h"
#include "scene.h"
#include "inputsystem.h"
#include <fmod.hpp>

enum PauseState {
	PAUSE_WAITING, 
	PAUSE_PLAY,
	PAUSE_RETRY,
	PAUSE_EXIT 
};

enum PauseFocus {
	PAUSE_FOCUS_NONE, 
	PAUSE_FOCUS_PLAY, 
	PAUSE_FOCUS_RETRY,
	PAUSE_FOCUS_EXIT 
};

class Sprite;
class Renderer;
class InputSystem;

class PauseMenu 
{
public:
	PauseMenu();
	~PauseMenu();

	bool Initialise(Renderer& renderer);
	void ProcessInput(InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	void Reset();

	PauseState GetState() const;


private:
	Sprite* m_pauseBack;
	Sprite* m_pausePlay;
	Sprite* m_pauseRetry;
	Sprite* m_pauseExit;

	bool hoverPlay;
	bool hoverRetry;
	bool hoverExit;

	PauseState m_state;
	PauseFocus m_focus;

	float m_navCooldown = 0.0f; 
	const float m_navDelay = 0.2f; 

	FMOD::Sound* m_hoverSound;
	float m_buttonVolume;
	bool m_hoverPlayButton;
	bool m_hoverRetryButton;
	bool m_hoverExitButton;
};

#endif // PAUSE_MENU_H
