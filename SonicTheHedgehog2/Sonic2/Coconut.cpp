#include "pch.h"
#include "Coconut.h"
#include "Texture.h"

Texture* Coconut::PROJECTILESPRITESHEETTEXTUREPTR{ nullptr };
int Coconut::INSTANCECOUNTER{ 0 };
const float Coconut::GRAVITY{ -100 };
const float Coconut::TRAVELTIME{ 1.5f };

Coconut::Coconut(const Point2f& targetCenter, const Point2f centerPos) :
	Projectile(targetCenter, centerPos, 16, 16, 100.f, 1, 1),
	m_DeltaTime{ 0.f },
	m_SpawnPos{ centerPos }
{
	if (INSTANCECOUNTER++ == 0)
	{
		PROJECTILESPRITESHEETTEXTUREPTR = new Texture{ "SpriteSheets/Projectiles/Coconut.png" };
	}

	setProjectileTexture(PROJECTILESPRITESHEETTEXTUREPTR);
	SetVelocity();
}

Coconut::~Coconut()
{
	if (--INSTANCECOUNTER == 0)
	{
		delete PROJECTILESPRITESHEETTEXTUREPTR;
		PROJECTILESPRITESHEETTEXTUREPTR = nullptr;
	}
}

void Coconut::Update(float elapsedSec)
{
	Projectile::Update(elapsedSec);

	float xPos{ m_SpawnPos.x + (m_XVelocity * m_DeltaTime) };
	float yPos{ m_SpawnPos.y + (m_YVelocity * m_DeltaTime) + ((GRAVITY * powf(m_DeltaTime,2.f)) / 2.f) };

	SetCenterPos(Point2f{ xPos,yPos });

	m_DeltaTime += elapsedSec;
}

void Coconut::ResetProjectile(const Point2f& targetCenter, const Point2f centerPos)
{
	Projectile::ResetProjectile(targetCenter, centerPos);
	m_SpawnPos = centerPos;
	m_DeltaTime = 0.f;
	SetVelocity();
}

void Coconut::SetVelocity()
{
	m_XVelocity = (GetTargetPos().x - m_SpawnPos.x) / TRAVELTIME;
	m_YVelocity = (GetTargetPos().y - m_SpawnPos.y - (GRAVITY * powf(TRAVELTIME,2.f)) / 2.f) / TRAVELTIME;

	if (abs(m_XVelocity) > 300.f)
	{
		SetInActive();
	}
}
