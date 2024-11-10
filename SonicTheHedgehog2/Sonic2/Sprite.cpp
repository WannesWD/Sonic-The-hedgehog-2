#include "pch.h"
#include "Sprite.h"

Sprite::Sprite(Point2f position) :
	Sprite(position, 0.f, 1.f, 1.f)
{
}

Sprite::Sprite(Point2f position, float angle, float xScale, float yScale) :
	m_Position{ position },
	m_Angle{ angle },
	m_XScale{ xScale },
	m_YScale{ yScale }
{
}

void Sprite::SetPos(Point2f newPos)
{
	m_Position = newPos;
}

void Sprite::SetScale(float xScale, float yScale)
{
	m_XScale = xScale;
	m_YScale = yScale;
}

void Sprite::SetAngle(float angle)
{
	m_Angle = angle;
}

void Sprite::ApplyTRS() const
{
	glTranslatef(m_Position.x, m_Position.y, 0.f);
	glRotatef(m_Angle, 0.f, 0.f, 1.f);
	glScalef(m_XScale, m_YScale, 1.f);
}
