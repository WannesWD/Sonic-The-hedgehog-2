#include "pch.h"
#include "Spring.h"
#include "Texture.h"

Texture* Spring::ANGLEDSPRINGSPRITESHEET{ nullptr };
Texture* Spring::STRAIGHTSPRINGSPRITESHEET{ nullptr };
int Spring::INSTANCECOUNTER{ 0 };

Spring::Spring(const Point2f& centerPos, int angle) :
	Interactable(centerPos, 43.f, 42.f, 0.1f, 1, 2, 32.f, 32.f, 0.1f, 1, 2),
	m_Angle{ angle },
	m_MaxSprungTime{ 1.f },
	m_SprungTime{ 0.f },
	m_FrameTime{ 0.1f },
	m_MaxTimesAnimated{ 2 },
	m_TimesAnimated{ 0 },
	m_Angled{ true }
{
	if (INSTANCECOUNTER++ == 0)
	{
		ANGLEDSPRINGSPRITESHEET = new Texture{ "SpriteSheets/CommonObjects/Springs/DiagonalSpring.png" };
		STRAIGHTSPRINGSPRITESHEET = new Texture{ "SpriteSheets/CommonObjects/Springs/HorizontalSpring.png" };
	}
	
	setSpriteTextures(ANGLEDSPRINGSPRITESHEET, STRAIGHTSPRINGSPRITESHEET);

	if (m_Angle % 90 == 0 || m_Angle == 0)
	{
		m_Angled = false;
		SetAngle(float(m_Angle));
	}
	else
	{
		SetAngle(float(m_Angle - 45));
	}

}

Spring::~Spring()
{
	if (--INSTANCECOUNTER == 0)
	{
		delete ANGLEDSPRINGSPRITESHEET;
		delete STRAIGHTSPRINGSPRITESHEET;
	}
}

void Spring::Draw() const
{
	if (m_Angled)
	{
		DrawSprite();
	}
	else
	{
		DrawParticle();
	}
}

void Spring::Update(float elapsedSec)
{

	if ((m_SprungTime > 0.f))
	{
		m_SprungTime += elapsedSec;

		if (m_SprungTime >= m_MaxSprungTime)
		{
			m_SprungTime = 0.f;
		
		}
	}

	if ((m_SprungTime <= 0.f) && IsHit())
	{
		if (m_Angled)
		{
			UpdateSprite(m_FrameTime);
		}
		else
		{
			UpdateParticleSprite(m_FrameTime);
		}

		if (++m_TimesAnimated >= m_MaxTimesAnimated)
		{
			Sethit(false);
			m_TimesAnimated = 0;
		}
		m_SprungTime += elapsedSec;
	}


}

Rectf Spring::GetHitbox()
{
	if (m_Angled)
	{
		return GetSpriteHitbox();
	}
	else
	{
		return GetParticleHitbox();
	}
}

void Spring::SpringHit()
{
	Sethit(true);
}

int Spring::GetSpringAngle()
{
	return m_Angle;
}

