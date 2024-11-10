#include "pch.h"
#include "Sprite.h"
#include "Aiaiai.h"
#include "ProjectileManager.h"
#include "Texture.h"
#include "utils.h"

Texture* Aiaiai::SPRITESHEETTEXTUREPTR{ nullptr };
int Aiaiai::INSTANCECOUNTER{ 0 };
const float 
	Aiaiai::MOVEMENTDISTANCE{ 40.f },
	Aiaiai::MAXLERPTIME{ 1.f },
	Aiaiai::MAXCOOLDOWNTIME{ 3.f },
	Aiaiai::DETECTIONRADIUS{ 100.f };

Aiaiai::Aiaiai(const Point2f& centerPos) :
	Enemy(centerPos, 32.f, 48.f, 0.25f, 2, 2),
	m_SpawnPos{ centerPos },
	m_LerpFactor{ 0.f },
	m_LerpTime{ 0.f },
	m_OnCooldown{ false },
	m_CooldownTime{ 0.f }
{
	if (INSTANCECOUNTER == 0)
	{
		SPRITESHEETTEXTUREPTR = new Texture{ "SpriteSheets/Enemies/aiaiai.png" };
	}

	++INSTANCECOUNTER;

	setSpriteTexture(SPRITESHEETTEXTUREPTR);
}

Aiaiai::~Aiaiai()
{
	if (--INSTANCECOUNTER < 1)
	{
		delete SPRITESHEETTEXTUREPTR;
		SPRITESHEETTEXTUREPTR = nullptr;
	}
}

void Aiaiai::Update(float elapsedSec)
{
	Interactable::Update(elapsedSec);

	if (!IsHit())
	{
		float lerpFactor{};
		m_LerpTime += elapsedSec;
		lerpFactor = m_LerpTime / MAXLERPTIME;
		if (m_LerpTime >= MAXLERPTIME)
		{
			lerpFactor = 1 - ((m_LerpTime - MAXLERPTIME) / MAXLERPTIME);
			if (m_LerpTime >= 2 * MAXLERPTIME)
			{
				m_LerpTime = 0.f;
			}
		}
		SetCenter(Point2f{ m_SpawnPos.x,utils::Lerp(m_SpawnPos.y, m_SpawnPos.y + MOVEMENTDISTANCE, lerpFactor) });

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


void Aiaiai::Attack(const Rectf& playerHitbox)
{
	Point2f playerCenter{ playerHitbox.left + (playerHitbox.width / 2.f) ,
						playerHitbox.bottom + (playerHitbox.height / 2.f) };
	if (!m_OnCooldown)
	{
		if (utils::IsOverlapping(playerHitbox, Circlef{ GetCenter(),DETECTIONRADIUS }))
		{
			ProjectileManager::GetInstance()->AddProjectile(ProjectileManager::ProjectileType::coconut,
				playerCenter, GetCenter());

			m_OnCooldown = true;
		}
	}
}