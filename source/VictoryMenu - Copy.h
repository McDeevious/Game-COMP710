#ifndef VICTORYMENU_H
#define VICTORYMENU_H

#include "sprite.h"
#include "scene.h"
#include <fmod.hpp>

enum VictoryState {
	VICTORY_WAITING,
	VICTORY_RETRY,
	VICTORY_EXIT
};

enum VictoryFocus {
	VICTORY_FOCUS_NONE,
	VICTORY_FOCUS_RETRY,
	VICTORY_FOCUS_EXIT
};

class Sprite;
class Renderer;
class InputSystem;

class VictoryMenu
{
public:
	VictoryMenu();
	~VictoryMenu();

	bool Initialise(Renderer& renderer);
	void ProcessInput(InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	void Reset();

	VictoryState GetState() const;


private:
	Sprite* m_victoryBack;
	Sprite* m_victoryRetry;
	Sprite* m_victoryExit; 

	bool hoverRetry;
	bool hoverExit;

	VictoryState m_state;
	VictoryFocus m_focus;

	float m_navCooldown = 0.0f;
	const float m_navDelay = 0.2f;

	FMOD::Sound* m_hoverSound;
	float m_buttonVolume;
	bool m_hoverRetryButton;
	bool m_hoverExitButton;
};

#endif // VICTORYMENU_H

