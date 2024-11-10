#include "pch.h"
#include "FloatingPlatform.h"
#include "Texture.h"
#include "utils.h"

Texture* FloatingPlatform::PLATFORMTEXTUREPTR{ nullptr };
int FloatingPlatform::INSTANCECOUNTER{ 0 };
const float
	FloatingPlatform::MAXLERPTIME{ 2.5f },
	FloatingPlatform::MAXLERPDISTANCE{ 100.f };

FloatingPlatform::FloatingPlatform(const PlatformType& platformType,
	const Point2f& bottomLeftPos, bool oppositMovement) :
	m_PlatformType{ platformType },
	m_SpawnPos{ bottomLeftPos },
	m_CurrentPos{ bottomLeftPos }, 
	m_OppositeMovement{ oppositMovement },
	m_LerpFactor{ 0.f },
	m_LerpTime{ 0.f }
{
	if (INSTANCECOUNTER++ == 0)
	{
		PLATFORMTEXTUREPTR = new Texture{ "FullMap/FloatingPlatforms/FloatingPlatform.png" };
	}
}

FloatingPlatform::~FloatingPlatform()
{
	if (--INSTANCECOUNTER == 0)
	{
		delete PLATFORMTEXTUREPTR;
		PLATFORMTEXTUREPTR = nullptr;
	}
}

void FloatingPlatform::Update(float elapsedSec)
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
	float newX{};
	switch (m_PlatformType)
	{
	case FloatingPlatform::horizontal:
		newX = (!m_OppositeMovement) ? utils::Lerp(m_SpawnPos.x, m_SpawnPos.x + MAXLERPDISTANCE, lerpFactor) :
			utils::Lerp(m_SpawnPos.x, m_SpawnPos.x - MAXLERPDISTANCE, lerpFactor);

		m_CurrentPos = Point2f{ newX, m_SpawnPos.y };
		break;
	case FloatingPlatform::vertical:
		m_CurrentPos = Point2f{ m_SpawnPos.x,utils::Lerp(m_SpawnPos.y, m_SpawnPos.y + MAXLERPDISTANCE, lerpFactor) };
		break;
	default:
		break;
	}

}

void FloatingPlatform::Draw() const
{
	PLATFORMTEXTUREPTR->Draw(m_CurrentPos);
}

std::vector<Point2f> FloatingPlatform::GetPlatformSVG()
{
	const float margin{ 5.f };
	Point2f TopLeft{ m_CurrentPos.x,m_CurrentPos.y + PLATFORMTEXTUREPTR->GetHeight() - margin };
	Point2f TopRight{ m_CurrentPos.x + PLATFORMTEXTUREPTR->GetWidth(), 
						m_CurrentPos.y + PLATFORMTEXTUREPTR->GetHeight() - margin};

	return std::vector<Point2f>{TopLeft, TopRight};
}
