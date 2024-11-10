#include "pch.h"
#include "Chopper.h"
#include "Texture.h"

Texture* Chopper::SPRITESHEETTEXTUREPTR{ nullptr };
int Chopper::INSTANCECOUNTER{ 0 };
const float Chopper::ROTATIONSPEED{ 4.f };
const float Chopper::GRAVITY{ 0.09375f * 60.f * 25.f };
const float Chopper::JUMPVELOCITY{ 7.f * 25.f };

Chopper::Chopper(const Point2f& centerPos) :
	Enemy(centerPos, 32.f, 32.f, 0.5f, 1, 2),
	m_SpawnPos{ centerPos },
	m_YVelocity{ 0.f },
	m_spriteAngle{ 0.f }
{
	if (INSTANCECOUNTER == 0)
	{
		SPRITESHEETTEXTUREPTR = new Texture{ "SpriteSheets/Enemies/Choppers.png" };
	}

	++INSTANCECOUNTER;

	Interactable::setSpriteTexture(SPRITESHEETTEXTUREPTR);
}

Chopper::~Chopper()
{
	if (--INSTANCECOUNTER == 0)
	{
		delete SPRITESHEETTEXTUREPTR;
		SPRITESHEETTEXTUREPTR = nullptr;
	}
}

void Chopper::Update(float elapsedSec)
{
	const float rotationIncrement{ (ROTATIONSPEED / 180)};
	Enemy::Update(elapsedSec);
	if (!IsHit())
	{
		Point2f centerPos{ GetCenter() };
		if (GetCenter().y <= m_SpawnPos.y)
		{
			SetCenter(m_SpawnPos);
			m_YVelocity = JUMPVELOCITY;
			m_spriteAngle = 0.f;
		}

		m_YVelocity -= GRAVITY * elapsedSec;
		centerPos.y += m_YVelocity * elapsedSec;
		SetCenter(centerPos);

		if (m_YVelocity <= 0 && m_spriteAngle < 180)
		{
			m_spriteAngle = 180;
		}
		SetAngle(m_spriteAngle);
	}
}

