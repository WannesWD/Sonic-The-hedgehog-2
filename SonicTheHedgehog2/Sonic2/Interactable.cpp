#include "pch.h"
#include "Interactable.h"
#include "Sprite.h"
#include "Texture.h"
#include "StaticAnimatedSprite.h"

Interactable::Interactable(const Point2f& centerPos,
	float spriteWidth, float spriteHeight,
	float spriteFrameTime, int spriteRows, int spriteCols,
	float particleWidth, float particleHeight,
	float particleFrameTime, int particleRows, int particleCols)
	: m_Center{ centerPos },
	m_Hit{ false },
	m_Deleted{ false },
	m_Flipped{ false },
	m_ParticleTime{ 0.f },
	m_SpriteWidth{ spriteWidth },
	m_SpriteHeight{ spriteHeight },
	m_SpriteFrameTime{ spriteFrameTime },
	m_ParticleWidth{ particleWidth },
	m_ParticleHeight{ particleHeight },
	m_MaxParticleTime{ particleFrameTime * particleCols * particleRows }

{
	m_SpritePtr = new StaticAnimatedSprite{ Point2f{centerPos.x - spriteWidth / 2.f, centerPos.y - spriteHeight / 2.f  }, spriteFrameTime, spriteCols, spriteRows, nullptr };
	m_ParticleSpritePtr = new StaticAnimatedSprite{ Point2f{centerPos.x - particleWidth / 2.f, centerPos.y - particleHeight / 2.f  }, particleFrameTime, particleCols, particleRows, nullptr};
}

Interactable::~Interactable()
{
	delete m_SpritePtr;
	m_SpritePtr = nullptr;
	delete m_ParticleSpritePtr;
	m_ParticleSpritePtr = nullptr;
	m_Deleted = true;

	m_Hit = true;

}

void Interactable::Update(float elapsedSec)
{
	if (!m_Hit)
	{
		m_SpritePtr->Update(elapsedSec);
	}
	else if (m_ParticleSpritePtr != nullptr)
	{
		m_ParticleSpritePtr->Update(elapsedSec);

		m_ParticleTime += elapsedSec;
		if (m_ParticleTime >= m_MaxParticleTime)
		{
			delete m_ParticleSpritePtr;
			m_ParticleSpritePtr = nullptr;
			m_Deleted = true;
		}
	}
}

void Interactable::Draw() const
{
	if (!m_Hit)
	{
		m_SpritePtr->Draw();
	}
	else if (m_ParticleSpritePtr != nullptr)
	{
		m_ParticleSpritePtr->Draw();
	}
}

Rectf Interactable::GetHitbox()
{
	if (!m_Hit)
	{
		return GetSpriteHitbox();
	}
	else
	{
		return Rectf{};
	}
}

Point2f Interactable::GetCenter()
{
	return m_Center;
}

void Interactable::Delete()
{
	delete m_SpritePtr;
	m_SpritePtr = nullptr;
	m_Hit = true;
}

bool Interactable::IsDeleted()
{
	return m_Deleted;
}

void Interactable::setSpriteTextures(Texture* spriteSheetTexturePtr, Texture* particleSpriteSheetTexturePtr)
{
	setSpriteTexture(spriteSheetTexturePtr);
	setParticleTexture(particleSpriteSheetTexturePtr);
}

void Interactable::setSpriteTexture(Texture* spriteSheetTexturePtr)
{
	dynamic_cast<StaticAnimatedSprite*>(m_SpritePtr)->SetSpriteSheetTexturePtr(spriteSheetTexturePtr);
}

void Interactable::setParticleTexture(Texture* particleSpriteSheetTexturePtr)
{
	dynamic_cast<StaticAnimatedSprite*>(m_ParticleSpritePtr)->SetSpriteSheetTexturePtr(particleSpriteSheetTexturePtr);
}

bool Interactable::IsHit()
{
	return m_Hit;
}

const Point2f& Interactable::GetCenter() const 
{
	return m_Center;
}

void Interactable::FlipSprite()
{
	if (!m_Flipped)
	{
		m_SpritePtr->SetScale(-1.f,1.f);
		m_Flipped = true;
	}
	else
	{
		m_SpritePtr->SetScale(1.f, 1.f);
		m_Flipped = false;
	}
}

void Interactable::SetCenter(Point2f newCenter)
{
	m_Center = newCenter;
	m_SpritePtr->SetPos(Point2f{ newCenter.x - m_SpriteWidth / 2.f, newCenter.y - m_SpriteHeight / 2.f });
	m_ParticleSpritePtr->SetPos(Point2f{ newCenter.x - m_SpriteWidth / 2.f, newCenter.y - m_SpriteHeight / 2.f });
}

void Interactable::SetAngle(float angle)
{
	m_SpritePtr->SetAngle(angle);
	m_ParticleSpritePtr->SetAngle(angle);
}

void Interactable::Sethit(bool hit)
{
	m_Hit = hit;
}

void Interactable::UpdateSprite(float elapsedSec)
{
	m_SpritePtr->Update(elapsedSec);
}

void Interactable::UpdateParticleSprite(float elapsedSec)
{
	m_ParticleSpritePtr->Update(elapsedSec);
}

void Interactable::DrawSprite() const
{
	m_SpritePtr->Draw();
}

void Interactable::DrawParticle() const
{
	m_ParticleSpritePtr->Draw();
}

Rectf Interactable::GetSpriteHitbox()
{
	return Rectf{ m_Center.x - m_SpriteWidth / 2.f,m_Center.y - m_SpriteHeight / 2.f,
					m_SpriteWidth,m_SpriteHeight };
}

Rectf Interactable::GetParticleHitbox()
{
	return Rectf{ m_Center.x - m_ParticleWidth / 2.f,m_Center.y - m_ParticleHeight / 2.f,
				m_ParticleWidth,m_ParticleHeight };
}
