#include "projectile.h"
#include "collision.h"
#include <fmod.hpp>
#include "sharedenums.h"
#include "logmanager.h"
#include "renderer.h"
#include "sprite.h"
#include "animatedsprite.h"

Projectile::Projectile()
	: m_fSpeed(500.0f)
	, m_pProjectile(0)
	, m_fHitBoxHeight(32.0f)
	, m_fHitBoxWidth(32.0f)
	, m_bActive(false)
	, m_bJustDied(false)
	, m_fLifeSpan(2.0f)
	, m_fTimeElapsed(0.0f)
	, m_fBaseY(0.0f)
	, m_fAmplitude(2.5f)
	, m_fFrequency(6.0f)
{

}

Projectile::~Projectile()
{
	delete m_pProjectile;
	m_pProjectile = nullptr;

	delete m_pAnimatedProjectile;
	m_pAnimatedProjectile = nullptr;
}

bool Projectile::Initialise(Renderer& renderer)
{
	if (m_projectileType == ProjectileType::ARROW)
	{
		m_pProjectile = renderer.CreateSprite("../game/assets/Sprites/Characters/Archer/Arrow(projectile)/Arrow02(32x32).png");
	}
	else if (m_projectileType == ProjectileType::FIRE)
	{
		m_pAnimatedProjectile = renderer.CreateAnimatedSprite("../game/assets/Sprites/Characters/Wizard/Magic(projectile)/Wizard-Attack02_Effect.png");
		//m_pProjectile = renderer.CreateSprite("../game/assets/Sprites/Characters/Archer/Arrow(projectile)/Arrow02(32x32).png");
	}

	m_position.x = 0.0f;
	m_position.y = 0.0f;
	if (m_projectileType == ARROW && m_pProjectile)
	{
		m_pProjectile->SetX(m_position.x);
		m_pProjectile->SetY(m_position.y);
		m_pProjectile->SetScale(3.0f, 3.0f);
;	}
	else if (m_projectileType == FIRE && m_pAnimatedProjectile)
	{
		m_pAnimatedProjectile->SetupFrames(100, 100);
		m_pAnimatedProjectile->SetFrameDuration(0.2f);
		m_pAnimatedProjectile->SetLooping(false);
		m_pAnimatedProjectile->SetX(m_position.x);
		m_pAnimatedProjectile->SetY(m_position.y);
		m_pAnimatedProjectile->SetScale(3.0f, 3.0f);

		m_fLifeSpan = 1.4f;
	}
	
	if (!m_pProjectile)
	{
		LogManager::GetInstance().Log("Failed to load Projectile sprite!");
		return false;
	}
	else
	{
		return true;
	}
}

void Projectile::SetProjectileType(ProjectileType projectile)
{
	m_projectileType = projectile;
}

ProjectileType Projectile::GetProjectileType()
{
	return m_projectileType;
}

void Projectile::Process(float deltaTime)
{
	m_fTimeElapsed += deltaTime;
	
	m_position.x += m_fSpeed * deltaTime;
	if (m_projectileType == ARROW && m_pProjectile)
	{
		m_pProjectile->Process(deltaTime);

		//sin wave
		m_position.y = m_fBaseY + sinf(m_position.x * m_fFrequency * 0.01f) * m_fAmplitude;
		
		m_pProjectile->SetX(m_position.x);
		m_pProjectile->SetY(m_position.y);

		if (m_bActive && m_fTimeElapsed > m_fLifeSpan)
		{
			m_bActive = false;
			m_bJustDied = true;
			m_fTimeElapsed = 0.0f;
		}
	}
	else if (m_projectileType == FIRE && m_pAnimatedProjectile)
	{
		m_pAnimatedProjectile->Process(deltaTime);
		
		if (!m_pAnimatedProjectile->IsAnimating() && m_bActive)
		{
			m_pAnimatedProjectile->Animate();
		}
		//sin wave
		m_position.y = m_fBaseY + sinf(m_position.x * m_fFrequency * 0.01f) * m_fAmplitude;

		m_pAnimatedProjectile->SetX(m_position.x);
		m_pAnimatedProjectile->SetY(m_position.y);

		if (m_bActive && m_fTimeElapsed > m_fLifeSpan)
		{
			m_bActive = false;
			m_bJustDied = true;
			m_fTimeElapsed = 0.0f;
		}
	}
}

void Projectile::Draw(Renderer& renderer)
{
	if (m_projectileType == ARROW && m_pProjectile)
	{
		m_pProjectile->Draw(renderer);
	}
	else if (m_projectileType == FIRE && m_pAnimatedProjectile)
	{
		m_pAnimatedProjectile->Draw(renderer);
	}
}

Hitbox Projectile::GetHitbox() const
{
	return {
		m_position.x,
		m_position.y,
		m_fHitBoxWidth,
		m_fHitBoxHeight
	};
}

const Vector2& Projectile::GetPosition() const
{
	return m_position;
}

void Projectile::SetPosition(Vector2 newPos)
{
	m_position.x = newPos.x;
	m_position.y = newPos.y;
	m_fBaseY = newPos.y;
}