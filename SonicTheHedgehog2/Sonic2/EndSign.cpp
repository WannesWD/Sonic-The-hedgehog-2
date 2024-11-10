#include "pch.h"
#include "EndSign.h"
#include "Texture.h"

Texture* EndSign::STATIONARYSPRITEPTR{ nullptr };
Texture* EndSign::ROTATINGSPRITEPTR{ nullptr };
int EndSign::INSTANCECOUNTER{ 0 };

EndSign::EndSign(const Point2f& centerPos) :
	m_StationaryFrameTime{ 2.f },
	Interactable{ centerPos, 48.f, 48.f, m_StationaryFrameTime, 1, 2,
					48.f, 48.f, 0.05f, 1, 3 },
	m_Collected{ false },
	m_MaxRotatingTime{ 0.05f * 30.f },
	m_AnimationTime{},
	m_Finished{ false }
{
	if (INSTANCECOUNTER < 1)
	{
		STATIONARYSPRITEPTR = new Texture{ "SpriteSheets/CommonObjects/EndSign/StationarySign.png" };
		ROTATINGSPRITEPTR = new Texture{ "SpriteSheets/CommonObjects/EndSign/RotatingSign.png" };
	}
	INSTANCECOUNTER++;
	setSpriteTextures(STATIONARYSPRITEPTR, ROTATINGSPRITEPTR);
}

EndSign::~EndSign()
{
	if (--INSTANCECOUNTER < 1)
	{
		delete STATIONARYSPRITEPTR;
		STATIONARYSPRITEPTR = nullptr;
		delete ROTATINGSPRITEPTR;
		ROTATINGSPRITEPTR = nullptr;
	}
}

void EndSign::Update(float elapsedSec)
{
	if (m_Collected && !m_Finished)
	{
		if (m_AnimationTime <= m_MaxRotatingTime)
		{
			Sethit(true);
			UpdateParticleSprite(elapsedSec);
			m_AnimationTime += elapsedSec;
		}
		else
		{
			Sethit(false);
			UpdateSprite(m_StationaryFrameTime);
			m_Finished = true;
		}
	}
}

void EndSign::Delete()
{
	m_Collected = true;
}

bool EndSign::HasFinished() const
{
	return m_Finished;
}
