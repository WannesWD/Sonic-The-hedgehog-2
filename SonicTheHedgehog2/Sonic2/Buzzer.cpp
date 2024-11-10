#include "pch.h"
#include "Buzzer.h"
#include "ProjectileManager.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>

Texture* Buzzer::SPRITESHEETTEXTUREPTR{ nullptr };
int Buzzer::INSTANCECOUNTER{ 0 };
const float Buzzer::VELOCITY{ 4.f * 25.f };
const float Buzzer::MAXMOVEMENTDISTANCE{ 350.f };
const float Buzzer::MAXLERPTIME{ 6.f };
const float Buzzer::MAXCOOLDOWNTIME{ 3.f };
const float Buzzer::DETECTIONRADIUS{ 150.f };

Buzzer::Buzzer(const Point2f& centerPos) :
	Enemy(centerPos, 48, 16, 0.4f, 1, 3),
	m_SpawnPos{ centerPos },
	m_LerpTime{},
	m_CooldownTime{},
	m_Flipped{ false },
	m_OnCooldown{ false }
{
	if (INSTANCECOUNTER == 0)
	{
		SPRITESHEETTEXTUREPTR = new Texture{ "SpriteSheets/Enemies/BuzzerMovement.png" };
	}

	++INSTANCECOUNTER;

	setSpriteTexture(SPRITESHEETTEXTUREPTR);
}

Buzzer::~Buzzer()
{
	if (--INSTANCECOUNTER == 0)
	{
		delete SPRITESHEETTEXTUREPTR;
		SPRITESHEETTEXTUREPTR = nullptr;
	}
}

void Buzzer::Update(float elapsedSec)
{
	Interactable::Update(elapsedSec);
	
	if (!IsHit())
	{
		float lerpFactor{};
		m_LerpTime += elapsedSec;
		lerpFactor = m_LerpTime / MAXLERPTIME;
		if (m_LerpTime >= MAXLERPTIME)
		{
			if (!m_Flipped)
			{
				FlipSprite();
				m_Flipped = true;
			}

			lerpFactor = 1 - ((m_LerpTime - MAXLERPTIME) / MAXLERPTIME);
			if (m_LerpTime >= 2 * MAXLERPTIME)
			{
				m_LerpTime = 0.f;
				FlipSprite();
				m_Flipped = false;
			}
		}

		SetCenter(Point2f{ utils::Lerp(m_SpawnPos.x, m_SpawnPos.x - MAXMOVEMENTDISTANCE, lerpFactor), m_SpawnPos.y });

		if (m_OnCooldown)
		{
			if (m_CooldownTime >= MAXCOOLDOWNTIME)
			{
				m_OnCooldown = false;
				m_CooldownTime = 0.f;
			}
			m_CooldownTime += elapsedSec;
		}

	}
}

void Buzzer::Attack(const Rectf& playerHitbox)
{
	Point2f playerCenter{ playerHitbox.left + (playerHitbox.width / 2.f) , 
						playerHitbox.bottom + (playerHitbox.height / 2.f) };
	if (!m_OnCooldown)
	{
		if (utils::IsOverlapping(playerHitbox, Circlef{ GetCenter(),DETECTIONRADIUS }))
		{
			ProjectileManager::GetInstance()->AddProjectile(ProjectileManager::ProjectileType::stinger,
				playerCenter, GetCenter());

			m_OnCooldown = true;
		}
	}
}
