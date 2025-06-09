#include "GameOverMenu.h"
#include "renderer.h"
#include "inputsystem.h"
#include "logmanager.h"
#include "xboxcontroller.h"
#include "game.h"

#include <SDL.h>

GameOverMenu::GameOverMenu()
	: m_gameOverBack(nullptr)
	, m_gameOverRetry(nullptr)
	, m_gameOverExit(nullptr)
	, hoverRetry(false)
	, hoverExit(false)
	, m_state(GAME_OVER_WAITING)
	, m_hoverSound(nullptr)
	, m_buttonVolume(0.4f)
	, m_hoverRetryButton(false)
	, m_hoverExitButton(false)
	, m_focus(GAMEOVER_FOCUS_RETRY)
	, m_navCooldown(0.0f)
{
}

GameOverMenu::~GameOverMenu() {
	delete  m_gameOverBack; 
	m_gameOverBack = nullptr;

	delete  m_gameOverRetry;
	m_gameOverRetry = nullptr;

	delete  m_gameOverExit; 
	m_gameOverExit = nullptr; 

	if (m_hoverSound) {
		m_hoverSound->release();
		m_hoverSound = nullptr;
	}
}

bool GameOverMenu::Initialise(Renderer& renderer) {
	int screenW = renderer.GetWidth();
	int screenH = renderer.GetHeight();

	SDL_ShowCursor(SDL_ENABLE);

	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
	fmod->createSound("../game/assets/Audio/Menu-Audio/button.wav", FMOD_DEFAULT, 0, &m_hoverSound);

	//Initialise the Pause Background
	m_gameOverBack = renderer.CreateSprite("../game/assets/Sprites/UI/Game-Over_Menu.png");
	if (m_gameOverBack) {
		int backW = m_gameOverBack->GetWidth();
		int backH = m_gameOverBack->GetHeight();

		float scaleX = (screenW / backW) * 0.75f; 
		float scaleY = (screenH / backH) * 0.75f;

		m_gameOverBack->SetScale(scaleX, -scaleY);
		m_gameOverBack->SetX(screenW / 1.75);
		m_gameOverBack->SetY(screenH / 2);
		m_gameOverBack->SetAlpha(1.0f);
	}

	//Initialise the Retry Button
	m_gameOverRetry = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Retry.png"); 
	if (m_gameOverRetry) {
		int retryW = m_gameOverRetry->GetWidth();
		int retryH = m_gameOverRetry->GetHeight();

		float scaleX = (screenW / retryW) * 0.2f;
		float scaleY = (screenH / retryH) * 0.2f;

		m_gameOverRetry->SetScale(scaleX, -scaleY);
		m_gameOverRetry->SetX(screenW / 2.5);
		m_gameOverRetry->SetY(screenH / 1.41);
		m_gameOverRetry->SetAlpha(1.0f);
	}

	//Initialise the Exit Button
	m_gameOverExit = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Exit.png"); 
	if (m_gameOverExit) {
		int exitW = m_gameOverExit->GetWidth();
		int exitH = m_gameOverExit->GetHeight();

		float scaleX = (screenW / exitW) * 0.2f;
		float scaleY = (screenH / exitH) * 0.2f;

		m_gameOverExit->SetScale(scaleX, -scaleY);
		m_gameOverExit->SetX(screenW / 1.75);
		m_gameOverExit->SetY(screenH / 1.40);
		m_gameOverExit->SetAlpha(1.0f);
	}

	return m_gameOverBack && m_gameOverRetry && m_gameOverExit; 
}

void GameOverMenu::ProcessInput(InputSystem& inputSystem) { 
	const Vector2& mousePos = inputSystem.GetMousePosition();

	//Process hover effect on the retry button
	float retryW = m_gameOverRetry->GetWidth() * m_gameOverRetry->GetScaleX();
	float retryH = m_gameOverRetry->GetHeight() * m_gameOverRetry->GetScaleY();

	bool isOverRetry =
		mousePos.x >= m_gameOverRetry->GetX() - retryW / 4 &&
		mousePos.x <= m_gameOverRetry->GetX() + retryW / 4 &&
		mousePos.y >= m_gameOverRetry->GetY() - retryH / 4 &&
		mousePos.y <= m_gameOverRetry->GetY() + retryH / 4;

	//Process hover effect on the exit button
	float exitW = m_gameOverExit->GetWidth() * m_gameOverExit->GetScaleX();
	float exitH = m_gameOverExit->GetHeight() * m_gameOverExit->GetScaleY();

	bool isOverExit =
		mousePos.x >= m_gameOverExit->GetX() - exitW / 4 &&
		mousePos.x <= m_gameOverExit->GetX() + exitW / 4 &&
		mousePos.y >= m_gameOverExit->GetY() - exitH / 4 &&
		mousePos.y <= m_gameOverExit->GetY() + exitH / 4;

	//Process the mouse click for the buttons
	if (inputSystem.GetMouseButtonState(0) == BS_PRESSED) {

		if (isOverRetry) {
			m_state = GAME_OVER_RETRY; 
		}
		else if (isOverExit) {
			m_state = GAME_OVER_EXIT; 
		}
	}

	XboxController* controller = inputSystem.GetController(0);

	float deltaTime = 1.0f / 60.0f;
	m_navCooldown -= deltaTime;

	if (controller && m_navCooldown <= 0.0f) {
		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_PRESSED) {  
			// Move focus up
			if (m_focus == GAMEOVER_FOCUS_EXIT) {
				m_focus = GAMEOVER_FOCUS_RETRY;
			}
			m_navCooldown = m_navDelay;
		}
		else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_PRESSED) {  
			// Move focus down
			if (m_focus == GAMEOVER_FOCUS_RETRY) {
				m_focus = GAMEOVER_FOCUS_EXIT;
			}
			m_navCooldown = m_navDelay;
		}
	}

	if (controller) {
		hoverRetry = isOverRetry || (m_focus == GAMEOVER_FOCUS_RETRY); 
		hoverExit = isOverExit || (m_focus == GAMEOVER_FOCUS_EXIT); 
	}
	else {
		hoverRetry = isOverRetry;
		hoverExit = isOverExit;
	}

	if (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED) {
		if (m_focus == GAMEOVER_FOCUS_RETRY) {
			m_state = GAME_OVER_RETRY; 
		}
		else if (m_focus == GAMEOVER_FOCUS_EXIT) {
			m_state = GAME_OVER_EXIT; 
		}
	}

	//Add sound to Retry button hover
	if (hoverRetry && !m_hoverRetryButton && m_hoverSound) {
		FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
		FMOD::Channel* m_hoverChannel = nullptr;

		fmod->playSound(m_hoverSound, nullptr, false, &m_hoverChannel);
		if (m_hoverChannel) {
			m_hoverChannel->setVolume(m_buttonVolume);
		}
		m_hoverRetryButton = true;
	}
	else if (!hoverRetry) {
		m_hoverRetryButton = false;
	}

	//Add sound to Exit button hover
	if (hoverExit && !m_hoverExitButton && m_hoverSound) {
		FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
		FMOD::Channel* m_hoverChannel = nullptr;

		fmod->playSound(m_hoverSound, nullptr, false, &m_hoverChannel);
		if (m_hoverChannel) {
			m_hoverChannel->setVolume(m_buttonVolume);
		}
		m_hoverExitButton = true;
	}
	else if (!hoverExit) {
		m_hoverExitButton = false;
	}

}

void GameOverMenu::Draw(Renderer& renderer) {
	//Draw back of the Game Over screen
	if (m_gameOverBack) { 
		m_gameOverBack->Draw(renderer); 
	}

	//Draw Retry button
	if (m_gameOverRetry) { 
		if (hoverRetry) { 
			m_gameOverRetry->SetScale(1.1f, -1.1f); 
		}
		else {
			m_gameOverRetry->SetScale(1.0f, -1.0f);  
		}
		m_gameOverRetry->Draw(renderer); 
	}

	//Draw Exit button
	if (m_gameOverExit) {
		if (hoverExit) {
			m_gameOverExit->SetScale(1.1f, -1.1f);
		}
		else {
			m_gameOverExit->SetScale(1.0f, -1.0f);
		}
		m_gameOverExit->Draw(renderer);
	}
}

void GameOverMenu::Reset() {
	m_state = GAME_OVER_WAITING; 
}

GameOverState GameOverMenu::GetState() const {
	return m_state; 
}
