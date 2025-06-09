#ifndef __SCENEGAME_H_
#define __SCENEGAME_H_

#include "scene.h"
#include "inputsystem.h"
#include "vector2.h"
#include "orc.h"
#include <vector>
#include <string>
#include <fmod.hpp>

class KnightClass;
class Orc;
class Renderer;
class BackgroundManager;
class PauseMenu; 
class GameOverMenu;
class AnimatedSprite; 
class ArmoredOrc;
class EliteOrc;
class RiderOrc;
class KnightHUD; 
class SceneGuide; 

// Define game states
enum GameState {
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED, 
    GAME_STATE_GAME_OVER,
    GAME_STATE_RESTART
};

// Define orc positions and behaviors for a more interesting level layout
struct OrcPlacement {
    float posX;
    float posY;
    OrcBehavior behavior;
    float patrolRange;
    OrcType type;
};

class SceneGame : public Scene
{
public:
    SceneGame();
    virtual ~SceneGame();

    virtual bool Initialise(Renderer& renderer);
    virtual void Process(float deltaTime);
    virtual void Draw(Renderer& renderer);

    void ProcessInput(InputSystem& inputSystem);

    // Spawn orcs 
    void SpawnOrcs(Renderer& renderer);
    void SpawnOrcWave(const OrcPlacement* placements, int count, float offset, Renderer& renderer);

    // Helper method to determine damage based on orc type
    int GetOrcAttackDamage(Orc* orc) const;

    // Helper to check if knight is dead and manage state transitions
    void CheckKnightState();

    
    // Restart the game and reset all game elements
    void RestartGame();

protected:
    // Game components
    BackgroundManager* m_pBackgroundManager;
    PauseMenu* m_pPauseMenu;
    GameOverMenu* m_pGameOverMenu;
    KnightClass* m_pKnightClass;
    Renderer* m_pRenderer;
    KnightHUD* m_pKnightHUD;
    SceneGuide* m_pSceneGuide; 

    // Enemy list
    std::vector<Orc*> m_orcs;   
    
    // Game state
    float m_scrollDistance;
    GameState m_gameState;

    int m_score;

    //Audio setup
    FMOD::Sound* m_gameMusic;
    FMOD::Channel* m_gameChannel;
    float m_gameVolume;
    bool m_gameStartPlayed;

    float m_nextWaveOffset;

};

#endif // __SCENEGAME_H_