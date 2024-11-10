#include "pch.h"
#include "Camera.h"
#include "utils.h"
#include <iostream>

Camera::Camera(float width, float height) :
	m_ZoomScale{ 2.3f },
	m_CamVelocity{ 40.f },
	m_Width{ width / m_ZoomScale },
	m_Height{ height / m_ZoomScale },
	m_NotCenteredX{ false },
	m_NotCenteredY{ false },
	m_CamPos{}
{
}

void Camera::Update(float elapsedSec, const Point2f& target)
{
	float marginWidth{ 24 };
	float marginHeight{ 64 };
	Rectf margin{ m_CamPos.x + m_Width / 2.f - marginWidth / 2.f, m_CamPos.y + m_Height / 2.f - marginHeight / 2.f,
					marginWidth,marginHeight };

	m_CamPos = Track(elapsedSec, target, margin);
	Clamp(m_CamPos);
}

void Camera::SetLevelBoundries(const Rectf& levelBoundaries)
{
	m_LevelBoundaris = levelBoundaries;
}

void Camera::Transform() const
{
	glScalef(m_ZoomScale, m_ZoomScale, 1.f);
	glTranslatef(-m_CamPos.x, -m_CamPos.y, 0.f);
}
void Camera::Draw() const
{
	float marginWidth{ 24 };
	float marginHeight{ 64 };

	Rectf margin{ (m_CamPos.x + m_Width / 2.f) - marginWidth / 2.f, (m_CamPos.y + m_Height / 2.f) - marginHeight / 2.f,
		marginWidth,marginHeight };
	//utils::DrawRect(margin);
}

Point2f Camera::getCameraBottomLeft() const
{
	return m_CamPos;
}

float Camera::getZoomScale() const
{
	return m_ZoomScale;
}


Point2f Camera::Track(float elapseSec, const Point2f& targetCenter, const Rectf& margin)
{
	Point2f marginCenter{ margin.left + margin.width / 2.f, margin.bottom + margin.height / 2.f };
	Point2f camPos{ m_CamPos };
	Point2f camCenter{ (m_CamPos.x + m_Width / 2.f), (m_CamPos.y + m_Height / 2.f) };

	if (targetCenter.x > (marginCenter.x + margin.width / 2.f))
	{
		camPos.x = (targetCenter.x - m_Width / 2.f) - margin.width / 2.f;
		m_NotCenteredX = true;
	}

	else if(targetCenter.x < (marginCenter.x - margin.width / 2.f))
	{
		camPos.x = (targetCenter.x - m_Width / 2.f) + margin.width/2.f;
		m_NotCenteredX = true;
	}

	else if (m_NotCenteredX)
	{
		float newCamPosX{ ((camCenter.x < targetCenter.x) ? m_CamVelocity : -m_CamVelocity) * elapseSec };
		if ((camCenter.x + newCamPosX < targetCenter.x && camCenter.x > targetCenter.x) ||
			(camCenter.x + newCamPosX > targetCenter.x && camCenter.x < targetCenter.x))
		{
			camPos.x = targetCenter.x - m_Width/2.f;
			m_NotCenteredX = false;
		}
		else
		{
			camPos.x += newCamPosX;
		}
	}

	if (targetCenter.y > (marginCenter.y + margin.height / 2.f))
	{
		camPos.y = (targetCenter.y - m_Height / 2.f) - margin.height / 2.f;
		m_NotCenteredY = true;
	}

	else if(targetCenter.y < (marginCenter.y - margin.height / 2.f))
	{
		camPos.y = (targetCenter.y - m_Height / 2.f) + margin.height / 2.f;
		m_NotCenteredY = true;
	}

	if (m_NotCenteredY)
	{
		float newCamPosY{ ((camCenter.y < targetCenter.y) ? m_CamVelocity : -m_CamVelocity) * elapseSec };
		if ((camCenter.y + newCamPosY < targetCenter.y && camCenter.y > targetCenter.y) ||
			(camCenter.y + newCamPosY > targetCenter.y && camCenter.y < targetCenter.y))
		{
			camPos.y = targetCenter.y - m_Height / 2.f;
			m_NotCenteredY = false;
		}
		else
		{
			camPos.y += newCamPosY;
		}
	}

	return Point2f{ camPos };
}

void Camera::Clamp(Point2f& bottomLeftPos) const
{

	bottomLeftPos.x = std::max(bottomLeftPos.x, m_LevelBoundaris.left);
	bottomLeftPos.x = std::min(bottomLeftPos.x, m_LevelBoundaris.width - m_Width);
	bottomLeftPos.y = std::max(bottomLeftPos.y, m_LevelBoundaris.bottom);
	bottomLeftPos.y = std::min(bottomLeftPos.y, m_LevelBoundaris.height - m_Height);

}
