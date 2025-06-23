#include "werewolf.h"
#include "animatedsprite.h"
#include "renderer.h"
#include "logmanager.h"
#include "game.h"
#include <cmath>
#include <cstdlib>
#include <string>

Werewolf::Werewolf()
    : m_werewolfIdle(nullptr)
    , m_werewolfWalk(nullptr)
    , m_werewolfHurt(nullptr)
    , m_werewolfDeath(nullptr)
    , m_werewolfAttack1(nullptr)
    , m_werewolfAttack2(nullptr)
{
    m_type = WEREWOLF;
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

Werewolf::~Werewolf() {
    delete m_werewolfIdle;
    m_werewolfIdle = nullptr;

    delete m_werewolfWalk;
    m_werewolfWalk = nullptr;

    delete m_werewolfHurt;
    m_werewolfHurt = nullptr;

    delete m_werewolfDeath;
    m_werewolfDeath = nullptr;

    delete m_werewolfAttack1;
    m_werewolfAttack1 = nullptr;

    delete m_werewolfAttack2;
    m_werewolfAttack2 = nullptr;
}

bool Werewolf::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    m_flipSprites = {
    m_werewolfIdle,
    m_werewolfWalk,
    m_werewolfAttack1,
    m_werewolfAttack2,
    m_werewolfHurt,
    m_werewolfDeath
    };


    //Load audio
    fmod->createSound("../game/assets/Audio/Lycan-Audio/lycan_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Lycan-Audio/lycan_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Lycan-Audio/lycan_death.wav", FMOD_DEFAULT, 0, &m_deathSound);

    //Load idle sprite
    m_werewolfIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werewolf/Werewolf-Idle.png");
    if (m_werewolfIdle) {
        m_werewolfIdle->SetupFrames(100, 100);
        m_werewolfIdle->SetFrameDuration(0.2f);
        m_werewolfIdle->SetLooping(true);
        m_werewolfIdle->Animate();
        m_werewolfIdle->SetScale(m_enemySize, -m_enemySize);
    }

    //Load walking sprite
    m_werewolfWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werewolf/Werewolf-Walk.png");
    if (m_werewolfWalk) {
        m_werewolfWalk->SetupFrames(100, 100);
        m_werewolfWalk->SetFrameDuration(0.1f);
        m_werewolfWalk->SetLooping(true);
        m_werewolfWalk->Animate();
        m_werewolfWalk->SetScale(m_enemySize, -m_enemySize);
    }

    //Load hurt sprite
    m_werewolfHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werewolf/Werewolf-Hurt.png"); 
    if (m_werewolfHurt) {
        m_werewolfHurt->SetupFrames(100, 100);
        m_werewolfHurt->SetFrameDuration(0.1f);
        m_werewolfHurt->SetLooping(false);
        m_werewolfHurt->Animate();
        m_werewolfHurt->SetScale(m_enemySize, -m_enemySize);
    }

    //Load death sprite
    m_werewolfDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werewolf/Werewolf-Death.png"); 
    if (m_werewolfDeath) {
        m_werewolfDeath->SetupFrames(100, 100);
        m_werewolfDeath->SetFrameDuration(0.1f);
        m_werewolfDeath->SetLooping(false);
        m_werewolfDeath->Animate();
        m_werewolfDeath->SetScale(m_enemySize, -m_enemySize);
    }

    //Load attack1 sprite
    m_werewolfAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werewolf/Werewolf-Attack01.png");
    if (m_werewolfAttack1) {
        m_werewolfAttack1->SetupFrames(100, 100);
        m_werewolfAttack1->SetFrameDuration(0.1f);
        m_werewolfAttack1->SetLooping(false);
        m_werewolfAttack1->SetScale(m_enemySize, -m_enemySize);
    }

    //Load attack2 sprite
    m_werewolfAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Ghouls/Werewolf/Werewolf-Attack02.png"); 
    if (m_werewolfAttack2) {
        m_werewolfAttack2->SetupFrames(100, 100);
        m_werewolfAttack2->SetFrameDuration(0.1f);
        m_werewolfAttack2->SetLooping(false);
        m_werewolfAttack2->SetScale(m_enemySize, -m_enemySize);
    }

    if (!m_werewolfIdle || !m_werewolfWalk || !m_werewolfHurt || !m_werewolfDeath || !m_werewolfAttack1 || !m_werewolfAttack2)
    {
        LogManager::GetInstance().Log("Failed to load Wearwolf sprites!");
        return false;
    }
    // Set initial direction scale for all sprites
    UpdateSpriteScales();

    return true;
}

void Werewolf::Process(float deltaTime) {
    // Process death animation
    if (!m_isAlive) {
        if (m_werewolfDeath) {
            m_werewolfDeath->Process(deltaTime);

            // Stop at last frame of death sprtie
            if (m_werewolfDeath->GetCurrentFrame() <= 3) {
                m_werewolfDeath->StopAnimating();
                m_werewolfDeath->SetCurrentFrame(3);
            }
        }
        return;
    }

    // Process hurt animation
    if (m_isHurt) {
        if (m_isHurt) {
            m_werewolfHurt->Process(deltaTime);

            //Check animation state if the hurt sprite exists
            if (!m_werewolfHurt->IsAnimating() || m_werewolfHurt->GetCurrentFrame() >= 3) {
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
            activeAttack = m_werewolfAttack1;
            timeoutDuration = 0.9f;
            break;
        case ATTACK_2: 
            activeAttack = m_werewolfAttack2;
            timeoutDuration = 0.13f;
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

        // Only update scales if we have sprites
        if (m_werewolfIdle || m_werewolfWalk || m_werewolfAttack1 || m_werewolfAttack2) { 
            UpdateSpriteScales();
        }
    }
}

void Werewolf::Draw(Renderer& renderer, float scrollX) {
    // Calculate screen position
    float drawX = m_position.x - scrollX;
    float drawY = m_position.y;

    // Draw the death animation
    if (!m_isAlive) {
        if (m_werewolfDeath) {
            m_werewolfDeath->SetX(drawX);
            m_werewolfDeath->SetY(drawY);
            m_werewolfDeath->Draw(renderer);
        }
        return;
    }

    // Draw the hurt animation
    if (m_isHurt) {
        if (m_werewolfHurt) {
            m_werewolfHurt->SetX(drawX);
            m_werewolfHurt->SetY(drawY);
            m_werewolfHurt->Draw(renderer);
        }
        else {
            m_isHurt = false; // Reset hurt state to avoid getting stuck
        }
        return;
    }

    // Draw the attack animations
    if (m_isAttacking) {
        bool drewAttack = false;

        if (m_attackState == ATTACK_1 && m_werewolfAttack1) {
            m_werewolfAttack1->SetX(drawX);
            m_werewolfAttack1->SetY(drawY);
            m_werewolfAttack1->Draw(renderer);
            drewAttack = true;
        } 
        else if (m_attackState == ATTACK_2 && m_werewolfAttack2) {
            m_werewolfAttack2->SetX(drawX);
            m_werewolfAttack2->SetY(drawY);
            m_werewolfAttack2->Draw(renderer);
            drewAttack = true;
        }

        if (drewAttack) {
            return;
        }
    }

    // Draw walking or idle animation
    if (m_isMoving && m_werewolfWalk) {
        m_werewolfWalk->SetX(drawX);
        m_werewolfWalk->SetY(drawY);
        m_werewolfWalk->Draw(renderer);
    }
    else if (m_werewolfIdle) {
        m_werewolfIdle->SetX(drawX);
        m_werewolfIdle->SetY(drawY);
        m_werewolfIdle->Draw(renderer);
    }
}

void Werewolf::SetPosition(float x, float y) {
    m_position.Set(x, y);
}

Vector2 Werewolf::GetPosition() const {
    return m_position;
}

void Werewolf::SetBehavior(EnemyBehavior behavior) {
    m_behavior = behavior;
}

void Werewolf::SetPatrolRange(float left, float right) {
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

bool Werewolf::IsAlive() const {
    return m_isAlive;
}

void Werewolf::TakeDamage(int amount) {
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

        if (m_werewolfDeath) {
            m_werewolfDeath->Restart();
            m_werewolfDeath->SetCurrentFrame(0);
            m_werewolfDeath->Animate();
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

        if (m_werewolfHurt) {
            m_werewolfHurt->SetCurrentFrame(0);
            m_werewolfHurt->Restart();
            m_werewolfHurt->Animate();
        }
    }
}

EnemyAttackType Werewolf::GetAttackState() const {
    return m_attackState;
}

bool Werewolf::IsAttacking() const {
    return m_isAttacking;
}

Hitbox Werewolf::GetHitbox() const {
    float halfWidth = (100.0f * 5.0f) / 2.0f;
    float halfHeight = (100.0f * 5.0f) / 2.0f;

    return {
        m_position.x - halfWidth,
        m_position.y - halfHeight,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };
}

Hitbox Werewolf::GetAttackHitbox() const {
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

int Werewolf::GetScore() const {
    int score = 0;

    switch (m_type)
    {
    case WEREWOLF:  
        score = 125;
        break;
    default:
        score = 0;
        break;;
    }

    return score;
}

bool Werewolf::WasScored() const {
    return m_wasScored;
}

void Werewolf::MarkScored() {
    m_wasScored = true;
}

void Werewolf::UpdateSpriteScales() {
    float scaleX = (m_direction > 0) ? m_enemySize : -m_enemySize;

    if (m_werewolfIdle && m_werewolfIdle->GetScaleX() != scaleX) {
        m_werewolfIdle->SetScale(scaleX, -m_enemySize);
    }
    if (m_werewolfWalk && m_werewolfWalk->GetScaleX() != scaleX) {
        m_werewolfWalk->SetScale(scaleX, -m_enemySize);
    }
    if (m_werewolfAttack1 && m_werewolfAttack1->GetScaleX() != scaleX) {
        m_werewolfAttack1->SetScale(scaleX, -m_enemySize);
    }
    if (m_werewolfAttack2 && m_werewolfAttack2->GetScaleX() != scaleX) {
        m_werewolfAttack2->SetScale(scaleX, -m_enemySize);
    }
    if (m_werewolfHurt && m_werewolfHurt->GetScaleX() != scaleX) {
        m_werewolfHurt->SetScale(scaleX, -m_enemySize);
    }
    if (m_werewolfDeath && m_werewolfDeath->GetScaleX() != scaleX) {
        m_werewolfDeath->SetScale(scaleX, -m_enemySize);
    }
}

bool Werewolf::IsAnimatingDeath() const
{
    return !m_isAlive && m_werewolfDeath && m_werewolfDeath->IsAnimating();  // check it's not null first 
}
