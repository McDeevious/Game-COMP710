#include "enemy.h"
#include "animatedsprite.h"
#include "game.h"
#include "SceneGame.h"
#include "collision.h"
#include <iostream>

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
    , m_enemySize(5.0f)
    , m_currentAttackCooldown(0.0f)
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_sfxVolume(0.4f)
    , xoffset(0)
{
}

Enemy::~Enemy() {}

void Enemy::setoffset(float x)
{
    if (xoffset != x)
    {
        m_patrolRangeLeft += (x- xoffset);
        m_patrolRangeRight += (x-xoffset);
        m_position.x += (x-xoffset);
        xoffset = x;
    }
 
  //  float newPos = m_patrolRangeLeft;
  //  m_patrolRangeLeft = temPos.x + x;
  //  m_patrolRangeRight = temPos.y + x;
   // m_position.x += m_patrolRangeLeft - newPos; 

}


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
  //  temPos.x = m_patrolRangeLeft;
   // temPos.y = m_patrolRangeRight;
}
void Enemy::passHitbox(Hitbox temp)
{
    enemyhb = temp;
}

void Enemy::UpdateAI(const Vector2& playerPos, float deltaTime, SceneGame& game) {
    if (!m_isAlive) return;
  //  Hitbox t = GetHitbox();
    m_position.y += (950.0f) * deltaTime;
    //enemyhb.x = m_position.x;
   // enemyhb.y = m_position.y;
    setBounds(game);
    if (collision(1, game))
        // if (wasTouchingGround)
    {
       // exit(0);
        m_position.y -= (950.0f)  * deltaTime;
    }
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
        setBounds(game);
        if (collision(4, game))
        {
           m_position.x -= m_direction * moveAmount;

        }
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
            float newX = + m_direction * moveAmount;
           // float moveAmount = m_speed * deltaTime * 60.0f;
           // m_position.x += m_direction * moveAmount;
            m_position.x += newX;
       
           // m_position.x = newX;
            setBounds(game);
            if (collision(4, game))
            {
                m_position.x -= newX;
           }
            m_isMoving = true;

        }
        else {
            m_isMoving = false;
        }



        if (distance > m_detectionRange * 2.0f) {
          //  float tempbounds1 = m_patrolRangeLeft;
         //   float tempbounds2 = m_patrolRangeRight;
            float dis = m_patrolRangeRight-m_patrolRangeLeft;
            m_patrolRangeLeft = m_position.x -dis / 2;
            m_patrolRangeRight = m_position.x +dis / 2;
            m_behavior = PATROL;
        }
        break;
    }

    default:
        m_behavior = IDLE;
        m_isMoving = false;
        break;
    }
    //idle
  
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

void Enemy::UpdateSpriteScales() 
{
   
}

void Enemy::setBounds(SceneGame& game)
{
   // Hitbox temp = enemyhb;
    Hitbox temp = GetHitbox();
    upperBounds.x = ((temp.width + temp.x + 5 * game.getSize()) - game.getSize() / 2 - game.getOffsetX()) / game.getSize();
    lowerBounds.x = ((temp.x - 5 * game.getSize()) - game.getSize() / 2 - game.getOffsetX()) / game.getSize();
    upperBounds.y = ((temp.height + temp.y + 5 * game.getSize()) - game.getSize() / 2) / game.getSize();
    lowerBounds.y = ((temp.y - 5 * game.getSize()) - game.getSize() / 2) / game.getSize();
    if (upperBounds.x < 10)
    {
        upperBounds.x = 10;
    }
    if (upperBounds.x > 120)
    {
        upperBounds.x = 120;
    }

    if (upperBounds.y < 10)
    {
        upperBounds.y = 10;
    }
    if (upperBounds.y > 40)
    {
        upperBounds.y = 40;
    }

    if (lowerBounds.x < 0)
    {
        lowerBounds.x = 0;
    }
    if (lowerBounds.x > 115)
    {
        lowerBounds.x = 115;
    }

    if (lowerBounds.y < 0)
    {
        lowerBounds.y = 0;
    }
    if (lowerBounds.y > 30)
    {
        lowerBounds.y = 30;
    }
}
bool Enemy::collision(int type, SceneGame& game)
{
   // Hitbox temp = enemyhb;
    Hitbox temp = GetHitbox();
    // Hitbox temp2 = GetHitbox();
    // Hitbox temp;
     //temp.height = temp2.height;
     //temp.width = temp2.width;
    // temp.x = temp2.x;
     //temp.y = temp2.y;
    if (type == 1)
    {
        //floor
        temp.y += temp.height;
        temp.height = temp.height / 10;
        temp.width -= 0.4;
        temp.x += 0.2;

    }
    else if (type == 2)
    {
        //  temp.y -= temp.height;
      //    temp.y += temp.height;

        temp.height = temp.height / 10;
      //  temp.width -= 0.4;
       // temp.x += 0.2;
       
        temp.width = temp.width / 10;

        temp.x += temp.width / 2;
        //roof
    }
    else if (type == 3)
    {
        //around
        temp.height -= 0.4;
        temp.y += 0.2;
    }
    else if (type == 4)
    {

        temp.height -= 0.4;
        temp.y -= 0.2;


        //right
       // temp.width -= 0.4;
      //  temp.x += 0.2;

       // temp.width -= 0.4;
       // temp.x += 0.4;
      //  temp.y += 0.2;
        //temp.height -= 0.4;
     //   temp.y += .4;
       // temp.height -= .8;
      //  temp.x += temp.width / 2;

      //  temp.height = temp.height / 10;
       // temp.width = temp.width / 10;
    }
    else if (type == 5)
    {
        //left
        temp.width -= 0.4;
        temp.x += 0.4;
        temp.y -= -0.2;
        temp.height -= 0.4;
    }// it gets the hitboxs that works it gets the tiles that works but idk it doesnt compare
     float gamesize =  game.getSize();
    for (int y = lowerBounds.y; y < upperBounds.y; y++)
    {
        for (int x = lowerBounds.x; x < upperBounds.x; x++)
        {
            if (areaArray[x][y] != -1)//nah cuase the
            {
                Hitbox arena; 
                arena.height = (gamesize * (y + 1) + gamesize / 2) - (gamesize * y + gamesize / 2);
                arena.width = (gamesize * (x + 1) + gamesize / 2 + game.getOffsetX()) - (gamesize * x + gamesize / 2  +game.getOffsetX());
                arena.x = (gamesize * x + gamesize / 2 + game.getOffsetX());
                arena.y = (gamesize * y + gamesize / 2);
                if (Collision::PointHitbox(temp, arena))
                {
                    return 1;
                }
            }

        }
    }
    return 0;
}


void Enemy::getAreaArray(SceneGame& game)
{
    for (int y = 0; y < 40; y++)
    {
        int* temp = game.tilearray(y);
        for (int x = 0; x < 120; x++)
        {
            areaArray[x][y] = temp[x];
        }
        delete[] temp;
    }
}
int* Enemy::tilearray(int row)
{
    int* array = new int[120];
    for (int i = 0; i < 120; i++)
    {
        if (areaArray[i][row] != NULL)
        {
            array[i] = areaArray[i][row];
        }
        else
        {
            array[i] = -1;
        }
    }
    return array;
}