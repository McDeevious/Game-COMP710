#include "bossDemon.h"
#include "animatedsprite.h"
#include "renderer.h"
#include "logmanager.h"
#include "game.h"
#include <cmath>
#include <cstdlib>
#include <string>

BossDemon::BossDemon()
    : m_demonIdle(nullptr)
    , m_demonWalk(nullptr)
    , m_demonHurt(nullptr)
    , m_demonDeath(nullptr)
    , m_demonCleave(nullptr)
    , m_demonSmash(nullptr)
{
    m_type = DEMON_BOSS; 
    m_health = 50;
    m_speed = 1.0f;
    m_direction = 1;
    m_isMoving = false;
    m_isHurt = false;
    m_wasScored = false;
    m_attackState = ATTACK_NONE;
    m_isAttacking = false;
    m_attackDuration = 0.0f;
    m_behavior = IDLE;
    m_patrolRangeLeft = 0.0f;
    m_patrolRangeRight = 0.0f;
    m_detectionRange = 350.0f;
    m_attackRange = 120.0f;
    m_isAlive = true;
    m_attackCooldown = 2.5f;
    m_currentAttackCooldown = 0.0f;
    m_attackSound = nullptr;
    m_hurtSound = nullptr;
    m_deathSound = nullptr;
    m_sfxVolume = 0.4f;
}

BossDemon::~BossDemon() {
    delete m_demonIdle;
    m_demonIdle = nullptr;

    delete m_demonWalk;
    m_demonWalk = nullptr;

    delete m_demonHurt;
    m_demonHurt = nullptr;

    delete m_demonDeath;
    m_demonDeath = nullptr;

    delete m_demonCleave;
    m_demonCleave = nullptr;

    delete m_demonSmash;
    m_demonSmash = nullptr;
}

bool BossDemon::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    m_flipSprites = {
    m_demonIdle,
    m_demonWalk,
    m_demonCleave,
    m_demonSmash,
    m_demonHurt,
    m_demonDeath
    };

    //Load audio
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_death.wav", FMOD_DEFAULT, 0, &m_deathSound);

    //Load idle sprite
    m_demonIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/boss_demon_slime_FULL_v1.0/demon_idle_1.png");
    if (m_demonIdle) {
        m_demonIdle->SetupFrames(160, 160);
        m_demonIdle->SetFrameDuration(0.2f);
        m_demonIdle->SetLooping(true);
        m_demonIdle->Animate();
        m_demonIdle->SetScale(7.5f, -7.5f);
    }

    //Load walking sprite
    m_demonWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/boss_demon_slime_FULL_v1.0/demon_walk_1.png");
    if (m_demonWalk) {
        m_demonWalk->SetupFrames(160, 160);
        m_demonWalk->SetFrameDuration(0.1f);
        m_demonWalk->SetLooping(true);
        m_demonWalk->Animate();
        m_demonWalk->SetScale(7.5f, -7.5f);
    }

    //Load hurt sprite
    m_demonHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/boss_demon_slime_FULL_v1.0/demon_take_hit_1.png");
    if (m_demonHurt) {
        m_demonHurt->SetupFrames(160, 160);
        m_demonHurt->SetFrameDuration(0.1f);
        m_demonHurt->SetLooping(false);
        m_demonHurt->Animate();
        m_demonHurt->SetScale(7.5f, -7.5f);
    }

    //Load death sprite
    m_demonDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/boss_demon_slime_FULL_v1.0/demon_death_1.png");
    if (m_demonDeath) {
        m_demonDeath->SetupFrames(160, 160);
        m_demonDeath->SetFrameDuration(0.1f);
        m_demonDeath->SetLooping(false);
        m_demonDeath->Animate();
        m_demonDeath->SetScale(7.5f, -7.5f);
    }

    //Load attack1 sprite
    m_demonCleave = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/boss_demon_slime_FULL_v1.0/demon_cleave_1.png");
    if (m_demonCleave) {
        m_demonCleave->SetupFrames(160, 160);
        m_demonCleave->SetFrameDuration(0.1f);
        m_demonCleave->SetLooping(false);
        m_demonCleave->SetScale(7.5f, -7.5f);
    }

    //Load attack2 sprite
    m_demonSmash = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/boss_demon_slime_FULL_v1.0/demon_smash_1.png");
    if (m_demonSmash) {
        m_demonSmash->SetupFrames(160, 160);
        m_demonSmash->SetFrameDuration(0.1f);
        m_demonSmash->SetLooping(false);
        m_demonSmash->SetScale(7.5f, -7.5f);
    }

    if (!m_demonIdle || !m_demonWalk || !m_demonHurt || !m_demonDeath || !m_demonCleave || !m_demonSmash)
    {
        LogManager::GetInstance().Log("Failed to load Demon sprites!");
        return false;
    }

    // Set initial direction scale for all sprites
    UpdateSpriteScales();

    return true;
}

// Need to alter some of the frame values, I think some of the file frames widths are not the same btw
void BossDemon::Process(float deltaTime) {
    // Process death animation
    if (!m_isAlive) {
        if (m_demonDeath) {
            m_demonDeath->Process(deltaTime);

            // Stop at last frame of death sprtie
            if (m_demonDeath->GetCurrentFrame() <= 3) {
                m_demonDeath->StopAnimating();
                m_demonDeath->SetCurrentFrame(3);
            }
        }
        return;
    }

    // Process hurt animation
    if (m_isHurt) {
        if (m_isHurt) {
            m_demonHurt->Process(deltaTime);

            //Check animation state if the hurt sprite exists
            if (!m_demonHurt->IsAnimating() || m_demonHurt->GetCurrentFrame() >= 3) {
                m_isHurt = false;
            }
        }
        else {
            // If no hurt sprite, exit hurt state
            m_isHurt = false;
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
        case ATTACK_1:
            activeAttack = m_demonCleave;
            timeoutDuration = 2.0f;
            break;
        case ATTACK_2:
            activeAttack = m_demonSmash;
            timeoutDuration = 2.0f;
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
            // No valid attack sprite found
            m_isAttacking = false;
            m_attackState = ATTACK_NONE;
            m_attackDuration = 0.0f;
        }
    }

    //Process the movements when not attacking
    if (!m_isAttacking) {
        if (m_isMoving && m_demonWalk) {
            if (!m_demonWalk->IsAnimating()) {
                m_demonWalk->Animate();
            }
            m_demonWalk->Process(deltaTime);
        }
        else if (m_demonIdle) {
            if (!m_demonIdle->IsAnimating()) {
                m_demonIdle->Animate();
            }
            m_demonIdle->Process(deltaTime);
        }

        // Only update scales if we have sprites
        if (m_demonIdle || m_demonWalk || m_demonCleave || m_demonSmash) {
            UpdateSpriteScales();
        }
    }
}

void BossDemon::Draw(Renderer& renderer, float scrollX) {
    // Calculate screen position
    float drawX = m_position.x - scrollX;
    float drawY = m_position.y;

    // Draw the death animation
    if (!m_isAlive) {
        if (m_demonDeath) {
            m_demonDeath->SetX(drawX);
            m_demonDeath->SetY(drawY);
            m_demonDeath->Draw(renderer);
        }
        return;
    }

    // Draw the hurt animation
    if (m_isHurt) {
        if (m_demonHurt) {
            m_demonHurt->SetX(drawX);
            m_demonHurt->SetY(drawY);
            m_demonHurt->Draw(renderer);
        }
        else {
            m_isHurt = false; // Reset hurt state to avoid getting stuck
        }
        return;
    }

    // Draw the attack animations
    if (m_isAttacking) {
        bool drewAttack = false;

        if (m_attackState == ATTACK_1 && m_demonCleave) {
            m_demonCleave->SetX(drawX);
            m_demonCleave->SetY(drawY);
            m_demonCleave->Draw(renderer);
            drewAttack = true;
        }
        else if (m_attackState == ATTACK_2 && m_demonSmash) {
            m_demonSmash->SetX(drawX);
            m_demonSmash->SetY(drawY);
            m_demonSmash->Draw(renderer);
            drewAttack = true;
        }

        if (drewAttack) {
            return;
        }
    }

    // Draw walking or idle animation
    if (m_isMoving && m_demonWalk) {
        m_demonWalk->SetX(drawX);
        m_demonWalk->SetY(drawY);
        m_demonWalk->Draw(renderer);
    }
    else if (m_demonIdle) {
        m_demonIdle->SetX(drawX);
        m_demonIdle->SetY(drawY);
        m_demonIdle->Draw(renderer);
    }
}

void BossDemon::SetPosition(float x, float y) {
    m_position.Set(x, y);
}

Vector2 BossDemon::GetPosition() const {
    return m_position;
}

void BossDemon::SetBehavior(EnemyBehavior behavior) {
    m_behavior = behavior;
}

void BossDemon::SetPatrolRange(float left, float right) {
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
    if (m_position.x <= m_patrolRangeLeft) {
        m_direction = 1;  // Move right
    }
    else if (m_position.x >= m_patrolRangeRight) {
        m_direction = -1; // Move left
    }
    else if (m_direction == 0) {
        m_direction = 1;  // Default to moving right
    }
}

bool BossDemon::IsAlive() const {
    return m_isAlive;
}

void BossDemon::TakeDamage(int amount) {
    // Already dead or in hurt state, can't take more damage
    if (!m_isAlive || m_isHurt) return;

    m_health -= amount;

    //Check to see if orc is dead or taking damage
    if (m_health <= 0) {
        m_health = 0;
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

        if (m_demonDeath) {
            m_demonDeath->Restart();
            m_demonDeath->SetCurrentFrame(0);
            m_demonDeath->Animate();
        }
    }
    else {
        // Trigger hurt state
        m_isHurt = true;

        //Play the hurt audio when orc gets hurt
        if (m_hurtSound) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            FMOD::Channel* m_hurtChannel = nullptr;
            fmod->playSound(m_hurtSound, nullptr, false, &m_hurtChannel);

            if (m_hurtChannel) {
                m_hurtChannel->setVolume(m_sfxVolume);
            }
        }

        if (m_demonHurt) {
            m_demonHurt->SetCurrentFrame(0);
            m_demonHurt->Restart();
            m_demonHurt->Animate();
        }
    }
}

EnemyAttackType BossDemon::GetAttackState() const {
    return m_attackState;
}

bool BossDemon::IsAttacking() const {
    return m_isAttacking;
}

Hitbox BossDemon::GetHitbox() const {
    float halfWidth = (100.0f * 5.0f) / 2.0f;
    float halfHeight = (100.0f * 5.0f) / 2.0f;

    return {
        m_position.x - halfWidth,
        m_position.y - halfHeight,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };
}

Hitbox BossDemon::GetAttackHitbox() const {
    // Only return a valid hitbox if the orc is actually attacking
    if (!m_isAttacking) {
        // Return an empty/invalid hitbox when not attacking
        return { 0, 0, 0, 0 };
    }

    float attackWidth = 70.0f;
    float attackHeight = 100.0f * 5.0f;

    // Make Attack2 hitbox slightly larger for more impact
    if (m_attackState == ATTACK_2) {
        attackWidth = 85.0f; // Wider attack hitbox for Attack2
    }

    float offsetX = (m_direction == 1) ? 40.0f : -attackWidth - 40.0f;

    return {
        m_position.x + offsetX,
        m_position.y - (attackHeight / 2.0f),
        attackWidth,
        attackHeight
    };
}

int BossDemon::GetScore() const {
    int score = 0;

    switch (m_type)
    {
    case DEMON_BOSS:  
        score = 350;
        break;
    default:
        score = 0;
        break;;
    }

    return score;
}

bool BossDemon::WasScored() const {
    return m_wasScored;
}

void BossDemon::MarkScored() {
    m_wasScored = true;
}

void  BossDemon::UpdateSpriteScales() {
    float scaleX = (m_direction > 0) ? 7.5f : -7.5f;

    if (m_demonIdle && m_demonIdle->GetScaleX() != scaleX) {
        m_demonIdle->SetScale(scaleX, -7.5f);
    }
    if (m_demonWalk && m_demonWalk->GetScaleX() != scaleX) {
        m_demonWalk->SetScale(scaleX, -7.5f);
    }
    if (m_demonCleave && m_demonCleave->GetScaleX() != scaleX) {
        m_demonCleave->SetScale(scaleX, -7.5f);
    }
    if (m_demonSmash && m_demonSmash->GetScaleX() != scaleX) {
        m_demonSmash->SetScale(scaleX, -7.5f);
    }
    if (m_demonHurt && m_demonHurt->GetScaleX() != scaleX) {
        m_demonHurt->SetScale(scaleX, -7.5f);
    }
    if (m_demonDeath && m_demonDeath->GetScaleX() != scaleX) {
        m_demonDeath->SetScale(scaleX, -7.5f);
    }
}

