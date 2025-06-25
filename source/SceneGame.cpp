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
#include "sharedenums.h"
#include "VictoryMenu.h"
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

SceneGame::SceneGame()
    : m_pBackgroundManager(nullptr)
    , m_pVictoryMenu(nullptr)
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
    , m_isLevelComplete(false)
    , m_loadNextWave(true)
{
}

SceneGame::~SceneGame()
{
    delete m_pBackgroundManager;
    m_pBackgroundManager = nullptr;

    delete m_pPauseMenu;
    m_pPauseMenu = nullptr;

    delete m_pVictoryMenu;
    m_pVictoryMenu = nullptr;

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
    character_num = data;
    //m_pKnightClass->SetBoundaries(0,0,0,0);
    m_pKnightClass->getAreaArray(*this);

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

    m_pVictoryMenu = new VictoryMenu();
    if (!m_pVictoryMenu->Initialise(renderer))
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
    getAreaArray();

    SpawnEnemies(renderer);

    // Initialize score HUD after orcs are created
    if (m_pKnightHUD) {
        m_pKnightHUD->InitialiseScore(renderer);
    }
    return true;
}

void SceneGame::getAreaArray()
{
    for (int y = 0; y < 40; y++)
    {
        int* temp = m_pBackgroundManager->tilearray(y);
        for (int x = 0; x < 120; x++)
        {
            areaArray[x][y] = temp[x];
        }
        delete[] temp;
    }
}
int* SceneGame::tilearray(int row)
{
    int* array = new int[120];
    for (int i = 0; i < 120; i++)
    {
        if (areaArray[i][row] != NULL)
        {
            array[i] = areaArray[i][row];
        }
        else
        {
            array[i] = -1;
        }
    }
    return array;
}
float SceneGame::getSize()
{
    return m_pBackgroundManager->getSize();
}
float SceneGame::getWide()
{
    return m_pBackgroundManager->getWide();
}
float SceneGame::getHeight()
{
    return m_pBackgroundManager->getHeight();
}
float SceneGame::getWH()
{
    return m_pBackgroundManager->getWH();
}
float SceneGame::getOffsetX() {
    return m_pBackgroundManager->getOffsetX();
}




void SceneGame::Process(float deltaTime)
{
    // Process level change
    if (m_isLevelComplete)
    {
        m_isLevelComplete = false;
        LoadNextLevel();
    }
    
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
    m_pKnightClass->Process(deltaTime, *this);
    m_pBackgroundManager->Process(deltaTime);

    if (m_pKnightHUD && m_pKnightClass) {
        m_pKnightHUD->HealthUpdate(m_pKnightClass->GetHealth());
    }

    if (m_pSceneGuide && !m_pSceneGuide->IsFinished()) {
        m_pSceneGuide->Process(deltaTime);
    }

    if (m_gameState == GAME_STATE_VICTORY)
    {
        m_pVictoryMenu->Process(deltaTime);
    }

    if (m_gameState == GAME_STATE_PLAYING)
    {
        m_pBackgroundManager->changePos(m_pKnightClass->getArenaPos(), 0);
      //  Vector2 move = m_pKnightClass->GetLastMovementDirection();
        Vector2 move;
        move.x = m_pKnightClass->getArenaPos();
        if (move.x > 0)
        {
            move.x = -1;
        }
        else if (move.x < 0)
        {
            move.x = 1;
        }
        else
        {
            move.x = 0;
        }
        m_scrollDistance += move.x * deltaTime * 120.0f;
      //  m_scrollDistance += move.x * deltaTime * 120.0f;
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
        float tempOffset = m_pBackgroundManager->getOffsetX();
        CheckKnightState();

        float worldX =/** m_scrollDistance**/+ m_pKnightClass->GetPosition().x; 
        Vector2 worldKnightPos(worldX, m_pKnightClass->GetPosition().y); 

        //SpawnEnemies(*m_pRenderer);

        for (Orc* orc : m_orcs) {
            if (orc && orc->m_bVisible) {
                if (orc->IsAlive()) {
                    orc->setoffset(tempOffset);
                    orc->UpdateAI(worldKnightPos, deltaTime, *this);
                }
                orc->Process(deltaTime, *this);
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (skeleton && skeleton->IsAlive() && skeleton->m_bVisible) {
                skeleton->setoffset(tempOffset);
                skeleton->UpdateAI(worldKnightPos, deltaTime, *this);
            }
            if (skeleton && skeleton->m_bVisible) {
                skeleton->Process(deltaTime, *this);
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (wolf && wolf->IsAlive() && wolf->m_bVisible) {
                wolf->setoffset(tempOffset);

                wolf->UpdateAI(worldKnightPos, deltaTime, *this);
            }
            if (wolf && wolf->m_bVisible) {
                wolf->Process(deltaTime, *this);
            }
        }

        for (Werebear* bear : m_werebear) {
            if (bear && bear->IsAlive() && bear->m_bVisible) {
                bear->setoffset(tempOffset);

                bear->UpdateAI(worldKnightPos, deltaTime, *this);
            }
            if (bear && bear->m_bVisible) {
                bear->Process(deltaTime, *this);
            }
        }

        for (Orc* orc : m_orcs) {
            if (!orc || orc->IsAlive() || orc->WasScored() || !orc->m_bVisible)
                continue;
            m_score += orc->GetScore();
            orc->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (!skeleton || skeleton->IsAlive() || skeleton->WasScored() || !skeleton->m_bVisible)
                continue;

            m_score += skeleton->GetScore();
            skeleton->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (!wolf || wolf->IsAlive() || wolf->WasScored() || !wolf->m_bVisible)
                continue;
            m_score += wolf->GetScore();
            wolf->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werebear* bear : m_werebear) {
            if (!bear || bear->IsAlive() || bear->WasScored() || !bear->m_bVisible)
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
                if (!orc || !orc->IsAlive() || !orc->m_bVisible) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*orc);
        //        attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, orc->GetHitbox())) {
                    orc->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }

            for (Skeleton* skeleton : m_skeletons)
            {
                if (!skeleton || !skeleton->IsAlive() || !skeleton->m_bVisible) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*skeleton);
           //     attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, skeleton->GetHitbox())) {
                    skeleton->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }

            for (Werewolf* wolf : m_werewolf)
            {
                if (!wolf || !wolf->IsAlive() || !wolf->m_bVisible) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*wolf);
              //  attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, wolf->GetHitbox())) {
                    wolf->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }

            for (Werebear* bear : m_werebear)
            {
                if (!bear || !bear->IsAlive() || !bear->m_bVisible) continue;

                attackHitbox = m_pKnightClass->GetAttackHitbox(*bear);
               // attackHitbox.x += m_scrollDistance;

                if (Collision::CheckCollision(attackHitbox, bear->GetHitbox())) {
                    bear->TakeDamage(m_pKnightClass->AttackDamage());
                }
            }
        }

        Hitbox knightHitbox = m_pKnightClass->GetHitbox();

        for (Orc* orc : m_orcs) {
            if (orc && orc->IsAlive() && orc->IsAttacking()) {
                Hitbox orcAttackBox = orc->GetAttackHitbox();
            //    orcAttackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(orcAttackBox, knightHitbox)) {
                    int damage = GetOrcAttackDamage(orc);
                    m_pKnightClass->TakeDamage(damage);
                }
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (skeleton && skeleton->IsAlive() && skeleton->IsAttacking() && skeleton->m_bVisible) {
                Hitbox attackBox = skeleton->GetAttackHitbox();
              //  attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(10);  // You can customize per enemy
                }
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (wolf && wolf->IsAlive() && wolf->IsAttacking() && wolf->m_bVisible) {
                Hitbox attackBox = wolf->GetAttackHitbox();
              //  attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(15);
                }
            }
        }

        for (Werebear* bear : m_werebear) {
            if (bear && bear->IsAlive() && bear->IsAttacking() && bear->m_bVisible) {
                Hitbox attackBox = bear->GetAttackHitbox();
                //attackBox.x -= m_scrollDistance;

                if (Collision::CheckCollision(attackBox, knightHitbox)) {
                    m_pKnightClass->TakeDamage(20);
                }
            }
        }
        // Update score and trigger BuffMenu if Werebear is the final enemy

        for (Orc* orc : m_orcs) {
            if (!orc || orc->IsAlive() || orc->WasScored() || !orc->m_bVisible)
                continue;

            m_score += orc->GetScore();
            orc->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }

            if (orc->m_type == ORC_ARMORED)
            {
                if (m_triggerBuffMenuNext && !m_showBuffMenu) {
                    SDL_ShowCursor(SDL_ENABLE);
                    m_showBuffMenu = true;
                    m_triggerBuffMenuNext = false;
                    m_pBuffMenu->Reset();

                    // set level complete bool value to true to load next level
                    m_isLevelComplete = true;

                    return; // pause game logic until buff is selected
                }
            }
        }

        for (Skeleton* skeleton : m_skeletons) {
            if (!skeleton || skeleton->IsAlive() || skeleton->WasScored() || !skeleton->m_bVisible)
                continue;

            m_score += skeleton->GetScore();
            skeleton->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }

            if (skeleton->m_type == SKELETON_GREAT)
            {
                SDL_ShowCursor(SDL_ENABLE);
                m_gameState = GAME_STATE_VICTORY;
                m_pVictoryMenu->Reset();
                return;
            }
        }

        for (Werewolf* wolf : m_werewolf) {
            if (!wolf || wolf->IsAlive() || wolf->WasScored() || !wolf->m_bVisible)
                continue;

            m_score += wolf->GetScore();
            wolf->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
            }
        }

        for (Werebear* bear : m_werebear) {
            if (!bear || bear->IsAlive() || bear->WasScored() || !bear->m_bVisible)
                continue;

            m_score += bear->GetScore();
            bear->MarkScored();

            if (m_pKnightHUD) {
                m_pKnightHUD->ScoreUpdate(m_score, *m_pRenderer);
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
        if (orc && orc->m_bVisible) { // Draw all orcs, even dead ones
            orc->Draw(renderer, m_scrollDistance);
        }
    }

    for (Skeleton* skeleton : m_skeletons) {
        if (skeleton && skeleton->m_bVisible) {
            skeleton->Draw(renderer, m_scrollDistance);
        }
    }

    for (Werewolf* wolf : m_werewolf) {
        if (wolf && wolf->m_bVisible) {
            wolf->Draw(renderer, m_scrollDistance);
        }
    }

    for (Werebear* bear : m_werebear) {
        if (bear && bear->m_bVisible) {
            bear->Draw(renderer, m_scrollDistance);
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

    if (m_gameState == GAME_STATE_VICTORY) {
        m_pVictoryMenu->Draw(renderer);
    }

    // Display game over message
    if (m_gameState == GAME_STATE_GAME_OVER) {
        m_pGameOverMenu->Draw(renderer); 
    }
}

void SceneGame::ProcessInput(InputSystem& inputSystem, Renderer& renderer)
{
    //Process input during gameplay
    if (m_gameState == GAME_STATE_PLAYING) {
        m_pKnightClass->ProcessInput(inputSystem, *this);
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
            RestartGame(renderer);
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
            RestartGame(renderer); 
            m_gameState = GAME_STATE_RESTART; 
            m_pGameOverMenu->Reset();
        }
        else if (state == GAME_OVER_EXIT) {
            SDL_Quit(); 
            exit(0); 
        }
    }

    if (m_gameState == GAME_STATE_VICTORY) {
        m_pVictoryMenu->ProcessInput(inputSystem);

        VictoryState state = m_pVictoryMenu->GetState();

        if (state == VICTORY_RETRY) {
            RestartGame(renderer);
            m_gameState = GAME_STATE_RESTART;
            m_pVictoryMenu->Reset();
        }
        else if (state == VICTORY_EXIT) {
            SDL_Quit();
            exit(0);
        }

        return;
    }
}

void SceneGame::LoadNextLevel()
{
    for (Orc* orc : m_orcs) {
        //delete orc;
        orc->FlipVisibility();
    }
    //m_orcs.clear();
    //m_orcs.begin();
    for (Skeleton* skeleton : m_skeletons) {
        //delete skeleton;
        skeleton->FlipVisibility();
    }
    //m_skeletons.clear();
    //m_skeletons.begin();
    for (Werewolf* werewolf : m_werewolf) {
        //delete werewolf;
        werewolf->FlipVisibility();
    }
    //m_werewolf.clear();
    //m_werewolf.begin();
    for (Werebear* werebear : m_werebear) {
        //delete werebear;
        werebear->FlipVisibility();
    }
    //m_werebear.clear();
    //m_werebear.clear();
    // Update player position to start position
    m_pKnightClass->SetPosition(m_pKnightClass->startPos);
    // Update the scroll position to go back to normal (not sure how to implement this)
    m_scrollDistance = 0.0f;
    // Update the backgrounds to a difference sprite (change background manager to have multiple sprites)
    m_pBackgroundManager->ChangeBackgrounds(MUSHROOM_FOREST);
    int stage = m_pBackgroundManager->getStage();
    if (stage < 2)
    {
        m_pBackgroundManager->setStage(stage++);
    }
    m_waveCount = stage;
    m_loadNextWave = true;
    //SpawnEnemies(*m_pRenderer);


}

void SceneGame::SpawnEnemies(Renderer& renderer)
{
    if ((m_waveCount == 0) && (m_loadNextWave== true))
    {
        float sizeTemp = m_pBackgroundManager->getSize();
        float groundY = (40 - 5) * sizeTemp + sizeTemp / 2;//- m_pBackgroundManager->getHeight() / m_pBackgroundManager->getSize();
        //float groundY = renderer.GetHeight() * 0.8f;
        float patrolRangeGround = sizeTemp * (29) + sizeTemp / 2;
        const EnemyPlacement wave1[] = {
            { sizeTemp * (17) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, SKELETON },
            { sizeTemp * (30) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, ORC_ARMORED },
            { sizeTemp * (40) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, ORC_ELITE },
            { sizeTemp * (8), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , ORC},//maybe be orc rider
            { sizeTemp * (10), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , ORC },
            { sizeTemp * (12), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , ORC },
            { sizeTemp * (50), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , ORC_ELITE },
            { sizeTemp * (54), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , ORC_ELITE },
            { sizeTemp * (58), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , ORC_ELITE },
            { sizeTemp * (90), sizeTemp * 4 + sizeTemp / 2, PATROL,sizeTemp * 1 , ORC_ARMORED },
            { sizeTemp * (10), sizeTemp * 14 - sizeTemp / 2, PATROL,sizeTemp * 9 , ORC },
            { sizeTemp * (85) + sizeTemp / 2,sizeTemp * (40 - 5) + sizeTemp / 2, PATROL, sizeTemp * 15.0f + sizeTemp / 2, ORC_RIDER },
            { sizeTemp * (17) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, ORC },
            { sizeTemp * (30) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, ORC },
            { sizeTemp * (40) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, SKELETON_ARMORED },
            { sizeTemp * (8), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , SKELETON_ARMORED},//maybe be orc rider
            { sizeTemp * (10), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , SKELETON_ARMORED },
            { sizeTemp * (12), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , WEREBEAR },
            { sizeTemp * (50), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , WEREBEAR },
            { sizeTemp * (54), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , WEREBEAR },
            { sizeTemp * (58), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , WEREBEAR },
            { sizeTemp * (90), sizeTemp * 4 + sizeTemp / 2, PATROL,sizeTemp * 1 , SKELETON_ARMORED },
            { sizeTemp * (10), sizeTemp * 14 - sizeTemp / 2, PATROL,sizeTemp * 9 , SKELETON },
            { sizeTemp * (85) + sizeTemp / 2,sizeTemp * (40 - 5) + sizeTemp / 2, PATROL, sizeTemp * 15.0f + sizeTemp / 2, SKELETON_GREAT }
        };

        const int waveCount = sizeof(wave1) / sizeof(wave1[0]);

        SpawnEnemyWave(wave1, waveCount, 0.0f, renderer);  // offset = 0
        m_waveCount = 1;
        m_loadNextWave = false;
        m_triggerBuffMenuNext = true;

        LogManager::GetInstance().Log("One-time wave spawned. Buff menu will appear after defeat.");
    }
    //else if ((m_waveCount == 1) && (m_loadNextWave == true))
    //{
    //    float sizeTemp = m_pBackgroundManager->getSize();
    //    float groundY = (40 - 5) * sizeTemp + sizeTemp / 2;//- m_pBackgroundManager->getHeight() / m_pBackgroundManager->getSize();
    //    //float groundY = renderer.GetHeight() * 0.8f;
    //    float patrolRangeGround = sizeTemp * (29) + sizeTemp / 2;
    //    const EnemyPlacement wave1[] = {
    //        { sizeTemp * (17) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, ORC },
    //        { sizeTemp * (30) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, ORC },
    //        { sizeTemp * (40) + sizeTemp / 2, groundY, PATROL, 10 * sizeTemp + sizeTemp / 2, SKELETON_ARMORED },
    //        { sizeTemp * (8), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , SKELETON_ARMORED},//maybe be orc rider
    //        { sizeTemp * (10), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , SKELETON_ARMORED },
    //        { sizeTemp * (12), sizeTemp * 18 - sizeTemp / 2, PATROL,sizeTemp * 9 , WEREBEAR },
    //        { sizeTemp * (50), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , WEREBEAR },
    //        { sizeTemp * (54), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , WEREBEAR },
    //        { sizeTemp * (58), sizeTemp * 12 + sizeTemp / 2, PATROL,sizeTemp * 8 , WEREBEAR },
    //        { sizeTemp * (90), sizeTemp * 4 + sizeTemp / 2, PATROL,sizeTemp * 1 , SKELETON_ARMORED },
    //        { sizeTemp * (10), sizeTemp * 14 - sizeTemp / 2, PATROL,sizeTemp * 9 , SKELETON },
    //        { sizeTemp * (85) + sizeTemp / 2,sizeTemp * (40 - 5) + sizeTemp / 2, PATROL, sizeTemp * 15.0f + sizeTemp / 2, SKELETON_GREAT },
    //        { 2000.0f, groundY, PATROL, 300.0f, SKELETON_ARMORED },
    //        { 2300.0f, groundY, AGGRESSIVE, 0.0f, SKELETON_GREAT },
    //        { 2600.0f, groundY, PATROL, 300.0f, WEREWOLF },
    //        { 2900.0f, groundY, AGGRESSIVE, 0.0f, WEREBEAR }
    //    };
    //    
    //    
    //    const int waveCount = sizeof(wave1) / sizeof(wave1[0]);

    //    SpawnEnemyWave(wave1, waveCount, 0.0f, renderer);  // offset = 0

    //    m_loadNextWave = false;
    //    m_waveCount = 2;
    //    m_triggerBuffMenuNext = true;

    //    LogManager::GetInstance().Log("One-time wave spawned. Buff menu will appear after defeat.");

    //    // Spawn next set of enemies based on the code above
    //}
}

void SceneGame::SpawnEnemyWave(const EnemyPlacement* wave, int count, float offset, Renderer& renderer)
{
    char buffer[256];
    sprintf_s(buffer, "Spawning Enemy Wave with offset: %.2f", offset);
    LogManager::GetInstance().Log(buffer);
  
   for (int i = 0; i < 24; i++)
   {
        Orc* orc = nullptr;
        Skeleton* skeleton = nullptr;
        Werebear* werebear = nullptr;
        Werewolf* werewolf = nullptr;
        float spawnX = wave[i].posX + offset;
        float spawnY = wave[i].posY;
        float patrolRange = wave[i].patrolRange;


        EnemyBehavior behavior = wave[i].behavior; 
        
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
        default:
            return;
        }

        // Spawn orc
        if (orc && orc->Initialise(renderer)) {
            orc->passHitbox(orc->GetHitbox());
            orc->getAreaArray(*this);
            orc->SetPosition(spawnX, spawnY);
            orc->SetBehavior(behavior);
            if (behavior != IDLE) {
                orc->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            orc->setoffset(m_pBackgroundManager->getOffsetX());
            
            if (i <= 12)
            {
                orc->m_bVisible = true;
            }

            m_orcs.push_back(orc);
        }
        else {
            delete orc;
        }

        // Spawn skeleton
        if (skeleton && skeleton->Initialise(renderer)) {
            skeleton->passHitbox(skeleton->GetHitbox());
            skeleton->getAreaArray(*this);
            skeleton->SetPosition(spawnX, spawnY);
            skeleton->SetBehavior(behavior);
            if (behavior != IDLE) {
                skeleton->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            skeleton->setoffset(m_pBackgroundManager->getOffsetX());

            if (i <= 12)
            {
                skeleton->m_bVisible = true;
            }

            m_skeletons.push_back(skeleton);
        }
        else {
            delete skeleton;
        }

        // Spawn werewolf
        if (werewolf && werewolf->Initialise(renderer)) {
            werewolf->getAreaArray(*this);

            werewolf->SetPosition(spawnX, spawnY);
            werewolf->SetBehavior(behavior);
            if (behavior != IDLE) {
                werewolf->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            werewolf->setoffset(m_pBackgroundManager->getOffsetX());

            if (i <= 12)
            {
                werewolf->m_bVisible = true;
            }

            m_werewolf.push_back(werewolf);
        }
        else {
            delete werewolf;
        }

        // Spawn werebear
        if (werebear && werebear->Initialise(renderer)) {
            werebear->getAreaArray(*this);
            werebear->SetPosition(spawnX, spawnY);
            werebear->SetBehavior(behavior);
            if (behavior != IDLE) {
                werebear->SetPatrolRange(spawnX - patrolRange, spawnX + patrolRange);
            }
            werebear->setoffset(m_pBackgroundManager->getOffsetX());

            if (i <= 12)
            {
                werebear->m_bVisible = true;
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

void SceneGame::RestartGame(Renderer& renderer)
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
  //  m_waveCount = 0;
//    m_pBackgroundManager->changePos(m_pBackgroundManager->getOffsetX(),0);
    // Reset game state variables
    m_pBackgroundManager->setStage(m_pBackgroundManager->getStage());
    m_scrollDistance = 0.0f;
    m_nextWaveOffset = 0.0f;
    m_score = 0; // Reset score

    m_waveCount = m_pBackgroundManager->getStage()-1;
    m_loadNextWave = true;
    SpawnEnemies(*m_pRenderer);

    delete m_pKnightClass;
    m_pSwordsman = new KnightClass();
    m_pSwordsman->Initialise(renderer);

    m_pArcher = new Archer();
    m_pArcher->Initialise(renderer);

    m_pWizard = new Wizard();
    m_pWizard->Initialise(renderer);
    m_pKnightClass = m_pWizard;
    setData(0, character_num);
    m_pKnightClass->SetPosition(m_pKnightClass->startPos);
    // Update the scroll position to go back to normal (not sure how to implement this)
    m_scrollDistance = 0.0f;
    
    // Respaen orcs

    // Reinitialize score after orcs are created
    if (m_pKnightHUD) {
        m_pKnightHUD->InitialiseScore(*m_pRenderer);
    }

    LogManager::GetInstance().Log("Game restarted!");
}