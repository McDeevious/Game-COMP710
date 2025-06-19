#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "scene.h"
#include "inputsystem.h"

#include <fmod.hpp>

enum MenuState { 
	MENU_WAITING,
	MENU_START_GAME, 
	MENU_EXIT 
};

enum MenuFocus {
	FOCUS_NONE,
	FOCUS_START,
	FOCUS_EXIT
};

class Sprite;
class AnimatedSprite;
class Renderer;
class InputSystem; 

class SceneMenu : public Scene 
{
public:
	SceneMenu();
	~SceneMenu();

	bool Initialise(Renderer& renderer) override;
	void Process(float deltaTime) override;
	void ProcessInput(InputSystem& inputSystem); 
	void Draw(Renderer& renderer) override; 
	int getData(int type) override;
	void setData(int type, float data) override;
	bool spriteComparetoMouse(Sprite* sprite, Vector2 xy);
	bool setSpriteSettings(Sprite& temp,float xpos,float ypos,float Xscale,float Yscale, float alpha);
	MenuState GetState() const;
	void StopMusic();

private:
	Sprite* m_title; 
	Sprite* m_startButton;
	Sprite* m_exitButton;
	MenuState m_state;
	MenuFocus m_focus; 
	Sprite* m_selectionBackground;
	Sprite* m_selectButton[2];
	Sprite* m_settings;
	Sprite* m_saveButton;
	Sprite* character[3];
	int currentCharacter;
	float screenW;
	float screenH;
	float m_navCooldown = 0.0f;
	const float m_navDelay = 0.2f;

	bool hoverStart;
	bool hoverExit;

	float m_timer;
	float m_alpha; 
	bool m_fadingIn;
	bool m_finished; 
	bool isMenu;
	FMOD::Sound* m_menuMusic;
	FMOD::Sound* m_hoverSound;
	FMOD::Channel* m_menuChannel;
	float m_menuVolume;  
	float m_buttonVolume;
	bool m_hoverStartPlayed; 
	bool m_hoverExitPlayed;
};

#endif // SCENEMENU_H
