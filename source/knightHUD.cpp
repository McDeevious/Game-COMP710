#include "knighthud.h"
#include "animatedsprite.h" 
#include "renderer.h"
#include "logmanager.h"
#include <string>

KnightHUD::KnightHUD()
	: m_healthHUD(nullptr)
	, m_frameW(354.0f)
	, m_frameH(102.0f)
	, m_maxHealth(125)
	, m_score(0)
	, m_scoreHUD(nullptr)
{
}

KnightHUD::~KnightHUD() {
	delete m_healthHUD;
	m_healthHUD = nullptr;

	delete m_scoreHUD;
	m_scoreHUD = nullptr;
}

//Initialise the Health bar only
bool KnightHUD::InitialiseHealth(Renderer& renderer) {

	float screenW = renderer.GetWidth(); 
	float screenH = renderer.GetHeight();

	//Health HUD
	m_healthHUD = renderer.CreateAnimatedSprite("../game/assets/Sprites/UI/Knight_HealthUI.png");
	if (m_healthHUD) {

		m_healthHUD->SetupFrames(320, 102);
		m_healthHUD->SetFrameDuration(0.1f);
		m_healthHUD->SetScale(1.5f, -1.5f); 
		m_healthHUD->SetX(screenW / 5.6f);
		m_healthHUD->SetY(screenH / 12.0f);
		m_healthHUD->SetLooping(false);
		m_healthHUD->StopAnimating();
		m_healthHUD->SetCurrentFrame(0);
	}

	return true;
}

// Initialise the Score only
void KnightHUD::InitialiseScore(Renderer& renderer) {
	int screenW = renderer.GetWidth();
	int screenH = renderer.GetHeight();

	// Delete any existing score HUD
	if (m_scoreHUD) {
		delete m_scoreHUD;
		m_scoreHUD = nullptr;
	}

	// Reset score
	m_score = 0;

	// Create the display text
	char displayBuffer[32];
	sprintf_s(displayBuffer, "SCORE: %d", m_score);


	// Create the text texture
	renderer.CreateStaticText(displayBuffer, 48);

	// Create the sprite
	m_scoreHUD = renderer.CreateSprite(displayBuffer);

	if (m_scoreHUD) {

		m_scoreHUD->SetScale(0.5f, -0.5f);
		m_scoreHUD->SetX(screenW / 1.2f);
		m_scoreHUD->SetY(screenH / 12.0f);
		m_scoreHUD->SetAlpha(1.0f);
	}
}

void KnightHUD::ScoreUpdate(int newScore, Renderer& renderer) {
	m_score = newScore;

	// Delete the old sprite first to prevent memory leaks
	if (m_scoreHUD) {
		delete m_scoreHUD;
		m_scoreHUD = nullptr;
	}

	// Create the display text
	char displayBuffer[32];
	sprintf_s(displayBuffer, "SCORE: %d", m_score);

	// Create the text texture with the DISPLAY text (not the unique key)
	renderer.CreateStaticText(displayBuffer, 48);

	// Create new sprite using the DISPLAY text
	m_scoreHUD = renderer.CreateSprite(displayBuffer);

	if (m_scoreHUD) {

		int screenW = renderer.GetWidth();
		int screenH = renderer.GetHeight();

		m_scoreHUD->SetScale(0.5f, -0.5f);
		m_scoreHUD->SetX(screenW / 1.2f);
		m_scoreHUD->SetY(screenH / 12.0f);
		m_scoreHUD->SetAlpha(1.0f);
	}
}

void KnightHUD::HealthUpdate(int currentHealth) {

	if (currentHealth >= 0 && currentHealth <= m_maxHealth) {
		//Get health percentage
		float healthPercentage = static_cast<float>(currentHealth) / static_cast<float>(m_maxHealth);

		//Get the health frame to display
		int numFrames = 6;
		int healthFrame = static_cast<int>((1.0f - healthPercentage) * (numFrames - 1));

		if (healthFrame < 0) {
			healthFrame = 0;
		}

		if (healthFrame >= numFrames) {
			healthFrame = numFrames - 1;
		}

		m_healthHUD->SetCurrentFrame(healthFrame);
	}
}

void KnightHUD::Draw(Renderer& renderer) {
	if (m_healthHUD) {
		m_healthHUD->Draw(renderer);
	}

	if (m_scoreHUD) {
		m_scoreHUD->Draw(renderer);
	}
}