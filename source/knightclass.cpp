#include "knightclass.h"
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

KnightClass::KnightClass()
    : m_knightIdle(nullptr)
    , m_knightWalk(nullptr)
    , m_knightHurt(nullptr)
    , m_knightDeath(nullptr)
    , m_knightSpeed(0.5f)
    , m_knightLeft(false)
    , m_isMoving(false)
    , m_isHurt(false)
    , m_isDead(false)
    , m_knighthealth(125)
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
    , m_knightAttack1(0) 
    , m_knightAttack2(0) 
    , m_knightSpecial(0)  
    , m_knightBlock(0) 
    , m_isAttacking(false)
    , m_alternateAttacks(false)
    , m_attackState(ATTACK_NONE)
    , m_attackDuration(0.0f)
    , m_attackSound(nullptr)
    , m_hurtSound(nullptr)
    , m_deathSound(nullptr)
    , m_jumpSound(nullptr)
    , m_sfxVolume(0.4f)
{
    m_knightPosition.Set(100, 618);
    m_lastMovementDirection.Set(0.0f, 0.0f); 
} 

KnightClass::~KnightClass() {
    //Clean up normal animations
    delete m_knightWalk;
    m_knightWalk = nullptr;

    delete m_knightIdle;
    m_knightIdle = nullptr;

    delete m_knightHurt;
    m_knightHurt = nullptr;

    delete m_knightDeath; 
    m_knightDeath = nullptr;
    
    // Clean up attack animations
    delete m_knightAttack1; 
    m_knightAttack1 = nullptr;
    
    delete m_knightAttack2;
    m_knightAttack2 = nullptr;
    
    delete m_knightSpecial;
    m_knightSpecial = nullptr;
    
    delete m_knightBlock;
    m_knightBlock = nullptr;

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

bool KnightClass::Initialise(Renderer& renderer)
{
    m_groundY = renderer.GetHeight() * 0.8; 
    m_knightPosition.y = m_groundY; 
    FMOD::System* fmod = Game::GetInstance().GetFMODSystem();

    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_attack.mp3", FMOD_DEFAULT, 0, &m_attackSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_hurt.wav", FMOD_DEFAULT, 0, &m_hurtSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_death.wav", FMOD_DEFAULT, 0, &m_deathSound);
    fmod->createSound("../game/assets/Audio/Knight-Audio/knight_jump.wav", FMOD_DEFAULT, 0, &m_jumpSound);  

    //Load knight's idle sprite
    m_knightIdle = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Idle.png");
    if (m_knightIdle)
    {
        m_knightIdle->SetupFrames(100, 100);
        m_knightIdle->SetFrameDuration(0.1f);
        m_knightIdle->SetLooping(true);
        m_knightIdle->SetX(m_knightPosition.x); 
        m_knightIdle->SetY(m_knightPosition.y);
        m_knightIdle->SetScale(7.5f, -7.5f);
        m_knightIdle->Animate(); 
    }
   
    //Load knight's walking sprite
    m_knightWalk = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Walk.png");
    if (m_knightWalk)
    { 
        m_knightWalk->SetupFrames(100, 100);
        m_knightWalk->SetFrameDuration(0.1f); 
        m_knightWalk->SetLooping(true); 
        m_knightWalk->SetX(m_knightPosition.x);
        m_knightWalk->SetY(m_knightPosition.y);
        m_knightWalk->SetScale(7.5f, -7.5f);
        m_knightWalk->Animate();
        SetBoundaries(0, renderer.GetWidth(), 0, renderer.GetHeight());  
    }

    //Load knight's hurt sprite
    m_knightHurt = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Hurt.png");
    if (m_knightHurt)
    {
        m_knightHurt->SetupFrames(100, 100);
        m_knightHurt->SetFrameDuration(0.12f);  
        m_knightHurt->SetLooping(false);       
        m_knightHurt->SetX(m_knightPosition.x);
        m_knightHurt->SetY(m_knightPosition.y);
        m_knightHurt->SetScale(7.5f, -7.5f);
    }

    //Load knight's death sprite
    m_knightDeath = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Death.png"); 
    if (m_knightDeath)
    {
        m_knightDeath->SetupFrames(100, 100);
        m_knightDeath->SetFrameDuration(0.15f); 
        m_knightDeath->SetLooping(false);       
        m_knightDeath->SetX(m_knightPosition.x);
        m_knightDeath->SetY(m_knightPosition.y);
        m_knightDeath->SetScale(7.5f, -7.5f); 
    }

    //Load Attack 1
    m_knightAttack1 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Attack01.png");
    if (m_knightAttack1)
    {
        m_knightAttack1->SetupFrames(100, 100);
        m_knightAttack1->SetFrameDuration(0.1f);
        m_knightAttack1->SetLooping(false);
        m_knightAttack1->SetX(m_knightPosition.x);
        m_knightAttack1->SetY(m_knightPosition.y);
        m_knightAttack1->SetScale(7.5f, -7.5f);
    }

    //Load Attack 2
    m_knightAttack2 = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Attack02.png");
    if (m_knightAttack2)
    {
        m_knightAttack2->SetupFrames(100, 100);
        m_knightAttack2->SetFrameDuration(0.1f);
        m_knightAttack2->SetLooping(false);
        m_knightAttack2->SetX(m_knightPosition.x);
        m_knightAttack2->SetY(m_knightPosition.y);
        m_knightAttack2->SetScale(7.5f, -7.5f);
    }

    //Load Special Attack
    m_knightSpecial = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Attack03.png");
    if (m_knightSpecial)
    {
        m_knightSpecial->SetupFrames(100, 100);
        m_knightSpecial->SetFrameDuration(0.1f);
        m_knightSpecial->SetLooping(false);
        m_knightSpecial->SetX(m_knightPosition.x);
        m_knightSpecial->SetY(m_knightPosition.y);
        m_knightSpecial->SetScale(7.5f, -7.5f);
    }

    //Load Block
    m_knightBlock = renderer.CreateAnimatedSprite("../game/assets/Sprites/Knight/Knight/Knight-Block.png");
    if (m_knightBlock)
    {
        m_knightBlock->SetupFrames(100, 100);
        m_knightBlock->SetFrameDuration(0.1f);
        m_knightBlock->SetLooping(false);
        m_knightBlock->SetX(m_knightPosition.x);
        m_knightBlock->SetY(m_knightPosition.y);
        m_knightBlock->SetScale(7.5f, -7.5f);
    }

    if (!m_knightIdle || !m_knightWalk || !m_knightHurt || !m_knightDeath || !m_knightAttack1 || !m_knightAttack2 || !m_knightSpecial || !m_knightBlock) 
    {
        LogManager::GetInstance().Log("Failed to load Knight sprites!");
        return false;
    }

    return true;
}

void KnightClass::Process(float deltaTime) {

    //Process hurt animation
    if (m_isHurt)
    {
        if (m_knightHurt) {
            m_knightHurt->Process(deltaTime);
            m_knightHurt->SetX(m_knightPosition.x);
            m_knightHurt->SetY(m_knightPosition.y);
        }


        if (!m_knightHurt->IsAnimating() || m_knightHurt->GetCurrentFrame() >= 3)
        {
            m_isHurt = false;
        }
        return;
    }
    
    //Process death animation
    if (m_isDead)
    {
        if (m_knightDeath) {

            m_knightDeath->Process(deltaTime);
            m_knightDeath->SetX(m_knightPosition.x);
            m_knightDeath->SetY(m_knightPosition.y);

            //freeze last death frame
            if (m_knightDeath->GetCurrentFrame() >= 3) {
                m_knightDeath->StopAnimating();
                m_knightDeath->SetCurrentFrame(3);
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
            activeAttack = m_knightAttack1;
            timeoutDuration = 0.8f;
            break;
        case ATTACK_2:
            activeAttack = m_knightAttack2;
            timeoutDuration = 0.9f;
            break;
        case SP_ATTACK:
            activeAttack = m_knightSpecial;
            timeoutDuration = 1.2f;
            break;
        case BLOCK:
            activeAttack = m_knightBlock;
            timeoutDuration = 10000.0f; // Very long timeout
            
            // Manual frame control for block animation
            if (activeAttack) {
                // Stop animation system
                activeAttack->StopAnimating();
                
                if (m_attackDuration < 0.1f) {
                    activeAttack->SetCurrentFrame(0);
                } 
                else if (m_attackDuration < 0.2f) {
                    activeAttack->SetCurrentFrame(1);
                } 
                else if (m_attackDuration < 0.3f) {
                    activeAttack->SetCurrentFrame(2);
                } 
                else {
                    // Lock to frame 3
                    activeAttack->SetCurrentFrame(3);
                }
            }
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
            
            activeAttack->SetX(m_knightPosition.x);
            activeAttack->SetY(m_knightPosition.y);

            float scaleX = (m_knightWalk) ? m_knightWalk->GetScaleX() : 7.5f; 
            float direction = (scaleX < 0) ? -7.5f : 7.5f; 
            activeAttack->SetScale(direction, -7.5f); 

            if ((!activeAttack->IsAnimating() && m_attackState != BLOCK) || m_attackDuration > timeoutDuration)
            {
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
    if (m_isJumping) {
        m_jumpVelocity += m_gravity * deltaTime;

        m_knightPosition.y += m_jumpVelocity * deltaTime;

        // Check for landing
        if (m_knightPosition.y >= m_groundY) {
            m_knightPosition.y = m_groundY;
            m_isJumping = false;
            m_jumpVelocity = 0.0f;

            // Resume animations after landing
            if (m_isMoving && m_knightWalk) { 
                m_knightWalk->Animate(); 
            }
            else if (m_knightIdle) { 
                m_knightIdle->Animate(); 
            }
        }
    }

    // Process knight sprite animations
    if (!m_isAttacking) {

        if (m_isJumping) {

            if (m_knightWalk) {
                m_knightWalk->StopAnimating();
                m_knightWalk->SetX(m_knightPosition.x);
                m_knightWalk->SetY(m_knightPosition.y);

                float direction = m_knightLeft ? -7.5f : 7.5f;
                m_knightWalk->SetScale(direction, -7.5f); 
            }
        }
        //Handle normal movement animations
        else if (m_isMoving) {
            //Use walking animation when moving
            if (m_knightWalk && !m_knightWalk->IsAnimating()) {
                m_knightWalk->Animate();
            }

            //Process walking animation
            if (m_knightWalk) {
                m_knightWalk->Process(deltaTime);
                m_knightWalk->SetX(m_knightPosition.x);
                m_knightWalk->SetY(m_knightPosition.y);

                float direction = m_knightLeft ? -7.5 : 7.5;
                m_knightWalk->SetScale(direction, -7.5);
            }
        }
        else {
            // Use idle animation when not moving
            if (m_knightIdle && !m_knightIdle->IsAnimating()) {
                m_knightIdle->Animate();
            }

            //Process idle animation
            if (m_knightIdle) {
                m_knightIdle->Process(deltaTime);
                m_knightIdle->SetX(m_knightPosition.x);
                m_knightIdle->SetY(m_knightPosition.y);

                float direction = m_knightLeft ? -7.5 : 7.5; 
                m_knightIdle->SetScale(direction, -7.5);  
            }

        }
    }
}  

void KnightClass::Draw(Renderer& renderer) {
    // Hurt override
    if (m_isHurt && m_knightHurt) {
        if (!m_knightHurt->IsAnimating()) {
            m_knightHurt->Restart();
            m_knightHurt->Animate();
        }
        m_knightHurt->Draw(renderer);
        return;
    }

    // Death override
    if (m_isDead && m_knightDeath) {
        m_knightDeath->Draw(renderer);
        return;
    }
    
    // Draw either the knight animation or the attack animation
    if (!m_isAttacking) { 

        if (m_isJumping) {
            if (m_knightWalk) {
                m_knightWalk->Draw(renderer);
            }
        }
        else if (m_isMoving) {
            // Use walking animation when moving
            if (m_knightWalk) {
                m_knightWalk->Draw(renderer); 
            }
        } else {
            // Use idle animation when not moving
            if (m_knightIdle) {
                m_knightIdle->Draw(renderer);
            } 
        }
    }
    else {
        // Draw appropriate attack animation 
        switch (m_attackState) {
        case ATTACK_1:
            if (m_knightAttack1) {
                m_knightAttack1->Draw(renderer);
            }
            break;
        case ATTACK_2:
            if (m_knightAttack2) {
                m_knightAttack2->Draw(renderer);
            }
            break;
        case SP_ATTACK:
            if (m_knightSpecial) {
                m_knightSpecial->Draw(renderer);
            }
            break;
        case BLOCK:
            if (m_knightBlock) {
                m_knightBlock->Draw(renderer);
            }
            break;
        default:
            // Fall back to appropriate idle/walk animation
            if (m_isJumping) {
                if (m_knightWalk) m_knightWalk->Draw(renderer);
            }
            else if (!m_isMoving && m_knightIdle) {
                m_knightIdle->Draw(renderer);
            }
            else if (m_knightWalk) {
                m_knightWalk->Draw(renderer);
            }
            break;
        }
    }
}

void KnightClass::ProcessInput(InputSystem& inputSystem) {
    bool isWalking = false;
    m_knightPosition.x = 150.0f;  
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
        m_knightLeft = true; // Left 
        isWalking = true; 
    } 
    else if (inputSystem.GetKeyState(SDL_SCANCODE_D) == BS_HELD || inputSystem.GetKeyState(SDL_SCANCODE_RIGHT) == BS_HELD || (controller && (stick.x > threshold || controller->GetButtonState(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == BS_HELD))) { 
        direction.x = 1.0f; 
        m_knightLeft = false; // Right 
        isWalking = true; 
    } 
    
    // Process attacking inputs
    if (!m_isAttacking && !m_isHurt && !m_isDead) { 
        // LCTRL for basic attack
        if (inputSystem.GetKeyState(SDL_SCANCODE_LCTRL) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_X) == BS_PRESSED)) { 
            StartAttack(m_alternateAttacks ? ATTACK_2 : ATTACK_1);
            m_alternateAttacks = !m_alternateAttacks;
        }
        // V for special attack
        else if (inputSystem.GetKeyState(SDL_SCANCODE_Q) == BS_PRESSED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_Y) == BS_PRESSED)) { 
            StartAttack(SP_ATTACK);
        }
        // W for block 
        else if (inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_HELD || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_B) == BS_PRESSED)) { 
            if (!m_isAttacking || m_attackState != BLOCK) {
                StartAttack(BLOCK);
            }
        }
    }

    // Check if we need to end blocking when button is released
    if (m_isAttacking && m_attackState == BLOCK && inputSystem.GetKeyState(SDL_SCANCODE_W) == BS_RELEASED || (controller && controller->GetButtonState(SDL_CONTROLLER_BUTTON_B) == BS_RELEASED)) {
        m_isAttacking = false;
        m_attackState = ATTACK_NONE;
        m_attackDuration = 0.0f;

        if (m_knightBlock) {
            m_knightBlock->SetCurrentFrame(0);
            m_knightBlock->StopAnimating();
        }
    } 

    // Store the movement direction for background scrolling
    m_lastMovementDirection = direction;
    // Update player horizontal position
    if (isWalking) { 
        Vector2 movement = direction * m_knightSpeed * 0.016f; 
        m_knightPosition.x += movement.x; 
        
        ClampPositionToBoundaries(); 
        m_isMoving = true;
    }
    else {
        m_isMoving = false; 
    }
}

void KnightClass::StartAttack(AttackType attackType) {
    m_attackState = attackType;
    m_attackDuration = 0.0f;
    
    AnimatedSprite* attackSprite = nullptr;
    float frameDuration = 0.1f; // Default frame duration
    
    switch (attackType) {
    case ATTACK_1:
        attackSprite = m_knightAttack1;
        break;
    case ATTACK_2:
        attackSprite = m_knightAttack2;
        break;
    case SP_ATTACK:
        attackSprite = m_knightSpecial;
        break;
    case BLOCK:
        attackSprite = m_knightBlock;
        // Special handling for block animation
        if (attackSprite) {
            m_attackDuration = 0.0f; // Reset duration to start animation sequence
            attackSprite->SetCurrentFrame(0); // Start at frame 0
            attackSprite->StopAnimating();
        }
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

int KnightClass::AttackDamage() const{
    switch (m_attackState) {
    case ATTACK_1:
        return 10;
        break;
    case ATTACK_2:
        return 10;
        break;
    case SP_ATTACK:
        return 25;
        break;
    case BLOCK:
        return 0;
        break;
    default:
        return 0;
    }
}

bool KnightClass::isBlocking() const{
    bool blocking = m_attackState == BLOCK && m_isAttacking;
   
    return blocking;
}

bool KnightClass::isAttacking() const {

    return m_isAttacking && m_attackState != ATTACK_NONE;  
}

void KnightClass::SetBoundaries(float left, float right, float top, float bottom)
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

void KnightClass::ClampPositionToBoundaries()
{
    // Clamp X position
    if (m_knightPosition.x < m_leftBoundary) 
    {
        m_knightPosition.x = m_leftBoundary; 
    }
    else if (m_knightPosition.x > m_rightBoundary)
    {
        m_knightPosition.x = m_rightBoundary;
    }
    
    // Clamp Y position
    if (m_knightPosition.y < m_topBoundary)
    {
        m_knightPosition.y = m_topBoundary;
    }
    else if (m_knightPosition.y > m_bottomBoundary)
    {
        m_knightPosition.y = m_bottomBoundary;
    }
}

const Vector2& KnightClass::GetLastMovementDirection() const
{
    return m_lastMovementDirection;
}

int KnightClass::GetHealth() const{
    return m_knighthealth;
}

const Vector2& KnightClass::GetPosition() const
{
    return m_knightPosition;
} 

Hitbox KnightClass::GetHitbox() const{ 

    float halfWidth = (100.0f * 7.5f) / 2.0f;
    float halfHeight = (100.0f * 7.5f) / 2.0f;

    if (m_isJumping) {
        halfHeight *= 0.75f; // 25% smaller in air
    }

    return {
        m_knightPosition.x - halfWidth,
        m_knightPosition.y - 100.0f * 7.5f,
        halfWidth * 2.0f,
        halfHeight * 2.0f
    };


} 

Hitbox KnightClass::GetAttackHitbox() const {
    float attackWidth = 80.0f;  // Width of the attack zone
    float attackHeight = 100.0f * 7.5f;
    float direction = (m_knightWalk && m_knightWalk->GetScaleX() < 0) ? -7.5f : 7.5f;

    float offsetX = (direction < 0) ? 50.0f : -attackWidth - 50.0f; 

    return {
        m_knightPosition.x + offsetX,
        m_knightPosition.y - (attackHeight / 2.0f),
        attackWidth,
        attackHeight
    };
} 

void KnightClass::TakeDamage(int amount) {
    // Don't process damage if already dead
    if (m_isDead) {
        return;
    }
    
    // Don't take damage during hurt animation
    if (m_isHurt && m_knightHurt && m_knightHurt->IsAnimating()) { 
        return;
    }
  
    m_knighthealth -= amount; 
    
    // Cancel any current attack
    m_isAttacking = false;
    m_attackState = ATTACK_NONE;
    m_attackDuration = 0.0f;
    
    if (m_knighthealth <= 0) { 
        // Handle death
        m_knighthealth = 0; 
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
        if (m_knightDeath) {
            m_knightDeath->SetCurrentFrame(0);
            m_knightDeath->Restart();
            m_knightDeath->Animate();
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
        if (m_knightHurt) {
            m_knightHurt->SetCurrentFrame(0);  
            m_knightHurt->Restart();
            m_knightHurt->Animate();
        } 
    }
}

bool KnightClass::IsDead() const 
{
    return m_isDead;
}



