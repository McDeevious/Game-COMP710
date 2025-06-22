#include "scenemenu.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "renderer.h"
#include "inputsystem.h"
#include "logmanager.h"
#include "xboxcontroller.h"
#include "game.h"
/**
wizard = 0
archer = 1
swordsmen = 2

**/
#include <SDL.h>
#include <cstdio>
#define num_of_characters 3

SceneMenu::SceneMenu()
	: m_title(nullptr)
	, m_startButton(nullptr)
	, m_exitButton(nullptr)
	, m_state(MENU_WAITING)
	, hoverStart(false)
	, hoverExit(false)
	, m_alpha(0.0f)
	, m_timer(0.0f)
	, m_fadingIn(true)
	, m_finished(false)
	, m_menuMusic(nullptr)
	, m_menuChannel(nullptr)
	, m_menuVolume(0.2f)
	, m_hoverSound(nullptr)
	, m_buttonVolume(0.4f)
	, m_hoverStartPlayed(false)
	, m_hoverExitPlayed(false)
	, m_focus(FOCUS_START)
	, m_navCooldown(0.0f)
	, m_selectionBackground(0)
	, m_selectButton{0}
	, isMenu(0)
	, m_settings(0)
	, character{0}
	, currentCharacter(0)
{
}

SceneMenu::~SceneMenu() {

	delete  m_title; 
	m_title = nullptr;

	delete m_startButton;
	m_startButton = nullptr;

	delete m_exitButton; 
	m_exitButton = nullptr;

	delete[] m_selectButton;
	delete m_selectionBackground;
	m_selectionBackground = 0;
	if (m_menuMusic) {
		m_menuMusic->release();
		m_menuMusic = nullptr;
	}

	if (m_menuChannel) { 
		m_menuChannel->stop();  
		m_menuChannel = nullptr; 
	}
}
bool SceneMenu::setSpriteSettings(Sprite& temp, float xpos, float ypos, float Xscale, float Yscale, float alpha)
{
	if (&temp) {
		temp.SetScale(1, 1);
		int exitW = temp.GetWidth();
		int exitH = temp.GetHeight();

		// Use a reasonable scale for the button
		float scaleX = (screenW / exitW) * Xscale;
		float scaleY = (screenH / exitH) * Yscale;

		temp.SetScale(scaleX, -scaleY);
		temp.SetX(xpos);
		temp.SetY(ypos);
		temp.SetAlpha(alpha);  // Start visible
		return 1;
	}
	return 0;
}



bool SceneMenu::Initialise(Renderer& renderer) {


	 screenW = renderer.GetWidth();
	 screenH = renderer.GetHeight();

	m_selectionBackground = renderer.CreateSprite("../game/assets/Sprites/UI/selection-background.png");
	setSpriteSettings(*m_selectionBackground, screenW / 2, screenH / 2, .9, .9, 1);


	m_selectButton[0] = renderer.CreateSprite("../game/assets/Sprites/UI/select-button.png");
	setSpriteSettings(*m_selectButton[0], screenW/100*5 , screenH / 8*3, .2, .1, 1);
	m_selectButton[0]->SetX(m_selectButton[0]->GetX() + m_selectButton[0]->GetWidth()/2);
	m_selectButton[0]->SetAngle(180);

	m_selectButton[1] = renderer.CreateSprite("../game/assets/Sprites/UI/select-button.png");
	setSpriteSettings(*m_selectButton[1], screenW/100*95, screenH / 8*3, .2, .1, 1);
	m_selectButton[1]->SetX(m_selectButton[1]->GetX() - m_selectButton[1]->GetWidth()/2);

	

	m_saveButton = renderer.CreateSprite("../game/assets/Sprites/UI/save.png");
	setSpriteSettings(*m_saveButton, screenW /2, screenH / 8*7, .2, .1, 1);


	character[0] = renderer.CreateSprite("../game/assets/Sprites/UI/wizard.png");
	character[1] = renderer.CreateSprite("../game/assets/Sprites/UI/archer.png");
	character[2] = renderer.CreateSprite("../game/assets/Sprites/UI/swordsmen.png");
	for (int i = 0; i < num_of_characters; i++)
	{
		setSpriteSettings(*character[i], screenW / 8, screenH / 2, .1, .25, 1);

	}
	m_settings = renderer.CreateSprite("../game/assets/Sprites/UI/settings-icon.png");
	setSpriteSettings(*m_settings, screenW / 8, character[0]->GetY()-character[0]->GetHeight() -10, .1, .1, 1);

	

	SDL_ShowCursor(SDL_ENABLE);

	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
	fmod->createSound("../game/assets/Audio/Menu-Audio/GrassyWorld.mp3", FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &m_menuMusic);
	fmod->createSound("../game/assets/Audio/Menu-Audio/button.wav", FMOD_DEFAULT, 0, &m_hoverSound);
	fmod->playSound(m_menuMusic, 0, false, &m_menuChannel);

	if (m_menuChannel) {
		m_menuChannel->setVolume(m_menuVolume);
	}

	//Initialise the Title
	m_title = renderer.CreateSprite("../game/assets/Sprites/UI/Menu-Title.png");
	if (m_title) {

		int titleW = m_title->GetWidth();
		int titleH = m_title->GetHeight();

		float scaleY = (screenH * 0.5f) / titleH;
		float scaleX = scaleY * (static_cast<float>(titleW) / titleH);

	m_title->SetScale(scaleX, -scaleY);
	m_title->SetX(screenW / 2);
	m_title->SetY(screenH / 2.6);
	m_title->SetAlpha(1.0f);

	}

	//Initialise the Start Button
	m_startButton = renderer.CreateSprite("../game/assets/Sprites/UI/Menu-Start.png");
	setSpriteSettings(*m_startButton, screenW / 2, screenH / 1.4, .2, .1, 1);

	//Initialise the Exit Button
	m_exitButton = renderer.CreateSprite("../game/assets/Sprites/UI/Menu-Exit.png");
	setSpriteSettings(*m_exitButton, screenW / 2, screenH / 1.1, .2, .1, 1);

	return m_title && m_startButton && m_exitButton;

}

void SceneMenu::Process(float deltaTime) {

	if (m_finished) return;

	m_timer += deltaTime;

	if (m_fadingIn)
	{
		m_alpha += deltaTime * 0.5f; // 1 second fade-in 
		if (m_alpha >= 1.0f)
		{
			m_alpha = 1.0f;
			m_fadingIn = false;
			m_timer = 0.0f;
		}
	}
	else if (m_state != MENU_WAITING)
	{
		m_alpha -= deltaTime * 0.5f;; // fade out
		if (m_alpha <= 0.0f)
		{
			m_alpha = 0.0f;
			m_finished = true;
		}
	}


	if (m_title) {
		m_title->SetAlpha(m_alpha);
	}

	if (m_startButton) {
		m_startButton->SetAlpha(m_alpha);
	}

	if (m_exitButton) {
		m_exitButton->SetAlpha(m_alpha);
	}

}

bool SceneMenu::spriteComparetoMouse(Sprite* sprite, Vector2 xy)
{
	int x = sprite->GetX();
	int y = sprite->GetY();
	int h = sprite->GetHeight();
	if (h < 0)
	{
		h = -h;
	}
	int w = sprite->GetWidth();
	if (x - w / 2 <= xy.x && x + w / 2 >= xy.x && y - h / 2 <= xy.y && y + h / 2 >= xy.y)
	{
		return 1;
	}
	return 0;
}


void SceneMenu::ProcessInput(InputSystem& inputSystem) {
	const Vector2& mousePos = inputSystem.GetMousePosition();

	
	
	
	if (inputSystem.GetMouseButtonState(0) == BS_RELEASED)
	{
		//
		if (isMenu)//in the character select menu
		{
			if (spriteComparetoMouse(m_saveButton, mousePos))
			{
				isMenu = 0;
			}
			else if (spriteComparetoMouse(m_selectButton[0], mousePos))
			{//left
				currentCharacter--;
				if (currentCharacter < 0)
				{
					currentCharacter = num_of_characters-1;
				}
			}
			else if (spriteComparetoMouse(m_selectButton[1], mousePos))
			{//right
				currentCharacter++;
				if (currentCharacter >= num_of_characters)
				{
					currentCharacter = 0;
				}
			}
		}
		else //in the main menu
		{
			if (spriteComparetoMouse(m_settings, mousePos))
			{
				isMenu = 1;
			}


		}
	}
	if (isMenu ==0)
	{
			// Calculate button hit areas using actual scaled dimensions
			float startW = m_startButton->GetWidth() * m_startButton->GetScaleX();
			float startH = m_startButton->GetHeight() * m_startButton->GetScaleY();

			bool isOverStart =
				mousePos.x >= m_startButton->GetX() - startW / 4 &&
				mousePos.x <= m_startButton->GetX() + startW / 4 &&
				mousePos.y >= m_startButton->GetY() - startH / 4 &&
				mousePos.y <= m_startButton->GetY() + startH / 4;

			float exitW = m_exitButton->GetWidth() * m_exitButton->GetScaleX();
			float exitH = m_exitButton->GetHeight() * m_exitButton->GetScaleY();

			bool isOverExit =
				mousePos.x >= m_exitButton->GetX() - exitW / 4 &&
				mousePos.x <= m_exitButton->GetX() + exitW / 4 &&
				mousePos.y >= m_exitButton->GetY() - exitH / 4 &&
				mousePos.y <= m_exitButton->GetY() + exitH / 4;

			// Update hover states 
			hoverStart = isOverStart;
			hoverExit = isOverExit;

			// Check for mouse clicks
			if (inputSystem.GetMouseButtonState(0) == BS_PRESSED) {

				if (isOverStart) {
					m_state = MENU_START_GAME;
				}
				else if (isOverExit) {
					m_state = MENU_EXIT;
				}
			}

			XboxController* controller = inputSystem.GetController(0);

			float deltaTime = 1.0f / 60.0f;
			m_navCooldown -= deltaTime;

			if (controller && m_navCooldown <= 0.0f) {
				if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED) {
					// Move focus up
					if (m_focus == FOCUS_EXIT) {
						m_focus = FOCUS_START;
					}
					m_navCooldown = m_navDelay;
				}
				else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED) {
					// Move focus down
					if (m_focus == FOCUS_START) {
						m_focus = FOCUS_EXIT;
					}
					m_navCooldown = m_navDelay;
				}
			}

			if (controller) {
				hoverStart = isOverStart || (m_focus == FOCUS_START);
				hoverExit = isOverExit || (m_focus == FOCUS_EXIT);
			}
			else {
				hoverStart = isOverStart;
				hoverExit = isOverExit;
			}

			if (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED) {
				if (m_focus == FOCUS_START) {
					m_state = MENU_START_GAME;
				}
				else if (m_focus == FOCUS_EXIT) {
					m_state = MENU_EXIT;
				}
			}


//Add sound to button hovers
if (hoverStart && !m_hoverStartPlayed && m_hoverSound) {
	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
	FMOD::Channel* m_hoverChannel = nullptr;

	fmod->playSound(m_hoverSound, nullptr, false, &m_hoverChannel);
	if (m_hoverChannel) {
		m_hoverChannel->setVolume(m_buttonVolume);
	}
	m_hoverStartPlayed = true;
}
else if (!hoverStart) {
	m_hoverStartPlayed = false;
}

//Add sound to button hovers 
if (hoverExit && !m_hoverExitPlayed && m_hoverSound) {
	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
	FMOD::Channel* m_hoverChannel = nullptr;

	fmod->playSound(m_hoverSound, nullptr, false, &m_hoverChannel);
	if (m_hoverChannel) {
		m_hoverChannel->setVolume(m_buttonVolume);
	}
	m_hoverExitPlayed = true;
}
else if (!hoverExit) {
	m_hoverExitPlayed = false;
}
		}

}

void SceneMenu::Draw(Renderer& renderer) {


	if (m_title) {
		m_title->Draw(renderer);

	}

	if (m_startButton) {
		if (hoverStart == 1 && isMenu == 0) {
			m_startButton->SetScale(1.2f, -1.2f);
		}
		else {
			m_startButton->SetScale(1.0f, -1.0f);
		}
		m_startButton->Draw(renderer);
	}

	if (m_exitButton) {
		if (hoverExit && isMenu == 0) {
			m_exitButton->SetScale(1.2f, -1.2f);
		}
		else {
			m_exitButton->SetScale(1.0f, -1.0f);
		}
		m_exitButton->Draw(renderer);
	}
	setSpriteSettings(*character[currentCharacter], screenW / 8, screenH / 2, .1, .25, 1);
	character[currentCharacter]->Draw(renderer);
	m_settings->Draw(renderer);
	if (isMenu)
	{
		m_selectionBackground->Draw(renderer);
		m_saveButton->Draw(renderer);
		m_selectButton[0]->Draw(renderer);
		m_selectButton[1]->Draw(renderer);

		//character wheel
		int temp = currentCharacter - 1;
		//left
		if (temp < 0)
		{
			temp = num_of_characters - 1;
		}
		setSpriteSettings(*character[temp], screenW / 8 * 3, screenH - screenH / 1.5, .1, .25, 1);
		character[temp]->Draw(renderer);
		//middle
		setSpriteSettings(*character[currentCharacter], screenW / 2, screenH - screenH / 1.4, .15, .25 + 0.125, 1);
		character[currentCharacter]->Draw(renderer);
		//right
		temp = currentCharacter + 1;
		if (temp >= num_of_characters)
		{
			temp = 0;
		}
		setSpriteSettings(*character[temp], screenW / 8 * 5, screenH - screenH / 1.5, .1, .25, 1);
		character[temp]->Draw(renderer);

	}
}


int SceneMenu::getData(int type)
{
	if (type == 0)
	{
		return currentCharacter;
	}
	return 0;
}
void SceneMenu::setData(int type, float data)
{

}

MenuState SceneMenu::GetState() const  
{
	return m_state;   
}

void SceneMenu::StopMusic() {
	if (m_menuChannel) {
		m_menuChannel->stop();
		m_menuChannel = nullptr;
	}
}
