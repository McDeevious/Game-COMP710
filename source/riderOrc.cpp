#include "RiderOrc.h"
#include "renderer.h"
#include "animatedsprite.h"
#include "logmanager.h"
#include "game.h"

RiderOrc::RiderOrc()
{
    m_orcHealth = 100;
    m_attackCooldown = 4.5f;
    m_detectionRange = 450.0f;
    m_attackRange = 150.0f;
    m_orcSpeed = 0.25f;
    m_orcType = ORC_RIDER;
} 

RiderOrc::~RiderOrc()
{
}

bool RiderOrc::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem(); 

    fmod->createSound("../game/assets/Audio/Orc-Audio/orc_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Orc-Audio/orc_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Orc-Audio/orc_death.wav", FMOD_DEFAULT, 0, &m_deathSound);


    //Load orc's idle sprite
    m_orcIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc rider/Orc rider/Orc rider-Idle.png"); 
    if (m_orcIdle) {
        m_orcIdle->SetupFrames(100, 100);
        m_orcIdle->SetFrameDuration(0.2f);
        m_orcIdle->SetLooping(true);
        m_orcIdle->Animate();
        m_orcIdle->SetScale(7.5f, -7.5f);
    }

    //Load orc's walking sprite
    m_orcWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc rider/Orc rider/Orc rider-Walk.png");
    if (m_orcWalk) {
        m_orcWalk->SetupFrames(100, 100);
        m_orcWalk->SetFrameDuration(0.1f);
        m_orcWalk->SetLooping(true);
        m_orcWalk->Animate();
        m_orcWalk->SetScale(7.5f, -7.5f);
    }

    //Load orc's idle sprite 
    m_orcHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc rider/Orc rider/Orc rider-Hurt.png");
    if (m_orcHurt) {
        m_orcHurt->SetupFrames(100, 100);
        m_orcHurt->SetFrameDuration(0.2f);
        m_orcHurt->SetLooping(false);
        m_orcHurt->Animate();
        m_orcHurt->SetScale(7.5f, -7.5f);
    }

    //  Load orc's walking sprite 
    m_orcDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc rider/Orc rider/Orc rider-Death.png");
    if (m_orcDeath) {
        m_orcDeath->SetupFrames(100, 100);
        m_orcDeath->SetFrameDuration(0.1f);
        m_orcDeath->SetLooping(false);
        m_orcDeath->Animate();
        m_orcDeath->SetScale(7.5f, -7.5f);
    }

    //Load orc's attack1 sprite 
    m_orcAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc rider/Orc rider/Orc rider-Attack01.png");
    if (m_orcAttack1) {
        m_orcAttack1->SetupFrames(100, 100);
        m_orcAttack1->SetFrameDuration(0.1f);
        m_orcAttack1->SetLooping(false);
        m_orcAttack1->SetScale(7.5f, -7.5f);
    }

    //Load orc's attack2 sprite
    m_orcAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc rider/Orc rider/Orc rider-Attack02.png");
    if (m_orcAttack2) {
        m_orcAttack2->SetupFrames(100, 100);
        m_orcAttack2->SetFrameDuration(0.1f);
        m_orcAttack2->SetLooping(false);
        m_orcAttack2->SetScale(7.5f, -7.5f);
    }

    if (!m_orcIdle || !m_orcWalk || !m_orcHurt || !m_orcDeath || !m_orcAttack1 || !m_orcAttack2)
    {
        LogManager::GetInstance().Log("Failed to load Armored Orc sprites!");
        return false;
    }
    // Set initial direction scale for all sprites
    UpdateSpriteScales();

    return true;
}

void RiderOrc::Process(float deltaTime) {
    //Process death animation
    if (!m_isAlive) {
        if (m_orcDeath) {
            m_orcDeath->Process(deltaTime);

            if (m_orcDeath->GetCurrentFrame() <= 3) {
                m_orcDeath->StopAnimating();
                m_orcDeath->SetCurrentFrame(3);
            }
        }
        return;
    }

    //Process hurt animation
    if (m_orcIsHurt) {
        if (m_orcHurt) {
            m_orcHurt->Process(deltaTime);
            
            if (!m_orcHurt->IsAnimating() || m_orcHurt->GetCurrentFrame() >= 3) {
                m_orcIsHurt = false;
            }
        } else {
            m_orcIsHurt = false;
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
        case ORC_ATTACK_1:
            activeAttack = m_orcAttack1;
            timeoutDuration = 0.8f;
            break;
        case ORC_ATTACK_2:
            activeAttack = m_orcAttack2;
            timeoutDuration = 0.9f;
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
                m_attackState = ORC_ATTACK_NONE;
                m_attackDuration = 0.0f;

                if (m_currentBehavior == ORC_AGGRESSIVE) {
                    m_orcIsMoving = true;
                }
            }
        }
        else {
            m_isAttacking = false;
            m_attackState = ORC_ATTACK_NONE;
            m_attackDuration = 0.0f;
        }
    }

    if (!m_isAttacking) {
        if (m_orcIsMoving && m_orcWalk) {
            if (!m_orcWalk->IsAnimating()) {
                m_orcWalk->Animate();
            }
            m_orcWalk->Process(deltaTime);
        }
        else if (m_orcIdle) {
            if (!m_orcIdle->IsAnimating()) {
                m_orcIdle->Animate();
            }
            m_orcIdle->Process(deltaTime);
        }
        UpdateSpriteScales();
    }
}