#include "pch.h"
#include "CheckPoint.h"
#include "SoundManager.h"
#include "Texture.h"

Texture* CheckPoint::SPRITESHEETTEXTUREPTR{ nullptr };
Texture* CheckPoint::PARTICLETEXTUREPTR{ nullptr };
int CheckPoint::INSTANCECOUNTER{ 0 };
const float CheckPoint::MAXANIMATIONTIME{ 0.05f * 2 * 8 };

CheckPoint::CheckPoint(const Point2f& centerPos) :
	Interactable{ centerPos,40.f,64.f,0.05f,2,8,16,64,0.1f,1,2 },
	m_Collected{ false },
	m_AnimationTime{ 0.f }
{
	if (INSTANCECOUNTER < 1)
	{
		SPRITESHEETTEXTUREPTR = new Texture{ "SpriteSheets/CommonObjects/CheckPoint/CheckPointRotation.png" };
		PARTICLETEXTUREPTR = new Texture{ "SpriteSheets/CommonObjects/CheckPoint/CheckPointCollected.png" };
	}
	INSTANCECOUNTER++;
	setSpriteTextures(SPRITESHEETTEXTUREPTR, PARTICLETEXTUREPTR);
}

CheckPoint::~CheckPoint()
{
	if (--INSTANCECOUNTER < 1)
	{
		delete SPRITESHEETTEXTUREPTR;
		SPRITESHEETTEXTUREPTR = nullptr;
		delete PARTICLETEXTUREPTR;
		PARTICLETEXTUREPTR = nullptr;
	}
}
 
void CheckPoint::Update(float elapsedSec)
{
	if (m_Collected)
	{
		UpdateSprite(elapsedSec);
		
		if (m_AnimationTime >= MAXANIMATIONTIME)
		{
			Sethit(true);
			UpdateParticleSprite(elapsedSec);
		}
		else
		{
			m_AnimationTime += elapsedSec;
		}
	}

}

void CheckPoint::Delete()
{
	m_Collected = true;
}




