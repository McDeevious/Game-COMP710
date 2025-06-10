#include "projectile.h"
#include "collision.h"
#include <fmod.hpp>
#include "sharedenums.h"
#include "logmanager.h"
#include "renderer.h"
#include "sprite.h"

Projectile::Projectile()
	: m_fSpeed(2.0f)
	, m_pProjectile(0)
	, m_fHitBoxHeight(32.0f)
	, m_fHitBoxWidth(32.0f)
	, m_bActive(false)
{

}

Projectile::~Projectile()
{
	delete m_pProjectile;
	m_pProjectile = nullptr;
}

bool Projectile::Initialise(Renderer& renderer)
{
	m_pProjectile = renderer.CreateSprite("../game/assets/Sprites/Characters/Archer/Arrow(projectile)/Arrow02(32x32).png");
	m_position.x = 0.0f;
	m_position.y = 0.0f;
	if (m_pProjectile)
	{
		m_pProjectile->SetX(m_position.x);
		m_pProjectile->SetY(m_position.y);
;	}
	
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

void Projectile::Process(float deltaTime)
{
	m_position.x += m_fSpeed * deltaTime;
	if (m_pProjectile)
	{
		m_pProjectile->SetX(m_position.x);
		m_pProjectile->SetY(m_position.y);
	}
}

void Projectile::Draw(Renderer& renderer)
{
	if (m_pProjectile)
	{
		m_pProjectile->Draw(renderer);
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
}