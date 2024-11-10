#include "pch.h"
#include "StaticAnimatedSprite.h"
#include "Bridge.h"
#include "Texture.h"
#include "utils.h"

std::vector<Texture*> Bridge::m_BridgeSpriteSheetVector{}; //memory leak message because of this vector
int Bridge::INSTANCECOUNTER{ 0 };
const int Bridge::NUMBRIDGEVARIANTS{ 4 };

Bridge::Bridge(const Point2f& centerPos, const NumLogs& numLogs) :
	m_NumLogs{ numLogs }
{
	if (++INSTANCECOUNTER == 1)
	{
		InitSpriteSheetVector();
	}

	InitBridgeSpriteAndHitbox(centerPos);
}

Bridge::~Bridge()
{
	delete m_BridgeSprite;
	m_BridgeSprite = nullptr;

	if (--INSTANCECOUNTER < 1)
	{
		for (Texture*& m_BridgeSpriteSheet : m_BridgeSpriteSheetVector)
		{
			delete m_BridgeSpriteSheet;
			m_BridgeSpriteSheet = nullptr;
		}
	}
}

void Bridge::Draw()
{
	m_BridgeSprite->Draw();
}

void Bridge::Update(const Rectf& playerHitbox, bool playerOnground)
{
	StaticAnimatedSprite* bridgeSprite{ dynamic_cast<StaticAnimatedSprite*>(m_BridgeSprite) };
	if (utils::IsOverlapping(playerHitbox,m_Hitbox))
	{
		const int numAnchorPoints{ 2 };
		int numSpriteSegments{};
		switch (m_NumLogs)
		{
		case Bridge::eight:
			numSpriteSegments = 8;
			break;
		case Bridge::ten:
			numSpriteSegments = 10;
			break;
		case Bridge::twelve:
			numSpriteSegments = 12;
			break;
		case Bridge::sixteen:
			numSpriteSegments = 16;
			break;
		default:
			break;
		}
		numSpriteSegments += numAnchorPoints;

		Point2f playerCenter{ playerHitbox.left + playerHitbox.width / 2.f,playerHitbox.bottom + playerHitbox.height / 2.f };
		
		for (int segmentIdx = 0; segmentIdx < numSpriteSegments; segmentIdx++)
		{
			float maxSpriteSegmentPosition{ m_Hitbox.left + ((1 + segmentIdx) * (m_Hitbox.width / numSpriteSegments)) };
			float minSpriteSegmentPosition{ m_Hitbox.left + (segmentIdx * (m_Hitbox.width / numSpriteSegments)) };

			if (playerCenter.x < maxSpriteSegmentPosition && playerCenter.x > minSpriteSegmentPosition)
			{
				if (segmentIdx > numSpriteSegments/2.f)
				{
					bridgeSprite->SetScale(-1.f, 1.f);
					bridgeSprite->UpdateFrame(numSpriteSegments - segmentIdx);
				}
				else
				{
					bridgeSprite->SetScale(1.f, 1.f);
					bridgeSprite->UpdateFrame(segmentIdx);
				}
				
			}
		}
	}
	else
		bridgeSprite->UpdateFrame(0);
}

void Bridge::InitSpriteSheetVector()
{
	const int minNumLogs{ 8 };
	const int maxNumLogs{ 16 };

	for (int bridgeIdx = 0; bridgeIdx < NUMBRIDGEVARIANTS; bridgeIdx++)
	{
		int numLogs{ (bridgeIdx == (NUMBRIDGEVARIANTS - 1)) ? maxNumLogs :
						(minNumLogs + (bridgeIdx * 2))};
		if (bridgeIdx < m_BridgeSpriteSheetVector.size() && m_BridgeSpriteSheetVector[bridgeIdx] == nullptr)
		{
			m_BridgeSpriteSheetVector[bridgeIdx] = new Texture{"FullMap/Bridges/" + std::to_string(numLogs) + "LogBridgeSprite.png" };
		}
		else
		{
			m_BridgeSpriteSheetVector.push_back(new Texture{ "FullMap/Bridges/" + std::to_string(numLogs) + "LogBridgeSprite.png" });
		}
	}

}

void Bridge::InitBridgeSpriteAndHitbox(const Point2f& centerPos)
{
	const int numCols{ 1 };
	const float frameTime{ 0.f }; 
	std::vector<int>numRows{ 5, 6, 7, 9 };

	const float spriteWidth{ m_BridgeSpriteSheetVector[m_NumLogs]->GetWidth() / numCols };
	const float spriteHeight{ m_BridgeSpriteSheetVector[m_NumLogs]->GetHeight() / numRows[m_NumLogs] };

	InitHitbox(centerPos, spriteWidth, spriteHeight);

	m_BridgeSprite = new StaticAnimatedSprite{ Point2f{m_Hitbox.left,m_Hitbox.bottom},
				frameTime,numCols,numRows[int(m_NumLogs)], m_BridgeSpriteSheetVector[int(m_NumLogs)] };
}

void Bridge::InitHitbox(const Point2f centerPos, float spriteWidth, float spriteHeight)
{
	Point2f bottomleft{ centerPos.x - spriteWidth / 2.f,centerPos.y - spriteHeight / 2.f };
	m_Hitbox = Rectf{ bottomleft.x, bottomleft.y, spriteWidth,spriteHeight };
}
