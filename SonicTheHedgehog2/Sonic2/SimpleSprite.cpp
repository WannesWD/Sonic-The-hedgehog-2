#include "pch.h"
#include "SimpleSprite.h"

SimpleSprite::SimpleSprite(Point2f position, Texture* sprite, float xScale, float yScale, float angle) :
	Sprite(position),
	m_Sprite{sprite},
	m_XScale{ xScale },
	m_YScale{ yScale },
	m_Angle{ angle }
{}

SimpleSprite::~SimpleSprite()
{
	delete m_Sprite;
	m_Sprite = nullptr;
}

void SimpleSprite::Draw() const
{
	glPushMatrix();

	ApplyTRS();
	m_Sprite->Draw();

	glPopMatrix();
}

float SimpleSprite::GetSpriteHeight() const
{
	return m_Sprite->GetHeight();
}

float SimpleSprite::GetSpriteWidth() const
{
	return m_Sprite->GetWidth();
}

Point2f SimpleSprite::GetPos() const
{
	return m_Position;
}
