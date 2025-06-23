#include "victorymenu.h"
#include "renderer.h"
#include "inputsystem.h"
#include "logmanager.h"
#include "xboxcontroller.h"
#include "game.h"

#include <SDL.h>

VictoryMenu::VictoryMenu()
	: m_victoryBack(nullptr)
	, m_victoryRetry(nullptr)
	, m_victoryExit(nullptr)
	, hoverRetry(false)
	, hoverExit(false)
	, m_state(VICTORY_WAITING)
	, m_hoverSound(nullptr)
	, m_buttonVolume(0.4f)
	, m_hoverRetryButton(false)
	, m_hoverExitButton(false)
	, m_focus(VICTORY_FOCUS_NONE)
	, m_navCooldown(0.0f)
{
}

VictoryMenu::~VictoryMenu() {
	delete  m_victoryBack;
	m_victoryBack = nullptr;

	delete  m_victoryRetry;
	m_victoryRetry = nullptr;

	delete  m_victoryExit;
	m_victoryExit = nullptr;

	if (m_hoverSound) {
		m_hoverSound->release();
		m_hoverSound = nullptr;
	}
}

bool VictoryMenu::Initialise(Renderer& renderer) {
	int screenW = renderer.GetWidth();
	int screenH = renderer.GetHeight();

	SDL_ShowCursor(SDL_ENABLE);

	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
	fmod->createSound("../game/assets/Audio/Menu-Audio/button.wav", FMOD_DEFAULT, 0, &m_hoverSound);

	//Initialise the Pause Background
	m_victoryBack = renderer.CreateSprite("../game/assets/Sprites/UI/FateSevered.png"); 
	if (m_victoryBack) {
		int backW = m_victoryBack->GetWidth();
		int backH = m_victoryBack->GetHeight();

		float scaleX = (screenW / backW) * 0.75f;
		float scaleY = (screenH / backH) * 0.75f;

		m_victoryBack->SetScale(scaleX, -scaleY);
		m_victoryBack->SetX(screenW / 1.75);
		m_victoryBack->SetY(screenH / 2);
		m_victoryBack->SetAlpha(1.0f);
	}

	//Initialise the Retry Button
	m_victoryRetry = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Retry.png"); 
	if (m_victoryRetry) {
		int retryW = m_victoryRetry->GetWidth();
		int retryH = m_victoryRetry->GetHeight();

		float scaleX = (screenW / retryW) * 0.2f;
		float scaleY = (screenH / retryH) * 0.2f;

		m_victoryRetry->SetScale(scaleX, -scaleY);
		m_victoryRetry->SetX(screenW / 2.5);
		m_victoryRetry->SetY(screenH / 1.41);
		m_victoryRetry->SetAlpha(1.0f);
	}

	//Initialise the Exit Button
	m_victoryExit = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Exit.png");
	if (m_victoryExit) {
		int exitW = m_victoryExit->GetWidth();
		int exitH = m_victoryExit->GetHeight();

		float scaleX = (screenW / exitW) * 0.2f;
		float scaleY = (screenH / exitH) * 0.2f;

		m_victoryExit->SetScale(scaleX, -scaleY);
		m_victoryExit->SetX(screenW / 1.75);
		m_victoryExit->SetY(screenH / 1.40);
		m_victoryExit->SetAlpha(1.0f);
	}

	return m_victoryBack && m_victoryRetry && m_victoryExit; 
}

void VictoryMenu::ProcessInput(InputSystem& inputSystem) {
	const Vector2& mousePos = inputSystem.GetMousePosition();

	//Process hover effect on the retry button
	float retryW = m_victoryRetry->GetWidth() * m_victoryRetry->GetScaleX();
	float retryH = m_victoryRetry->GetHeight() * m_victoryRetry->GetScaleY();

	bool isOverRetry =
		mousePos.x >= m_victoryRetry->GetX() - retryW / 4 &&
		mousePos.x <= m_victoryRetry->GetX() + retryW / 4 &&
		mousePos.y >= m_victoryRetry->GetY() - retryH / 4 &&
		mousePos.y <= m_victoryRetry->GetY() + retryH / 4;

	//Process hover effect on the exit button
	float exitW = m_victoryExit->GetWidth() * m_victoryExit->GetScaleX();
	float exitH = m_victoryExit->GetHeight() * m_victoryExit->GetScaleY();

	bool isOverExit =
		mousePos.x >= m_victoryExit->GetX() - exitW / 4 &&
		mousePos.x <= m_victoryExit->GetX() + exitW / 4 &&
		mousePos.y >= m_victoryExit->GetY() - exitH / 4 &&
		mousePos.y <= m_victoryExit->GetY() + exitH / 4;

	//Process the mouse click for the buttons
	if (inputSystem.GetMouseButtonState(0) == BS_PRESSED) {

		if (isOverRetry) {
			m_state = VICTORY_RETRY;
		}
		else if (isOverExit) {
			m_state = VICTORY_EXIT;
		}
	}

	XboxController* controller = inputSystem.GetController(0);

	float deltaTime = 1.0f / 60.0f;
	m_navCooldown -= deltaTime;

	if (controller && m_navCooldown <= 0.0f) {
		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_PRESSED) {
			// Move focus up
			if (m_focus == VICTORY_FOCUS_EXIT) {
				m_focus = VICTORY_FOCUS_RETRY; 
			}
			m_navCooldown = m_navDelay;
		}
		else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_PRESSED) {
			// Move focus down
			if (m_focus == VICTORY_FOCUS_RETRY) { 
				m_focus = VICTORY_FOCUS_EXIT; 
			}
			m_navCooldown = m_navDelay;
		}
	}

	if (controller) {
		hoverRetry = isOverRetry || (m_focus == VICTORY_FOCUS_RETRY);
		hoverExit = isOverExit || (m_focus == VICTORY_FOCUS_EXIT);
	}
	else {
		hoverRetry = isOverRetry;
		hoverExit = isOverExit;
	}

	if (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED) {
		if (m_focus == VICTORY_FOCUS_RETRY) {
			m_state = VICTORY_RETRY; 
		}
		else if (m_focus == VICTORY_FOCUS_EXIT) {
			m_state = VICTORY_EXIT;  
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

void VictoryMenu::Draw(Renderer& renderer) {
	//Draw back of the Game Over screen
	if (m_victoryBack) {
		m_victoryBack->Draw(renderer);
	}

	//Draw Retry button
	if (m_victoryRetry) {
		if (hoverRetry) {
			m_victoryRetry->SetScale(1.1f, -1.1f);
		}
		else {
			m_victoryRetry->SetScale(1.0f, -1.0f);
		}
		m_victoryRetry->Draw(renderer);
	}

	//Draw Exit button
	if (m_victoryExit) {
		if (hoverExit) {
			m_victoryExit->SetScale(1.1f, -1.1f);
		}
		else {
			m_victoryExit->SetScale(1.0f, -1.0f);
		}
		m_victoryExit->Draw(renderer);
	}
}

void VictoryMenu::Reset() {
	m_state = VICTORY_WAITING; 
}

VictoryState VictoryMenu::GetState() const {
	return m_state;
}
