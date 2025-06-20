#include "skeleton.h"
#include "animatedsprite.h"
#include "renderer.h"
#include "logmanager.h"
#include "game.h"
#include <cmath>
#include <cstdlib>
#include <string>

Skeleton::Skeleton()
    : m_skeletonIdle(nullptr)
    , m_skeletonWalk(nullptr)
    , m_skeletonHurt(nullptr)
    , m_skeletonDeath(nullptr)
    , m_skeletonAttack1(nullptr)
    , m_skeletonAttack2(nullptr)
    , m_skeletonSpeed(1.0f)
    , m_skeletonDirection(1)
    , m_skeletonIsMoving(false)
    , m_skeletonIsHurt(false)
    , m_skeletonType(SKELETON)
    , m_wasScored(false)
    // Attack attributes
    , m_attackState(SKELETON_ATTACK_NONE)
    , m_isAttacking(false)
    , m_attackDuration(0.0f)
    // AI attributes
    , m_currentBehavior(IDLE)
    , m_patrolRangeLeft(0.0f)
    , m_patrolRangeRight(0.0f)
    , m_detectionRange(350.0f)
    , m_attackRange(120.0f)
    // Combat attributes
    , m_skeletonHealth(50)
    , m_isAlive(true)
    , m_attackCooldown(2.5f)
    , m_currentAttackCooldown(0.0f)
    //Skeleton Audio
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_sfxVolume(0.4f)
{
}

Skeleton::~Skeleton() {
    delete m_skeletonIdle;
    m_skeletonIdle = nullptr;

    delete m_skeletonWalk;
    m_skeletonWalk = nullptr;

    delete m_skeletonHurt;
    m_skeletonHurt = nullptr;

    delete m_skeletonDeath;
    m_skeletonDeath = nullptr;

    delete m_skeletonAttack1;
    m_skeletonAttack1 = nullptr;

    delete m_skeletonAttack2;
    m_skeletonAttack2 = nullptr;
}

bool Skeleton::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    //Load audio
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_death.wav", FMOD_DEFAULT, 0, &m_deathSound);

    //Load idle sprite
    m_skeletonIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Skeleton/Skeleton-Idle.png");
    if (m_skeletonIdle) {
        m_skeletonIdle->SetupFrames(100, 100);
        m_skeletonIdle->SetFrameDuration(0.2f);
        m_skeletonIdle->SetLooping(true);
        m_skeletonIdle->Animate();
        m_skeletonIdle->SetScale(7.5f, -7.5f);
    }

    //Load walking sprite
    m_skeletonWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Skeleton/Skeleton-Walk.png");
    if (m_skeletonWalk) {
        m_skeletonWalk->SetupFrames(100, 100);
        m_skeletonWalk->SetFrameDuration(0.1f);
        m_skeletonWalk->SetLooping(true);
        m_skeletonWalk->Animate();
        m_skeletonWalk->SetScale(7.5f, -7.5f);
    }

    //Load hurt sprite
    m_skeletonHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Skeleton/Skeleton-Hurt.png");
    if (m_skeletonHurt) {
        m_skeletonHurt->SetupFrames(100, 100);
        m_skeletonHurt->SetFrameDuration(0.1f);
        m_skeletonHurt->SetLooping(false);
        m_skeletonHurt->Animate();
        m_skeletonHurt->SetScale(7.5f, -7.5f);
    }

    //Load death sprite
    m_skeletonDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Skeleton/Skeleton-Death.png");
    if (m_skeletonDeath) {
        m_skeletonDeath->SetupFrames(100, 100);
        m_skeletonDeath->SetFrameDuration(0.1f);
        m_skeletonDeath->SetLooping(false);
        m_skeletonDeath->Animate();
        m_skeletonDeath->SetScale(7.5f, -7.5f);
    }

    //Load attack1 sprite
    m_skeletonAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Skeleton/Skeleton-Attack01.png");
    if (m_skeletonAttack1) {
        m_skeletonAttack1->SetupFrames(100, 100);
        m_skeletonAttack1->SetFrameDuration(0.1f);
        m_skeletonAttack1->SetLooping(false);
        m_skeletonAttack1->SetScale(7.5f, -7.5f);
    }

    //Load attack2 sprite
    m_skeletonAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Skeleton/Skeleton/Skeleton-Attack02.png");
    if (m_skeletonAttack2) {
        m_skeletonAttack2->SetupFrames(100, 100);
        m_skeletonAttack2->SetFrameDuration(0.1f);
        m_skeletonAttack2->SetLooping(false);
        m_skeletonAttack2->SetScale(7.5f, -7.5f);
    }

    if (!m_skeletonIdle || !m_skeletonWalk || !m_skeletonHurt || !m_skeletonDeath || !m_skeletonAttack1 || !m_skeletonAttack2)
    {
        LogManager::GetInstance().Log("Failed to load Skeleton sprites!");
        return false;
    }
    // Set initial direction scale for all sprites
    UpdateSpriteScales();

    return true;
}

void Skeleton::Process(float deltaTime) {
    // Process death animation
    if (!m_isAlive) {
        if (m_skeletonDeath) {
            m_skeletonDeath->Process(deltaTime);

            // Stop at last frame of death sprtie
            if (m_skeletonDeath->GetCurrentFrame() <= 3) {
                m_skeletonDeath->StopAnimating();
                m_skeletonDeath->SetCurrentFrame(3);
            }
        }
        return;
    }

    // Process hurt animation
    if (m_skeletonIsHurt) {
        if (m_skeletonIsHurt) {
            m_skeletonHurt->Process(deltaTime);

            //Check animation state if the hurt sprite exists
            if (!m_skeletonHurt->IsAnimating() || m_skeletonHurt->GetCurrentFrame() >= 3) {
                m_skeletonIsHurt = false;
            }
        }
        else {
            // If no hurt sprite, exit hurt state
            m_skeletonIsHurt = false;
        }
        return;
    }

    //Reduce attack cooldown
    if (m_currentAttackCooldown > 0.0f) {
        m_currentAttackCooldown -= deltaTime;
    }

    //Process attack animations
    if (m_isAttacking) {
        m_attackDuration += deltaTime;

        AnimatedSprite* activeAttack = nullptr;
        float timeoutDuration = 1.2f;
        switch (m_attackState)
        {
        case SKELETON_ATTACK_1:
            activeAttack = m_skeletonAttack1;
            timeoutDuration = 0.6f;
            break;
        case SKELETON_ATTACK_2:
            activeAttack = m_skeletonAttack2;
            timeoutDuration = 0.7f; 
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
                m_attackState = SKELETON_ATTACK_NONE; 
                m_attackDuration = 0.0f;

                if (m_currentBehavior == AGGRESSIVE) { 
                    m_skeletonIsMoving = true;  
                }
            }
        }
        else {
            // No valid attack sprite found
            m_isAttacking = false;
            m_attackState = SKELETON_ATTACK_NONE; 
            m_attackDuration = 0.0f;
        }
    }

    //Process the movements when not attacking
    if (!m_isAttacking) {
        if (m_skeletonIsMoving && m_skeletonWalk) {
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

        // Only update scales if we have sprites
        if (m_skeletonIdle || m_skeletonWalk || m_skeletonAttack1 || m_skeletonAttack2) { 
            UpdateSpriteScales();
        }
    }
}

void Skeleton::Draw(Renderer& renderer, float scrollX) {
    // Calculate screen position
    float drawX = m_skeletonPosition.x - scrollX; 
    float drawY = m_skeletonPosition.y; 

    // Draw the death animation
    if (!m_isAlive) {
        if (m_skeletonDeath) { 
            m_skeletonDeath->SetX(drawX); 
            m_skeletonDeath->SetY(drawY); 
            m_skeletonDeath->Draw(renderer); 
        }
        return;
    }

    // Draw the hurt animation
    if (m_skeletonIsHurt) {
        if (m_skeletonHurt) {
            m_skeletonHurt->SetX(drawX);
            m_skeletonHurt->SetY(drawY);
            m_skeletonHurt->Draw(renderer);
        }
        else {
            m_skeletonIsHurt = false; // Reset hurt state to avoid getting stuck
        }
        return;
    }

    // Draw the attack animations
    if (m_isAttacking) {
        bool drewAttack = false;

        if (m_attackState == SKELETON_ATTACK_1 && m_skeletonAttack1) {
            m_skeletonAttack1->SetX(drawX);
            m_skeletonAttack1->SetY(drawY);
            m_skeletonAttack1->Draw(renderer);
            drewAttack = true;
        }
        else if (m_attackState == SKELETON_ATTACK_2 && m_skeletonAttack2) {
            m_skeletonAttack2->SetX(drawX);
            m_skeletonAttack2->SetY(drawY);
            m_skeletonAttack2->Draw(renderer);
            drewAttack = true;
        }

        if (drewAttack) {
            return;
        }
    }

    // Draw walking or idle animation
    if (m_skeletonIsMoving && m_skeletonWalk) {
        m_skeletonWalk->SetX(drawX);
        m_skeletonWalk->SetY(drawY);
        m_skeletonWalk->Draw(renderer);
    }
    else if (m_skeletonIdle) {
        m_skeletonIdle->SetX(drawX);
        m_skeletonIdle->SetY(drawY);
        m_skeletonIdle->Draw(renderer);
    }
}

void Skeleton::SetPosition(float x, float y) {
    m_skeletonPosition.Set(x, y);
}

Vector2 Skeleton::GetPosition() const {
    return m_skeletonPosition;
}

void Skeleton::SetBehavior(EnemyBehavior behavior) {
    m_currentBehavior = behavior;
}

void Skeleton::SetPatrolRange(float left, float right) {
    // Ensure right is greater than left
    if (left >= right) {
        float temp = left;
        left = right;
        right = temp;

        // Add 100 units to make sure there's enough space
        right += 100.0f;
    }

    // Ensure minimum patrol width
    float width = right - left;
    if (width < 50.0f) {
        right = left + 50.0f;
    }

    m_patrolRangeLeft = left;
    m_patrolRangeRight = right;

    // Set initial direction based on position
    if (m_skeletonPosition.x <= m_patrolRangeLeft) {
        m_skeletonDirection = 1;  // Move right
    }
    else if (m_skeletonPosition.x >= m_patrolRangeRight) {
        m_skeletonDirection = -1; // Move left
    }
    else if (m_skeletonDirection == 0) {
        m_skeletonDirection = 1;  // Default to moving right
    }
}

bool Skeleton::IsAlive() const {
    return m_isAlive;
}

void Skeleton::TakeDamage(int amount) {
    // Already dead or in hurt state, can't take more damage
    if (!m_isAlive || m_skeletonIsHurt) return;

    m_skeletonHealth -= amount;

    //Check to see if orc is dead or taking damage
    if (m_skeletonHealth <= 0) {
        m_skeletonHealth = 0;
        m_isAlive = false;

        //Play death sound when orc dies
        if (m_deathSound) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            FMOD::Channel* m_deathChannel = nullptr;
            fmod->playSound(m_deathSound, nullptr, false, &m_deathChannel);

            if (m_deathChannel) {
                m_deathChannel->setVolume(m_sfxVolume);
            }
        }

        if (m_skeletonDeath) {
            m_skeletonDeath->Restart();
            m_skeletonDeath->SetCurrentFrame(0);
            m_skeletonDeath->Animate();
        }
    }
    else {
        // Trigger hurt state
        m_skeletonIsHurt = true;

        //Play the hurt audio when orc gets hurt
        if (m_hurtSound) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            FMOD::Channel* m_hurtChannel = nullptr;
            fmod->playSound(m_hurtSound, nullptr, false, &m_hurtChannel);

            if (m_hurtChannel) {
                m_hurtChannel->setVolume(m_sfxVolume);
            }
        }

        if (m_skeletonHurt) {
            m_skeletonHurt->SetCurrentFrame(0);
            m_skeletonHurt->Restart();
            m_skeletonHurt->Animate();
        }
    }
}

EnemyAttackType Skeleton::GetAttackState() const {
    return m_attackState;
}

bool Skeleton::IsAttacking() const {
    return m_isAttacking;
}

void Skeleton::UpdateAI(const Vector2& playerPos, float deltaTime) {
    if (!m_isAlive) return;

    float distance = fabs(playerPos.x - m_skeletonPosition.x);
    bool playerInAttackRange = distance <= m_attackRange;

    //Define thebehaviors of the orc
    switch (m_currentBehavior)
    {
        //Orc in idle
    case IDLE: 
        m_skeletonIsMoving = false;

        if (distance < m_detectionRange) {
            m_currentBehavior = AGGRESSIVE; 
        }
        break;

        //Orc in patrol
    case PATROL: 
    {
        m_skeletonIsMoving = true;

        float moveAmount = m_skeletonSpeed * deltaTime * 60.0f;
        m_skeletonPosition.x += m_skeletonDirection * moveAmount;

        if (m_skeletonPosition.x <= m_patrolRangeLeft) {
            m_skeletonPosition.x = m_patrolRangeLeft;
            m_skeletonDirection = 1;
        }
        else if (m_skeletonPosition.x >= m_patrolRangeRight) {
            m_skeletonPosition.x = m_patrolRangeRight;
            m_skeletonDirection = -1;  
        }

        if (distance < m_detectionRange) {
            m_currentBehavior = AGGRESSIVE; 
        }
        break;
    }

    //Orc is aggressive
    case AGGRESSIVE: 
    {
        //face the player
        m_skeletonDirection = (playerPos.x < m_skeletonPosition.x) ? -1 : 1; 

        //distance from player to attack
        float attackDist = 80.0f;

        if (playerInAttackRange && m_currentAttackCooldown <= 0.0f) {
            if (m_attackSound) {
                FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
                FMOD::Channel* m_attackChannel = nullptr;

                fmod->playSound(m_attackSound, nullptr, false, &m_attackChannel);
                if (m_attackChannel) {
                    m_attackChannel->setVolume(m_sfxVolume);
                }
            }

            m_isAttacking = true;
            m_attackState = (rand() % 2 == 0) ? SKELETON_ATTACK_1 : SKELETON_ATTACK_2; 
            m_attackDuration = 0.0f;
            m_currentAttackCooldown = m_attackCooldown;
            m_skeletonIsMoving = false;
        }
        else if (!m_isAttacking && distance > attackDist) {
            float moveAmount = m_skeletonSpeed * deltaTime * 60.0f;
            float newX = m_skeletonPosition.x + m_skeletonDirection * moveAmount;

            m_skeletonPosition.x = newX;
            m_skeletonIsMoving = true;

        }
        else {
            m_skeletonIsMoving = false;
        }



        if (distance > m_detectionRange * 2.0f) {
            m_currentBehavior = PATROL;
        }
        break;
    }

    default:
        m_currentBehavior = IDLE; 
        m_skeletonIsMoving = false;
        break;
    }

    UpdateSpriteScales();

}

Hitbox Skeleton::GetHitbox() const {
    float halfWidth = (100.0f * 5.0f) / 2.0f;
    float halfHeight = (100.0f * 5.0f) / 2.0f;

    return {
        m_skeletonPosition.x - halfWidth, 
        m_skeletonPosition.y - halfHeight, 
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };
}

Hitbox Skeleton::GetAttackHitbox() const {
    // Only return a valid hitbox if the orc is actually attacking
    if (!m_isAttacking) {
        // Return an empty/invalid hitbox when not attacking
        return { 0, 0, 0, 0 };
    }

    float attackWidth = 70.0f;
    float attackHeight = 100.0f * 5.0f;

    // Make Attack2 hitbox slightly larger for more impact
    if (m_attackState == ORC_ATTACK_2) {
        attackWidth = 85.0f; // Wider attack hitbox for Attack2
    }

    float offsetX = (m_skeletonDirection == 1) ? 40.0f : -attackWidth - 40.0f;

    return {
        m_skeletonPosition.x + offsetX,
        m_skeletonPosition.y - (attackHeight / 2.0f),
        attackWidth,
        attackHeight
    };
}

void Skeleton::UpdateSpriteScales() {
    // Set sprite scale based on direction
    float scaleX = (m_skeletonDirection > 0) ? 7.5f : -7.5f;

    if (m_skeletonWalk && m_skeletonWalk->GetScaleX() != scaleX) {
        m_skeletonWalk->SetScale(scaleX, -7.5f);
    }

    if (m_skeletonIdle && m_skeletonIdle->GetScaleX() != scaleX) {
        m_skeletonIdle->SetScale(scaleX, -7.5f);
    }

    if (m_skeletonAttack1 && m_skeletonAttack1->GetScaleX() != scaleX) {
        m_skeletonAttack1->SetScale(scaleX, -7.5f);
    }

    if (m_skeletonAttack2 && m_skeletonAttack2->GetScaleX() != scaleX) {
        m_skeletonAttack2->SetScale(scaleX, -7.5f);
    }
}

int Skeleton::GetScore() const {
    int score = 0;

    switch (m_skeletonType)
    {
    case SKELETON:
        score = 100;
        break;
    case SKELETON_ARMORED:
        score = 150;
        break;
    case SKELETON_GREAT:
        score = 150;
        break;
    default:
        score = 0;
        break;;
    }

    return score;
}

bool Skeleton::WasScored() const {
    return m_wasScored;
}

void Skeleton::MarkScored() {
    m_wasScored = true;
}