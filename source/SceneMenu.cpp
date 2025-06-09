#include "scenemenu.h"
#include "sprite.h"
#include "renderer.h"
#include "inputsystem.h"
#include "logmanager.h"
#include "xboxcontroller.h"
#include "game.h"

#include <SDL.h>
#include <cstdio>

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
	,m_focus(FOCUS_START)
	, m_navCooldown(0.0f)
{
}

SceneMenu::~SceneMenu() {

	delete  m_title; 
	m_title = nullptr;

	delete m_startButton;
	m_startButton = nullptr;

	delete m_exitButton; 
	m_exitButton = nullptr;

	if (m_menuMusic) {
		m_menuMusic->release();
		m_menuMusic = nullptr;
	}

	if (m_menuChannel) { 
		m_menuChannel->stop();  
		m_menuChannel = nullptr; 
	}
}

bool SceneMenu::Initialise(Renderer& renderer) {

	int screenW = renderer.GetWidth(); 
	int screenH = renderer.GetHeight();

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
	if (m_startButton) { 
		int startW = m_startButton->GetWidth();
		int startH = m_startButton->GetHeight();

		// Use a reasonable scale for the button
		float scaleX = (screenW / startW) * 0.2f;  
		float scaleY = (screenH / startH) * 0.1f;  

		m_startButton->SetScale(scaleX, scaleY);
		m_startButton->SetX(screenW / 2); 
		m_startButton->SetY(screenH / 1.4); 
		m_startButton->SetAlpha(1.0f);  // Start visible
	}

	//Initialise the Exit Button
	m_exitButton = renderer.CreateSprite("../game/assets/Sprites/UI/Menu-Exit.png"); 
	if (m_exitButton) { 
		int exitW = m_exitButton->GetWidth(); 
		int exitH = m_exitButton->GetHeight(); 

		// Use a reasonable scale for the button
		float scaleX = (screenW / exitW) * 0.2f;  
		float scaleY = (screenH / exitH) * 0.1f; 

		m_exitButton->SetScale(scaleX, scaleY); 
		m_exitButton->SetX(screenW / 2);  
		m_exitButton->SetY(screenH / 1.1);  
		m_exitButton->SetAlpha(1.0f);  // Start visible
	}

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

void SceneMenu::ProcessInput(InputSystem& inputSystem) {
	const Vector2& mousePos = inputSystem.GetMousePosition();

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

void SceneMenu::Draw(Renderer& renderer) {
	if (m_title) {
		m_title->Draw(renderer);

	}

	if (m_startButton) {
		if (hoverStart) {
			m_startButton->SetScale(1.2f, -1.2f);
		}
		else {
			m_startButton->SetScale(1.0f, -1.0f);
		}
		m_startButton->Draw(renderer);
	}

	if (m_exitButton) {
		if (hoverExit) {
			m_exitButton->SetScale(1.2f, -1.2f);
		}
		else {
			m_exitButton->SetScale(1.0f, -1.0f);
		}
		m_exitButton->Draw(renderer);
	}
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
