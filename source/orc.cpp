#include "Orc.h"
#include "enemy.h"
#include "animatedsprite.h"
#include "renderer.h"
#include "logmanager.h"
#include "game.h"
#include <cmath>
#include <cstdlib>
#include <string>

Orc::Orc()
    : m_orcIdle(nullptr)
    , m_orcWalk(nullptr)
    , m_orcHurt(nullptr)
    , m_orcDeath(nullptr)
    , m_orcAttack1(nullptr)
    , m_orcAttack2(nullptr)
    
{
    m_type = ORC;               
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

Orc::~Orc() {
    delete m_orcIdle; 
    m_orcIdle = nullptr; 

    delete m_orcWalk; 
    m_orcWalk = nullptr; 

    delete m_orcHurt;
    m_orcHurt = nullptr;

    delete m_orcDeath;
    m_orcDeath = nullptr; 

    delete m_orcAttack1;
    m_orcAttack1 = nullptr;

    delete m_orcAttack2;  
    m_orcAttack2 = nullptr;  
}

bool Orc::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    m_flipSprites = {
    m_orcIdle,
    m_orcWalk,
    m_orcAttack1,
    m_orcAttack2,
    m_orcHurt,
    m_orcDeath
    };

    //Load orc's audio
    fmod->createSound("../game/assets/Audio/Orc-Audio/orc_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound); 
    fmod->createSound("../game/assets/Audio/Orc-Audio/orc_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound); 
    fmod->createSound("../game/assets/Audio/Orc-Audio/orc_death.wav", FMOD_DEFAULT, 0, &m_deathSound); 

    //Load orc's idle sprite
    m_orcIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Idle.png");
    if (m_orcIdle) {
        m_orcIdle->SetupFrames(100, 100);
        m_orcIdle->SetFrameDuration(0.2f);
        m_orcIdle->SetLooping(true);
        m_orcIdle->Animate();
        m_orcIdle->SetScale(m_enemySize, -m_enemySize);
    }

    //Load orc's walking sprite
    m_orcWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Walk.png");
    if (m_orcWalk) {
        m_orcWalk->SetupFrames(100, 100);
        m_orcWalk->SetFrameDuration(0.1f);
        m_orcWalk->SetLooping(true);
        m_orcWalk->Animate();
        m_orcWalk->SetScale(m_enemySize, -m_enemySize);
    }

    //Load orc's hurt sprite
    m_orcHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Hurt.png"); 
    if (m_orcHurt) {
        m_orcHurt->SetupFrames(100, 100);
        m_orcHurt->SetFrameDuration(0.1f);
        m_orcHurt->SetLooping(false);
        m_orcHurt->Animate();
        m_orcHurt->SetScale(m_enemySize, -m_enemySize);
    }

    //Load orc's death sprite
    m_orcDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Death.png"); 
    if (m_orcDeath) {
        m_orcDeath->SetupFrames(100, 100);
        m_orcDeath->SetFrameDuration(0.1f);
        m_orcDeath->SetLooping(false);
        m_orcDeath->Animate(); 
        m_orcDeath->SetScale(m_enemySize, -m_enemySize);
    }

    //Load orc's attack1 sprite
    m_orcAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Attack01.png");
    if (m_orcAttack1) {
        m_orcAttack1->SetupFrames(100, 100);
        m_orcAttack1->SetFrameDuration(0.1f);
        m_orcAttack1->SetLooping(false);
        m_orcAttack1->SetScale(m_enemySize, -m_enemySize);
    }

    //Load orc's attack2 sprite
    m_orcAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Attack02.png");
    if (m_orcAttack2) {
        m_orcAttack2->SetupFrames(100, 100);
        m_orcAttack2->SetFrameDuration(0.1f);
        m_orcAttack2->SetLooping(false);
        m_orcAttack2->SetScale(m_enemySize, -m_enemySize);
    }
    
    if (!m_orcIdle || !m_orcWalk || !m_orcHurt || !m_orcDeath || !m_orcAttack1 || !m_orcAttack2) 
    {
        LogManager::GetInstance().Log("Failed to load Orc sprites!");
        return false;
    }
    // Set initial direction scale for all sprites

    UpdateSpriteScales();
    
    return true;
}

void Orc::Process(float deltaTime, SceneGame& game) {
    // Process death animation
    if (!m_isAlive) { 
        if (m_orcDeath) { 
            m_orcDeath->Process(deltaTime); 

            // Stop at last frame of death sprtie
            if (m_orcDeath->GetCurrentFrame() <= 3) {  
                m_orcDeath->StopAnimating();    
                m_orcDeath->SetCurrentFrame(3); 
            }
        }
        return;
    }

    // Process hurt animation
    if (m_isHurt) {
        if (m_orcHurt) {
            m_orcHurt->Process(deltaTime);
            
            //Check animation state if the hurt sprite exists
            if (!m_orcHurt->IsAnimating() || m_orcHurt->GetCurrentFrame() >= 3) {
                m_isHurt = false;
            }
        } else {
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
            activeAttack = m_orcAttack1;
            timeoutDuration = 0.6f;
            break;
        case ATTACK_2:
            activeAttack = m_orcAttack2;
            timeoutDuration = 0.6f;
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
        if (m_isMoving && m_orcWalk) {
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

        // Only update scales if we have sprites
        if (m_orcIdle || m_orcWalk || m_orcAttack1 || m_orcAttack2) {
            UpdateSpriteScales();
        }
    }
}

void Orc::Draw(Renderer& renderer, float scrollX) {
    // Calculate screen position
      float drawX = m_position.x; //- scrollX;
    float drawY = m_position.y;

    // Draw the death animation
    if (!m_isAlive) {
        if (m_orcDeath) {
            m_orcDeath->SetX(drawX); 
            m_orcDeath->SetY(drawY); 
            m_orcDeath->Draw(renderer);
        } 
        return;
    }

    // Draw the hurt animation
    if (m_isHurt) {
        if (m_orcHurt) {
            m_orcHurt->SetX(drawX); 
            m_orcHurt->SetY(drawY); 
            m_orcHurt->Draw(renderer); 
        } else {
            m_isHurt = false; // Reset hurt state to avoid getting stuck
        }
        return;
    }

    // Draw the attack animations
    if (m_isAttacking) {
        bool drewAttack = false;
        
        if (m_attackState == ATTACK_1 && m_orcAttack1) {
            m_orcAttack1->SetX(drawX);
            m_orcAttack1->SetY(drawY);
            m_orcAttack1->Draw(renderer);
            drewAttack = true;
        }
        else if (m_attackState == ATTACK_2 && m_orcAttack2) {
            m_orcAttack2->SetX(drawX);
            m_orcAttack2->SetY(drawY);
            m_orcAttack2->Draw(renderer);
            drewAttack = true;
        }
        
        if (drewAttack) {
            return; 
        }
    }

    // Draw walking or idle animation
    if (m_isMoving && m_orcWalk) {
        m_orcWalk->SetX(drawX);
        m_orcWalk->SetY(drawY);
        m_orcWalk->Draw(renderer);
    }
    else if (m_orcIdle) {
        m_orcIdle->SetX(drawX);
        m_orcIdle->SetY(drawY);
        m_orcIdle->Draw(renderer);
    }
}

void Orc::SetPosition(float x, float y) { 
    m_position.Set(x, y);
}

Vector2 Orc::GetPosition() const {
    return m_position;
}

void Orc::SetBehavior(EnemyBehavior behavior) {
    m_behavior = behavior;
}
 
void Orc::SetPatrolRange(float left, float right) {
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
    } else if (m_position.x >= m_patrolRangeRight) {
        m_direction = -1; // Move left
    } else if (m_direction == 0) {
        m_direction = 1;  // Default to moving right
    }
}

bool Orc::IsAlive() const { 
    return m_isAlive; 
}

void Orc::TakeDamage(int amount) {
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

        if (m_orcDeath) {
            m_orcDeath->Restart();
            m_orcDeath->SetCurrentFrame(0);
            m_orcDeath->Animate();
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

        if (m_orcHurt) {
            m_orcHurt->SetCurrentFrame(0);
            m_orcHurt->Restart();
            m_orcHurt->Animate();
        }
    }
}

EnemyAttackType Orc::GetAttackState() const {
    return m_attackState;
}

bool Orc::IsAttacking() const {
    return m_isAttacking;
}

Hitbox Orc::GetHitbox() const {
    float halfWidth = (10.0f * m_enemySize) / 2.0f;
    float halfHeight = (16.0f * m_enemySize) / 2.0f;

    return {
        m_position.x - halfWidth,
        m_position.y - halfHeight,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };
}

Hitbox Orc::GetAttackHitbox() const {
    // Only return a valid hitbox if the orc is actually attacking
    if (!m_isAttacking) {
        // Return an empty/invalid hitbox when not attacking
        return { 0, 0, 0, 0 };
    }

    float attackWidth = 12 * m_enemySize;
    float attackHeight = 16 * m_enemySize;

    // Make Attack2 hitbox slightly larger for more impact
    if (m_attackState == ATTACK_2) {
        attackWidth = 14*m_enemySize; // Wider attack hitbox for Attack2
    }

    float offsetXs = (m_direction == 1) ? -attackWidth : attackWidth; //- 40.0f;
    return {
        m_position.x - offsetXs / 2, //+ offsetX,
        m_position.y - attackHeight / 2, //- (attackHeight / 2.0f),
        attackWidth,
        attackHeight
    };
}

int Orc::GetScore() const {
    int score = 0;

    switch (m_type) 
    {
    case ORC:
        score = 100;
        break;
    case ORC_ARMORED:
        score = 150;
        break;
    case ORC_ELITE:
        score = 150;
        break;
    case ORC_RIDER:
        score = 200;
        break;
    default:
        score = 0;
        break;;
    }

    return score;
}

bool Orc::WasScored() const {
    return m_wasScored; 
}

void Orc::MarkScored() {
    m_wasScored = true;
}

void Orc::UpdateSpriteScales() {
    float scaleX = (m_direction > 0) ? m_enemySize : -m_enemySize;

    if (m_orcIdle && m_orcIdle->GetScaleX() != scaleX) {
        m_orcIdle->SetScale(scaleX, -m_enemySize);
    }
    if (m_orcWalk && m_orcWalk->GetScaleX() != scaleX) {
        m_orcWalk->SetScale(scaleX, -m_enemySize);
    }
    if (m_orcAttack1 && m_orcAttack1->GetScaleX() != scaleX) {
        m_orcAttack1->SetScale(scaleX, -m_enemySize);
    }
    if (m_orcAttack2 && m_orcAttack2->GetScaleX() != scaleX) {
        m_orcAttack2->SetScale(scaleX, -m_enemySize);
    }
    if (m_orcHurt && m_orcHurt->GetScaleX() != scaleX) {
        m_orcHurt->SetScale(scaleX, -m_enemySize);
    }
    if (m_orcDeath && m_orcDeath->GetScaleX() != scaleX) {
        m_orcDeath->SetScale(scaleX, -m_enemySize);
    }
}

bool Orc::IsAnimatingDeath() const
{
    return !m_isAlive && m_orcDeath && m_orcDeath->IsAnimating();
}
