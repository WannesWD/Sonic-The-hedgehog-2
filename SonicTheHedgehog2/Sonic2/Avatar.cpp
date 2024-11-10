#include "pch.h"
#include "Avatar.h"
#include "Texture.h"


Avatar::Avatar(const std::vector<Texture*>& animationSprites,
	const std::vector<float>& frameTimePerSprite,
	const std::vector<int>& numColsPerSprite,
	const std::vector<int>& numRowsPerSprite,
	int currentSpriteIndex) :

	AnimatedSprite(Point2f{}, frameTimePerSprite[currentSpriteIndex],
		numColsPerSprite[currentSpriteIndex], numRowsPerSprite[currentSpriteIndex]),
	m_pAnimationSprites{ animationSprites },
	m_FrameTimePerSprite{ frameTimePerSprite },
	m_NumColsPerSprite{ numColsPerSprite },
	m_NumRowsPerSprite{ numRowsPerSprite },
	m_CurrentSpriteIndex{ currentSpriteIndex }
{
}

Avatar::~Avatar()
{
	for (Texture*& pAnimationSprite : m_pAnimationSprites)
	{
		delete pAnimationSprite;
	}
}

void Avatar::Draw() const
{
	AnimatedSprite::Draw(*m_pAnimationSprites[m_CurrentSpriteIndex]);
}

void Avatar::UpdateFrameTime(float newFrameTime)
{
	m_FrameTime = newFrameTime;
}

void Avatar::UpdateAnimationSpriteIndex(int newSpriteIndex)
{
	m_CurrentSpriteIndex = newSpriteIndex;
	m_NumCols = m_NumColsPerSprite[m_CurrentSpriteIndex];
	m_NumRows = m_NumRowsPerSprite[m_CurrentSpriteIndex];
	m_FrameTime = m_FrameTimePerSprite[m_CurrentSpriteIndex];
}

void Avatar::Update(float elapsedSec)
{
	AnimatedSprite::Update(elapsedSec);
}

void Avatar::SetPos(Point2f center)
{
	float srcRectWidth{ m_pAnimationSprites[m_CurrentSpriteIndex]->GetWidth() / m_NumCols };
	float srcRectHeight{ m_pAnimationSprites[m_CurrentSpriteIndex]->GetHeight() / m_NumRows };
	Sprite::SetPos(Point2f{ center.x - (srcRectWidth / 2.f), center.y - (srcRectHeight / 2.f) });
}

Point2f Avatar::GetCenter()
{
	float centerX{ m_pAnimationSprites[m_CurrentSpriteIndex]->GetWidth() / m_NumCols };
	float centerY{ m_pAnimationSprites[m_CurrentSpriteIndex]->GetHeight() / m_NumRows };
	return Point2f(m_Position.x + centerX/2.f, m_Position.y + centerY/2.f);
}
