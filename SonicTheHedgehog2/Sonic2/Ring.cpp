#include "pch.h"
#include "Ring.h"
#include "Sprite.h"
#include "Texture.h"
#include "StaticAnimatedSprite.h"

Texture* Ring::m_RingSpriteSheetTexture{ nullptr };
Texture* Ring::m_PickupParticleSpriteSheetTexture{ nullptr };

int Ring::m_InstanceCounter{ 0 };

Ring::Ring(Point2f centerPos) :
	m_Center{ centerPos },
	m_SpriteSize{ 16.f },
	m_Hit{ false },
	m_ParticleFrameTime{ 0.0166f },
	m_ParticleTime{ 0.f },
	m_MaxParticleTime{ m_ParticleFrameTime * 4.f }
{
	if (m_InstanceCounter == 0)
	{
		m_RingSpriteSheetTexture = new Texture{ "D:/DAE/Sem2/Prog2/EndAssignment/SpriteSheets/CommonObjects/Rings/RotatingRings.png" };
		m_PickupParticleSpriteSheetTexture = new Texture{ "D:/DAE/Sem2/Prog2/EndAssignment/SpriteSheets/CommonObjects/Rings/RingPickupParticle.png" };
	}
	m_pRingSprite = new StaticAnimatedSprite{ Point2f{centerPos.x - m_SpriteSize / 2.f, centerPos.y - m_SpriteSize / 2.f  }, 0.166f, 4, 1, m_RingSpriteSheetTexture };
	m_pPickupParticleSprite = new StaticAnimatedSprite{ Point2f{centerPos.x - m_SpriteSize / 2.f, centerPos.y - m_SpriteSize / 2.f  }, 0.166f,4,1, m_PickupParticleSpriteSheetTexture };

	++m_InstanceCounter;
}

Ring::~Ring()
{
	if (!m_Hit)
	{
		delete m_pRingSprite;
		m_pRingSprite = nullptr;
		delete m_pPickupParticleSprite;
		m_pPickupParticleSprite = nullptr;
	}

	m_Hit = true;

	if (--m_InstanceCounter < 1)
	{
		delete m_RingSpriteSheetTexture;
		m_RingSpriteSheetTexture = nullptr;
		delete m_PickupParticleSpriteSheetTexture;
		m_PickupParticleSpriteSheetTexture = nullptr;
	}
}

void Ring::Update(float elapsedSec)
{
	if (!m_Hit)
	{
		m_pRingSprite->Update(elapsedSec);
	}
	else if(m_pPickupParticleSprite != nullptr)
	{
		m_pPickupParticleSprite->Update(elapsedSec);

		m_ParticleTime += elapsedSec;
		if (m_ParticleTime >= m_MaxParticleTime)
		{
			delete m_pPickupParticleSprite;
			m_pPickupParticleSprite = nullptr;
		}
	}
}

void Ring::Draw() const
{
	if (!m_Hit)
	{
		m_pRingSprite->Draw();
	}
	else if (m_pPickupParticleSprite != nullptr)
	{
		m_pPickupParticleSprite->Draw();
	}
}

Rectf Ring::GetHitbox()
{
	if (!m_Hit)
	{
		return Rectf{ m_Center.x - m_SpriteSize / 2.f,m_Center.y - m_SpriteSize / 2.f,m_SpriteSize,m_SpriteSize };
	}
	else
	{
		return Rectf{};
	}
}

void Ring::Delete()
{
	delete m_pRingSprite;
	m_pRingSprite = nullptr;
	m_Hit = true;
}


