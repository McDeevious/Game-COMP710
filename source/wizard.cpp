#include "wizard.h"
#include "projectile.h"
#include "orc.h" 
#include "animatedsprite.h"
#include "renderer.h"
#include "collision.h" 
#include "inputsystem.h"
#include "xboxcontroller.h"
#include "logmanager.h"
#include "game.h"
#include <cstdio>
#include <string>  
#include "sharedenums.h"
#include <vector>
#include <iostream>

Wizard::Wizard()
    : m_wizardIdle(nullptr)
    , m_wizardWalk(nullptr)
    , m_wizardHurt(nullptr)
    , m_wizardDeath(nullptr)
    , m_wizardSpeed(0.5f)
    , m_wizardLeft(false)
    , m_isMoving(false)
    , m_isHurt(false)
    , m_isDead(false)
    , m_wizardhealth(125)
    //boundaries
    , m_leftBoundary(0.0f)
    , m_rightBoundary(1024.0f)
    , m_topBoundary(0.0f)
    , m_bottomBoundary(768.0f)
    //jumping 
    , m_isJumping(false)
    , m_jumpVelocity(0.0f)
    , m_gravity(981.0f)
    , m_jumpStrength(-650.0f / 10*9.2)
    , m_groundY(0)
    //atacking
    , m_wizardAttack1(0)
    , m_wizardSpecial(0)
    , m_isAttacking(false)
    , m_attackState(ATTACK_NONE)
    , m_attackDuration(0.0f)
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_jumpSound(nullptr)
    , m_sfxVolume(0.4f)
    , m_iActiveFire(0)
    
{
    character_size = 5;
    m_wizardPosition.Set(100, 618);
    m_lastMovementDirection.Set(0.0f, 0.0f);
    
}

Wizard::~Wizard() {
    //Clean up normal animations
    delete m_wizardWalk;
    m_wizardWalk = nullptr;

    delete m_wizardIdle;
    m_wizardIdle = nullptr;

    delete m_wizardHurt;
    m_wizardHurt = nullptr;

    delete m_wizardDeath;
    m_wizardDeath = nullptr;

    // Clean up attack animations
    delete m_wizardAttack1;
    m_wizardAttack1 = nullptr;

    delete m_wizardSpecial;
    m_wizardSpecial = nullptr;

    for (Projectile* arrow : m_pFire)
    {
        delete arrow;
    }

    //Clean upi the audio
    if (m_attackSound) {
        m_attackSound->release();
        m_attackSound = nullptr;
    }

    if (m_hurtSound) {
        m_hurtSound->release();
        m_hurtSound = nullptr;
    }

    if (m_deathSound) {
        m_deathSound->release();
        m_deathSound = nullptr;
    }

    if (m_jumpSound) {
        m_jumpSound->release();
        m_jumpSound = nullptr;
    }

}

bool Wizard::Initialise(Renderer& renderer)
{
    m_groundY = renderer.GetHeight() * 0.8;
    m_wizardPosition.y = m_groundY;
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_death.wav", FMOD_DEFAULT, 0, &m_deathSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_jump.wav", FMOD_DEFAULT, 0, &m_jumpSound);

    //Load knight's idle sprite
    m_wizardIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Wizard with shadows/Wizard-Idle.png");
    if (m_wizardIdle)
    {
        m_wizardIdle->SetupFrames(100, 100);
        m_wizardIdle->SetFrameDuration(0.1f);
        m_wizardIdle->SetLooping(true);
        m_wizardIdle->SetX(m_wizardPosition.x);
        m_wizardIdle->SetY(m_wizardPosition.y);
        m_wizardIdle->SetScale(character_size, -character_size);
        m_wizardIdle->Animate();
    }

    //Load knight's walking sprite
    m_wizardWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Wizard with shadows/Wizard-Walk.png");
    if (m_wizardWalk)
    {
        m_wizardWalk->SetupFrames(100, 100);
        m_wizardWalk->SetFrameDuration(0.1f);
        m_wizardWalk->SetLooping(true);
        m_wizardWalk->SetX(m_wizardPosition.x);
        m_wizardWalk->SetY(m_wizardPosition.y);
        m_wizardWalk->SetScale(character_size, -character_size);
        m_wizardWalk->Animate();
        SetBoundaries(0, renderer.GetWidth(), 0, renderer.GetHeight());
    }

    //Load knight's hurt sprite
    m_wizardHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Wizard with shadows/Wizard-Hurt.png");
    if (m_wizardHurt)
    {
        m_wizardHurt->SetupFrames(100, 100);
        m_wizardHurt->SetFrameDuration(0.12f);
        m_wizardHurt->SetLooping(false);
        m_wizardHurt->SetX(m_wizardPosition.x);
        m_wizardHurt->SetY(m_wizardPosition.y);
        m_wizardHurt->SetScale(character_size, -character_size);
    }

    //Load knight's death sprite
    m_wizardDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Wizard with shadows/Wizard-DEATH.png");
    if (m_wizardDeath)
    {
        m_wizardDeath->SetupFrames(100, 100);
        m_wizardDeath->SetFrameDuration(0.15f);
        m_wizardDeath->SetLooping(false);
        m_wizardDeath->SetX(m_wizardPosition.x);
        m_wizardDeath->SetY(m_wizardPosition.y);
        m_wizardDeath->SetScale(character_size, -character_size);
    }

    //Load Attack 1
    m_wizardAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Wizard with shadows/Wizard-Attack02.png");
    if (m_wizardAttack1)
    {
        m_wizardAttack1->SetupFrames(100, 100);
        m_wizardAttack1->SetFrameDuration(0.1f);
        m_wizardAttack1->SetLooping(false);
        m_wizardAttack1->SetX(m_wizardPosition.x);
        m_wizardAttack1->SetY(m_wizardPosition.y);
        m_wizardAttack1->SetScale(character_size, -character_size);
    }

    //Load Special Attack
    m_wizardSpecial = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Wizard with shadows/Wizard-Attack01.png");
    if (m_wizardSpecial)
    {
        m_wizardSpecial->SetupFrames(100, 100);
        m_wizardSpecial->SetFrameDuration(0.1f);
        m_wizardSpecial->SetLooping(false);
        m_wizardSpecial->SetX(m_wizardPosition.x);
        m_wizardSpecial->SetY(m_wizardPosition.y);
        m_wizardSpecial->SetScale(character_size, -character_size);
    }

    for (int i = 0; i < 5; i++)
    {
        Projectile* thisProjectile = new Projectile();
        thisProjectile->SetProjectileType(ProjectileType::FIRE);
        thisProjectile->Initialise(renderer);
        thisProjectile->SetPosition(m_wizardPosition + Vector2(60.0f, 0.0f));
        m_pFire.push_back(thisProjectile);
    }

    if (!m_wizardIdle || !m_wizardWalk || !m_wizardHurt || !m_wizardDeath || !m_wizardAttack1 || !m_wizardSpecial)
    {
        LogManager::GetInstance().Log("Failed to load Knight sprites!");
        return false;
    }

    return true;
}

void Wizard::Process(float deltaTime, SceneGame& game) {
    //  wasTouchingGround = 0;
      //Process hurt animation
    if (m_isHurt)
    {
        if (m_wizardHurt) {
            m_wizardHurt->Process(deltaTime);
            m_wizardHurt->SetX(m_wizardPosition.x);
            m_wizardHurt->SetY(m_wizardPosition.y);
        }


        if (!m_wizardHurt->IsAnimating() || m_wizardHurt->GetCurrentFrame() >= 3)
        {
            m_isHurt = false;
        }
        return;
    }

    //Process death animation
    if (m_isDead)
    {
        if (m_wizardDeath) {

            m_wizardDeath->Process(deltaTime);
            m_wizardDeath->SetX(m_wizardPosition.x);
            m_wizardDeath->SetY(m_wizardPosition.y);

            //freeze last death frame
            if (m_wizardDeath->GetCurrentFrame() >= 3) {
                m_wizardDeath->StopAnimating();
                m_wizardDeath->SetCurrentFrame(3);
            }
        }

        return;
    }

    // Process attack animations
    if (m_isAttacking)
    {
        m_attackDuration += deltaTime;

        AnimatedSprite* activeAttack = nullptr;
        float timeoutDuration = 1.2f;

        switch (m_attackState)
        {
        case ATTACK_1:
            activeAttack = m_wizardAttack1;
            timeoutDuration = 1.2f;
            break;
        case SP_ATTACK:
            activeAttack = m_wizardSpecial;
            timeoutDuration = 1.2f;
            break;
        default:
            m_isAttacking = false;
            break;
        }

        if (activeAttack)
        {
            if (m_attackState != BLOCK) {
                activeAttack->Process(deltaTime);
            }

            activeAttack->SetX(m_wizardPosition.x);
            activeAttack->SetY(m_wizardPosition.y);

            float scaleX = (m_wizardWalk) ? m_wizardWalk->GetScaleX() : character_size;
            float direction = (scaleX < 0) ? -character_size : character_size;
            activeAttack->SetScale(direction, -character_size);

            if ((!activeAttack->IsAnimating() && m_attackState != BLOCK) || m_attackDuration > timeoutDuration)
            {
                if (m_iActiveFire < 5)
                {
                    for (Projectile* arrow : m_pFire)
                    {
                        if (!arrow->m_bActive)
                        {
                            arrow->m_bActive = true;
                            arrow->SetPosition(m_wizardPosition + Vector2(60.0f, 0.0f));
                            m_iActiveFire++;
                            break;
                        }
                    }
                }
                m_isAttacking = false;
                m_attackState = ATTACK_NONE;
                m_attackDuration = 0.0f;
            }
        }
        else
        {
            m_isAttacking = false;
            m_attackState = ATTACK_NONE;
            m_attackDuration = 0.0f;
        }
    }

    // Process jump physics

     // Process jump physics
     wasTouchingGround = 0;
 wasTouchingRoof = 0;
 if (m_jumpVelocity > 0)
 {
     //is going down
     m_jumpVelocity += m_gravity * deltaTime;
     m_wizardPosition.y += m_jumpVelocity * deltaTime;
     setBounds(game);
    if ( collision(1,game))
   //  if (wasTouchingGround)
     {
         //   m_jumpVelocity += m_gravity * deltaTime;
         m_wizardPosition.y -= m_jumpVelocity * deltaTime;
         m_isJumping = false;
         m_jumpVelocity = 0;
         if (m_isMoving && m_wizardWalk) {
             m_wizardWalk->Animate();
         }
         else if (m_wizardIdle) {
             m_wizardIdle->Animate();
         }
     }
 }
 else if (m_jumpVelocity < 0)
 {

     m_jumpVelocity += m_gravity * deltaTime;
     m_wizardPosition.y += m_jumpVelocity * deltaTime;
     setBounds(game);
     if (collision(2, game))
    // if (wasTouchingRoof)
     {
         //  m_jumpVelocity += m_gravity * deltaTime;
         m_wizardPosition.y -= m_jumpVelocity * deltaTime;

     }
     //going up
 }
 else
 {
     //idle
     m_wizardPosition.y += m_gravity * deltaTime;
     setBounds(game);
     if (collision(1, game))
    // if (wasTouchingGround)
     {
         m_wizardPosition.y -= m_gravity * deltaTime;
     }
 }
 std::cout << m_wizardPosition.x << " y = " << m_wizardPosition.y << std::endl;
    // Process Arrows
    for (int i = 0; i < m_pFire.size(); i++)
    {
        if (m_pFire[i]->m_bActive)
        {
            m_pFire[i]->Process(deltaTime);
            if (m_pFire[i]->m_bJustDied)
            {
                m_iActiveFire--;
            }
        }
    }

    // Process knight sprite animations
    if (!m_isAttacking) {

        if (m_isJumping) {

            if (m_wizardWalk) {
                m_wizardWalk->StopAnimating();
                m_wizardWalk->SetX(m_wizardPosition.x);
                m_wizardWalk->SetY(m_wizardPosition.y);

                float direction = m_wizardLeft ? -character_size : character_size;
                m_wizardWalk->SetScale(direction, -character_size);
            }
        }
        //Handle normal movement animations
        else if (m_isMoving) {
            //Use walking animation when moving
            if (m_wizardWalk && !m_wizardWalk->IsAnimating()) {
                m_wizardWalk->Animate();
            }

            //Process walking animation
            if (m_wizardWalk) {
                m_wizardWalk->Process(deltaTime);
                m_wizardWalk->SetX(m_wizardPosition.x);
                m_wizardWalk->SetY(m_wizardPosition.y);

                float direction = m_wizardLeft ? -character_size : character_size;
                m_wizardWalk->SetScale(direction, -character_size);
            }
        }
        else {
            // Use idle animation when not moving
            if (m_wizardIdle && !m_wizardIdle->IsAnimating()) {
                m_wizardIdle->Animate();
            }

            //Process idle animation
            if (m_wizardIdle) {
                m_wizardIdle->Process(deltaTime);
                m_wizardIdle->SetX(m_wizardPosition.x);
                m_wizardIdle->SetY(m_wizardPosition.y);

                float direction = m_wizardLeft ? -character_size : character_size;
                m_wizardIdle->SetScale(direction, -character_size);
            }

        }
    }
}

void Wizard::Draw(Renderer& renderer) {
    // Hurt override
    if (m_isHurt && m_wizardHurt) {
        if (!m_wizardHurt->IsAnimating()) {
            m_wizardHurt->Restart();
            m_wizardHurt->Animate();
        }
        m_wizardHurt->Draw(renderer);
        return;
    }

    // Death override
    if (m_isDead && m_wizardDeath) {
        m_wizardDeath->Draw(renderer);
        return;
    }

    // Draw Arrows
    for (int i = 0; i < m_pFire.size(); i++)
    {
        if (m_pFire[i]->m_bActive)
        {
            m_pFire[i]->Draw(renderer);
        }
    }

    // Draw either the knight animation or the attack animation
    if (!m_isAttacking) {

        if (m_isJumping) {
            if (m_wizardWalk) {
                m_wizardWalk->Draw(renderer);
            }
        }
        else if (m_isMoving) {
            // Use walking animation when moving
            if (m_wizardWalk) {
                m_wizardWalk->Draw(renderer);
            }
        }
        else {
            // Use idle animation when not moving
            if (m_wizardIdle) {
                m_wizardIdle->Draw(renderer);
            }
        }
    }
    else {
        // Draw appropriate attack animation 
        switch (m_attackState) {
        case ATTACK_1:
            if (m_wizardAttack1) {
                m_wizardAttack1->Draw(renderer);
            }
            break;
        case SP_ATTACK:
            if (m_wizardSpecial) {
                m_wizardSpecial->Draw(renderer);
            }
            break;
        default:
            // Fall back to appropriate idle/walk animation
            if (m_isJumping) {
                if (m_wizardWalk) m_wizardWalk->Draw(renderer);
            }
            else if (!m_isMoving && m_wizardIdle) {
                m_wizardIdle->Draw(renderer);
            }
            else if (m_wizardWalk) {
                m_wizardWalk->Draw(renderer);
            }
            break;
        }
    }
}

void Wizard::ProcessInput(InputSystem& inputSystem, SceneGame& game) {
    wasTouchingGround = 0;
    wasTouchingRoof = 0;
    bool isWalking = false;
    // m_wizardPosition.x = 150.0f;
    Vector2 direction;
    Vector2 direction2;
    direction.Set(0, 0);
    arenaXY.Set(0, 0);

    // Get controller if connected
    XboxController* controller = inputSystem.GetController(0);
    Vector2 stick = controller ? controller->GetLeftStick() : Vector2(0, 0);
    float threshold = 8000.0f;

    // Track the previous horizontal direction for background scrolling
    float previousDirectionX = m_lastMovementDirection.x;

    // Only allow jumping when on the ground and not attacking
    if ((inputSystem.GetKeyState(SDL_SCANCODE_SPACE) == BS_HELD || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_A) == BS_PRESSED)) && !m_isJumping) {

        if (m_jumpSound) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            FMOD::Channel* m_jumpChannel = nullptr;
            fmod->playSound(m_jumpSound, nullptr, false, &m_jumpChannel);
            if (m_jumpChannel) {
                m_jumpChannel->setVolume(m_sfxVolume);
            }
        }
        m_isJumping = true;
        //  if (wasTouchingRoof)
        //  {
        m_jumpVelocity = m_jumpStrength;
        //  }
    }

    if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || (controller && (stick.x < -threshold || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_HELD))) {
        if (m_wizardPosition.x < 200)
        {
            direction.x = -1.0f;
            arenaXY.x = -2.5f;
            if (collision(3,game))
            {
                arenaXY.x = 2.5f;

            }
        }
        else {
            m_wizardPosition.x -= 2.5f;
            if (collision(3,game))
            {
                m_wizardPosition.x += 2.5f;
            }
        }
        //   direction.x = -1.0f; 
        m_wizardLeft = true; // Left 
        isWalking = true;
    }

    else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || (controller && (stick.x > threshold || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_HELD))) {
        // direction.x = 1.0f; 
        if (m_wizardPosition.x > 1000)
        {
            direction.x = 1.0f;
            arenaXY.x = 2.5f;
            if (collision(5,game))
            {
                arenaXY.x = -2.5f;
            }
        }
        else {
            m_wizardPosition.x += 2.5;
            if (collision(4,game))
            {
                m_wizardPosition.x -= 2.5f;
            }
        }
        m_wizardLeft = false; // Right 
        isWalking = true;
    }
    else
    {
        m_wizardLeft = 0;
      //  direction.x = 0;
        arenaXY.x = 0;
        isWalking = 0;
    }

    // Process attacking inputs
    if (!m_isAttacking && !m_isHurt && !m_isDead) {
        // LCTRL for basic attack
        if (inputSystem.GetKeyState(SDL_SCANCODE_LCTRL) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_X) == BS_PRESSED)) {
            StartAttack(ATTACK_1);
        }
        // V for special attack
        else if (inputSystem.GetKeyState(SDL_SCANCODE_Q) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_Y) == BS_PRESSED)) {
            StartAttack(SP_ATTACK);
        }
    }

    // Check if we need to end blocking when button is released
    if (m_isAttacking && m_attackState == BLOCK && inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_RELEASED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_B) == BS_RELEASED)) {
        m_isAttacking = false;
        m_attackState = ATTACK_NONE;
        m_attackDuration = 0.0f;
    }

    // Store the movement direction for background scrolling
    setAreanapos = -arenaXY.x;
    m_lastMovementDirection = direction;
    // Update player horizontal position
    if (isWalking) {
        // Vector2 movement = direction * m_knightSpeed * 0.016f; 
       ///  m_knightPosition.x += movement.x; 

        ClampPositionToBoundaries();
        m_isMoving = true;
    }
    else {
        m_isMoving = false;
    }
    collision(3,game);
}

void Wizard::StartAttack(AttackType attackType) {
    m_attackState = attackType;
    m_attackDuration = 0.0f;

    AnimatedSprite* attackSprite = nullptr;
    float frameDuration = 0.2f; // Default frame duration

    switch (attackType) {
    case ATTACK_1:
        attackSprite = m_wizardAttack1;
        break;
    case SP_ATTACK:
        attackSprite = m_wizardSpecial;
        break;
    default:
        m_isAttacking = false;
        return;
    }

    if (attackSprite) {

        m_isAttacking = true;

        attackSprite->SetFrameDuration(frameDuration);
        attackSprite->SetLooping(false);
        attackSprite->Restart();
        attackSprite->Animate();
    }
    else {
        m_isAttacking = false;
        m_attackState = ATTACK_NONE;
    }

    if (m_attackState != BLOCK && m_attackSound) {
        FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
        FMOD::Channel* m_attackChannel = nullptr;
        fmod->playSound(m_attackSound, nullptr, false, &m_attackChannel);
        if (m_attackChannel) {
            m_attackChannel->setVolume(m_sfxVolume);
        }
    }

}

int Wizard::AttackDamage() const {
    switch (m_attackState) {
    case ATTACK_1:
        return 1000;
        break;
    case SP_ATTACK:
        return 2500;
        break;
    case BLOCK:
        return 0;
        break;
    default:
        return 0;
    }
}

bool Wizard::isBlocking() const {
    bool blocking = m_attackState == BLOCK && m_isAttacking;

    return blocking;
}

bool Wizard::isAttacking() const {

    return m_isAttacking && m_attackState != ATTACK_NONE;
}

bool Wizard::isProjectilesActive() const {
    // Loop through projects and return true if any active
    for (Projectile* proj : m_pFire)
    {
        if (proj->m_bActive)
        {
            return true;
        }
    }
    return false;
}

void Wizard::SetBoundaries(float left, float right, float top, float bottom)
{
    left = 0;
    top = 0;
    bottom = 2999;
    right = 4000;
    // Calculate effective sprite size for boundary padding
    float effectiveWidth = 100.0f * 2.0f * 0.5f;
    float effectiveHeight = 100.0f * 2.0f * 0.5f;

    // Set left boundary to knight's initial X position 
  //  m_leftBoundary = 100.0f;

    // Set other boundaries with appropriate padding
    m_leftBoundary = left - effectiveWidth;
    m_rightBoundary = right - effectiveWidth;
    m_topBoundary = top + effectiveHeight;
    m_bottomBoundary = bottom - effectiveHeight;

}

void Wizard::ClampPositionToBoundaries()
{
    // Clamp X position
    if (m_wizardPosition.x < m_leftBoundary)
    {
        m_wizardPosition.x = m_leftBoundary;
    }
    else if (m_wizardPosition.x > m_rightBoundary)
    {
        m_wizardPosition.x = m_rightBoundary;
    }

    // Clamp Y position
    if (m_wizardPosition.y < m_topBoundary)
    {
        m_wizardPosition.y = m_topBoundary;
    }
    else if (m_wizardPosition.y > m_bottomBoundary)
    {
        m_wizardPosition.y = m_bottomBoundary;
    }
}

const Vector2& Wizard::GetLastMovementDirection() const
{
    return m_lastMovementDirection;
}

int Wizard::GetHealth() const {
    return m_wizardhealth;
}

const Vector2& Wizard::GetPosition() const
{
    return m_wizardPosition;
}

Hitbox Wizard::GetHitbox() const {

    float halfWidth = (8.0f * character_size) / 2.0f;
    float halfHeight = (16.0f * character_size) / 2.0f;

    if (m_isJumping) {
        //  halfHeight *= 0.75f; // 25% smaller in air
    }

    return {
        m_wizardPosition.x - halfWidth,
        m_wizardPosition.y - halfHeight,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };


}

// Need to change it so that it will only calculate it for the closest arrow projectile
Hitbox Wizard::GetAttackHitbox(const Orc& orc) const {
    if (m_pFire.empty())
    {
        return { 0, 0, 0, 0 };
    }

    // Get orc x pos and set closest current distance value to high val
    float orcX = orc.GetPosition().x;
    float closestDist = FLT_MAX;
    Projectile* closestProj = nullptr;

    for (Projectile* proj : m_pFire)
    {
        if (proj && proj->m_bActive)
        {
            // absolute value of difference between orcX and projectile X
            float dist = fabs(proj->GetPosition().x - orcX);
            if (dist < closestDist)
            {
                closestDist = dist;
                closestProj = proj;
            }
        }
    }

    return (closestProj ? closestProj->GetHitbox() : Hitbox{ 0, 0, 0, 0 });
}

void Wizard::TakeDamage(int amount) {
    // Don't process damage if already dead
    if (m_isDead) {
        return;
    }

    // Don't take damage during hurt animation
    if (m_isHurt && m_wizardHurt && m_wizardHurt->IsAnimating()) {
        return;
    }

    m_wizardhealth -= amount;

    // Cancel any current attack
    m_isAttacking = false;
    m_attackState = ATTACK_NONE;
    m_attackDuration = 0.0f;

    if (m_wizardhealth <= 0) {
        // Handle death
        m_wizardhealth = 0;
        m_isDead = true;
        m_isHurt = false;

        if (m_deathSound) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            FMOD::Channel* m_deathChannel = nullptr;
            fmod->playSound(m_deathSound, nullptr, false, &m_deathChannel);

            if (m_deathChannel) {
                m_deathChannel->setVolume(m_sfxVolume);
            }
        }

        // Start death animation
        if (m_wizardDeath) {
            m_wizardDeath->SetCurrentFrame(0);
            m_wizardDeath->Restart();
            m_wizardDeath->Animate();
        }
    }
    else {
        // Handle hurt ainmation
        m_isHurt = true;

        if (m_hurtSound) {
            FMOD::System* fmod = Game::GetInstance().GetFMODSystem();
            FMOD::Channel* m_hurtChannel = nullptr;
            fmod->playSound(m_hurtSound, nullptr, false, &m_hurtChannel);

            if (m_hurtChannel) {
                m_hurtChannel->setVolume(m_sfxVolume);
            }
        }

        // Start hurt animation
        if (m_wizardHurt) {
            m_wizardHurt->SetCurrentFrame(0);
            m_wizardHurt->Restart();
            m_wizardHurt->Animate();
        }
    }
}

bool Wizard::IsDead() const
{
    return m_isDead;
}