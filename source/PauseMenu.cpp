#include "pausemenu.h"
#include "renderer.h"
#include "inputsystem.h"
#include "logmanager.h"
#include "xboxcontroller.h"
#include "game.h"

#include <SDL.h>

PauseMenu::PauseMenu() 
	: m_pauseBack(nullptr)
	, m_pausePlay(nullptr)
	, m_pauseRetry(nullptr)
	, m_pauseExit(nullptr)
	, m_state(PAUSE_WAITING)
	, hoverPlay(false)
	, hoverRetry(false)
	, hoverExit(false)
	, m_hoverSound(nullptr)
	, m_buttonVolume(0.4f)
	, m_hoverPlayButton(false)
	, m_hoverRetryButton(false) 
	, m_hoverExitButton(false)
	, m_focus(PAUSE_FOCUS_PLAY)
	, m_navCooldown(0.0f) 
{
}

PauseMenu::~PauseMenu() {
	delete  m_pauseBack; 
	m_pauseBack = nullptr;

	delete  m_pausePlay; 
	m_pausePlay = nullptr; 

	delete  m_pauseRetry; 
	m_pauseRetry = nullptr; 

	delete  m_pauseExit; 
	m_pauseExit = nullptr;  

	if (m_hoverSound) {
		m_hoverSound->release();
		m_hoverSound = nullptr;
	}
}

bool PauseMenu::Initialise(Renderer& renderer) {
	int screenW = renderer.GetWidth(); 
	int screenH = renderer.GetHeight(); 

	SDL_ShowCursor(SDL_ENABLE);

	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();  
	fmod->createSound("../game/assets/Audio/Menu-Audio/button.wav", FMOD_DEFAULT, 0, &m_hoverSound);

	//Initialise the Pause Background
	m_pauseBack = renderer.CreateSprite("../game/assets/Sprites/UI/Pause.png");
	if (m_pauseBack) {
		int backW = m_pauseBack->GetWidth();
		int backH = m_pauseBack->GetHeight();

		float scaleY = (screenH * 0.75f) / backH;
		float scaleX = scaleY * (static_cast<float>(backW) / backH);

		m_pauseBack->SetScale(scaleX, -scaleY); 
		m_pauseBack->SetX(screenW / 2); 
		m_pauseBack->SetY(screenH / 2); 
		m_pauseBack->SetAlpha(1.0f); 
	}

	//Initialise the Play Button
	m_pausePlay = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Play.png");
	if (m_pausePlay) {
		int playW = m_pausePlay->GetWidth();
		int playH = m_pausePlay->GetHeight();

		float scaleX = (screenW / playW) * 0.2f;
		float scaleY = (screenH / playH) * 0.2f; 

		m_pausePlay->SetScale(scaleX, -scaleY); 
		m_pausePlay->SetX(screenW / 1.96f);  
		m_pausePlay->SetY(screenH / 2.6);
		m_pausePlay->SetAlpha(1.0f);
	}

	//Initialise the Retry Button
	m_pauseRetry = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Retry.png");
	if (m_pauseRetry) {
		int retryW = m_pauseRetry->GetWidth(); 
		int retryH = m_pauseRetry->GetHeight(); 

		float scaleX = (screenW / retryW) * 0.2f;
		float scaleY = (screenH / retryH) * 0.2f; 

		m_pauseRetry->SetScale(scaleX, -scaleY);
		m_pauseRetry->SetX(screenW / 1.98f); 
		m_pauseRetry->SetY(screenH / 2);
		m_pauseRetry->SetAlpha(1.0f); 
	}

	//Initialise the Retry Button
	m_pauseExit = renderer.CreateSprite("../game/assets/Sprites/UI/Pause-Exit.png"); 
	if (m_pauseExit) { 
		int exitW = m_pauseExit->GetWidth();
		int exitH = m_pauseExit->GetHeight();

		float scaleX = (screenW / exitW) * 0.2f;
		float scaleY = (screenH / exitH) * 0.2f;

		m_pauseExit->SetScale(scaleX, -scaleY);   
		m_pauseExit->SetX(screenW / 2); 
		m_pauseExit->SetY(screenH / 1.6); 
		m_pauseExit->SetAlpha(1.0f);  
	}

	return m_pauseBack && m_pausePlay && m_pauseRetry && m_pauseExit;
}

void PauseMenu::ProcessInput(InputSystem& inputSystem) {
	const Vector2& mousePos = inputSystem.GetMousePosition();

	//Add sound to Play button hover
	if (hoverPlay && !m_hoverPlayButton && m_hoverSound) {
		FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
		FMOD::Channel* m_hoverChannel = nullptr;

		fmod->playSound(m_hoverSound, nullptr, false, &m_hoverChannel);
		if (m_hoverChannel) {
			m_hoverChannel->setVolume(m_buttonVolume);
		}
		m_hoverPlayButton = true;
	}
	else if (!hoverPlay) {
		m_hoverPlayButton = false;
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

	//Process the hover effect on the play button
	float playW = m_pausePlay->GetWidth() * m_pausePlay->GetScaleX();
	float playH = m_pausePlay->GetHeight() * m_pausePlay->GetScaleY(); 

	bool isOverPlay = 
		mousePos.x >= m_pausePlay->GetX() - playW / 4 &&
		mousePos.x <= m_pausePlay->GetX() + playW / 4 &&
		mousePos.y >= m_pausePlay->GetY() - playH / 4 && 
		mousePos.y <= m_pausePlay->GetY() + playH / 4; 

	//Process hover effect on the retry button
	float retryW = m_pauseRetry->GetWidth() * m_pauseRetry->GetScaleX(); 
	float retryH = m_pauseRetry->GetHeight() * m_pauseRetry->GetScaleY(); 

	bool isOverRetry = 
		mousePos.x >= m_pauseRetry->GetX() - retryW / 4 &&
		mousePos.x <= m_pauseRetry->GetX() + retryW / 4 &&
		mousePos.y >= m_pauseRetry->GetY() - retryH / 4 &&
		mousePos.y <= m_pauseRetry->GetY() + retryH / 4;
	
	//Process hover effect on the exit button
	float exitW = m_pauseExit->GetWidth() * m_pauseExit->GetScaleX();
	float exitH = m_pauseExit->GetHeight() * m_pauseExit->GetScaleY();

	bool isOverExit =  
		mousePos.x >= m_pauseExit->GetX() - exitW / 4 &&
		mousePos.x <= m_pauseExit->GetX() + exitW / 4 &&
		mousePos.y >= m_pauseExit->GetY() - exitH / 4 && 
		mousePos.y <= m_pauseExit->GetY() + exitH / 4; 

	hoverPlay = isOverPlay;
	hoverRetry = isOverRetry;
	hoverExit = isOverExit;

	if (inputSystem.GetMouseButtonState(0) == BS_PRESSED) {

		if (isOverPlay) {
			m_state = PAUSE_PLAY; 
		}
		else if (isOverRetry) {
			m_state = PAUSE_RETRY;
		}
		else if (isOverExit) {
			m_state = PAUSE_EXIT;
		}
	}

	XboxController* controller = inputSystem.GetController(0); 

	float deltaTime = 1.0f / 60.0f;
	m_navCooldown -= deltaTime;  

	if (controller && m_navCooldown <= 0.0f) {
		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED) {
			// Move focus up
			if (m_focus == PAUSE_FOCUS_RETRY) {
				m_focus = PAUSE_FOCUS_PLAY;
			}
			else if (m_focus == PAUSE_FOCUS_EXIT) {
				m_focus = PAUSE_FOCUS_RETRY;
			}
			m_navCooldown = m_navDelay;
		}
		else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED) {
			// Move focus down
			if (m_focus == PAUSE_FOCUS_PLAY) {
				m_focus = PAUSE_FOCUS_RETRY;
			}
			else if (m_focus == PAUSE_FOCUS_RETRY) {
				m_focus = PAUSE_FOCUS_EXIT;
			}
			m_navCooldown = m_navDelay;
		}
	}

	if (controller) {
		hoverPlay = isOverPlay || (m_focus == PAUSE_FOCUS_PLAY);
		hoverRetry = isOverRetry || (m_focus == PAUSE_FOCUS_RETRY);
		hoverExit = isOverExit || (m_focus == PAUSE_FOCUS_EXIT);
	}
	else {
		hoverPlay = isOverPlay;
		hoverExit = isOverExit;
	}

	if (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED) {
		if (m_focus == PAUSE_FOCUS_PLAY) {
			m_state = PAUSE_PLAY; 
		}
		else if (m_focus == PAUSE_FOCUS_RETRY) {
			m_state = PAUSE_RETRY; 
		}
		else if (m_focus == PAUSE_FOCUS_EXIT) {
			m_state = PAUSE_EXIT; 
		}
	}

}

void PauseMenu::Draw(Renderer& renderer) {
	//Draw the back of the back of the pause menu
	if (m_pauseBack) {
		m_pauseBack->Draw(renderer);
	}

	//Draw the play button
	if (m_pausePlay) {
		if (hoverPlay) {
			m_pausePlay->SetScale(1.1f, -1.1f);
		}
		else {
			m_pausePlay->SetScale(1.0f, -1.0f);
		}
		m_pausePlay->Draw(renderer);
	}

	//Draw the Retry button
	if (m_pauseRetry) { 
		if (hoverRetry) {
			m_pauseRetry->SetScale(1.1f, -1.1f); 
		}
		else {
			m_pauseRetry->SetScale(1.0f, -1.0f); 
		}
		m_pauseRetry->Draw(renderer);  
	}

	//Draw the Exit button
	if (m_pauseExit) {
		if (hoverExit) {
			m_pauseExit->SetScale(1.1f, -1.1f);
		}
		else {
			m_pauseExit->SetScale(1.0f, -1.0f);
		}
		m_pauseExit->Draw(renderer);
	}

}

void PauseMenu::Reset() {
	m_state = PAUSE_WAITING;
}

PauseState PauseMenu::GetState() const {
	return m_state; 
}
