// COMP710 GP Framework 2022
// This include:
#include "game.h"
#include "sprite.h"
// Local includes:
#include "renderer.h"
#include "logmanager.h"
#include "scenesplash.h"
#include "SceneGame.h"
#include "SceneMenu.h"
#include "inputsystem.h"
// Static Members:
Game* Game::sm_pInstance = 0;

Game& Game::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new Game();
	}
	return (*sm_pInstance);
}

void Game::DestroyInstance()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

Game::Game() 
	: m_pRenderer(0)
	, m_pInputSystem(0)
	, m_pSceneSplash(0)
	, m_pSceneGame(0)
	, m_bLooping(true)
	, m_bDebugWindowEnabled(false)
	,m_pSceneMenu(0)
	,m_menuComplete(false)
	, m_fmodSystem(nullptr) 
{
}

Game::~Game()
{
	delete m_pRenderer;
	m_pRenderer = 0;
	
	delete m_pInputSystem;
	m_pInputSystem = 0;
	
	delete m_pSceneSplash; 
	m_pSceneSplash = nullptr; 
	
	delete m_pSceneGame;
	m_pSceneGame = nullptr;

	delete m_pSceneMenu;
	m_pSceneMenu = nullptr;

	if (m_fmodSystem) {
		m_fmodSystem->close();
		m_fmodSystem->release();
		m_fmodSystem = nullptr;
	}
}

void Game::Quit()
{
	m_bLooping = false;
}

void Game::ToggleDebugWindow()
{
	m_bDebugWindowEnabled = !m_bDebugWindowEnabled;
	LogManager::GetInstance().Log("Debug window toggled");
}

bool Game::Initialise()
{

	//Initialize the FMOD system
	FMOD::System_Create(&m_fmodSystem); 
	m_fmodSystem->init(32, FMOD_INIT_NORMAL, nullptr); 

	int bbWidth = 1024;
	int bbHeight = 768;
	m_pRenderer = new Renderer();
	if (!m_pRenderer->Initialise(false, bbWidth, bbHeight))
	{
		LogManager::GetInstance().Log("Renderer failed to initialise!");
		return false;
	}
	
	// Initialize input system
	m_pInputSystem = new InputSystem();
	if (!m_pInputSystem->Initialise())
	{
		LogManager::GetInstance().Log("Input System failed to initialise!");
		return false;
	}
	
	bbWidth = m_pRenderer->GetWidth();
	bbHeight = m_pRenderer->GetHeight();
	m_iLastTime = SDL_GetPerformanceCounter();
	m_pRenderer->SetClearColour(0, 0, 0);

	// Initialize splash scene
	m_pSceneSplash = new SceneSplash();  
	if (!m_pSceneSplash->Initialise(*m_pRenderer)) {
		return false;
	}
	 
	//Initialise menu scene
	m_pSceneMenu = new SceneMenu();
	if (!m_pSceneMenu->Initialise(*m_pRenderer)) {
		return false;
	}

	// Initialize game scene
	m_pSceneGame = new SceneGame();
	if (!m_pSceneGame->Initialise(*m_pRenderer)) {
		return false;
	}

	//Initialise with AUT logo
	m_pRenderer->CreateStaticText("COMP710 GAME PROGRAMMING", 48);
	m_pTextSprite = m_pRenderer->CreateSprite("COMP710 GAME PROGRAMMING");  
	m_pTextSprite->SetScale(0.5f, -0.5f);

	int screenW = m_pRenderer->GetWidth();
	int screenH = m_pRenderer->GetHeight();

	m_pTextSprite->SetAlpha(1.0f);
	m_pTextSprite->SetX(screenW / 2);
	m_pTextSprite->SetY(screenH / 1.6); 

	return true;
}

bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;
	
	// Process input
	m_pInputSystem->ProcessInput();
	
	if (m_bLooping)
	{
		Uint64 current = SDL_GetPerformanceCounter();
		float deltaTime = (current - m_iLastTime) / static_cast<float>(SDL_GetPerformanceFrequency());
		m_iLastTime = current;
		m_fExecutionTime += deltaTime;
		Process(deltaTime);
#ifdef USE_LAG
		m_fLag += deltaTime;
		int innerLag = 0;
		while (m_fLag >= stepSize)
		{
			Process(stepSize);
			m_fLag -= stepSize;
			++m_iUpdateCount;
			++innerLag;
		}
#endif //USE_LAG
		Draw(*m_pRenderer);
	}
	return m_bLooping;
}

void Game::Process(float deltaTime)
{
	ProcessFrameCounting(deltaTime);
	
	// Process the active scene
	if (m_pSceneSplash && !m_pSceneSplash->IsFinished())
	{
		m_pSceneSplash->Process(deltaTime);
		if (m_pTextSprite) 
		{ 
			if (m_pSceneSplash->GetStage() == 0) {
				m_pTextSprite->SetAlpha(m_pSceneSplash->GetAlpha());
			}
			else {
				m_pTextSprite->SetAlpha(0.0f);
			}
		}
	}
	else if (!m_menuComplete) {
		m_pSceneMenu->ProcessInput(*m_pInputSystem);
		m_pSceneMenu->Process(deltaTime);

		switch (m_pSceneMenu->GetState()) {
		case MENU_START_GAME:
			m_pSceneMenu->StopMusic();
			m_menuComplete = true;
			break;
		case MENU_EXIT:
			Quit();
			break;
		default:
			break;
		}
	}
	else if (m_pSceneGame)
	{
		// Process game scene and pass input to it
		m_pSceneGame->ProcessInput(*m_pInputSystem);
		m_pSceneGame->Process(deltaTime);
	}

	if (m_fmodSystem) {
		m_fmodSystem->update();
	}
}

void Game::Draw(Renderer& renderer)
{
	++m_iFrameCount;
	renderer.Clear();
	
	// Draw the active scene
	if (m_pSceneSplash && !m_pSceneSplash->IsFinished())
	{
		m_pSceneSplash->Draw(renderer);
		if (m_pTextSprite)  
		{
			m_pTextSprite->Draw(renderer);  
		}
	}
	else if (m_pSceneMenu && !m_menuComplete) 
	{
		m_pSceneMenu->Draw(renderer);
	}
	else if (m_pSceneGame)
	{
		m_pSceneGame->Draw(renderer);
	}
	
	renderer.Present();
}

void Game::ProcessFrameCounting(float deltaTime)
{
	// Count total simulation time elapsed:
	m_fElapsedSeconds += deltaTime;
	// Frame Counter:
	if (m_fElapsedSeconds > 1.0f)
	{
		m_fElapsedSeconds -= 1.0f;
		m_iFPS = m_iFrameCount;
		m_iFrameCount = 0;
	}
}
