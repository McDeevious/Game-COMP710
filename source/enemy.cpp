#include "enemy.h"
#include "animatedsprite.h"
#include "game.h"

Enemy::Enemy()
    : m_position(0.0f, 0.0f)
    , m_speed(0.0f)
    , m_direction(1)
    , m_isMoving(false)
    , m_isHurt(false)
    , m_isAlive(true)
    , m_health(0)
    , m_wasScored(false)
    , m_attackState(ATTACK_NONE)
    , m_isAttacking(false)
    , m_attackDuration(0.0f)
    , m_type(ENEMY_NONE)
    , m_behavior(IDLE)
    , m_patrolRangeLeft(0.0f)
    , m_patrolRangeRight(0.0f)
    , m_detectionRange(0.0f)
    , m_attackRange(0.0f)
    , m_attackCooldown(0.0f)
    , m_currentAttackCooldown(0.0f)
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_sfxVolume(0.4f)
{
}

Enemy::~Enemy() {}

void Enemy::SetPosition(float x, float y) {
    m_position.Set(x, y);
}

Vector2 Enemy::GetPosition() const {
    return m_position;
}

void Enemy::SetBehavior(EnemyBehavior behavior) {
    m_behavior = behavior;
}

void Enemy::SetPatrolRange(float left, float right) {
    m_patrolRangeLeft = left;
    m_patrolRangeRight = right;
}

void Enemy::UpdateAI(const Vector2& playerPos, float deltaTime) {
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
            if (m_attackSound) {
                FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
                FMOD::Channel* m_attackChannel = nullptr;

                fmod->playSound(m_attackSound, nullptr, false, &m_attackChannel);
                if (m_attackChannel) {
                    m_attackChannel->setVolume(m_sfxVolume);
                }
            }

            m_isAttacking = true;
            m_attackState = (rand() % 2 == 0) ? ATTACK_1 : ATTACK_2;
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

void Enemy::TakeDamage(int amount) {
    // Default: do nothing
}

bool Enemy::IsAlive() const {
    return m_isAlive;
}

bool Enemy::IsAttacking() const {
    return m_isAttacking;
}

EnemyAttackType Enemy::GetAttackState() const {
    return m_attackState;
}

int Enemy::GetScore() const {
    return 0; // Default
}

bool Enemy::WasScored() const {
    return m_wasScored;
}

void Enemy::MarkScored() {
    m_wasScored = true;
}

void Enemy::UpdateSpriteScales() {
    float scaleX = (m_direction > 0) ? 7.5f : -7.5f;

    for (AnimatedSprite* sprite : m_flipSprites) {
        if (sprite && sprite->GetScaleX() != scaleX) {
            sprite->SetScale(scaleX, -7.5f);
        }
}
