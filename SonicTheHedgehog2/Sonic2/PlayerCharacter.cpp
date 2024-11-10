#include "pch.h"
#include "PlayerInput.h"
#include "PlayerCharacter.h"
#include "Texture.h"
#include "Avatar.h"
#include <iostream>

PlayerCharacter::PlayerCharacter(PlayerCharacter::CharacterName CharacterName, Point2f startPos) :
	PlayerInput(Vector2f{ 0.f,0.f }, Rectf{ startPos.x, startPos.y,19.f,39.f }),
	m_CharacterName{ CharacterName }, 
	m_MaxFlickerTime{ 0.1f },
	m_FlickerTime{}
{
	GetSpriteSheets();
}

PlayerCharacter::~PlayerCharacter()
{
	delete m_pSpriteSheets;
}

void PlayerCharacter::Draw() const
{
	if (!IsInvincible())
	{
		m_pSpriteSheets->Draw();
	}
	else
	{
		if (m_FlickerTime > m_MaxFlickerTime)
		{
			m_pSpriteSheets->Draw();
		}
	}

	float sensorLength{};
	if (m_OnGround)
	{
		sensorLength = std::min(abs(m_Velocity.x / 100.f) + 4.f, 14.f);
	}
	else
	{
		sensorLength = m_Velocity.y / 100.f + 8;
	}
	if (m_Velocity.y > 0.f)
	{
		sensorLength = 0.f;
	}

	//Point2f wallSensorEnd{};
	//float sensorAngle{};
	//float groundSensorAngle{};
	//switch (m_DirectionMode)
	//{
	//case PlayerPhysics::ground:
	//	sensorAngle = 0.f;
	//	groundSensorAngle = float(M_PI / 2.f);
	//	break;
	//case PlayerPhysics::rightWall:
	//	sensorAngle = float(M_PI / 2.f);
	//	groundSensorAngle = float(M_PI);
	//	break;
	//case PlayerPhysics::ceiling:
	//	sensorAngle = float(M_PI);
	//	groundSensorAngle = float(3.f * M_PI / 2.f);
	//	break;
	//case PlayerPhysics::leftWall:
	//	sensorAngle = float(3.f * M_PI / 2.f);
	//	groundSensorAngle = float(0.f);
	//	break;
	//default:
	//	break;
	//}
	//
	//float extrudingSensorLength{ 3.f };
	//float wallsensorLength{ (m_HitBox.width / 2.f) + extrudingSensorLength };
	//
	//if (m_FacingRight)
	//{
	//	wallSensorEnd.x = GetCenter().x + (wallsensorLength * cosf(sensorAngle));
	//	wallSensorEnd.y = GetCenter().y + (wallsensorLength * sinf(sensorAngle));
	//}
	//else if (!m_FacingRight)
	//{
	//	wallSensorEnd.x = GetCenter().x - (wallsensorLength * cosf(sensorAngle));
	//	wallSensorEnd.y = GetCenter().y - (wallsensorLength * sinf(sensorAngle));
	//}
	//
	//Point2f topLeftSensor{};
	//Point2f topRightSensor{};
	//Point2f bottomLeftSensor{};
	//Point2f bottomRightSensor{};
	//
	//topLeftSensor.x = m_HitBoxVertices[1].x + m_HitBox.height / 2.f * cosf(groundSensorAngle);
	//topLeftSensor.y = m_HitBoxVertices[1].y + m_HitBox.height / 2.f * sinf(groundSensorAngle);
	//
	//topRightSensor.x = m_HitBoxVertices[2].x + m_HitBox.height / 2.f * cosf(groundSensorAngle);
	//topRightSensor.y = m_HitBoxVertices[2].y + m_HitBox.height / 2.f * sinf(groundSensorAngle);
	//
	//bottomLeftSensor.x = m_HitBoxVertices[1].x - sensorLength * cosf(groundSensorAngle);
	//bottomLeftSensor.y = m_HitBoxVertices[1].y - sensorLength * sinf(groundSensorAngle);
	//
	//bottomRightSensor.x = m_HitBoxVertices[2].x - sensorLength * cosf(groundSensorAngle);
	//bottomRightSensor.y = m_HitBoxVertices[2].y - sensorLength * sinf(groundSensorAngle);
	//
	//Point2f leftCeilingSensor{};
	//Point2f rightCeilingSensor{};
	//leftCeilingSensor.x = topLeftSensor.x + m_HitBox.height / 2.f * cosf(groundSensorAngle);
	//leftCeilingSensor.y = topLeftSensor.y + m_HitBox.height / 2.f * sinf(groundSensorAngle);
	//
	//rightCeilingSensor.x = topRightSensor.x + m_HitBox.height / 2.f * cosf(groundSensorAngle);
	//rightCeilingSensor.y = topRightSensor.y + m_HitBox.height / 2.f * sinf(groundSensorAngle);
	//
	//utils::SetColor(Color4f(1.f, 0.f, 0.5f, 1.f));
	//utils::DrawLine(GetCenter(), wallSensorEnd);
	////utils::DrawEllipse(GetCenter(), 10.f, 10.f);
	////utils::SetColor(Color4f(1.f, 0.f, 0.f, 1.f));
	////utils::DrawPolygon(m_HitBoxVertices);
	//if (m_OnBackground)
	//	utils::SetColor(Color4f(0.f, 1.f, 0.f, 1.f));
	//else
	//	utils::SetColor(Color4f(1.f, 0.f, 0.f, 1.f));
	//
	//utils::DrawLine(topLeftSensor, leftCeilingSensor);
	//utils::DrawLine(topRightSensor, rightCeilingSensor);
	//
	//utils::DrawLine(topLeftSensor, bottomLeftSensor);
	//utils::DrawLine(topRightSensor, bottomRightSensor);
	////for (Point2f vertex : m_HitBoxVertices)
	////{
	////	utils::DrawEllipse(vertex, 3.f, 3.f);
	////}
}

void PlayerCharacter::Update(float elapsedSec, Level* level)
{
	HandleInput(elapsedSec);
	PlayerPhysics::Update(elapsedSec,level);
	UpdateActionState(elapsedSec);
	UpdatePlayerStats(elapsedSec);

	dynamic_cast<Avatar*>(m_pSpriteSheets)->UpdateAnimationSpriteIndex(int(m_ActionState));

	float maxAnimationSpeed{};
	switch (m_CharacterName)
	{
	case PlayerCharacter::Sonic:
		maxAnimationSpeed = 800.f;
		break;
	case PlayerCharacter::Shadow:
		maxAnimationSpeed = 1000.f;
		break;
	default:
		break;
	}

	switch (m_ActionState)
	{
	case PlayerInput::Actionstate::basicMotion:
	case PlayerInput::Actionstate::fullSpeed:
	case PlayerInput::Actionstate::roll:
		dynamic_cast<Avatar*>(m_pSpriteSheets)->UpdateFrameTime((std::max(0.f, maxAnimationSpeed - abs(m_GroundVelocity.x)) / 100.f) / 60.f);
		break;
	case PlayerInput::Actionstate::skidding:
		break;
	case PlayerInput::Actionstate::turn:
		break;
	case PlayerInput::Actionstate::corkScrew:
		dynamic_cast<Avatar*>(m_pSpriteSheets)->UpdateFrameTime((std::max(0.f, 1000.f - abs(m_GroundVelocity.x)) / 100.f) / 60.f);
		break;
	case PlayerInput::Actionstate::pushing:
		break;
	case PlayerInput::Actionstate::bouncedUp:
		break;
	case PlayerInput::Actionstate::hurt:
		break;
	default:
		break;
	}

	m_pSpriteSheets->Update(elapsedSec);
	m_pSpriteSheets->SetPos(GetCenter());

	float xScale{ m_FacingRight ? 1.f : -1.f };
	m_pSpriteSheets->SetScale(xScale, 1.f);

	float groundAngle{ (m_GroundAngle / float(M_PI) * 180) };

	if (abs(groundAngle) > 45.f && abs(groundAngle) < 315.f)
	{
		m_pSpriteSheets->SetAngle(groundAngle);
	}
	else
	{
		m_pSpriteSheets->SetAngle(0.f);
	}

	if (IsInvincible())
	{
		UpdateFlickerTime(elapsedSec);
	}
}

PlayerCharacter::CharacterName PlayerCharacter::GetCharacterName()
{
	return m_CharacterName;
}

std::vector<Texture*> PlayerCharacter::GenSprites()
{
	std::string characterName{};

	switch (m_CharacterName)
	{
	case PlayerCharacter::Sonic:
		characterName = "Sonic";
		break;
	case PlayerCharacter::Shadow:
		characterName = "Shadow";
		break;
	}

	std::vector<Texture*> pAnimationSprites;
	for (int stateIdx = 0; stateIdx < int(PlayerInput::Actionstate::maxStateVal); stateIdx++)
	{
		pAnimationSprites.push_back(new Texture{ "SpriteSheets/" + characterName + "/" + characterName + 
													std::to_string(stateIdx) + ".png" });
	}
	return pAnimationSprites;
}

void PlayerCharacter::UpdateFlickerTime(float elapsedSec)
{
	m_FlickerTime += elapsedSec;
	if (m_FlickerTime >= m_MaxFlickerTime * 2.f)
	{
		m_FlickerTime = 0.f;
	}
}

void PlayerCharacter::GetSpriteSheets()
{
	std::vector<float> frameTimes{};
	std::vector<int> numCols{};
	std::vector<int> numRows{};

	switch (m_CharacterName)
	{
	case PlayerCharacter::Sonic:
		frameTimes = std::vector<float>{ 1.f, 0.5f, 0.3f, 0.f, 0.f, 0.133f, 0.133f, 0.f, 0.09166f, 0.f, 0.8f, 0.5f };
		numCols = std::vector<int>{ 1, 2, 5, 8, 4, 3, 1, 2, 6, 4, 1, 2 };
		numRows = std::vector<int>{ 1, 1, 4, 1, 1, 1, 1, 4, 2, 1, 1, 1 };
		break;
	case PlayerCharacter::Shadow:
		frameTimes = std::vector<float>{ 1.f, 0.5f, 0.1f, 0.f, 0.f, 0.133f, 0.133f, 0.f, 0.09166f, 0.f, 0.133f, 0.5f };
		numCols = std::vector<int>{ 1, 2, 5, 8, 16, 2, 1, 2, 6, 4, 4, 2 };
		numRows = std::vector<int>{ 1, 1, 4, 1, 1, 1, 1, 4, 2, 1, 1, 1 };
		break;
	default:
		break;
	}
	m_pSpriteSheets = new Avatar{ GenSprites(), frameTimes, numCols, numRows, int(PlayerInput::Actionstate::idle) };
}
