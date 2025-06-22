#include "BossDeath.h"
#include "animatedsprite.h"
#include "renderer.h"
#include "logmanager.h"
#include "game.h"
#include <cmath>
#include <cstdlib>
#include <string>

BossDeath::BossDeath()
    : m_deathIdle(nullptr) 
    , m_deathWalk(nullptr)
    , m_deathHurt(nullptr)
    , m_deathDeath(nullptr)
    , m_deathAttack(nullptr)
    , m_deathCast(nullptr)
    , m_deathSpell(nullptr)
    , m_spellTriggered(false) 
    , m_targetPosition(Vector2(0, 0)) 
{
    m_type = DEATH_BOSS; 
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

BossDeath::~BossDeath() {
    delete m_deathIdle;
    m_deathIdle = nullptr;

    delete m_deathWalk;
    m_deathWalk = nullptr;

    delete m_deathHurt;
    m_deathHurt = nullptr;

    delete m_deathDeath;
    m_deathDeath = nullptr;

    delete m_deathAttack;
    m_deathAttack = nullptr;

    delete m_deathCast;
    m_deathCast = nullptr;
}

bool BossDeath::Initialise(Renderer& renderer) {
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    m_flipSprites = {
    m_deathIdle,
    m_deathWalk,
    m_deathAttack,
    m_deathCast,
    m_deathSpell, 
    m_deathHurt,
    m_deathDeath
    };

    //Load audio
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Skeleton-Audio/skeleton_death.wav", FMOD_DEFAULT, 0, &m_deathSound);

    //Load idle sprite
    m_deathIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Idle_1.png");
    if (m_deathIdle) {
        m_deathIdle->SetupFrames(160, 160);
        m_deathIdle->SetFrameDuration(0.2f);
        m_deathIdle->SetLooping(true);
        m_deathIdle->Animate();
        m_deathIdle->SetScale(7.5f, -7.5f);
    }

    //Load walking sprite
    m_deathWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Walk_1.png");
    if (m_deathWalk) {
        m_deathWalk->SetupFrames(160, 160);
        m_deathWalk->SetFrameDuration(0.1f);
        m_deathWalk->SetLooping(true);
        m_deathWalk->Animate();
        m_deathWalk->SetScale(7.5f, -7.5f);
    }

    //Load hurt sprite
    m_deathHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Hurt_1.png");
    if (m_deathHurt) {
        m_deathHurt->SetupFrames(160, 160);
        m_deathHurt->SetFrameDuration(0.1f);
        m_deathHurt->SetLooping(false);
        m_deathHurt->Animate();
        m_deathHurt->SetScale(7.5f, -7.5f);
    }

    //Load death sprite
    m_deathDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Death_1.png");
    if (m_deathDeath) {
        m_deathDeath->SetupFrames(160, 160);
        m_deathDeath->SetFrameDuration(0.1f);
        m_deathDeath->SetLooping(false);
        m_deathDeath->Animate();
        m_deathDeath->SetScale(7.5f, -7.5f);
    }

    //Load attack1 sprite
    m_deathAttack = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Attack_1.png");
    if (m_deathAttack) {
        m_deathAttack->SetupFrames(160, 160);
        m_deathAttack->SetFrameDuration(0.1f);
        m_deathAttack->SetLooping(false);
        m_deathAttack->SetScale(7.5f, -7.5f);
    }

    //Load attack2 sprite
    m_deathCast = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Cast_1.png");
    if (m_deathCast) {
        m_deathCast->SetupFrames(160, 160);
        m_deathCast->SetFrameDuration(0.1f);
        m_deathCast->SetLooping(false);
        m_deathCast->SetScale(7.5f, -7.5f);
    }

    m_deathSpell = renderer.CreateAnimatedSprite("../game/assets/Sprites/Boss/Bringer-Of-Death/Bringer-of-Death_Spell_1.png");
    if (m_deathSpell) {
        m_deathSpell->SetupFrames(160, 160);
        m_deathSpell->SetFrameDuration(0.1f);
        m_deathSpell->SetLooping(false);
        m_deathSpell->SetScale(7.5f, -7.5f);
    }

    if (!m_deathIdle || !m_deathWalk || !m_deathHurt || !m_deathDeath || !m_deathAttack || !m_deathCast || !m_deathSpell)
    {
        LogManager::GetInstance().Log("Failed to load Demon sprites!");
        return false;
    }

    // Set initial direction scale for all sprites
    UpdateSpriteScales();

    return true;
}

// Need to alter some of the frame values, I think some of the file frames widths are not the same btw
void BossDeath::Process(float deltaTime) {
    // Process death animation
    if (!m_isAlive) {
        if (m_deathDeath) { 
            m_deathDeath->Process(deltaTime); 

            // Stop at last frame of death sprtie
            if (m_deathDeath->GetCurrentFrame() <= 3) { 
                m_deathDeath->StopAnimating(); 
                m_deathDeath->SetCurrentFrame(3); 
            }
        }
        return;
    }

    // Process hurt animation
    if (m_isHurt) {
        if (m_isHurt) {
            m_deathHurt->Process(deltaTime);

            //Check animation state if the hurt sprite exists
            if (!m_deathHurt->IsAnimating() || m_deathHurt->GetCurrentFrame() >= 3) {
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
        float timeoutDuration = 2.0f;

        switch (m_attackState)
        {
        case ATTACK_1:
            activeAttack = m_deathAttack;
            timeoutDuration = 2.0f;
            break;

        case ATTACK_2:
            activeAttack = m_deathCast;
            timeoutDuration = 2.0f;

            // Trigger the spell effect ONCE per cast
            if (!m_spellTriggered && m_deathSpell) { 
                m_deathSpell->SetX(m_targetPosition.x); 
                m_deathSpell->SetY(m_targetPosition.y - 60); // vertical offset 
                m_deathSpell->Restart();
                m_deathSpell->Animate();
                m_spellTriggered = true;
            }

            // Update spell animation
            if (m_deathSpell && m_deathSpell->IsAnimating()) {
                m_deathSpell->Process(deltaTime);
            }

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
                m_spellTriggered = false; // reset for next cast

                if (m_behavior == AGGRESSIVE) {
                    m_isMoving = true;
                }
            }
        }
        else {
            m_isAttacking = false;
            m_attackState = ATTACK_NONE;
            m_attackDuration = 0.0f;
        }
    }


    //Process the movements when not attacking
    if (!m_isAttacking) {
        if (m_isMoving && m_deathWalk) {
            if (!m_deathWalk->IsAnimating()) {
                m_deathWalk->Animate();
            }
            m_deathWalk->Process(deltaTime);
        }
        else if (m_deathIdle) {
            if (!m_deathIdle->IsAnimating()) {
                m_deathIdle->Animate();
            }
            m_deathIdle->Process(deltaTime);
        }

        // Only update scales if we have sprites
        if (m_deathIdle || m_deathWalk || m_deathAttack || m_deathCast) {
            UpdateSpriteScales();
        }
    }
}

void BossDeath::Draw(Renderer& renderer, float scrollX) {
    // Calculate screen position
    float drawX = m_position.x - scrollX;
    float drawY = m_position.y;

    // Draw the death animation
    if (!m_isAlive) {
        if (m_deathDeath) {
            m_deathDeath->SetX(drawX);
            m_deathDeath->SetY(drawY);
            m_deathDeath->Draw(renderer);
        }
        return;
    }

    // Draw the hurt animation
    if (m_isHurt) {
        if (m_deathHurt) {
            m_deathHurt->SetX(drawX);
            m_deathHurt->SetY(drawY);
            m_deathHurt->Draw(renderer);
        }
        else {
            m_isHurt = false; // Reset hurt state to avoid getting stuck
        }
        return;
    }

    // Draw the attack animations
    if (m_isAttacking) {
        bool drewAttack = false;

        if (m_attackState == ATTACK_1 && m_deathAttack) {
            m_deathAttack->SetX(drawX);
            m_deathAttack->SetY(drawY);
            m_deathAttack->Draw(renderer);
            drewAttack = true;
        }
        else if (m_attackState == ATTACK_2 && m_deathCast) {
            m_deathCast->SetX(drawX);
            m_deathCast->SetY(drawY);
            m_deathCast->Draw(renderer);
            drewAttack = true;
        }

        if (drewAttack) {
            return;
        }
    }

    // Draw walking or idle animation
    if (m_isMoving && m_deathWalk) {
        m_deathWalk->SetX(drawX);
        m_deathWalk->SetY(drawY);
        m_deathWalk->Draw(renderer);
    }
    else if (m_deathIdle) {
        m_deathIdle->SetX(drawX);
        m_deathIdle->SetY(drawY);
        m_deathIdle->Draw(renderer);
    }

    if (m_deathSpell && m_deathSpell->IsAnimating()) {
        m_deathSpell->Draw(renderer);
    }
}

void BossDeath::SetPosition(float x, float y) {
    m_position.Set(x, y);
}

Vector2 BossDeath::GetPosition() const {
    return m_position;
}

void BossDeath::SetBehavior(EnemyBehavior behavior) {
    m_behavior = behavior;
}

void BossDeath::SetPatrolRange(float left, float right) {
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

void BossDeath::UpdateAI(const Vector2& playerPos, float deltaTime) {
    if (!m_isAlive) return;

    float distance = fabs(playerPos.x - m_position.x);
    bool playerInAttackRange = distance <= m_attackRange;

    //Define thebehaviors of the orc
    switch (m_behavior)
    {
        //Orc in idle
    case IDLE:
        m_isMoving = false;

        if (distance < m_detectionRange) {
            m_behavior = AGGRESSIVE;
        }
        break;

        //Orc in patrol
    case PATROL:
    {
        m_isMoving = true;

        float moveAmount = m_speed * deltaTime * 60.0f;
        m_position.x += m_direction * moveAmount;

        if (m_position.x <= m_patrolRangeLeft) {
            m_position.x = m_patrolRangeLeft;
            m_direction = 1;
        }
        else if (m_position.x >= m_patrolRangeRight) {
            m_position.x = m_patrolRangeRight;
            m_direction = -1;
        }

        if (distance < m_detectionRange) {
            m_behavior = AGGRESSIVE;
        }
        break;
    }

    //Orc is aggressive
    case AGGRESSIVE:
    {
        //face the player
        m_direction = (playerPos.x < m_position.x) ? -1 : 1;

        //distance from player to attack
        float attackDist = 80.0f;

        if (playerInAttackRange && m_currentAttackCooldown <= 0.0f) {
            m_attackState = (rand() % 2 == 0) ? ATTACK_1 : ATTACK_2;

            if (m_type == DEATH_BOSS && m_attackState == ATTACK_2) {
                m_targetPosition = playerPos;
            }

            if (m_attackSound) {
                FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
                FMOD::Channel* m_attackChannel = nullptr;
                fmod->playSound(m_attackSound, nullptr, false, &m_attackChannel);
                if (m_attackChannel) {
                    m_attackChannel->setVolume(m_sfxVolume);
                }
            }

            m_isAttacking = true;
            m_attackDuration = 0.0f;
            m_currentAttackCooldown = m_attackCooldown;
            m_isMoving = false;
        }

        else if (!m_isAttacking && distance > attackDist) {
            float moveAmount = m_speed * deltaTime * 60.0f;
            float newX = m_position.x + m_direction * moveAmount;

            m_position.x = newX;
            m_isMoving = true;

        }
        else {
            m_isMoving = false;
        }



        if (distance > m_detectionRange * 2.0f) {
            m_behavior = PATROL;
        }
        break;
    }

    default:
        m_behavior = IDLE;
        m_isMoving = false;
        break;
    }

    UpdateSpriteScales();

}

bool BossDeath::IsAlive() const {
    return m_isAlive;
}

void BossDeath::TakeDamage(int amount) {
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

        if (m_deathDeath) {
            m_deathDeath->Restart();
            m_deathDeath->SetCurrentFrame(0);
            m_deathDeath->Animate();
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

        if (m_deathHurt) {
            m_deathHurt->SetCurrentFrame(0);
            m_deathHurt->Restart();
            m_deathHurt->Animate();
        }
    }
}

EnemyAttackType BossDeath::GetAttackState() const {
    return m_attackState;
}

bool BossDeath::IsAttacking() const {
    return m_isAttacking;
}

Hitbox BossDeath::GetHitbox() const {
    float halfWidth = (100.0f * 5.0f) / 2.0f;
    float halfHeight = (100.0f * 5.0f) / 2.0f;

    return {
        m_position.x - halfWidth,
        m_position.y - halfHeight,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };
}

Hitbox BossDeath::GetAttackHitbox() const {
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

int BossDeath::GetScore() const {
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

bool BossDeath::WasScored() const {
    return m_wasScored;
}

void BossDeath::MarkScored() {
    m_wasScored = true;
}

void  BossDeath::UpdateSpriteScales() {
    float scaleX = (m_direction > 0) ? 7.5f : -7.5f;

    if (m_deathIdle && m_deathIdle->GetScaleX() != scaleX) {
        m_deathIdle->SetScale(scaleX, -7.5f);
    }
    if (m_deathWalk && m_deathWalk->GetScaleX() != scaleX) {
        m_deathWalk->SetScale(scaleX, -7.5f);
    }
    if (m_deathAttack && m_deathAttack->GetScaleX() != scaleX) {
        m_deathAttack->SetScale(scaleX, -7.5f);
    }
    if (m_deathCast && m_deathCast->GetScaleX() != scaleX) {
        m_deathCast->SetScale(scaleX, -7.5f);
    }
    if (m_deathHurt && m_deathHurt->GetScaleX() != scaleX) {
        m_deathHurt->SetScale(scaleX, -7.5f);
    }
    if (m_deathDeath && m_deathDeath->GetScaleX() != scaleX) {
        m_deathDeath->SetScale(scaleX, -7.5f);
    }
}
