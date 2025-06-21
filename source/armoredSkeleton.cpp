#include "ArmoredSkeleton.h"
#include "renderer.h"
#include "animatedsprite.h"
#include "logmanager.h"
#include "game.h"

ArmoredSkeleton::ArmoredSkeleton()
{
    m_health = 100;
    m_attackCooldown = 4.5f;
    m_detectionRange = 450.0f;
    m_attackRange = 150.0f;
    m_speed = 0.25f;
    m_type = SKELETON_ARMORED; 
}

ArmoredSkeleton::~ArmoredSkeleton()
{
}

bool ArmoredSkeleton::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound); 
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_death.wav", FMOD_DEFAULT, 0, &m_deathSound); 

    LogManager::GetInstance().Log("Initializing ArmoredSkeleton sprites...");

    //Load idle sprite
    m_skeletonIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Armored Skeleton/Armored Skeleton-Idle.png");
    if (m_skeletonIdle) {
        m_skeletonIdle->SetupFrames(100, 100);
        m_skeletonIdle->SetFrameDuration(0.2f);
        m_skeletonIdle->SetLooping(true);
        m_skeletonIdle->Animate();
        m_skeletonIdle->SetScale(7.5f, -7.5f);
    }

    //Load walking sprite
    m_skeletonWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Armored Skeleton/Armored Skeleton-Walk.png");
    if (m_skeletonWalk) {
        m_skeletonWalk->SetupFrames(100, 100);
        m_skeletonWalk->SetFrameDuration(0.1f);
        m_skeletonWalk->SetLooping(true);
        m_skeletonWalk->Animate();
        m_skeletonWalk->SetScale(7.5f, -7.5f);
    }

    //Load hurt sprite
    m_skeletonHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Armored Skeleton/Armored Skeleton-Hurt.png");
    if (m_skeletonHurt) {
        m_skeletonHurt->SetupFrames(100, 100);
        m_skeletonHurt->SetFrameDuration(0.2f);
        m_skeletonHurt->SetLooping(false);
        m_skeletonHurt->Animate();
        m_skeletonHurt->SetScale(7.5f, -7.5f);
    }

    //Load death sprite
    m_skeletonDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Armored Skeleton/Armored Skeleton-Death.png");
    if (m_skeletonDeath) {
        m_skeletonDeath->SetupFrames(100, 100);
        m_skeletonDeath->SetFrameDuration(0.1f);
        m_skeletonDeath->SetLooping(false);
        m_skeletonDeath->Animate();
        m_skeletonDeath->SetScale(7.5f, -7.5f);
    }

    //Load attack1 sprite
    m_skeletonAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Armored Skeleton/Armored Skeleton-Attack01.png");
    if (m_skeletonAttack1) {
        m_skeletonAttack1->SetupFrames(100, 100);
        m_skeletonAttack1->SetFrameDuration(0.1f);
        m_skeletonAttack1->SetLooping(false);
        m_skeletonAttack1->SetScale(7.5f, -7.5f);
    }

    //Load attack2 sprite
    m_skeletonAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Armored Skeleton/Armored Skeleton-Attack02.png");
    if (m_skeletonAttack2) {
        m_skeletonAttack2->SetupFrames(100, 100);
        m_skeletonAttack2->SetFrameDuration(0.1f);
        m_skeletonAttack2->SetLooping(false);
        m_skeletonAttack2->SetScale(7.5f, -7.5f);
    }

    if (!m_skeletonIdle || !m_skeletonWalk)
    {
        return false;
    }

    UpdateSpriteScales();

    return true;
}

void ArmoredSkeleton::Process(float deltaTime) {
    // Process death animation
    if (!m_isAlive) {
        if (m_skeletonDeath) {
            m_skeletonDeath->Process(deltaTime);

            if (m_skeletonDeath->GetCurrentFrame() <= 3) {
                m_skeletonDeath->StopAnimating();
                m_skeletonDeath->SetCurrentFrame(3);
            }
        }
        return;
    }

    // Process hurt animation
    if (m_isHurt) {
        if (m_skeletonHurt) {
            m_skeletonHurt->Process(deltaTime);

            if (!m_skeletonHurt->IsAnimating() || m_skeletonHurt->GetCurrentFrame() >= 3) {
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
            activeAttack = m_skeletonAttack1; 
            timeoutDuration = 0.7f;
            break;
        case ATTACK_2: 
            activeAttack = m_skeletonAttack2; 
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
        if (m_isMoving && m_skeletonWalk) {
            if (!m_skeletonWalk->IsAnimating()) {
                m_skeletonWalk->Animate();
            }
            m_skeletonWalk->Process(deltaTime);
        }
        else if (m_skeletonIdle) {
            if (!m_skeletonIdle->IsAnimating()) {
                m_skeletonIdle->Animate();
            }
            m_skeletonIdle->Process(deltaTime);
        }
        UpdateSpriteScales();
    }
}