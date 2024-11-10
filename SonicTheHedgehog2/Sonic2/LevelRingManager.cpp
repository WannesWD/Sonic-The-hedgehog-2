#include "pch.h"
#include "Sprite.h"
#include "LevelRingManager.h"
#include "Ring.h"
#include "utils.h"
#include "StaticAnimatedSprite.h"
#include "SVGParser.h"

LevelRingManager::LevelRingManager(const std::string& ringLocationSVG)
{
	SVGParser::GetVerticesFromSvgFile(ringLocationSVG, m_RingPositions);
	for (int svgIdx = 0; svgIdx < m_RingPositions.size(); svgIdx++)
	{
		for (int ringIdx = 0; ringIdx < m_RingPositions[svgIdx].size(); ringIdx++)
		{
			m_pLevelRings.push_back(new Ring{ m_RingPositions[svgIdx][ringIdx] });
		}
	}
}

LevelRingManager::~LevelRingManager()
{
	for (Ring*& ring : m_pLevelRings)
	{
		delete ring;
		ring = nullptr;
	}
}

void LevelRingManager::Update(float elapsedSec)
{
	for (Ring* ring : m_pLevelRings)
	{
		if (ring != nullptr)
		{
			ring->Update(elapsedSec);
		}
	}
}

void LevelRingManager::Draw() const
{
	for (const Ring* ring : m_pLevelRings)
	{
		if (ring != nullptr)
		{
			ring->Draw();
		}
	}
}

bool LevelRingManager::RingHit(const Rectf& otherHitbox)
{
	for (Ring*& ring : m_pLevelRings)
	{
		if (ring != nullptr)
		{
			if (utils::IsOverlapping(ring->GetHitbox(),otherHitbox))
			{
				ring->Delete();
				return true;
			}
		}
	}


	return false;
}
