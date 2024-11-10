#include "pch.h"
#include "PlayerPhysics.h"
#include "Level.h"
#include "Matrix2x3.h"
#include <iostream>

PlayerPhysics::PlayerPhysics(Vector2f velocity, Rectf hitBox) :
	m_GroundVelocity{ velocity },
	m_Velocity{ Vector2f{} },
	m_HitBox{ hitBox },
	m_GroundNormal{ Vector2f{} },
	m_GroundAngle{ 0.f },
	m_OnGround{ false },
	m_DirectionMode{ ground },
	m_Gravity{ -21.875f * 60.f },
	m_PrevDirectionMode{ m_DirectionMode },
	m_appliedGravity{ 0.f },
	m_TotalRotationAngle{ 0.f },
	m_FacingRight{ true },
	m_OnBackground{ false },
	m_Jumping{ false },
	m_Hit{ false },
	m_HitSpring{ false },
	m_OnCorkscrew{ false },
	m_falling{ false },
	m_HitApex{ false },
	m_Dead{ false },
	m_ControlLockTimer{ 0.f },
	m_GroundedHitboxHeight{ hitBox.height },
	m_JumpingHitboxHeight{ hitBox.height * 0.6f }
{
	m_Center.x = m_HitBox.left + (m_HitBox.width / 2.f);
	m_Center.y = m_HitBox.bottom + (m_HitBox.height / 2.f);

	Point2f topLeftVertex{ m_HitBox.left, m_HitBox.bottom + m_HitBox.height };
	Point2f bottomLeftVertex{ m_HitBox.left, m_HitBox.bottom };

	Point2f topRightVertex{ m_HitBox.left + m_HitBox.width, m_HitBox.bottom + m_HitBox.height };
	Point2f bottomRightVertex{ m_HitBox.left + m_HitBox.width, m_HitBox.bottom };
	m_HitBoxVertices = std::vector<Point2f>{ topLeftVertex, bottomLeftVertex, bottomRightVertex, topRightVertex };
}

void PlayerPhysics::Update(float elapsedSec, Level* level)
{
	m_OnGround = (m_Dead) ? false : m_OnGround;
	if (m_OnGround)
	{
		groundedUpdate(elapsedSec, level);
	}
	else
	{
		aerialUpdate(elapsedSec, level);
	}

	if (m_OnGround)
	{
		m_GroundVelocity.y = 0.f;
		m_appliedGravity = 0.f;
	}

	if (m_HitSpring)
	{
		m_ControlLockTimer = 25.f;
		m_Jumping = false;
		m_HitSpring = false;
	}
	
	for (Point2f& vertex : m_HitBoxVertices)
	{
		vertex.x += m_Velocity.x;
		vertex.y += m_Velocity.y;
	}

	m_Center = Point2f{};

	for (Point2f vertex : m_HitBoxVertices)
	{
		m_Center.x += vertex.x;
		m_Center.y += vertex.y;
	}

	m_Center.x /= m_HitBoxVertices.size();
	m_Center.y /= m_HitBoxVertices.size();

	if (!HitboxIsValid())
	{
		Point2f topLeftVertex{ m_Center.x - m_HitBox.width/2.f, m_Center.y + m_HitBox.height/2.f };
		Point2f bottomLeftVertex{ m_Center.x - m_HitBox.width/2.f, m_Center.y - m_HitBox.height/2.f };

		Point2f topRightVertex{ m_Center.x + m_HitBox.width/2.f, m_Center.y + m_HitBox.height/2.f };
		Point2f bottomRightVertex{ m_Center.x + m_HitBox.width / 2.f, m_Center.y + m_HitBox.height/2.f };
		m_HitBoxVertices = std::vector<Point2f>{ topLeftVertex, bottomLeftVertex, bottomRightVertex, topRightVertex };
	}
} 



Point2f PlayerPhysics::GetCenter() const
{
	return m_Center;
}

bool PlayerPhysics::GetOnGround() const
{
	return m_OnGround;
}

Rectf PlayerPhysics::GetHitBox() const
{
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		return Rectf{ m_HitBoxVertices[1],m_HitBoxVertices[3] };
		break;
	case PlayerPhysics::rightWall:
		return Rectf{ m_HitBoxVertices[0],m_HitBoxVertices[2] };
		break;
	case PlayerPhysics::ceiling:
		return Rectf{ m_HitBoxVertices[3],m_HitBoxVertices[1] };
		break;
	case PlayerPhysics::leftWall:
		return Rectf{ m_HitBoxVertices[2],m_HitBoxVertices[0] };
		break;
	default:
		break;
	}
	return Rectf{};
}

void PlayerPhysics::setPosition(Point2f centerPos)
{
	m_HitBox.height = m_GroundedHitboxHeight;
	m_Center = centerPos;
	UpdateHitboxVertices();
}

void PlayerPhysics::groundedUpdate(float elapsedSec, Level* level)
{

	m_OnGround = IsOnGround(level);
	HandleGroundCollision(level);
	SetAngle();
	HandleSlopeSlipping(elapsedSec);

	if (m_OnGround && m_falling)
	{
		HandleSlopeLanding(elapsedSec);
	}
	m_falling = false;

	m_PrevDirectionMode = m_DirectionMode;
	UpdateDirectionMode();
	HandleWallCollision(level);

	if (m_PrevDirectionMode != m_DirectionMode)
	{
		UpdateHitBoxOrientation(m_HitBoxVertices);
	}

	if (m_DirectionMode != ground)
	{
		const float sloapFactor{ (0.125 * 100.f) * 60.f };
		m_GroundVelocity.x -= (sloapFactor * sinf(m_GroundAngle)) * elapsedSec;
	}

	SetVelocity(elapsedSec);

}

void PlayerPhysics::aerialUpdate(float elapsedSec, Level* level)
{

	HandleWallCollision(level);

	HandleCeilingCollision(level);

	if (!m_Dead)
	{
		m_OnGround = IsOnGround(level, true);
	}

	if (m_Jumping && m_Velocity.y < 0.f)
	{
		m_falling = true;
		m_PrevDirectionMode = m_DirectionMode;
		m_DirectionMode = ground;
	}

	if (!m_Jumping)
	{
		const float groundAngle{ (m_GroundAngle / float(M_PI) * 180) };
		if (groundAngle > 45.f && groundAngle < 315.f)
		{
			UpdateFallingAngle(elapsedSec, groundAngle);
		}

		if (!m_falling && m_Velocity.y < 0.f)
		{
			m_TotalRotationAngle = 0.f; //needs to be triggered once at beginning of launch
			m_PrevDirectionMode = m_DirectionMode;
			m_DirectionMode = ground;
			//m_appliedGravity = 0.f;
			m_falling = true;
			m_HitApex = true;
		}
	}

	if (m_PrevDirectionMode != m_DirectionMode)
	{
		UpdateHitBoxOrientation(m_HitBoxVertices);
		m_PrevDirectionMode = m_DirectionMode;
	}
	
	SetVelocity(elapsedSec);

	m_appliedGravity += m_Gravity * elapsedSec;
	m_Velocity.y += m_appliedGravity * elapsedSec;
}

void PlayerPhysics::UpdateDirectionMode()
{
	const float groundAngle{ (m_GroundAngle / float(M_PI) * 180) };
	if (groundAngle < 45.f || groundAngle > 315.f)
	{
		m_DirectionMode = ground;
	}
	if (groundAngle > 45.f && groundAngle < 135.f)
	{
		m_DirectionMode = rightWall;
	}
	if (groundAngle > 135.f && groundAngle < 225.f)
	{
		m_DirectionMode = ceiling;
	}
	if (groundAngle > 225.f && groundAngle < 315.f)
	{
		m_DirectionMode = leftWall;
	}
}

void PlayerPhysics::UpdateHitBoxOrientation(std::vector<Point2f>& hitBoxVertices)
{
	float nextHitBoxAngle{ 90.f };
	float prevHitBoxAngle{ -90.f };
	float hitBoxAngle{};
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		if (m_PrevDirectionMode == ceiling)
		{
			hitBoxAngle = 180.f;
			break;
		}
		hitBoxAngle = (m_PrevDirectionMode == leftWall) ? nextHitBoxAngle : prevHitBoxAngle;
		break;
	case PlayerPhysics::rightWall:
		hitBoxAngle = (m_PrevDirectionMode == ground) ? nextHitBoxAngle : prevHitBoxAngle;
		break;
	case PlayerPhysics::ceiling:
		hitBoxAngle = (m_PrevDirectionMode == rightWall) ? nextHitBoxAngle : prevHitBoxAngle;
		break;
	case PlayerPhysics::leftWall:
		hitBoxAngle = (m_PrevDirectionMode == ceiling) ? nextHitBoxAngle : prevHitBoxAngle;
		break;
	default:
		break;
	}

	hitBoxVertices = getRotatedVertices(hitBoxAngle, hitBoxVertices,
		GetCenter().x,GetCenter().y);
}

std::vector<Point2f> PlayerPhysics::getRotatedVertices(float angle, const std::vector<Point2f>& originalVertices, float xTranslate, float yTranslate) const
{
	Matrix2x3 resetMatrix{ Matrix2x3::CreateTranslationMatrix(xTranslate, yTranslate) };
	Matrix2x3 rotateMatrix{ Matrix2x3::CreateRotationMatrix(angle) };
	Matrix2x3 centerMatrix{ Matrix2x3::CreateTranslationMatrix(-xTranslate,-yTranslate) };

	Matrix2x3 compoundMatrix{ resetMatrix * rotateMatrix * centerMatrix };
	return compoundMatrix.Transform(originalVertices);
}


bool PlayerPhysics::IsOnGround(Level* level, bool aerialCheck)
{
	float sensorLength{};
	if (m_GroundVelocity.y == 0.f)
	{
		sensorLength = std::min(abs(m_Velocity.x / 100.f) + 4.f, 14.f);
	}
	else
	{
		sensorLength = (m_Velocity.y / 100.f) + 8;
	}
	if (abs(m_GroundVelocity.y) > 0.f)
	{
		sensorLength = -1.f;
	}

	float sensorAngle{};
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		sensorAngle = float(M_PI / 2.f);
		break;
	case PlayerPhysics::rightWall:
		sensorAngle = float(M_PI);
		break;
	case PlayerPhysics::ceiling:
		sensorAngle = float(3.f * M_PI / 2.f);
		break;
	case PlayerPhysics::leftWall:
		sensorAngle = float(0.f);
		break;
	default:
		break;
	}

	Point2f topLeftSensor{};
	Point2f topRightSensor{};
	Point2f bottomLeftSensor{};
	Point2f bottomRightSensor{};

	topLeftSensor.x = m_HitBoxVertices[1].x + m_HitBox.height / 2.f * cosf(sensorAngle);
	topLeftSensor.y = m_HitBoxVertices[1].y + m_HitBox.height / 2.f * sinf(sensorAngle);

	topRightSensor.x = m_HitBoxVertices[2].x + m_HitBox.height / 2.f * cosf(sensorAngle);
	topRightSensor.y = m_HitBoxVertices[2].y + m_HitBox.height / 2.f * sinf(sensorAngle);

	bottomLeftSensor.x = m_HitBoxVertices[1].x - sensorLength * cosf(sensorAngle);
	bottomLeftSensor.y = m_HitBoxVertices[1].y - sensorLength * sinf(sensorAngle);

	bottomRightSensor.x = m_HitBoxVertices[2].x - sensorLength * cosf(sensorAngle);
	bottomRightSensor.y = m_HitBoxVertices[2].y - sensorLength * sinf(sensorAngle);

	Point2f intersectingPoint{};

	utils::HitInfo leftSensorHitInfo{};
	utils::HitInfo rightSensorHitInfo{};
	
	bool hitLeft{ level->GetHitInfo(m_Velocity, m_GroundVelocity, topLeftSensor, bottomLeftSensor, leftSensorHitInfo) };
	bool hitRight{ level->GetHitInfo(m_Velocity, m_GroundVelocity, topRightSensor, bottomRightSensor, rightSensorHitInfo) };

	if ((hitLeft || hitRight) && (!aerialCheck))
	{
		return true;
	}
	if (aerialCheck && (hitLeft || hitRight))
	{
		if (m_DirectionMode == ground)
		{
			return true;
		}

		return false;
	}

	return false;
}

bool PlayerPhysics::HitboxIsValid()
{
	Point2f hitboxVertex{ m_HitBoxVertices[0] };
	for (int hitboxIdx = 1; hitboxIdx < m_HitBoxVertices.size(); hitboxIdx++)
	{
		if (hitboxVertex.x == m_HitBoxVertices[hitboxIdx].x &&
			hitboxVertex.y == m_HitBoxVertices[hitboxIdx].y)
		{
			return false;
		}
	}
	return true;
}

void PlayerPhysics::UpdateHitboxVertices()
{
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		m_HitBoxVertices[0] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		m_HitBoxVertices[1] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		m_HitBoxVertices[2] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		m_HitBoxVertices[3] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		break;
	case PlayerPhysics::rightWall:
		m_HitBoxVertices[0] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		m_HitBoxVertices[1] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		m_HitBoxVertices[2] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		m_HitBoxVertices[3] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		break;
	case PlayerPhysics::ceiling:
		m_HitBoxVertices[0] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		m_HitBoxVertices[1] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		m_HitBoxVertices[2] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		m_HitBoxVertices[3] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		break;
	case PlayerPhysics::leftWall:
		m_HitBoxVertices[0] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		m_HitBoxVertices[1] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y + m_HitBox.height / 2.f };
		m_HitBoxVertices[2] = Point2f{ m_Center.x - m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		m_HitBoxVertices[3] = Point2f{ m_Center.x + m_HitBox.width / 2.f,m_Center.y - m_HitBox.height / 2.f };
		break;
	default:
		break;
	}
}

void PlayerPhysics::SetAngle()
{
	m_GroundAngle = (atan2f(m_GroundNormal.y, m_GroundNormal.x) - float(M_PI / 2.f));
	if (m_GroundNormal == Vector2f{})
	{
		m_GroundAngle = 0.f;
	}
	if (m_GroundAngle < 0.f)
	{
		m_GroundAngle = float(2.f * M_PI) + m_GroundAngle;
	}
}

void PlayerPhysics::SetVelocity(float elapsedSec)
{
	const float m_MaxHorizontalVelocity{ 600.f };
	if (abs(m_GroundVelocity.x) > m_MaxHorizontalVelocity)
	{
		m_GroundVelocity.x = m_MaxHorizontalVelocity * (abs(m_GroundVelocity.x) / m_GroundVelocity.x);
	}

	m_Velocity.x = m_GroundVelocity.x * elapsedSec * cosf(m_GroundAngle);
	m_Velocity.y = m_GroundVelocity.x * elapsedSec * sinf(m_GroundAngle);

	m_Velocity.x += m_GroundVelocity.y * elapsedSec * cosf(m_GroundAngle + float(M_PI / 2.f));
	m_Velocity.y += m_GroundVelocity.y * elapsedSec * sinf(m_GroundAngle + float(M_PI / 2.f));
}

void PlayerPhysics::UpdateFallingAngle(float elapsedSec, float groundAngle)
{
	const float aerialRotationAngle{ 0.0490873852f * 60.f };
	if (m_HitApex)
	{
		m_HitApex = false;
	}

	m_TotalRotationAngle += aerialRotationAngle * elapsedSec;

	if (m_GroundAngle < float(M_PI))
	{
		if (m_TotalRotationAngle >= (float(M_PI)/10))
		{
			m_GroundAngle -= (float(M_PI) / 10);
			m_TotalRotationAngle = 0.f;
		}
	}
	else if (m_GroundAngle > float(M_PI))
	{
		if (m_TotalRotationAngle >= (float(M_PI) / 10))
		{
			m_GroundAngle += (float(M_PI) / 10);
			m_TotalRotationAngle = 0.f;
		}
	}

	if (m_GroundAngle == float(M_PI))
	{
		m_GroundAngle = 0.f;
	}
}

void PlayerPhysics::HandleSlopeSlipping(float elapsedSec)
{
	//code based on example provided for slope physics in https://info.sonicretro.org/SPG:Slope_Physics#Falling_and_Slipping_Down_Slopes
	float groundAngle{ (m_GroundAngle / float(M_PI) * 180) };
	const float minGroundVelocity{ 250.f };

	if (m_OnGround)
	{
		if (m_ControlLockTimer == 0.f)
		{
			if (abs(m_GroundVelocity.x) < minGroundVelocity && (groundAngle >= 45.f && groundAngle <= 315.f))
			{
				m_OnGround = false;
				m_falling = true;
				m_GroundVelocity.x = 0.f;
				m_ControlLockTimer = 100.f;
			}
		}
		else
		{
			m_ControlLockTimer -= 1.f;
			if (m_ControlLockTimer <= 0.f)
			{
				m_falling = false;
				m_ControlLockTimer = 0.f;
			}
		}
	}
}

void PlayerPhysics::HandleSlopeLanding(float elapsedSec)
{
	//code based info provided for slope physics in https://info.sonicretro.org/SPG:Slope_Physics#Falling_and_Slipping_Down_Slopes
	const float velocityScalar{ 100.f };
	const float groundAngle{ (m_GroundAngle / float(M_PI) * 180) };
	if (groundAngle <= 23.f || groundAngle > 339.f)
	{
		m_GroundVelocity.x = m_GroundVelocity.x;
		if (m_ControlLockTimer != 0.f && (abs(groundAngle) > 1.f))
		{
			m_GroundVelocity.x = m_Velocity.y * 0.5f * (sinf(m_GroundAngle) / abs(sinf(m_GroundAngle)));
			m_GroundVelocity.x *= velocityScalar;
		}
	}
	else if (groundAngle <= 45.f || groundAngle >= 316.f)
	{
		m_GroundVelocity.x = m_Velocity.y * 0.5f * (sinf(m_GroundAngle)/abs(sinf(m_GroundAngle)));
		m_GroundVelocity.x *= velocityScalar;
	}
	else if (groundAngle <= 90.f || groundAngle >= 271.f)
	{
		m_GroundVelocity.x = m_Velocity.y * (sinf(m_GroundAngle) / abs(sinf(m_GroundAngle)));
		m_GroundVelocity.x *= velocityScalar;
	}
}

void PlayerPhysics::SetWallSensorEnd(Point2f& wallSensorEnd, float sensorLength, float sensorAngle, bool facingRight)
{
	if (m_FacingRight)
	{
		wallSensorEnd.x = m_Center.x + (sensorLength * cosf(sensorAngle));
		wallSensorEnd.y = m_Center.y + (sensorLength * sinf(sensorAngle));
	}
	else if (!m_FacingRight)
	{
		wallSensorEnd.x = m_Center.x - (sensorLength * cosf(sensorAngle));
		wallSensorEnd.y = m_Center.y - (sensorLength * sinf(sensorAngle));
	}
}

void PlayerPhysics::HandleWallCollision(Level* level)
{
	Point2f wallSensorEnd{};
	float sensorAngle{};
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		sensorAngle = 0.f;
		break;
	case PlayerPhysics::rightWall:
		sensorAngle = float(M_PI / 2.f);
		break;
	case PlayerPhysics::ceiling:
		sensorAngle = float(M_PI);
		break;
	case PlayerPhysics::leftWall:
		sensorAngle = float(3.f * M_PI / 2.f);
		break;
	default:
		break;
	}

	float extrudingSensorLength{ 3.f };
	float sensorLength{ (m_HitBox.width / 2.f) + extrudingSensorLength};

	if (m_Velocity.x > 0.f)
	{
		SetWallSensorEnd(wallSensorEnd, sensorLength, sensorAngle, true);
	}
	else if(m_Velocity.x < 0.f)
	{
		SetWallSensorEnd(wallSensorEnd, sensorLength, sensorAngle, false);
	}
	else
	{
		SetWallSensorEnd(wallSensorEnd, sensorLength, sensorAngle, m_FacingRight);
	}

	utils::HitInfo wallHitInfo{};
	if (level->GetHitInfo(m_Velocity, m_GroundVelocity, m_Center, wallSensorEnd, wallHitInfo, m_OnBackground, true))
	{
		if (m_GroundVelocity.x < 0.f)
		{
			for (int vertexIdx{ 0 }; vertexIdx < m_HitBoxVertices.size(); vertexIdx++)
			{
				m_HitBoxVertices[vertexIdx].x = (vertexIdx < 2) ? wallHitInfo.intersectPoint.x + (extrudingSensorLength * cosf(sensorAngle)) :
																wallHitInfo.intersectPoint.x + ((extrudingSensorLength + m_HitBox.width) * cosf(sensorAngle));
			}
		}
		else if(m_GroundVelocity.x > 0.f)
		{
			for (int vertexIdx{ 0 }; vertexIdx < m_HitBoxVertices.size(); vertexIdx++)
			{
				m_HitBoxVertices[vertexIdx].x = (vertexIdx > 1) ? wallHitInfo.intersectPoint.x - (extrudingSensorLength * cosf(sensorAngle)) :
					wallHitInfo.intersectPoint.x - ((extrudingSensorLength + m_HitBox.width) * cosf(sensorAngle));
			}
		}
		m_GroundVelocity.x = 0.f;
	}
}

void PlayerPhysics::HandleCeilingCollision(Level* level)
{
	float sensorAngle{};
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		sensorAngle = float(M_PI / 2.f);
		break;
	case PlayerPhysics::rightWall:
		sensorAngle = float(M_PI);
		break;
	case PlayerPhysics::ceiling:
		sensorAngle = float(3.f * M_PI / 2.f);
		break;
	case PlayerPhysics::leftWall:
		sensorAngle = float(0.f);
		break;
	default:
		break;
	}

	Point2f bottomLeftSensor{};
	Point2f bottomRightSensor{};
	Point2f topLeftSensor{};
	Point2f topRightSensor{};

	bottomLeftSensor.x = m_HitBoxVertices[1].x + m_HitBox.height / 2.f * cosf(sensorAngle);
	bottomLeftSensor.y = m_HitBoxVertices[1].y + m_HitBox.height / 2.f * sinf(sensorAngle);

	bottomRightSensor.x = m_HitBoxVertices[2].x + m_HitBox.height / 2.f * cosf(sensorAngle);
	bottomRightSensor.y = m_HitBoxVertices[2].y + m_HitBox.height / 2.f * sinf(sensorAngle);

	topLeftSensor.x = bottomLeftSensor.x + m_HitBox.height / 2.f * cosf(sensorAngle);
	topLeftSensor.y = bottomLeftSensor.y + m_HitBox.height / 2.f * sinf(sensorAngle);

	topRightSensor.x = bottomRightSensor.x + m_HitBox.height / 2.f * cosf(sensorAngle);
	topRightSensor.y = bottomRightSensor.y + m_HitBox.height / 2.f * sinf(sensorAngle);

	utils::HitInfo leftCeilingHitInfo{};
	utils::HitInfo rightCeilingHitInfo{};

	bool hitLeft{ level->GetHitInfo(m_Velocity, m_GroundVelocity, bottomLeftSensor, topLeftSensor, leftCeilingHitInfo) };
	bool hitRight{ level->GetHitInfo(m_Velocity, m_GroundVelocity, bottomRightSensor, topRightSensor, rightCeilingHitInfo) };

	Point2f intersectingPoint{};
	if (hitLeft && hitRight)
	{
		if ((m_HitBoxVertices[0].y - leftCeilingHitInfo.intersectPoint.y) > (m_HitBoxVertices[0].y - rightCeilingHitInfo.intersectPoint.y))
		{
			m_GroundNormal = leftCeilingHitInfo.normal;
			intersectingPoint = leftCeilingHitInfo.intersectPoint;
		}
		else
		{
			m_GroundNormal = rightCeilingHitInfo.normal;
			intersectingPoint = rightCeilingHitInfo.intersectPoint;
		}
	}
	else if (hitLeft)
	{
		m_GroundNormal = leftCeilingHitInfo.normal;
		intersectingPoint = leftCeilingHitInfo.intersectPoint;
	}
	else if (hitRight)
	{
		m_GroundNormal = rightCeilingHitInfo.normal;
		intersectingPoint = rightCeilingHitInfo.intersectPoint;
	}

	if (hitLeft || hitRight)
	{
		if (m_Velocity.y > 0.f)
		{
			m_Velocity.y = 0.f;
			m_GroundVelocity.y = -0.1f;
			m_appliedGravity = 0.f; //makes him stick to ceiling, tps into it -> need fix
		}
		m_falling = true;
		switch (m_DirectionMode)
		{
		case PlayerPhysics::ground:
			m_HitBoxVertices[0].y = intersectingPoint.y;
			m_HitBoxVertices[1].y = intersectingPoint.y - m_HitBox.height;
			m_HitBoxVertices[2].y = intersectingPoint.y - m_HitBox.height;
			m_HitBoxVertices[3].y = intersectingPoint.y;
			break;
		case PlayerPhysics::rightWall:
			m_HitBoxVertices[0].x = intersectingPoint.x;
			m_HitBoxVertices[1].x = intersectingPoint.x + m_HitBox.height;
			m_HitBoxVertices[2].x = intersectingPoint.x + m_HitBox.height;
			m_HitBoxVertices[3].x = intersectingPoint.x;
			break;
		case PlayerPhysics::ceiling:
			m_HitBoxVertices[0].y = intersectingPoint.y;
			m_HitBoxVertices[1].y = intersectingPoint.y + m_HitBox.height;
			m_HitBoxVertices[2].y = intersectingPoint.y + m_HitBox.height;
			m_HitBoxVertices[3].y = intersectingPoint.y;
			break;
		case PlayerPhysics::leftWall:
			m_HitBoxVertices[0].x = intersectingPoint.x;
			m_HitBoxVertices[1].x = intersectingPoint.x - m_HitBox.height;
			m_HitBoxVertices[2].x = intersectingPoint.x - m_HitBox.height;
			m_HitBoxVertices[3].x = intersectingPoint.x;
			break;
		default:
			break;
		}
	}
}

void PlayerPhysics::HandleGroundCollision(Level* level)
{
	float sensorLength{};
	if (IsOnGround(level))
	{
		sensorLength = std::min(abs(m_Velocity.x / 100.f) + 4.f, 14.f);
	}
	else
	{
		sensorLength = (m_Velocity.y / 100.f) + 8;
	}

	if (abs(m_GroundVelocity.y) > 0.f)
	{
		sensorLength = -1.f;
	}

	float sensorAngle{};
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		sensorAngle = float(M_PI / 2.f);
		break;
	case PlayerPhysics::rightWall:
		sensorAngle = float(M_PI);
		break;
	case PlayerPhysics::ceiling:
		sensorAngle = float(3.f * M_PI / 2.f);
		break;
	case PlayerPhysics::leftWall:
		sensorAngle = float(0.f);
		break;
	default:
		break;
	}

	Point2f topLeftSensor{};
	Point2f topRightSensor{};
	Point2f bottomLeftSensor{};
	Point2f bottomRightSensor{};

	int numSensors{ 2 };
	for (int sensorIdx = 0; sensorIdx < numSensors; sensorIdx++)
	{
		int bottomHitboxVertex{ sensorIdx + 1 };
		Point2f topSensor{ m_HitBoxVertices[bottomHitboxVertex].x + m_HitBox.height / 2.f * cosf(sensorAngle),
							m_HitBoxVertices[bottomHitboxVertex].y + m_HitBox.height / 2.f * sinf(sensorAngle) };
		Point2f bottomSensor{ m_HitBoxVertices[bottomHitboxVertex].x - sensorLength * cosf(sensorAngle),
							 m_HitBoxVertices[bottomHitboxVertex].y - sensorLength * sinf(sensorAngle) };

		if ( bottomHitboxVertex < numSensors ) { topLeftSensor = topSensor; bottomLeftSensor = bottomSensor; }
		else { topRightSensor = topSensor; bottomRightSensor = bottomSensor; }
	}

	Point2f intersectingPoint{};

	utils::HitInfo leftSensorHitInfo{};
	utils::HitInfo rightSensorHitInfo{};

	bool leftOnBackGround{};
	bool rightOnBackGround{};

	bool hitLeft{ level->GetHitInfo(m_Velocity, m_GroundVelocity, topLeftSensor, bottomLeftSensor, leftSensorHitInfo, m_OnBackground, leftOnBackGround, m_OnCorkscrew, false) };
	bool hitRight{ level->GetHitInfo(m_Velocity, m_GroundVelocity, topRightSensor, bottomRightSensor, rightSensorHitInfo, m_OnBackground, rightOnBackGround, m_OnCorkscrew, false) };

	bool hitCondition{};
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		hitCondition = (bottomLeftSensor.y - leftSensorHitInfo.intersectPoint.y) < (bottomLeftSensor.y - rightSensorHitInfo.intersectPoint.y);
		break;
	case PlayerPhysics::rightWall:
		hitCondition = (bottomLeftSensor.x - leftSensorHitInfo.intersectPoint.x) > (bottomLeftSensor.x - rightSensorHitInfo.intersectPoint.x);
		break;
	case PlayerPhysics::ceiling:
		hitCondition = (bottomLeftSensor.y - leftSensorHitInfo.intersectPoint.y) > (bottomLeftSensor.y - rightSensorHitInfo.intersectPoint.y);
		break;
	case PlayerPhysics::leftWall:
		hitCondition = (bottomLeftSensor.x - leftSensorHitInfo.intersectPoint.x) < (bottomLeftSensor.x - rightSensorHitInfo.intersectPoint.x);
		break;
	default:
		break;
	}

	if (!m_OnBackground && ((rightOnBackGround && leftOnBackGround) || (rightOnBackGround && !hitLeft) || (leftOnBackGround && !hitRight)))
	{ 
		m_OnBackground = true;
	}
	if (m_OnBackground && !leftOnBackGround && !rightOnBackGround)
	{
		m_OnBackground = false;
	}

	if ((hitLeft && !hitRight) || (hitLeft && hitRight && hitCondition))
	{
		m_GroundNormal = leftSensorHitInfo.normal;
		intersectingPoint = leftSensorHitInfo.intersectPoint;
	}
	else if (hitRight)
	{
		m_GroundNormal = rightSensorHitInfo.normal;
		intersectingPoint = rightSensorHitInfo.intersectPoint;
	}

	if (hitLeft || hitRight && (intersectingPoint.x != 0.f && intersectingPoint.y != 0.f))
	{
		if ((hitLeft && !hitRight) || (hitLeft && hitRight && hitCondition))
		{
			m_HitBoxVertices[0] = Point2f{ intersectingPoint.x + (m_HitBox.height * cosf(sensorAngle)), 
											intersectingPoint.y + (m_HitBox.height * sinf(sensorAngle)) };
			m_HitBoxVertices[1] = intersectingPoint;
			m_HitBoxVertices[2] = Point2f{ intersectingPoint.x + (m_HitBox.width * cosf(sensorAngle - float(M_PI / 2.f))),
											intersectingPoint.y + (m_HitBox.width * sinf(sensorAngle - float(M_PI / 2.f))) };
			m_HitBoxVertices[3] = Point2f{ m_HitBoxVertices[2].x + (m_HitBox.height * cosf(sensorAngle)),
											m_HitBoxVertices[2].y + (m_HitBox.height * sinf(sensorAngle)) };
		}
		else if (hitRight)
		{
			m_HitBoxVertices[1] = Point2f{ intersectingPoint.x + (m_HitBox.width * cosf(sensorAngle + float(M_PI / 2.f))),
											intersectingPoint.y + (m_HitBox.width * sinf(sensorAngle + float(M_PI / 2.f))) };
			m_HitBoxVertices[0] = Point2f{ m_HitBoxVertices[1].x + (m_HitBox.height * cosf(sensorAngle)),
											m_HitBoxVertices[1].y + (m_HitBox.height * sinf(sensorAngle)) };
			m_HitBoxVertices[2] = intersectingPoint;
			m_HitBoxVertices[3] = Point2f{ intersectingPoint.x + (m_HitBox.height * cosf(sensorAngle)),
											intersectingPoint.y + (m_HitBox.height * sinf(sensorAngle)) };
		}
	}
}

