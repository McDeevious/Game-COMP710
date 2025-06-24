#include "werebear.h"
#include "renderer.h"
#include "animatedsprite.h"
#include "logmanager.h"
#include "game.h"

Werebear::Werebear()
{
    m_health = 100;
    m_attackCooldown = 4.5f;
    m_detectionRange = 450.0f;
    m_attackRange = 150.0f;
    m_speed = 0.25f;
    m_type = WEREBEAR;
}

Werebear::~Werebear()
{
}


bool Werebear::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    //Load audio
    fmod->createSound("../game/assets/Audio/Lycan-Audio/lycan_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Lycan-Audio/lycan_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Lycan-Audio/lycan_death.wav", FMOD_DEFAULT, 0, &m_deathSound);

    //Load idle sprite
    m_werewolfIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werebear/Werebear-Idle.png");
    if (m_werewolfIdle) {
        m_werewolfIdle->SetupFrames(100, 100);
        m_werewolfIdle->SetFrameDuration(0.2f);
        m_werewolfIdle->SetLooping(true);
        m_werewolfIdle->Animate();
        m_werewolfIdle->SetScale(m_enemySize, -m_enemySize);
    }

    //Load walking sprite
    m_werewolfWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werebear/Werebear-Walk.png");
    if (m_werewolfWalk) {
        m_werewolfWalk->SetupFrames(100, 100);
        m_werewolfWalk->SetFrameDuration(0.1f);
        m_werewolfWalk->SetLooping(true);
        m_werewolfWalk->Animate();
        m_werewolfWalk->SetScale(m_enemySize, -m_enemySize);
    }

    //Load hurt sprite
    m_werewolfHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werebear/Werebear-Hurt.png");
    if (m_werewolfHurt) {
        m_werewolfHurt->SetupFrames(100, 100);
        m_werewolfHurt->SetFrameDuration(0.1f);
        m_werewolfHurt->SetLooping(false);
        m_werewolfHurt->Animate();
        m_werewolfHurt->SetScale(m_enemySize, -m_enemySize);
    }

    //Load death sprite
    m_werewolfDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werebear/Werebear-Death.png");
    if (m_werewolfDeath) {
        m_werewolfDeath->SetupFrames(100, 100);
        m_werewolfDeath->SetFrameDuration(0.1f);
        m_werewolfDeath->SetLooping(false);
        m_werewolfDeath->Animate();
        m_werewolfDeath->SetScale(m_enemySize, -m_enemySize);
    }

    //Load attack1 sprite
    m_werewolfAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werebear/Werebear-Attack01.png");
    if (m_werewolfAttack1) {
        m_werewolfAttack1->SetupFrames(100, 100);
        m_werewolfAttack1->SetFrameDuration(0.1f);
        m_werewolfAttack1->SetLooping(false);
        m_werewolfAttack1->SetScale(m_enemySize, -m_enemySize);
    }

    //Load attack2 sprite
    m_werewolfAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werebear/Werebear-Attack02.png");
    if (m_werewolfAttack2) {
        m_werewolfAttack2->SetupFrames(100, 100);
        m_werewolfAttack2->SetFrameDuration(0.1f);
        m_werewolfAttack2->SetLooping(false);
        m_werewolfAttack2->SetScale(m_enemySize, -m_enemySize);
    }

    if (!m_werewolfIdle || !m_werewolfWalk)
    {
        LogManager::GetInstance().Log("Failed to load Wearbear sprites!");
        return false;
    }
    // Set initial direction scale for all sprites
    UpdateSpriteScales();

    return true;
}

void Werebear::Process(float deltaTime, SceneGame& game) {
    // Process death animation
    if (!m_isAlive) {
        if (m_werewolfDeath) {
            m_werewolfDeath->Process(deltaTime);

            if (m_werewolfDeath->GetCurrentFrame() <= 3) {
                m_werewolfDeath->StopAnimating();
                m_werewolfDeath->SetCurrentFrame(3);
            }
        }
        return;
    }

    // Process hurt animation
    if (m_isHurt) {
        if (m_werewolfHurt) {
            m_werewolfHurt->Process(deltaTime);

            if (!m_werewolfHurt->IsAnimating() || m_werewolfHurt->GetCurrentFrame() >= 3) {
                m_isHurt = false;
            }
        }
        else {
            m_isHurt = false;
        }
        return;
    }

    if (m_currentAttackCooldown > 0.0f) {
        m_currentAttackCooldown -= deltaTime;
    }

    if (m_isAttacking) {
        m_attackDuration += deltaTime;

        AnimatedSprite* activeAttack = nullptr;
        float timeoutDuration = 1.2f;
        switch (m_attackState)
        {
        case ATTACK_1:
            activeAttack = m_werewolfAttack1;
            timeoutDuration = 0.7f;
            break;
        case ATTACK_2:
            activeAttack = m_werewolfAttack2;
            timeoutDuration = 0.8f;
            break;
        default:
            m_isAttacking = false;
            break;
        }

        if (activeAttack) {
            if (!activeAttack->IsAnimating()) {
                activeAttack->Animate();
            }
            activeAttack->Process(deltaTime);

            bool animationComplete = !activeAttack->IsAnimating();
            bool timedOut = m_attackDuration > timeoutDuration;

            if (animationComplete || timedOut) {
                m_isAttacking = false;
                m_attackState = ATTACK_NONE;
                m_attackDuration = 0.0f;

                if (m_behavior == AGGRESSIVE) {
                    m_isMoving = true;
                }
            }
        }
        else {
            // No valid attack sprite, so cancel the attack state
            m_isAttacking = false;
            m_attackState = ATTACK_NONE; 
            m_attackDuration = 0.0f;
        }
    }

    if (!m_isAttacking) {
        if (m_isMoving && m_werewolfWalk) {
            if (!m_werewolfWalk->IsAnimating()) {
                m_werewolfWalk->Animate();
            }
            m_werewolfWalk->Process(deltaTime);
        }
        else if (m_werewolfIdle) {
            if (!m_werewolfIdle->IsAnimating()) {
                m_werewolfIdle->Animate();
            }
            m_werewolfIdle->Process(deltaTime);
        }
        UpdateSpriteScales();
    }
}