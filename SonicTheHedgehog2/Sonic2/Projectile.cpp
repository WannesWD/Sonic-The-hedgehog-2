#include "pch.h"
#include "Projectile.h"
#include "StaticAnimatedSprite.h"
#include "Texture.h"
#include <iostream>

Projectile::~Projectile()
{
	delete m_ProjectileSpritePtr;
	m_ProjectileSpritePtr = nullptr;
}

void Projectile::Update(float elapsedSec)
{
	m_ProjectileSpritePtr->Update(elapsedSec);
}

void Projectile::Draw() const
{
	m_ProjectileSpritePtr->Draw();
}

Projectile::Projectile(const Point2f& targetCenter, const Point2f& centerPos, float spriteWidth, float spriteHeight,
	float frameTime, int numCols, int numRows) :
	m_Position{ centerPos },
	m_SpriteWidth{ spriteWidth },
	m_SpriteHeight{ spriteHeight },
	m_Active{ true }
{
	m_ProjectileSpritePtr = new StaticAnimatedSprite{ Point2f{centerPos.x - spriteWidth/2.f, centerPos.y - spriteHeight/2.f},
														frameTime, numCols, numRows, nullptr };
}

void Projectile::setProjectileTexture(Texture* newProjectileTexturePtr)
{
	dynamic_cast<StaticAnimatedSprite*>(m_ProjectileSpritePtr)->SetSpriteSheetTexturePtr(newProjectileTexturePtr);
}

void Projectile::SetCenterPos(Point2f newPos)
{
	m_Position = newPos;
	m_ProjectileSpritePtr->SetPos(Point2f{ newPos.x - m_SpriteWidth / 2.f, newPos.y - m_SpriteHeight / 2.f });
}

const Point2f& Projectile::GetTargetPos()
{
	return m_Target;
}

const Point2f& Projectile::GetCenterPos()
{
	return m_Position;
}

void Projectile::SetInActive()
{
	m_Active = false;
}

bool Projectile::IsActive()
{
	return m_Active;
}

void Projectile::ResetProjectile(const Point2f& targetCenter, const Point2f centerPos)
{
	m_Target = targetCenter;
	m_Position = centerPos;
	m_Active = true;
}
