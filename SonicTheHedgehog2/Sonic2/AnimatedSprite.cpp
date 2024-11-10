#include "pch.h"
#include "AnimatedSprite.h"
#include "Texture.h"

AnimatedSprite::AnimatedSprite(Point2f position, float frameTime, int numCols, int numRows) :
	Sprite(position),
	m_FrameTime{ frameTime },
	m_NumCols{ numCols },
	m_NumRows{ numRows },
	m_TimeAnimated{ 0.f },
	m_CurrentFrame{ 0 }
{
}

void AnimatedSprite::Update(float elapsedSec)
{
	m_TimeAnimated += elapsedSec;
	if (m_TimeAnimated >= m_FrameTime)
	{
		++m_CurrentFrame %= (m_NumCols * m_NumRows);
		m_TimeAnimated -= m_FrameTime;
	}
}

void AnimatedSprite::Draw(const Texture& sprite) const
{
	Rectf srcRect{0.f,0.f,sprite.GetWidth()/m_NumCols, sprite.GetHeight()/m_NumRows};
	
	srcRect.left = srcRect.width * (m_CurrentFrame % m_NumCols);
	srcRect.bottom = (srcRect.height * ((m_CurrentFrame / m_NumCols) + 1));

	glPushMatrix();

	glTranslatef(srcRect.width / 2.f, srcRect.height / 2.f, 0.f);
	ApplyTRS();
	glTranslatef(-((srcRect.width / 2.f) + m_Position.x), -((srcRect.height / 2.f) + m_Position.y), 0.f);

	sprite.Draw(m_Position, srcRect);

	glPopMatrix();
}
