#include "Orc.h"
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
    , m_orcSpeed(1.0f)
    , m_orcDirection(1)
    , m_orcIsMoving(false)
    , m_orcIsHurt(false)
    , m_orcType(ORC)
    , m_wasScored(false)
    // Attack attributes
    , m_attackState(ORC_ATTACK_NONE)
    , m_isAttacking(false)
    , m_attackDuration(0.0f)
    // AI attributes
    , m_currentBehavior(ORC_IDLE) 
    , m_patrolRangeLeft(0.0f)
    , m_patrolRangeRight(0.0f)
    , m_detectionRange(350.0f)
    , m_attackRange(120.0f)
    // Combat attributes
    , m_orcHealth(50)
    , m_isAlive(true)
    , m_attackCooldown(2.5f)
    , m_currentAttackCooldown(0.0f)
    //Orc Audio
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_sfxVolume(0.4f)
{
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
        m_orcIdle->SetScale(7.5f, -7.5f);
    }

    //Load orc's walking sprite
    m_orcWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Walk.png");
    if (m_orcWalk) {
        m_orcWalk->SetupFrames(100, 100);
        m_orcWalk->SetFrameDuration(0.1f);
        m_orcWalk->SetLooping(true);
        m_orcWalk->Animate();
        m_orcWalk->SetScale(7.5f, -7.5f);
    }

    //Load orc's hurt sprite
    m_orcHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Hurt.png"); 
    if (m_orcHurt) {
        m_orcHurt->SetupFrames(100, 100);
        m_orcHurt->SetFrameDuration(0.1f);
        m_orcHurt->SetLooping(false);
        m_orcHurt->Animate();
        m_orcHurt->SetScale(7.5f, -7.5f);
    }

    //Load orc's death sprite
    m_orcDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Death.png"); 
    if (m_orcDeath) {
        m_orcDeath->SetupFrames(100, 100);
        m_orcDeath->SetFrameDuration(0.1f);
        m_orcDeath->SetLooping(false);
        m_orcDeath->Animate(); 
        m_orcDeath->SetScale(7.5f, -7.5f);
    }

    //Load orc's attack1 sprite
    m_orcAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Attack01.png");
    if (m_orcAttack1) {
        m_orcAttack1->SetupFrames(100, 100);
        m_orcAttack1->SetFrameDuration(0.1f);
        m_orcAttack1->SetLooping(false);
        m_orcAttack1->SetScale(7.5f, -7.5f);
    }

    //Load orc's attack2 sprite
    m_orcAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Orcs/Orc/Orc/Orc-Attack02.png");
    if (m_orcAttack2) {
        m_orcAttack2->SetupFrames(100, 100);
        m_orcAttack2->SetFrameDuration(0.1f);
        m_orcAttack2->SetLooping(false);
        m_orcAttack2->SetScale(7.5f, -7.5f);
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

void Orc::Process(float deltaTime) {
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
    if (m_orcIsHurt) {
        if (m_orcHurt) {
            m_orcHurt->Process(deltaTime);
            
            //Check animation state if the hurt sprite exists
            if (!m_orcHurt->IsAnimating() || m_orcHurt->GetCurrentFrame() >= 3) {
                m_orcIsHurt = false;
            }
        } else {
            // If no hurt sprite, exit hurt state
            m_orcIsHurt = false;
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
        case ORC_ATTACK_1:
            activeAttack = m_orcAttack1;
            timeoutDuration = 0.6f;
            break;
        case ORC_ATTACK_2:
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
                m_attackState = ORC_ATTACK_NONE;
                m_attackDuration = 0.0f;

                if (m_currentBehavior == ORC_AGGRESSIVE) {
                    m_orcIsMoving = true;
                }
            }
        }
        else {
            // No valid attack sprite found
            m_isAttacking = false;
            m_attackState = ORC_ATTACK_NONE;
            m_attackDuration = 0.0f;
        }
    }

    //Process the movements when not attacking
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

        // Only update scales if we have sprites
        if (m_orcIdle || m_orcWalk || m_orcAttack1 || m_orcAttack2) {
            UpdateSpriteScales();
        }
    }
}

void Orc::Draw(Renderer& renderer, float scrollX) {
    // Calculate screen position
    float drawX = m_orcPosition.x - scrollX;
    float drawY = m_orcPosition.y;

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
    if (m_orcIsHurt) {
        if (m_orcHurt) {
            m_orcHurt->SetX(drawX); 
            m_orcHurt->SetY(drawY); 
            m_orcHurt->Draw(renderer); 
        } else {
            m_orcIsHurt = false; // Reset hurt state to avoid getting stuck
        }
        return;
    }

    // Draw the attack animations
    if (m_isAttacking) {
        bool drewAttack = false;
        
        if (m_attackState == ORC_ATTACK_1 && m_orcAttack1) {
            m_orcAttack1->SetX(drawX);
            m_orcAttack1->SetY(drawY);
            m_orcAttack1->Draw(renderer);
            drewAttack = true;
        }
        else if (m_attackState == ORC_ATTACK_2 && m_orcAttack2) {
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
    if (m_orcIsMoving && m_orcWalk) {
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
    m_orcPosition.Set(x, y);
}

Vector2 Orc::GetPosition() const {
    return m_orcPosition;
}

void Orc::SetBehavior(OrcBehavior behavior) {
    m_currentBehavior = behavior;
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
    if (m_orcPosition.x <= m_patrolRangeLeft) {
        m_orcDirection = 1;  // Move right
    } else if (m_orcPosition.x >= m_patrolRangeRight) {
        m_orcDirection = -1; // Move left
    } else if (m_orcDirection == 0) {
        m_orcDirection = 1;  // Default to moving right
    }
}

bool Orc::IsAlive() const { 
    return m_isAlive; 
}

void Orc::TakeDamage(int amount) {
    // Already dead or in hurt state, can't take more damage
    if (!m_isAlive || m_orcIsHurt) return;

    m_orcHealth -= amount;

    //Check to see if orc is dead or taking damage
    if (m_orcHealth <= 0) {
        m_orcHealth = 0;
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
        m_orcIsHurt = true;

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

OrcAttackType Orc::GetAttackState() const {
    return m_attackState;
}

bool Orc::IsAttacking() const {
    return m_isAttacking;
}

void Orc::UpdateAI(const Vector2& playerPos, float deltaTime) {
    if (!m_isAlive) return; 

    float distance = fabs(playerPos.x - m_orcPosition.x);
    bool playerInAttackRange = distance <= m_attackRange;

    //Define thebehaviors of the orc
    switch (m_currentBehavior)
    {
    //Orc in idle
    case ORC_IDLE: 
        m_orcIsMoving = false;

        if (distance < m_detectionRange) {
            m_currentBehavior = ORC_AGGRESSIVE;
        }
        break;

    //Orc in patrol
    case ORC_PATROL:
    {
        m_orcIsMoving = true;

        float moveAmount = m_orcSpeed * deltaTime * 60.0f;
        m_orcPosition.x += m_orcDirection * moveAmount;

        if (m_orcPosition.x <= m_patrolRangeLeft) {
            m_orcPosition.x = m_patrolRangeLeft;
            m_orcDirection = 1;
        }
        else if (m_orcPosition.x >= m_patrolRangeRight) {
            m_orcPosition.x = m_patrolRangeRight;
            m_orcDirection = -1;
        }

        if (distance < m_detectionRange) {
            m_currentBehavior = ORC_AGGRESSIVE;
        }
        break;
    }

    //Orc is aggressive
    case ORC_AGGRESSIVE:
    {
        //face the player
        m_orcDirection = (playerPos.x < m_orcPosition.x) ? -1 : 1;

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
            m_attackState = (rand() % 2 == 0) ? ORC_ATTACK_1 : ORC_ATTACK_2;
            m_attackDuration = 0.0f;
            m_currentAttackCooldown = m_attackCooldown;
            m_orcIsMoving = false;
        }
        else if (!m_isAttacking && distance > attackDist) {
            float moveAmount = m_orcSpeed * deltaTime * 60.0f; 
            float newX = m_orcPosition.x + m_orcDirection * moveAmount;

            m_orcPosition.x = newX; 
            m_orcIsMoving = true;

        }
        else {
            m_orcIsMoving = false;
        }

        

        if (distance > m_detectionRange * 2.0f) {
            m_currentBehavior = ORC_PATROL;
        }
        break;
    }

    default:
        m_currentBehavior = ORC_IDLE;
        m_orcIsMoving = false;
        break;
    }

    UpdateSpriteScales();

}

Hitbox Orc::GetHitbox() const {
    float halfWidth = (100.0f * 5.0f) / 2.0f;
    float halfHeight = (100.0f * 5.0f) / 2.0f;

    return {
        m_orcPosition.x - halfWidth,
        m_orcPosition.y - halfHeight,
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

    float attackWidth = 70.0f;
    float attackHeight = 100.0f * 5.0f;

    // Make Attack2 hitbox slightly larger for more impact
    if (m_attackState == ORC_ATTACK_2) {
        attackWidth = 85.0f; // Wider attack hitbox for Attack2
    }

    float offsetX = (m_orcDirection == 1) ? 40.0f : -attackWidth - 40.0f;

    return {
        m_orcPosition.x + offsetX,
        m_orcPosition.y - (attackHeight / 2.0f),
        attackWidth,
        attackHeight
    };
}

void Orc::UpdateSpriteScales() {
    // Set sprite scale based on direction
    float scaleX = (m_orcDirection > 0) ? 7.5f : -7.5f;
    
    if (m_orcWalk && m_orcWalk->GetScaleX() != scaleX) {
        m_orcWalk->SetScale(scaleX, -7.5f);
    }
    
    if (m_orcIdle && m_orcIdle->GetScaleX() != scaleX) {
        m_orcIdle->SetScale(scaleX, -7.5f);
    }
    
    if (m_orcAttack1 && m_orcAttack1->GetScaleX() != scaleX) {
        m_orcAttack1->SetScale(scaleX, -7.5f);
    }
    
    if (m_orcAttack2 && m_orcAttack2->GetScaleX() != scaleX) {
        m_orcAttack2->SetScale(scaleX, -7.5f);
    }
}

int Orc::GetScore() const {
    int score = 0;

    switch (m_orcType) 
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