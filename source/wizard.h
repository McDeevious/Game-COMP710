#ifndef WIZARD_H
#define WIZARD_H

#include "vector2.h"
#include "collision.h"
#include "character.h"
#include <fmod.hpp>
#include "sharedenums.h"
#include "projectile.h"
#include <vector>
#include "renderer.h"

using namespace std;

class AnimatedSprite;
class Hitbox;
class Renderer;
class InputSystem;
class Projectile;
class Orc;

class Wizard : public Character
{
public:
	Wizard();
	~Wizard();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime, SceneGame& game);
	void Draw(Renderer& renderer);

	void ProcessInput(InputSystem& inputSystem, SceneGame& game);

	// Sets boundaries for knight movement
	void SetBoundaries(float left, float right, float top, float bottom);

	// Get the current movement direction (for background scrolling)
	const Vector2& GetLastMovementDirection() const;

	// Get the knight's current position
	const Vector2& GetPosition() const;

	// Get the knight's health
	int GetHealth() const;

	//Damage taking
	void TakeDamage(int amount);
	int AttackDamage() const;
	bool isBlocking() const;
	bool isAttacking() const;
	bool isProjectilesActive() const;

	// State checking
	bool IsDead() const;

	//Collisions 
	Hitbox GetHitbox() const;
	Hitbox GetAttackHitbox(const Orc& orc) const;

private:
	//Clamp knight to boundaries
	void ClampPositionToBoundaries();

	// Helper method to start an attack animation
	void StartAttack(AttackType attackType);

	// Player character
	AnimatedSprite* m_wizardIdle;
	AnimatedSprite* m_wizardWalk;
	AnimatedSprite* m_wizardHurt;
	AnimatedSprite* m_wizardDeath;

	//Movement and state
	Vector2 m_wizardPosition;
	float m_wizardSpeed;
	bool m_wizardLeft;
	bool m_isMoving;
	bool m_isDead;
	bool m_isHurt;
	int m_wizardhealth;

	//Player attacks
	AttackType m_attackState;
	bool m_isAttacking;
	AnimatedSprite* m_wizardAttack1;
	AnimatedSprite* m_wizardSpecial;
	float m_attackDuration;

	vector<Projectile*> m_pFire;
	int m_iActiveFire;

	// Movement tracking for background scrolling
	Vector2 m_lastMovementDirection;

	// Screen boundaries
	float m_leftBoundary;
	float m_rightBoundary;
	float m_topBoundary;
	float m_bottomBoundary;

	// Jumping mechanics
	bool m_isJumping;
	float m_jumpVelocity;
	float m_gravity;
	float m_jumpStrength;
	float m_groundY;

	FMOD::Sound* m_attackSound;
	FMOD::Sound* m_hurtSound;
	FMOD::Sound* m_deathSound;
	FMOD::Sound* m_jumpSound;
	float m_sfxVolume;

	int wasTouchingGround;
	int wasTouchingRoof;
	Vector2 arenaXY;
};

#endif // WIZARD_H