#include "SceneGame.h"
#include "SceneMenu.h"
#include "renderer.h"
#include "knightclass.h"
#include "BackgroundManager.h"
#include "PauseMenu.h"
#include "GameOverMenu.h"
#include "SceneGuide.h"
#include "logmanager.h"
#include "collision.h"
#include "knighthud.h"
#include "game.h"
#include "xboxcontroller.h"
//enemies
#include "orc.h"
#include "armoredOrc.h"
#include "eliteOrc.h"
#include "riderOrc.h"

SceneGame::SceneGame()
    : m_pBackgroundManager(nullptr)
    , m_pKnightClass(nullptr)
    , m_pPauseMenu(nullptr)
    , m_pGameOverMenu(nullptr)
    , m_pRenderer(nullptr)
    , m_pKnightHUD(nullptr)
    ,m_pSceneGuide(nullptr)
    , m_scrollDistance(0.0f)
    , m_gameState(GAME_STATE_PLAYING)
    , m_score(0)
    , m_gameMusic(nullptr)
    , m_gameChannel(nullptr)
    , m_gameVolume(0.2f)
    , m_gameStartPlayed(false)
    , m_nextWaveOffset(0.0f)
{
}

SceneGame::~SceneGame()
{
    delete m_pBackgroundManager;
    m_pBackgroundManager = nullptr;

    delete m_pPauseMenu;
    m_pPauseMenu = nullptr;

    delete m_pGameOverMenu; 
    m_pGameOverMenu = nullptr;

    delete m_pKnightClass;
    m_pKnightClass = nullptr;

    delete m_pKnightHUD;
    m_pKnightHUD = nullptr; 

    for (Orc* orc : m_orcs)
    {
        delete orc;
    }
    m_orcs.clear();

    if (m_gameChannel)
    {
        m_gameChannel->stop();
        m_gameChannel = nullptr;
    }

    if (m_gameMusic)
    {
        m_gameMusic->release();
        m_gameMusic = nullptr;
    }
}

bool SceneGame::Initialise(Renderer& renderer)
{
    m_pRenderer = &renderer;

    m_pBackgroundManager = new BackgroundManager();
    if (!m_pBackgroundManager->Initialise(renderer))
        return false;

    m_pPauseMenu = new PauseMenu();
    if (!m_pPauseMenu->Initialise(renderer))
        return false;

    m_pKnightClass = new KnightClass();
    if (!m_pKnightClass->Initialise(renderer))
        return false;

    m_pGameOverMenu = new GameOverMenu();
    if (!m_pGameOverMenu->Initialise(renderer))
        return false;

    m_pKnightHUD = new KnightHUD();
    if (!m_pKnightHUD->InitialiseHealth(renderer))
        return false;

    m_pSceneGuide = new SceneGuide();
    if (!m_pSceneGuide->Initialise(renderer))
        return false;

    m_pKnightClass->SetBoundaries(50, renderer.GetWidth() - 50, 50, renderer.GetHeight() - 50);

    SpawnOrcs(renderer);

    // Initialize score HUD after orcs are created
    if (m_pKnightHUD) {
        m_pKnightHUD->InitialiseScore(renderer);
    }

    return true;
}

void SceneGame::Process(float deltaTime)
{
    m_pKnightClass->Process(deltaTime);
    m_pBackgroundManager->Process(deltaTime);

    if (m_pKnightHUD && m_pKnightClass) {
        m_pKnightHUD->HealthUpdate(m_pKnightClass->GetHealth());
    }

    if (m_pSceneGuide && !m_pSceneGuide->IsFinished()) {
        m_pSceneGuide->Process(deltaTime);
    }

    if (m_gameState == GAME_STATE_PLAYING)
    {
        Vector2 move = m_pKnightClass->GetLastMovementDirection();
        m_scrollDistance += move.x * deltaTime * 120.0f;
        m_pBackgroundManager->UpdateScrollFromCharacterMovement(move, deltaTime);

        if (!m_gameStartPlayed) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            fmod->createSound("../game/assets/Audio/Menu-Audio/8BitGameplay.mp3", FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, 0, &m_gameMusic); 
            fmod->playSound(m_gameMusic, 0, false, &m_gameChannel);
            if (m_gameChannel) {
                m_gameChannel->setVolume(m_gameVolume);
            }
            m_gameStartPlayed = true;
        }

        CheckKnightState();

        float worldX = m_scrollDistance + m_pKnightClass->GetPosition().x; 
        Vector2 worldKnightPos(worldX, m_pKnightClass->GetPosition().y); 

        SpawnOrcs(*m_pRenderer); 

        for (Orc* orc : m_orcs) {
            if (orc) {
                if (orc->IsAlive()) {
                    orc->UpdateAI(worldKnightPos, deltaTime);
                }
                orc->Process(deltaTime);
            }
        }

        for (Orc* orc : m_orcs) {
            if (!orc || orc->IsAlive() || orc->WasScored())
                continue;

            m_score += orc->GetScore();
            orc->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        if (m_pKnightClass->isAttacking() && m_pKnightClass->AttackDamage() > 0) {
            Hitbox knightAttackBox = m_pKnightClass->GetAttackHitbox();
            knightAttackBox.x += m_scrollDistance;

            for (Orc* orc : m_orcs) {
                if (!orc || !orc->IsAlive()) continue;

                if (Collision::CheckCollision(knightAttackBox, orc->GetHitbox())) {
                    orc->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }
        }

        Hitbox knightHitbox = m_pKnightClass->GetHitbox();

        for (Orc* orc : m_orcs) {
            if (orc && orc->IsAlive() && orc->IsAttacking()) {
                Hitbox orcAttackBox = orc->GetAttackHitbox();
                orcAttackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(orcAttackBox, knightHitbox)) {
                    int damage = GetOrcAttackDamage(orc);
                    if (!m_pKnightClass->isBlocking()) {
                        m_pKnightClass->TakeDamage(damage);
                    }
                }
            }
        }
    }
    else if (m_gameState == GAME_STATE_RESTART)
    {
        m_gameState = GAME_STATE_PLAYING;
    }
}

void SceneGame::Draw(Renderer& renderer)
{
    m_pBackgroundManager->Draw(renderer);

    for (Orc* orc : m_orcs)
    {
        if (orc) { // Draw all orcs, even dead ones
            orc->Draw(renderer, m_scrollDistance);
        }
    }

    if (m_pSceneGuide && !m_pSceneGuide->IsFinished()) {
        m_pSceneGuide->Draw(renderer); 
    }

    m_pKnightClass->Draw(renderer);

    if (m_pKnightHUD) {
        m_pKnightHUD->Draw(renderer);
    }

    if (m_gameState == GAME_STATE_PAUSED)
    {
        m_pPauseMenu->Draw(renderer);  
    }

    // Display game over message
    if (m_gameState == GAME_STATE_GAME_OVER) {
        m_pGameOverMenu->Draw(renderer); 
    }
}

void SceneGame::ProcessInput(InputSystem& inputSystem)
{
    //Process input during gameplay
    if (m_gameState == GAME_STATE_PLAYING) {
        m_pKnightClass->ProcessInput(inputSystem);
    }

    XboxController* controller = inputSystem.GetController(0);

    //Process pause menu
    if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED)) {
        if (m_gameState == GAME_STATE_PLAYING) {
            m_gameState = GAME_STATE_PAUSED;
        }
        else if (m_gameState == GAME_STATE_PAUSED) {
            m_gameState = GAME_STATE_PLAYING;
        }
    }

    if (m_gameState == GAME_STATE_PAUSED) {
        m_pPauseMenu->ProcessInput(inputSystem);

        PauseState state = m_pPauseMenu->GetState();
        if (state == PAUSE_PLAY) {
            m_gameState = GAME_STATE_PLAYING;
            m_pPauseMenu->Reset();
        }
        else if (state == PAUSE_RETRY) {
            RestartGame();
            m_gameState = GAME_STATE_RESTART;
            m_pPauseMenu->Reset();
        }
        else if (state == PAUSE_EXIT) {
            SDL_Quit();
            exit(0);
        }

        return; // Skip knight input when paused
    }

    if (m_gameState == GAME_STATE_GAME_OVER) {
        m_pGameOverMenu->ProcessInput(inputSystem);

        GameOverState state = m_pGameOverMenu->GetState();

        if (state == GAME_OVER_RETRY) { 
            RestartGame(); 
            m_gameState = GAME_STATE_RESTART; 
            m_pGameOverMenu->Reset();
        }
        else if (state == GAME_OVER_EXIT) {
            SDL_Quit(); 
            exit(0); 
        }
    }
}

void SceneGame::SpawnOrcs(Renderer& renderer)
{
    float orcY = renderer.GetHeight() * 0.8f;

    // Array of orc placements with varied positions and behaviors
    const OrcPlacement wave1[] = {
        { 1000.0f, orcY, ORC_PATROL, 300.0f, ORC },
        { 2200.0f, orcY, ORC_IDLE, 0.0f, ORC },
        { 3300.0f, orcY, ORC_PATROL, 300.0f, ORC_ARMORED },
        { 4500.0f, orcY, ORC_IDLE, 300.0f, ORC_ELITE },
        { 5500.0f, orcY, ORC_PATROL, 400.0f, ORC_RIDER },
    };

    const int waveCount = sizeof(wave1) / sizeof(wave1[0]);
    float worldX = m_scrollDistance + m_pKnightClass->GetPosition().x;
    float screenW = static_cast<float>(m_pRenderer->GetWidth());

    char buffer[128];
    sprintf_s(buffer, "Checking wave spawn: worldX + screenW = %.2f | nextWaveOffset = %.2f", worldX + screenW, m_nextWaveOffset);
    LogManager::GetInstance().Log(buffer);

    if (worldX + screenW > m_nextWaveOffset) {
        SpawnOrcWave(wave1, waveCount, m_nextWaveOffset, renderer);
        m_nextWaveOffset += 6000.0f;
    }
}

void SceneGame::SpawnOrcWave(const OrcPlacement* wave, int count, float offset, Renderer& renderer)
{
    char buffer[256];
    sprintf_s(buffer, "Spawning Orc Wave with offset: %.2f", offset);
    LogManager::GetInstance().Log(buffer);

    for (int i = 0; i < count; i++) 
    { 
        Orc* orc = nullptr;

        switch (wave[i].type)
        {
        case ORC_RIDER:
            orc = new RiderOrc();
            break;
        case ORC_ELITE:
            orc = new EliteOrc();
            break;
        case ORC_ARMORED:
            orc = new ArmoredOrc();
            break;
        case ORC:
        default:
            orc = new Orc();
            break;
        }

        if (!orc || !orc->Initialise(renderer))
        {
            delete orc;
            continue;
        }

        float spawnX = wave[i].posX + offset;
        orc->SetPosition(spawnX, wave[i].posY); 
        orc->SetBehavior(wave[i].behavior); 

        if (wave[i].behavior != ORC_IDLE) { 
            orc->SetPatrolRange(spawnX - wave[i].patrolRange, spawnX + wave[i].patrolRange); 
            LogManager::GetInstance().Log(buffer);
        }

        m_orcs.push_back(orc);
    }

    sprintf_s(buffer, "Wave spawn complete. Total orcs now: %zu", m_orcs.size()); 
    LogManager::GetInstance().Log(buffer); 
}

int SceneGame::GetOrcAttackDamage(Orc* orc) const {
    // Base damage for regular orcs
    int baseDamage = 15;

    // Check orc type using dynamic_cast
    if (dynamic_cast<ArmoredOrc*>(orc)) {
        return baseDamage + 5; 
    }
    else if (dynamic_cast<EliteOrc*>(orc)) { 
        return baseDamage + 12;  
    }
    else if (dynamic_cast<RiderOrc*>(orc)) {
        return baseDamage + 10;
    }

    return baseDamage;
}

void SceneGame::CheckKnightState()
{
    // Check if knight has died and update game state
    if (m_pKnightClass->IsDead() && m_gameState == GAME_STATE_PLAYING) {
        m_gameState = GAME_STATE_GAME_OVER;
    }
}

void SceneGame::RestartGame()
{
    // Clean up existing objects
    for (Orc* orc : m_orcs) {
        delete orc;
    }
    m_orcs.clear();

    // Reset game state variables
    m_scrollDistance = 0.0f;
    m_score = 0; // Reset score
    m_nextWaveOffset = 0.0f;

    // Recreate knight
    delete m_pKnightClass;
    m_pKnightClass = new KnightClass();
    m_pKnightClass->Initialise(*m_pRenderer);
    m_pKnightClass->SetBoundaries(50, m_pRenderer->GetWidth() - 50, 50, m_pRenderer->GetHeight() - 50);

    // Respaen orcs
    SpawnOrcs(*m_pRenderer);

    // Reinitialize score after orcs are created
    if (m_pKnightHUD) {
        m_pKnightHUD->InitialiseScore(*m_pRenderer);
    }

    LogManager::GetInstance().Log("Game restarted!");
}