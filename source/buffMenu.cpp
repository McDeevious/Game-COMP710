#include "buffmenu.h"
#include "renderer.h"
#include "inputsystem.h"
#include "logmanager.h"
#include "xboxcontroller.h"
#include "game.h"

#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <SDL.h>

BuffMenu::BuffMenu()
	: m_buffBack(nullptr)
	, m_buffLeft(nullptr)
	, m_buffRight(nullptr)
	, m_state(BUFF_WAITING)
	, hoverBuff1(false)
	, hoverBuff2(false)
	, m_hoverSound(nullptr)
	, m_buttonVolume(0.4f)
	, m_hoverBuff1Button(false)
	, m_hoverBuff2Button(false)
	, m_focus(BUFF_FOCUS_1)
	, m_navCooldown(0.0f)
{
	srand(static_cast<unsigned>(time(nullptr)));  
}

BuffMenu::~BuffMenu() {
	delete m_buffBack;
	m_buffBack = nullptr;

	delete m_buffLeft;
	m_buffLeft = nullptr;

	delete m_buffRight;
	m_buffRight = nullptr;

	if (m_hoverSound) {
		m_hoverSound->release();
		m_hoverSound = nullptr;
	}
}

bool BuffMenu::Initialise(Renderer& renderer) {
	int screenW = renderer.GetWidth();
	int screenH = renderer.GetHeight();

	SDL_ShowCursor(SDL_ENABLE);

	FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
	fmod->createSound("../game/assets/Audio/Menu-Audio/button.wav", FMOD_DEFAULT, 0, &m_hoverSound);

	// Randomize buff types
	m_buffTypes[0] = static_cast<BuffType>(rand() % BUFF_COUNT); 

	do {
		m_buffTypes[1] = static_cast<BuffType>(rand() % BUFF_COUNT); 
	} while (m_buffTypes[1] == m_buffTypes[0]); // Ensure unique buffs 

	std::string paths[BUFF_COUNT] = { 
		"../game/assets/Sprites/Items/DEFUP.png",
		"../game/assets/Sprites/Items/DMGUP.png",
		"../game/assets/Sprites/Items/Speed.png",
		"../game/assets/Sprites/Items/JumpBoost.png",
		"../game/assets/Sprites/Items/MoreHealth.png",
		"../game/assets/Sprites/Items/Regen.png"
	};

	//Initialise the Buff Background
	m_buffBack = renderer.CreateSprite("../game/assets/Sprites/UI/Pause.png");
	if (m_buffBack) {
		int backW = m_buffBack->GetWidth();
		int backH = m_buffBack->GetHeight();

		float scaleY = (screenH * 0.75f) / backH;
		float scaleX = scaleY * (static_cast<float>(backW) / backH);

		m_buffBack->SetScale(scaleX, -scaleY);
		m_buffBack->SetX(screenW / 2);
		m_buffBack->SetY(screenH / 2);
		m_buffBack->SetAlpha(1.0f);
	}

	//Initialise the Play Button
	m_buffLeft = renderer.CreateSprite(paths[m_buffTypes[0]].c_str());
	if (m_buffLeft) {
		int playW = m_buffLeft->GetWidth();
		int playH = m_buffLeft->GetHeight();

		float scaleY = screenH * 0.5f; 
		float scaleX = scaleY / static_cast<float>(playH); 

		m_buffLeft->SetScale(scaleX, -scaleY);
		m_buffLeft->SetX(screenW / 2);
		m_buffLeft->SetY(screenH / 2.5f);
		m_buffLeft->SetAlpha(1.0f); 
	}

	m_buffRight = renderer.CreateSprite(paths[m_buffTypes[1]].c_str());  
	if (m_buffRight) {
		int playW = m_buffRight->GetWidth();
		int playH = m_buffRight->GetHeight();

		float scaleY = screenH * 0.5f;
		float scaleX = scaleY / static_cast<float>(playH); 

		m_buffRight->SetScale(scaleX, -scaleY);
		m_buffRight->SetX(screenW / 2);
		m_buffRight->SetY(screenH / 1.6);
		m_buffRight->SetAlpha(1.0f);
	}

	return m_buffBack && m_buffLeft && m_buffRight; 
}

void BuffMenu::ProcessInput(InputSystem& inputSystem) {
	const Vector2& mousePos = inputSystem.GetMousePosition();

	// Hover sound for Buff 1
	if (hoverBuff1 && !m_hoverBuff1Button && m_hoverSound) {
		FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
		FMOD::Channel* channel = nullptr;
		fmod->playSound(m_hoverSound, nullptr, false, &channel);
		if (channel) channel->setVolume(m_buttonVolume);
		m_hoverBuff1Button = true;
	}
	else if (!hoverBuff1) {
		m_hoverBuff1Button = false;
	}

	// Hover sound for Buff 2
	if (hoverBuff2 && !m_hoverBuff2Button && m_hoverSound) {
		FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
		FMOD::Channel* channel = nullptr;
		fmod->playSound(m_hoverSound, nullptr, false, &channel);
		if (channel) channel->setVolume(m_buttonVolume);
		m_hoverBuff2Button = true;
	}
	else if (!hoverBuff2) {
		m_hoverBuff2Button = false;
	}

	// --- Hover detection
	float buff1W = m_buffLeft->GetWidth() * m_buffLeft->GetScaleX();
	float buff1H = m_buffLeft->GetHeight() * m_buffLeft->GetScaleY();

	bool isOverBuff1 =
		mousePos.x >= m_buffLeft->GetX() - buff1W / 4 &&
		mousePos.x <= m_buffLeft->GetX() + buff1W / 4 &&
		mousePos.y >= m_buffLeft->GetY() - buff1H / 4 &&
		mousePos.y <= m_buffLeft->GetY() + buff1H / 4;

	float buff2W = m_buffRight->GetWidth() * m_buffRight->GetScaleX();
	float buff2H = m_buffRight->GetHeight() * m_buffRight->GetScaleY();

	bool isOverBuff2 =
		mousePos.x >= m_buffRight->GetX() - buff2W / 4 &&
		mousePos.x <= m_buffRight->GetX() + buff2W / 4 &&
		mousePos.y >= m_buffRight->GetY() - buff2H / 4 &&
		mousePos.y <= m_buffRight->GetY() + buff2H / 4;

	hoverBuff1 = isOverBuff1;
	hoverBuff2 = isOverBuff2;

	// --- Selection by mouse
	if (inputSystem.GetMouseButtonState(0) == BS_PRESSED) {
		if (isOverBuff1 && m_selectionCount < 2) {
			m_selectedBuffs[m_selectionCount++] = m_buffTypes[0];
		}
		else if (isOverBuff2 && m_selectionCount < 2) {
			m_selectedBuffs[m_selectionCount++] = m_buffTypes[1];
		}
	}

	// --- Controller input
	XboxController* controller = inputSystem.GetController(0);
	float deltaTime = 1.0f / 60.0f;
	m_navCooldown -= deltaTime;

	if (controller && m_navCooldown <= 0.0f) {
		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_UP) == BS_PRESSED) {
			m_focus = BUFF_FOCUS_1;
			m_navCooldown = m_navDelay;
		}
		else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == BS_PRESSED) {
			m_focus = BUFF_FOCUS_2;
			m_navCooldown = m_navDelay;
		}
	}

	// Focus → hover mapping
	if (controller) {
		hoverBuff1 = isOverBuff1 || (m_focus == BUFF_FOCUS_1);
		hoverBuff2 = isOverBuff2 || (m_focus == BUFF_FOCUS_2);
	}
	else {
		hoverBuff1 = isOverBuff1;
		hoverBuff2 = isOverBuff2;
	}

	// Controller confirm
	if (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED) {
		if (m_focus == BUFF_FOCUS_1 && m_selectionCount < 2) {
			m_selectedBuffs[m_selectionCount++] = m_buffTypes[0];
		}
		else if (m_focus == BUFF_FOCUS_2 && m_selectionCount < 2) {
			m_selectedBuffs[m_selectionCount++] = m_buffTypes[1];
		}
	}

	// --- Update state
	if (m_selectionCount == 1) {
		m_state = BUFF_SELECTED_1;
	}
	else if (m_selectionCount == 2) {
		m_state = BUFF_DONE;
	}
}

void BuffMenu::Draw(Renderer& renderer) {
	//Draw the back of the back of the pause menu
	if (m_buffBack) {
		m_buffBack->Draw(renderer);
	}

	//Draw the play button
	if (m_buffLeft) {
		if (hoverBuff1) {
			m_buffLeft->SetScale(2.1f, -2.1f);
		}
		else {
			m_buffLeft->SetScale(2.0f, -2.0f);
		}
		m_buffLeft->Draw(renderer);
	}

	//Draw the Retry button
	if (m_buffRight) {
		if (hoverBuff2) {
			m_buffRight->SetScale(2.1f, -2.1f);
		}
		else {
			m_buffRight->SetScale(2.0f, -2.0f);
		}
		m_buffRight->Draw(renderer);
	}
}

void BuffMenu::Reset() {
	m_state = BUFF_WAITING;
	m_selectionCount = 0; 
	m_hoverBuff1Button = false; 
	m_hoverBuff2Button = false; 
	hoverBuff1 = false; 
	hoverBuff2 = false; 
	m_focus = BUFF_FOCUS_1; 
	m_navCooldown = 0.0f;  
}

BuffChoiceState BuffMenu::GetState() const {
	return m_state; 
}

BuffType BuffMenu::GetBuff1() const {
	return m_selectedBuffs[0];
}

BuffType BuffMenu::GetBuff2() const {
	return m_selectedBuffs[1];
}

BuffType BuffMenu::GetSelectedBuff() const
{
	if (m_selectionCount > 0) 
	{
		return m_selectedBuffs[0];
	}
	else 
	{
		return BUFF_COUNT;
	}
}
