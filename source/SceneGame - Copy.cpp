#include "SceneGame.h"
#include "SceneMenu.h"
#include "renderer.h"
#include "knightclass.h"
#include "archer.h"
#include "wizard.h"
#include "BackgroundManager.h"
#include "PauseMenu.h"
#include "buffmenu.h"
#include "GameOverMenu.h"
#include "SceneGuide.h"
#include "logmanager.h"
#include "collision.h"
#include "knighthud.h"
#include "game.h"
#include "xboxcontroller.h"
//enemies
#include "enemy.h" 
#include "orc.h"
#include "armoredOrc.h"
#include "eliteOrc.h"
#include "riderOrc.h"
#include "skeleton.h"
#include "armoredskeleton.h"
#include "greatskeleton.h" 
#include "werewolf.h"
#include "werebear.h"
//boss
#include "bossDemon.h"
#include "bossDeath.h" 

SceneGame::SceneGame()
    : m_pBackgroundManager(nullptr)
    , m_pKnightClass(nullptr)
    , m_pArcher(nullptr)
    , m_pWizard(nullptr)
    , m_pSwordsman(nullptr)
    , m_pPauseMenu(nullptr)
    , m_pGameOverMenu(nullptr)
    , m_pRenderer(nullptr)
    , m_pKnightHUD(nullptr)
    , m_pSceneGuide(nullptr)
    , m_pBuffMenu(nullptr)
    , m_scrollDistance(0.0f)
    , m_gameState(GAME_STATE_PLAYING)
    , m_score(0)
    , m_gameMusic(nullptr)
    , m_gameChannel(nullptr)
    , m_gameVolume(0.2f)
    , m_gameStartPlayed(false)
    , m_nextWaveOffset(0.0f)
    , m_showBuffMenu(false)
    , m_allEnemiesCleared(false)
    , m_triggerBuffMenuNext(false)
    , m_waveCount(0)
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

    delete m_pBuffMenu; 
    m_pBuffMenu = nullptr; 

    delete m_pKnightClass;
    m_pKnightClass = nullptr;

    delete m_pWizard;
    m_pWizard = nullptr;

    delete m_pArcher;
    m_pArcher = nullptr;

    delete m_pSwordsman;
    m_pSwordsman = nullptr;

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

int SceneGame::getData(int type)
{
    return 0;
}

void SceneGame::setData(int type, float data)
{
    if (data == 0)
    {
        m_pKnightClass = m_pWizard;
        m_pKnightClass->characterType = WIZARD;
    }
    else if (data == 1)
    {
        m_pKnightClass = m_pArcher;
        m_pKnightClass->characterType = ARCHER;
    }
    else if (data == 2)
    {
        m_pKnightClass = m_pSwordsman;
        m_pKnightClass->characterType = SWORDSMAN;
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

    m_pBuffMenu = new BuffMenu(); 
    if (!m_pBuffMenu->Initialise(renderer)) 
        return false;

    // Character classes
    m_pSwordsman = new KnightClass();
    if (!m_pSwordsman->Initialise(renderer))
        return false;

    m_pArcher = new Archer();
    if (!m_pArcher->Initialise(renderer))
        return false;

    m_pWizard = new Wizard();
    if (!m_pWizard->Initialise(renderer))
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

    // Placeholder needed before selection
    m_pKnightClass = m_pWizard;

    m_pKnightClass->SetBoundaries(50, renderer.GetWidth() - 50, 50, renderer.GetHeight() - 50);

    SpawnEnemies(renderer);

    // Initialize score HUD after orcs are created
    if (m_pKnightHUD) {
        m_pKnightHUD->InitialiseScore(renderer);
    }

    return true;
}

void SceneGame::Process(float deltaTime)
{
    if (m_showBuffMenu && m_pBuffMenu)
    {
        InputSystem& inputSystem = Game::GetInstance().GetInputSystem();

        if (m_pBuffMenu->GetState() == BUFF_DONE)
        {
            m_showBuffMenu = false;
            // Gets the selected buff from the buff menu after it is done
            m_pKnightClass->buffCharacter(m_pBuffMenu->GetSelectedBuff());
        }
        else
        {
            m_pBuffMenu->ProcessInput(inputSystem);
            return; // pause the rest of the game logic
        }
    }
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

        SpawnEnemies(*m_pRenderer);

        for (Orc* orc : m_orcs) {
            if (orc) {
                if (orc->IsAlive()) {
                    orc->UpdateAI(worldKnightPos, deltaTime);
                }
                orc->Process(deltaTime);
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (skeleton && skeleton->IsAlive()) {
                skeleton->UpdateAI(worldKnightPos, deltaTime);
            }
            if (skeleton) {
                skeleton->Process(deltaTime);
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (wolf && wolf->IsAlive()) {
                wolf->UpdateAI(worldKnightPos, deltaTime);
            }
            if (wolf) {
                wolf->Process(deltaTime);
            }
        }

        for (Werebear* bear : m_werebear) {
            if (bear && bear->IsAlive()) {
                bear->UpdateAI(worldKnightPos, deltaTime);
            }
            if (bear) {
                bear->Process(deltaTime);
            }
        }

        for (BossDeath* deathBoss : m_bossDeaths) { 
            if (deathBoss && deathBoss->IsAlive()) { 
                deathBoss->UpdateAI(worldKnightPos, deltaTime);
            }
            if (deathBoss) { 
                deathBoss->Process(deltaTime); 
            }
        }

        for (BossDemon* demonBoss : m_bossDemons) {
            if (demonBoss && demonBoss->IsAlive()) {
                demonBoss->UpdateAI(worldKnightPos, deltaTime);
            }
            if (demonBoss) {
                demonBoss->Process(deltaTime);
            }
        }

        //Scores
        for (Orc* orc : m_orcs) {
            if (!orc || orc->IsAlive() || orc->WasScored())
                continue;

            m_score += orc->GetScore();
            orc->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (!skeleton || skeleton->IsAlive() || skeleton->WasScored())
                continue;

            m_score += skeleton->GetScore();
            skeleton->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (!wolf || wolf->IsAlive() || wolf->WasScored())
                continue;

            m_score += wolf->GetScore();
            wolf->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werebear* bear : m_werebear) {
            if (!bear || bear->IsAlive() || bear->WasScored())
                continue;

            m_score += bear->GetScore();
            bear->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        // Section to handle the attacking case of the character class
        if (m_pKnightClass && (m_pKnightClass->isAttacking() || m_pKnightClass->isProjectilesActive())) {
            
            Hitbox attackHitbox;

            for (Orc* orc : m_orcs) 
            {
                if (!orc || !orc->IsAlive()) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*orc);
                attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, orc->GetHitbox())) {
                    orc->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }

            for (Skeleton* skeleton : m_skeletons)
            {
                if (!skeleton || !skeleton->IsAlive()) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*skeleton);
                attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, skeleton->GetHitbox())) {
                    skeleton->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }

            for (Werewolf* wolf : m_werewolf)
            {
                if (!wolf || !wolf->IsAlive()) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*wolf);
                attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, wolf->GetHitbox())) {
                    wolf->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }

            for (Werebear* bear : m_werebear)
            {
                if (!bear || !bear->IsAlive()) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*bear);
                attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, bear->GetHitbox())) {
                    bear->TakeDamage(m_pKnightClass->AttackDamage());
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
                    m_pKnightClass->TakeDamage(damage);
                }
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (skeleton && skeleton->IsAlive() && skeleton->IsAttacking()) {
                Hitbox attackBox = skeleton->GetAttackHitbox();
                attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(10);  // You can customize per enemy
                }
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (wolf && wolf->IsAlive() && wolf->IsAttacking()) {
                Hitbox attackBox = wolf->GetAttackHitbox();
                attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(15);
                }
            }
        }

        for (Werebear* bear : m_werebear) {
            if (bear && bear->IsAlive() && bear->IsAttacking()) {
                Hitbox attackBox = bear->GetAttackHitbox();
                attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(20);
                }
            }
        }

        for (BossDeath* deathBoss : m_bossDeaths) {
            if (deathBoss && deathBoss->IsAlive() && deathBoss->IsAttacking()) {
                Hitbox attackBox = deathBoss->GetAttackHitbox();
                attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(20);
                }
            }
        }

        for (BossDemon* demonBoss : m_bossDemons) {
            if (demonBoss && demonBoss->IsAlive() && demonBoss->IsAttacking()) {
                Hitbox attackBox = demonBoss->GetAttackHitbox();
                attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(20);
                }
            }
        }

        // Update score and trigger BuffMenu if Werebear is the final enemy
        for (Orc* orc : m_orcs) {
            if (!orc || orc->IsAlive() || orc->WasScored())
                continue;

            m_score += orc->GetScore();
            orc->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (!skeleton || skeleton->IsAlive() || skeleton->WasScored())
                continue;

            m_score += skeleton->GetScore();
            skeleton->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (!wolf || wolf->IsAlive() || wolf->WasScored())
                continue;

            m_score += wolf->GetScore();
            wolf->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werebear* bear : m_werebear) {
            if (!bear || bear->IsAlive() || bear->WasScored())
                continue;

            m_score += bear->GetScore();
            bear->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }

            if (m_triggerBuffMenuNext && !m_showBuffMenu) {
                SDL_ShowCursor(SDL_ENABLE);
                m_showBuffMenu = true;
                m_triggerBuffMenuNext = false;
                m_pBuffMenu->Reset();
                return; // pause game logic until buff is selected
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

    for (Skeleton* skeleton : m_skeletons) {
        if (skeleton) {
            skeleton->Draw(renderer, m_scrollDistance);
        }
    }

    for (Werewolf* wolf : m_werewolf) {
        if (wolf) {
            wolf->Draw(renderer, m_scrollDistance);
        }
    }

    for (Werebear* bear : m_werebear) {
        if (bear) {
            bear->Draw(renderer, m_scrollDistance);
        }
    }

    for (BossDeath* deathBoss : m_bossDeaths) {
        if (deathBoss) { 
            deathBoss->Draw(renderer, m_scrollDistance); 
        }
    }

    for (BossDemon* boss : m_bossDemons) {
        if (boss) {
            boss->Draw(renderer, m_scrollDistance);
        }
    }

    if (m_pSceneGuide && !m_pSceneGuide->IsFinished()) {
        m_pSceneGuide->Draw(renderer); 
    }

    m_pKnightClass->Draw(renderer);

    if (m_pKnightHUD) {
        m_pKnightHUD->Draw(renderer);
    }

    if (m_showBuffMenu && m_pBuffMenu) 
    {
        m_pBuffMenu->Draw(renderer);
        return; // Only draw BuffMenu
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

void SceneGame::SpawnEnemies(Renderer& renderer)
{
    if (m_waveCount > 0) return; // Only spawn once

    float groundY = renderer.GetHeight() * 0.8f;

    const EnemyPlacement wave1[] = {
        { 1000.0f, groundY, PATROL, 300.0f, ORC },
        { 1600.0f, groundY, IDLE, 0.0f, ORC_ARMORED },
        { 2300.0f, groundY, PATROL, 300.0f, ORC_ELITE },
        { 3000.0f, groundY, IDLE, 0.0f, ORC_RIDER }, // Boss 1
        { 3700.0f, groundY, PATROL, 300.0f, SKELETON },
        { 4400.0f, groundY, PATROL, 300.0f, SKELETON_ARMORED },
        { 5100.0f, groundY, AGGRESSIVE, 0.0f, SKELETON_GREAT }, // Boss 2
        { 5800.0f, groundY, PATROL, 300.0f, WEREWOLF },
        { 6500.0f, groundY, AGGRESSIVE, 0.0f, WEREBEAR } // Boss 3
    };

    const int waveCount = sizeof(wave1) / sizeof(wave1[0]);

    SpawnEnemyWave(wave1, waveCount, 0.0f, renderer);  // offset = 0
    m_waveCount = 1;
    m_triggerBuffMenuNext = true;

    LogManager::GetInstance().Log("One-time wave spawned. Buff menu will appear after defeat.");
}

void SceneGame::SpawnEnemyWave(const EnemyPlacement* wave, int count, float offset, Renderer& renderer)
{
    char buffer[256];
    sprintf_s(buffer, "Spawning Enemy Wave with offset: %.2f", offset);
    LogManager::GetInstance().Log(buffer);

   for (int i = 0; i < count; i++)
   {
        Orc* orc = nullptr;
        Skeleton* skeleton = nullptr;
        Werebear* werebear = nullptr;
        Werewolf* werewolf = nullptr;
        BossDeath* deathBoss = nullptr;
        BossDemon* deathDemon = nullptr;

        float spawnX = wave[i].posX + offset; 
        float spawnY = wave[i].posY; 
        EnemyBehavior behavior = wave[i].behavior; 
        float patrolRange = wave[i].patrolRange; 

        switch (wave[i].type) 
        {
        case ORC:
            orc = new Orc();
            break;
        case ORC_ARMORED:
            orc = new ArmoredOrc();
            break;
        case ORC_ELITE:
            orc = new EliteOrc();
            break;
        case ORC_RIDER:
            orc = new RiderOrc();
            break;
        case SKELETON:
            skeleton = new Skeleton();
            break;
        case SKELETON_ARMORED:
            skeleton = new ArmoredSkeleton();
            break;
        case SKELETON_GREAT:
            skeleton = new GreatSkeleton();
            break;
        case WEREWOLF:
            werewolf = new Werewolf();
            break;
        case WEREBEAR:
            werebear = new Werebear();
            break;
        case DEATH_BOSS:
            deathBoss = new BossDeath(); 
            break;
        case DEMON_BOSS:
            deathDemon = new BossDemon();
            break;
        default:
            return;
        }

        // Spawn orc
        if (orc && orc->Initialise(renderer)) {
            orc->SetPosition(spawnX, spawnY);
            orc->SetBehavior(behavior);
            if (behavior != IDLE) {
                orc->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            m_orcs.push_back(orc);
        }
        else {
            delete orc;
        }

        // Spawn skeleton
        if (skeleton && skeleton->Initialise(renderer)) {
            skeleton->SetPosition(spawnX, spawnY);
            skeleton->SetBehavior(behavior);
            if (behavior != IDLE) {
                skeleton->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            m_skeletons.push_back(skeleton);
        }
        else {
            delete skeleton;
        }

        // Spawn werewolf
        if (werewolf && werewolf->Initialise(renderer)) {
            werewolf->SetPosition(spawnX, spawnY);
            werewolf->SetBehavior(behavior);
            if (behavior != IDLE) {
                werewolf->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            m_werewolf.push_back(werewolf);
        }
        else {
            delete werewolf;
        }

        // Spawn werebear
        if (werebear && werebear->Initialise(renderer)) {
            werebear->SetPosition(spawnX, spawnY);
            werebear->SetBehavior(behavior);
            if (behavior != IDLE) {
                werebear->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            m_werebear.push_back(werebear);
        }
        else {
            delete werebear;
        }
    }

    sprintf_s(buffer, "Wave spawn complete. Orcs: %zu | Skeletons: %zu | Werewolves: %zu | Werebears: %zu",
        m_orcs.size(), m_skeletons.size(), m_werewolf.size(), m_werebear.size());
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

    for (Skeleton* skeleton : m_skeletons) {
        delete skeleton;
    }
    m_skeletons.clear();

    for (Werewolf* werewolf : m_werewolf) {
        delete werewolf; 
    }
    m_werewolf.clear(); 

    for (Werebear* werebear : m_werebear) { 
        delete werebear; 
    }
    m_werebear.clear(); 

    // Reset game state variables
    m_scrollDistance = 0.0f;
    m_score = 0; // Reset score
    m_nextWaveOffset = 0.0f;

    // Recreate knight
    delete m_pKnightClass;
    m_pKnightClass = new Archer(); 
    m_pKnightClass->Initialise(*m_pRenderer);
    m_pKnightClass->SetBoundaries(50, m_pRenderer->GetWidth() - 50, 50, m_pRenderer->GetHeight() - 50);

    // Respaen orcs
    SpawnEnemies(*m_pRenderer);

    // Reinitialize score after orcs are created
    if (m_pKnightHUD) {
        m_pKnightHUD->InitialiseScore(*m_pRenderer);
    }

    LogManager::GetInstance().Log("Game restarted!");
}