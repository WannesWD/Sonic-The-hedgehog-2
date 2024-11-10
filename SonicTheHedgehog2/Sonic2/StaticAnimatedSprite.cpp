#include "pch.h"
#include "StaticAnimatedSprite.h"

StaticAnimatedSprite::StaticAnimatedSprite(Point2f bottomLeftPosition, float frameTime, int numCols, int numRows, Texture* spriteSheetTexturePtr) :
	AnimatedSprite(bottomLeftPosition, frameTime, numCols, numRows),
	m_pSpriteSheet{ spriteSheetTexturePtr }
{}

void StaticAnimatedSprite::Update(float elapsedSec)
{
	AnimatedSprite::Update(elapsedSec);
}

void StaticAnimatedSprite::UpdateFrame(int frameNumber)
{
	if (frameNumber < (m_NumRows * m_NumCols))
	{
		m_CurrentFrame = frameNumber;
	}
}

void StaticAnimatedSprite::Draw() const
{
	AnimatedSprite::Draw(*m_pSpriteSheet);
}

void StaticAnimatedSprite::SetSpriteSheetTexturePtr(Texture* spriteSheet)
{
	m_pSpriteSheet = spriteSheet;
}


