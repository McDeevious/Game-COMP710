#include "archer.h"
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

Archer::Archer()
    : m_archerIdle(nullptr)
    , m_archerWalk(nullptr)
    , m_archerHurt(nullptr)
    , m_archerDeath(nullptr)
    , m_archerSpeed(0.5f)
    , m_archerLeft(false)
    , m_isMoving(false)
    , m_isHurt(false)
    , m_isDead(false)
    , m_archerhealth(1250)
    //boundaries
    , m_leftBoundary(0.0f)
    , m_rightBoundary(1024.0f)
    , m_topBoundary(0.0f)
    , m_bottomBoundary(768.0f)
    //jumping 
    , m_isJumping(false)
    , m_jumpVelocity(0.0f)
    , m_gravity(981.0f)
    , m_jumpStrength(-650.0f)
    , m_groundY(0)
    //atacking
    , m_archerAttack1(0)
    , m_archerSpecial(0)
    , m_isAttacking(false)
    , m_attackState(CLASS_ATTACK_NONE)
    , m_attackDuration(0.0f)
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_jumpSound(nullptr)
    , m_sfxVolume(0.4f)
    , m_iActiveArrows(0)
{
    m_archerPosition.Set(100, 618);
    m_lastMovementDirection.Set(0.0f, 0.0f);
}

Archer::~Archer() {
    //Clean up normal animations
    delete m_archerWalk;
    m_archerWalk = nullptr;

    delete m_archerIdle;
    m_archerIdle = nullptr;

    delete m_archerHurt;
    m_archerHurt = nullptr;

    delete m_archerDeath;
    m_archerDeath = nullptr;

    // Clean up attack animations
    delete m_archerAttack1;
    m_archerAttack1 = nullptr;

    delete m_archerSpecial;
    m_archerSpecial = nullptr;
    
    for (Projectile* arrow : m_pArrows)
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

bool Archer::Initialise(Renderer& renderer)
{
    m_groundY = renderer.GetHeight() * 0.8;
    m_archerPosition.y = m_groundY;
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_death.wav", FMOD_DEFAULT, 0, &m_deathSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_jump.wav", FMOD_DEFAULT, 0, &m_jumpSound);

    //Load knight's idle sprite
    m_archerIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Archer/Archer with shadows/Archer-Idle.png");
    if (m_archerIdle)
    {
        m_archerIdle->SetupFrames(100, 100);
        m_archerIdle->SetFrameDuration(0.1f);
        m_archerIdle->SetLooping(true);
        m_archerIdle->SetX(m_archerPosition.x);
        m_archerIdle->SetY(m_archerPosition.y);
        m_archerIdle->SetScale(7.5f, -7.5f);
        m_archerIdle->Animate();
    }

    //Load knight's walking sprite
    m_archerWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Archer/Archer with shadows/Archer-Walk.png");
    if (m_archerWalk)
    {
        m_archerWalk->SetupFrames(100, 100);
        m_archerWalk->SetFrameDuration(0.1f);
        m_archerWalk->SetLooping(true);
        m_archerWalk->SetX(m_archerPosition.x);
        m_archerWalk->SetY(m_archerPosition.y);
        m_archerWalk->SetScale(7.5f, -7.5f);
        m_archerWalk->Animate();
        SetBoundaries(0, renderer.GetWidth(), 0, renderer.GetHeight());
    }

    //Load knight's hurt sprite
    m_archerHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Archer/Archer with shadows/Archer-Hurt.png");
    if (m_archerHurt)
    {
        m_archerHurt->SetupFrames(100, 100);
        m_archerHurt->SetFrameDuration(0.12f);
        m_archerHurt->SetLooping(false);
        m_archerHurt->SetX(m_archerPosition.x);
        m_archerHurt->SetY(m_archerPosition.y);
        m_archerHurt->SetScale(7.5f, -7.5f);
    }

    //Load knight's death sprite
    m_archerDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Archer/Archer with shadows/Archer-Death.png");
    if (m_archerDeath)
    {
        m_archerDeath->SetupFrames(100, 100);
        m_archerDeath->SetFrameDuration(0.15f);
        m_archerDeath->SetLooping(false);
        m_archerDeath->SetX(m_archerPosition.x);
        m_archerDeath->SetY(m_archerPosition.y);
        m_archerDeath->SetScale(7.5f, -7.5f);
    }

    //Load Attack 1
    m_archerAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Archer/Archer with shadows/Archer-Attack01.png");
    if (m_archerAttack1)
    {
        m_archerAttack1->SetupFrames(100, 100);
        m_archerAttack1->SetFrameDuration(0.1f);
        m_archerAttack1->SetLooping(false);
        m_archerAttack1->SetX(m_archerPosition.x);
        m_archerAttack1->SetY(m_archerPosition.y);
        m_archerAttack1->SetScale(7.5f, -7.5f);
    }

    //Load Special Attack
    m_archerSpecial = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Archer/Archer with shadows/Archer-Attack02.png");
    if (m_archerSpecial)
    {
        m_archerSpecial->SetupFrames(100, 100);
        m_archerSpecial->SetFrameDuration(0.1f);
        m_archerSpecial->SetLooping(false);
        m_archerSpecial->SetX(m_archerPosition.x);
        m_archerSpecial->SetY(m_archerPosition.y);
        m_archerSpecial->SetScale(7.5f, -7.5f);
    }

    for (int i = 0; i < 5; i++)
    {
        Projectile* thisProjectile = new Projectile();
        thisProjectile->SetProjectileType(ProjectileType::ARROW);
        thisProjectile->Initialise(renderer);
        thisProjectile->SetPosition(m_archerPosition + Vector2(60.0f, 0.0f));
        m_pArrows.push_back(thisProjectile);
    }

    if (!m_archerIdle || !m_archerWalk || !m_archerHurt || !m_archerDeath || !m_archerAttack1 || !m_archerSpecial)
    {
        LogManager::GetInstance().Log("Failed to load Knight sprites!");
        return false;
    }

    return true;
}

void Archer::Process(float deltaTime) {
    
    //Process hurt animation
    if (m_isHurt)
    {
        if (m_archerHurt) {
            m_archerHurt->Process(deltaTime);
            m_archerHurt->SetX(m_archerPosition.x);
            m_archerHurt->SetY(m_archerPosition.y);
        }


        if (!m_archerHurt->IsAnimating() || m_archerHurt->GetCurrentFrame() >= 3)
        {
            m_isHurt = false;
        }
        return;
    }

    //Process death animation
    if (m_isDead)
    {
        if (m_archerDeath) {

            m_archerDeath->Process(deltaTime);
            m_archerDeath->SetX(m_archerPosition.x);
            m_archerDeath->SetY(m_archerPosition.y);

            //freeze last death frame
            if (m_archerDeath->GetCurrentFrame() >= 3) {
                m_archerDeath->StopAnimating();
                m_archerDeath->SetCurrentFrame(3);
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
        case CLASS_ATTACK_1:
            activeAttack = m_archerAttack1;
            timeoutDuration = 0.9f;
            break;
        case CLASS_SP_ATTACK:
            activeAttack = m_archerSpecial;
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

            activeAttack->SetX(m_archerPosition.x);
            activeAttack->SetY(m_archerPosition.y);

            float scaleX = (m_archerWalk) ? m_archerWalk->GetScaleX() : 7.5f;
            float direction = (scaleX < 0) ? -7.5f : 7.5f;
            activeAttack->SetScale(direction, -7.5f);

            if ((!activeAttack->IsAnimating() && m_attackState != BLOCK) || m_attackDuration > timeoutDuration)
            {
                if (m_iActiveArrows < 5)
                {
                    for (Projectile* arrow : m_pArrows)
                    {
                        if (!arrow->m_bActive)
                        {
                            arrow->m_bActive = true;
                            arrow->SetPosition(m_archerPosition + Vector2(60.0f, 0.0f));
                            m_iActiveArrows++;
                            break;
                        }
                    }
                }
                m_isAttacking = false;
                m_attackState = CLASS_ATTACK_NONE;
                m_attackDuration = 0.0f;
            }
        }
        else
        {
            m_isAttacking = false;
            m_attackState = CLASS_ATTACK_NONE;
            m_attackDuration = 0.0f;
        }
    }

    // Process jump physics
    if (m_isJumping) {
        m_jumpVelocity += m_gravity * deltaTime;

        m_archerPosition.y += m_jumpVelocity * deltaTime;

        // Check for landing
        if (m_archerPosition.y >= m_groundY) {
            m_archerPosition.y = m_groundY;
            m_isJumping = false;
            m_jumpVelocity = 0.0f;

            // Resume animations after landing
            if (m_isMoving && m_archerWalk) {
                m_archerWalk->Animate();
            }
            else if (m_archerIdle) {
                m_archerIdle->Animate();
            }
        }
    }

    // Process Arrows
    for (int i = 0; i < m_pArrows.size(); i++)
    {
        if (m_pArrows[i]->m_bActive)
        {
            m_pArrows[i]->Process(deltaTime);
            if (m_pArrows[i]->m_bJustDied)
            {
                m_iActiveArrows--;
            }
        }
    }

    // Process knight sprite animations
    if (!m_isAttacking) {

        if (m_isJumping) {

            if (m_archerWalk) {
                m_archerWalk->StopAnimating();
                m_archerWalk->SetX(m_archerPosition.x);
                m_archerWalk->SetY(m_archerPosition.y);

                float direction = m_archerLeft ? -7.5f : 7.5f;
                m_archerWalk->SetScale(direction, -7.5f);
            }
        }
        //Handle normal movement animations
        else if (m_isMoving) {
            //Use walking animation when moving
            if (m_archerWalk && !m_archerWalk->IsAnimating()) {
                m_archerWalk->Animate();
            }

            //Process walking animation
            if (m_archerWalk) {
                m_archerWalk->Process(deltaTime);
                m_archerWalk->SetX(m_archerPosition.x);
                m_archerWalk->SetY(m_archerPosition.y);

                float direction = m_archerLeft ? -7.5 : 7.5;
                m_archerWalk->SetScale(direction, -7.5);
            }
        }
        else {
            // Use idle animation when not moving
            if (m_archerIdle && !m_archerIdle->IsAnimating()) {
                m_archerIdle->Animate();
            }

            //Process idle animation
            if (m_archerIdle) {
                m_archerIdle->Process(deltaTime);
                m_archerIdle->SetX(m_archerPosition.x);
                m_archerIdle->SetY(m_archerPosition.y);

                float direction = m_archerLeft ? -7.5 : 7.5;
                m_archerIdle->SetScale(direction, -7.5);
            }

        }
    }
}

void Archer::Draw(Renderer& renderer) {
    // Hurt override
    if (m_isHurt && m_archerHurt) {
        if (!m_archerHurt->IsAnimating()) {
            m_archerHurt->Restart();
            m_archerHurt->Animate();
        }
        m_archerHurt->Draw(renderer);
        return;
    }

    // Death override
    if (m_isDead && m_archerDeath) {
        m_archerDeath->Draw(renderer);
        return;
    }

    // Draw Arrows
    for (int i = 0; i < m_pArrows.size(); i++)
    {
        if (m_pArrows[i]->m_bActive)
        {
            m_pArrows[i]->Draw(renderer);
        }
    }

    // Draw either the knight animation or the attack animation
    if (!m_isAttacking) {

        if (m_isJumping) {
            if (m_archerWalk) {
                m_archerWalk->Draw(renderer);
            }
        }
        else if (m_isMoving) {
            // Use walking animation when moving
            if (m_archerWalk) {
                m_archerWalk->Draw(renderer);
            }
        }
        else {
            // Use idle animation when not moving
            if (m_archerIdle) {
                m_archerIdle->Draw(renderer);
            }
        }
    }
    else {
        // Draw appropriate attack animation 
        switch (m_attackState) {
        case CLASS_ATTACK_1:
            if (m_archerAttack1) {
                m_archerAttack1->Draw(renderer);
            }
            break;
        case CLASS_SP_ATTACK:
            if (m_archerSpecial) {
                m_archerSpecial->Draw(renderer);
            }
            break;
        default:
            // Fall back to appropriate idle/walk animation
            if (m_isJumping) {
                if (m_archerWalk) m_archerWalk->Draw(renderer);
            }
            else if (!m_isMoving && m_archerIdle) {
                m_archerIdle->Draw(renderer);
            }
            else if (m_archerWalk) {
                m_archerWalk->Draw(renderer);
            }
            break;
        }
    }
}

void Archer::ProcessInput(InputSystem& inputSystem) {
    bool isWalking = false;
    m_archerPosition.x = 150.0f;
    Vector2 direction;
    direction.Set(0, 0);

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
        m_jumpVelocity = m_jumpStrength;
    }

    if (inputSystem.GetKeyState(SDL_SCANCODE_A) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_LEFT) == BS_HELD || (controller && (stick.x < -threshold || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == BS_HELD))) {
        direction.x = -1.0f;
        m_archerLeft = true; // Left 
        isWalking = true;
    }
    else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || (controller && (stick.x > threshold || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_HELD))) {
        direction.x = 1.0f;
        m_archerLeft = false; // Right 
        isWalking = true;
    }

    // Process attacking inputs
    if (!m_isAttacking && !m_isHurt && !m_isDead) {
        // LCTRL for basic attack
        if (inputSystem.GetKeyState(SDL_SCANCODE_LCTRL) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_X) == BS_PRESSED)) {
            StartAttack(CLASS_ATTACK_1);
        }
        // V for special attack
        else if (inputSystem.GetKeyState(SDL_SCANCODE_Q) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_Y) == BS_PRESSED)) {
            StartAttack(CLASS_SP_ATTACK);
        }
    }

    // Check if we need to end blocking when button is released
    if (m_isAttacking && m_attackState == BLOCK && inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_RELEASED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_B) == BS_RELEASED)) {
        m_isAttacking = false;
        m_attackState = CLASS_ATTACK_NONE;
        m_attackDuration = 0.0f;
    }

    // Store the movement direction for background scrolling
    m_lastMovementDirection = direction;
    // Update player horizontal position
    if (isWalking) {
        Vector2 movement = direction * m_archerSpeed * 0.016f;
        m_archerPosition.x += movement.x;

        ClampPositionToBoundaries();
        m_isMoving = true;
    }
    else {
        m_isMoving = false;
    }
}

void Archer::StartAttack(AttackType attackType) {
    m_attackState = attackType;
    m_attackDuration = 0.0f;

    AnimatedSprite* attackSprite = nullptr;
    float frameDuration = 0.1f; // Default frame duration

    switch (attackType) {
    case CLASS_ATTACK_1:
        attackSprite = m_archerAttack1;
        break;
    case CLASS_SP_ATTACK:
        attackSprite = m_archerSpecial;
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
        m_attackState = CLASS_ATTACK_NONE;
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

int Archer::AttackDamage() const {
    switch (m_attackState) {
    case CLASS_ATTACK_1:
        return 10;
        break;
    case CLASS_SP_ATTACK:
        return 25;
        break;
    case BLOCK:
        return 0;
        break;
    default:
        return 0;
    }
}

bool Archer::isBlocking() const {
    bool blocking = m_attackState == BLOCK && m_isAttacking;

    return blocking;
}

bool Archer::isAttacking() const {

    return m_isAttacking && m_attackState != ATTACK_NONE;
}

bool Archer::isProjectilesActive() const {
    // Loop through projects and return true if any active
    for (Projectile* proj : m_pArrows)
    {
        if (proj->m_bActive)
        {
            return true;
        }
    }
    return false;
}

void Archer::SetBoundaries(float left, float right, float top, float bottom)
{
    // Calculate effective sprite size for boundary padding
    float effectiveWidth = 100.0f * 2.0f * 0.5f;
    float effectiveHeight = 100.0f * 2.0f * 0.5f;

    // Set left boundary to knight's initial X position 
    m_leftBoundary = 100.0f;

    // Set other boundaries with appropriate padding
    m_rightBoundary = right - effectiveWidth;
    m_topBoundary = top + effectiveHeight;
    m_bottomBoundary = bottom - effectiveHeight;

}

void Archer::ClampPositionToBoundaries()
{
    // Clamp X position
    if (m_archerPosition.x < m_leftBoundary)
    {
        m_archerPosition.x = m_leftBoundary;
    }
    else if (m_archerPosition.x > m_rightBoundary)
    {
        m_archerPosition.x = m_rightBoundary;
    }

    // Clamp Y position
    if (m_archerPosition.y < m_topBoundary)
    {
        m_archerPosition.y = m_topBoundary;
    }
    else if (m_archerPosition.y > m_bottomBoundary)
    {
        m_archerPosition.y = m_bottomBoundary;
    }
}

const Vector2& Archer::GetLastMovementDirection() const
{
    return m_lastMovementDirection;
}

int Archer::GetHealth() const {
    return m_archerhealth;
}

const Vector2& Archer::GetPosition() const
{
    return m_archerPosition;
}

Hitbox Archer::GetHitbox() const {

    float halfWidth = (100.0f * 7.5f) / 2.0f;
    float halfHeight = (100.0f * 7.5f) / 2.0f;

    if (m_isJumping) {
        halfHeight *= 0.75f; // 25% smaller in air
    }

    return {
        m_archerPosition.x - halfWidth,
        m_archerPosition.y - 100.0f * 7.5f,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };


}

// Need to change it so that it will only calculate it for the closest arrow projectile
Hitbox Archer::GetAttackHitbox(const Enemy& enemy) const
{
    if (m_pArrows.empty())
    {
        return { 0, 0, 0, 0 };
    }

    // Get orc x pos and set closest current distance value to high val
    float enemyX = enemy.GetPosition().x;
    float closestDist = FLT_MAX;
    Projectile* closestProj = nullptr;

    for (Projectile* proj : m_pArrows)
    {
        if (proj && proj->m_bActive)
        {
            // absolute value of difference between orcX and projectile X
            float dist = fabs(proj->GetPosition().x - enemyX);
            if (dist < closestDist)
            {
                closestDist = dist;
                closestProj = proj;
            }
        }
    }

    return (closestProj ? closestProj->GetHitbox() : Hitbox{0, 0, 0, 0});
}

void Archer::TakeDamage(int amount) {
    // Don't process damage if already dead
    if (m_isDead) {
        return;
    }

    // Don't take damage during hurt animation
    if (m_isHurt && m_archerHurt && m_archerHurt->IsAnimating()) {
        return;
    }

    m_archerhealth -= amount;

    // Cancel any current attack
    m_isAttacking = false;
    m_attackState = CLASS_ATTACK_NONE;
    m_attackDuration = 0.0f;

    if (m_archerhealth <= 0) {
        // Handle death
        m_archerhealth = 0;
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
        if (m_archerDeath) {
            m_archerDeath->SetCurrentFrame(0);
            m_archerDeath->Restart();
            m_archerDeath->Animate();
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
        if (m_archerHurt) {
            m_archerHurt->SetCurrentFrame(0);
            m_archerHurt->Restart();
            m_archerHurt->Animate();
        }
    }
}

bool Archer::IsDead() const
{
    return m_isDead;
}