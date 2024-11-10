#include "pch.h"
#include "Stinger.h"
#include "Texture.h"
#include <iostream>

Texture* Stinger::PROJECTILESPRITESHEETTEXTUREPTR{ nullptr };
int Stinger::INSTANCECOUNTER{ 0 };
float Stinger::PROJECTILEVELOCITY{ 2.f * 25.f };

Stinger::Stinger(const Point2f& targetCenter, const Point2f centerPos) :
	Projectile(targetCenter, centerPos, 8, 16, 0.2f, 1, 2)
{
	if (INSTANCECOUNTER == 0)
	{
		PROJECTILESPRITESHEETTEXTUREPTR = new Texture{ "SpriteSheets/Projectiles/Stinger.png" };
	}
	++INSTANCECOUNTER;

	setProjectileTexture(PROJECTILESPRITESHEETTEXTUREPTR);
	m_DirectionVector = GetDirectionVector(centerPos,targetCenter);
}

Stinger::~Stinger()
{
	if (--INSTANCECOUNTER == 0)
	{
		delete PROJECTILESPRITESHEETTEXTUREPTR;
		PROJECTILESPRITESHEETTEXTUREPTR = nullptr;
	}
}

void Stinger::Update(float elapsedSec)
{
	Projectile::Update(elapsedSec);
	Point2f currentPos{ GetCenterPos() };

	float xMovementDelta{ m_DirectionVector.x * (PROJECTILEVELOCITY * elapsedSec) };
	float yMovementDelta{ m_DirectionVector.y * (PROJECTILEVELOCITY * elapsedSec) };
	
	SetCenterPos(Point2f{ currentPos.x + xMovementDelta, currentPos.y + yMovementDelta });
}

void Stinger::ResetProjectile(const Point2f& targetCenter, const Point2f centerPos)
{
	Projectile::ResetProjectile(targetCenter, centerPos);
	m_DirectionVector = GetDirectionVector(centerPos, targetCenter);
}

Vector2f Stinger::GetDirectionVector(Point2f centerPos, Point2f targetCenter)
{
	return Vector2f{ centerPos,targetCenter }.Normalized();
}



