#include "pch.h"
#include "PlayerInput.h"
#include "SoundManager.h"
#include "utils.h"
#include "Matrix2x3.h"
#include <iostream>

PlayerInput::PlayerInput(Vector2f velocity, Rectf hitBox) :
	PlayerPhysics(velocity, hitBox),
	m_ActionState{ Actionstate::idle },
	m_PrevActionState{ m_ActionState },
	m_Acceleration{ Vector2f{ 4.6875f * 60.f, -21.875 * 60.f} },
	m_MaxHorizontalGroundVelocity{ 600.f },
	m_DecelerationVelocity{ 50.f * 60.f },
	m_JumpVelocity{ 650.f * 6.f * 1.3f },
	m_MinCorkscrewVelocity{ 400.f },
	m_MaxJumpTime{ 0.1f },
	m_JumpTime{ 0.f },
	m_Skidding{ false },
	m_StateTime{ 0 },
	m_PlayerStats{ 0,0,3 },
	m_SpawnPoint{ Point2f{ hitBox.left,hitBox.bottom } },
	m_StatsUpdated{ true },
	m_MaxInvincibleTime{ 3.f },
	m_InvincibleTime{ 0.f },
	m_Invincible{ false },
	m_Finished{ false },
	m_JumpSoundPlayed{ false },
	m_GameOver{ false }
{
}

void PlayerInput::HandleInput(float elapsedSec)
{
	const Uint8* pState = SDL_GetKeyboardState(nullptr);

	switch (m_ActionState)
	{
	case PlayerInput::Actionstate::idle:
	case PlayerInput::Actionstate::idleToBored:
	case PlayerInput::Actionstate::bored:
	case PlayerInput::Actionstate::basicMotion:
	case PlayerInput::Actionstate::fullSpeed:
	case PlayerInput::Actionstate::skidding:
	case PlayerInput::Actionstate::turn:
	case PlayerInput::Actionstate::corkScrew:
	case PlayerInput::Actionstate::roll:
		handlePlayerJump(elapsedSec, pState);
	case PlayerInput::Actionstate::bouncedUp:
		handlePlayerMove(elapsedSec, pState);
		break;
	case PlayerInput::Actionstate::pushing:
		break;
	case PlayerInput::Actionstate::hurt:
		break;
	default:
		break;
	}
	if (pState[SDL_SCANCODE_R])
	{
		Respawn();
	}
}

void PlayerInput::UpdateActionState(float elapsedSec)
{
	const float maxIdleTime{ 5.f };
	const float maxIdleToBoredTime{ 0.9f };
	bool running{ false };

	switch (m_ActionState)
	{
	case Actionstate::idle:
		if (m_StateTime >= maxIdleTime)
		{
			m_StateTime -= maxIdleTime;
			m_ActionState = Actionstate::idleToBored;
			m_StateTime = 0.f;
		}
		if (m_Jumping)
		{
			m_PrevActionState = m_ActionState;
			m_ActionState = Actionstate::roll;
			m_StateTime = 0.f;
		}
		else if (abs(m_GroundVelocity.x) > 0.f)
		{
			m_ActionState = Actionstate::basicMotion;
			m_StateTime = 0.f;
		}
		m_StateTime += elapsedSec;
		break;
	case Actionstate::idleToBored:
		if (m_Jumping)
		{
			m_PrevActionState = m_ActionState;
			m_ActionState = Actionstate::roll;
			m_StateTime = 0.f;
		}
		else if (abs(m_GroundVelocity.x) > 0.f)
		{
			m_ActionState = Actionstate::basicMotion;
			m_StateTime = 0.f;
		}
		else if (m_StateTime >= maxIdleToBoredTime)
		{
			m_StateTime -= maxIdleToBoredTime;
			m_ActionState = Actionstate::bored;
			m_StateTime = 0.f;
		}
		m_StateTime += elapsedSec;
		break;
	case Actionstate::bored:
		if (m_Jumping)
		{
			m_PrevActionState = m_ActionState;
			m_ActionState = Actionstate::roll;
			m_StateTime = 0.f;
		}
		else if (abs(m_GroundVelocity.x) > 0.f)
		{
			m_ActionState = Actionstate::basicMotion;
			m_StateTime = 0.f;
		}
		m_StateTime += elapsedSec;
		break;
	case Actionstate::basicMotion:
		if (m_OnCorkscrew && abs(m_GroundVelocity.x) > m_MinCorkscrewVelocity)
		{
			m_ActionState = Actionstate::corkScrew;
		}

		if (m_Skidding && m_OnGround && m_ControlLockTimer == 0.f)
		{
			m_ActionState = Actionstate::skidding;
		}

		if (m_Jumping)
		{
			m_PrevActionState = m_ActionState;
			m_ActionState = Actionstate::roll;
		}

		else if (abs(m_GroundVelocity.x) >= m_MaxHorizontalGroundVelocity)
		{
			m_ActionState = Actionstate::fullSpeed;
		}
		else if (abs(m_GroundVelocity.x) <= 0.f)
		{
			m_ActionState = Actionstate::idle;
		}
		break;
	case Actionstate::fullSpeed:
		if (m_OnCorkscrew && abs(m_GroundVelocity.x) > m_MinCorkscrewVelocity)
		{
			m_ActionState = Actionstate::corkScrew;
		}
		if (m_Skidding && m_OnGround && m_ControlLockTimer == 0.f)
		{
			m_ActionState = Actionstate::skidding;
		}
		if (m_Jumping)
		{
			m_PrevActionState = m_ActionState;
			m_ActionState = Actionstate::roll;
		}
		else if (abs(m_GroundVelocity.x) < m_MaxHorizontalGroundVelocity)
		{
			m_ActionState = Actionstate::basicMotion;
		}
		break;
	case Actionstate::skidding:
		SoundManager::GetSoundManager()->PlaySound(SoundManager::skidding, 0);
		if (!m_Skidding)
		{
			m_ActionState = Actionstate::basicMotion;
		}
		break;
	case Actionstate::turn:
		break;
	case Actionstate::roll:
		if (m_GroundVelocity.y == 0.f)
		{
			m_ActionState = m_PrevActionState;
			m_Jumping = false;
		}
		break;
	case Actionstate::corkScrew:
		if (abs(m_GroundVelocity.x) < m_MinCorkscrewVelocity || !m_OnCorkscrew)
		{
			m_ActionState = Actionstate::basicMotion;
		}
		break;
	case Actionstate::pushing:
		break;
	case Actionstate::bouncedUp:
		if (m_OnGround)
		{
			m_ActionState = Actionstate::basicMotion;
		}
		break;
	case Actionstate::hurt:
		if (m_OnGround)
		{
			m_ActionState = Actionstate::idle;
		}

		break;
	default:
		break;
	}
}

void PlayerInput::UpdatePlayerStats(float elapsedSec)
{
	if (m_Invincible)
	{
		m_InvincibleTime += elapsedSec;
		if (m_InvincibleTime >= m_MaxInvincibleTime)
		{
			m_Invincible = false;
			m_InvincibleTime = 0.f;
		}
	}
}

Rectf PlayerInput::GetHitbox()
{
	return PlayerPhysics::GetHitBox();
}

void PlayerInput::RingCollected()
{
	++m_PlayerStats.ringCount;
	m_StatsUpdated = true;
	SoundManager::GetSoundManager()->PlaySound(SoundManager::ringPickup, 0);
}

void PlayerInput::Hit()
{
	if (!m_Invincible)
	{
		Vector2f hitVelocity{ -150.f,200.f };
		m_Hit = true;
		m_appliedGravity = 0.f;
		m_GroundVelocity.y = hitVelocity.y;
		m_GroundVelocity.x = hitVelocity.x;

		if (m_PlayerStats.ringCount > 0)
		{
			m_PlayerStats.ringCount -= m_PlayerStats.ringCount;
			m_Invincible = true;
		}
		else if (--m_PlayerStats.lives >= 0)
		{
			m_Dead = true;
			m_Invincible = true;
		}
		else
		{
			m_Dead = true;
			m_GameOver = true;
		}

		m_StatsUpdated = true;
		m_ActionState = Actionstate::hurt;

		SoundManager::GetSoundManager()->PlaySound(SoundManager::ringLoss, 0);
	}
}

void PlayerInput::KilledEnemy()
{
	m_appliedGravity = 0.f;
	m_GroundVelocity.y /= 2.f;
	m_PlayerStats.score += 100;
	m_StatsUpdated = true;
	SoundManager::GetSoundManager()->PlaySound(SoundManager::enemykilled, 0);
}

void PlayerInput::SetSpawnPoint(Point2f spawnPoint)
{
	m_SpawnPoint = spawnPoint;
}

void PlayerInput::SetFinished(bool finished)
{
	m_Finished = finished;
}

void PlayerInput::Respawn()
{
	m_Velocity = Vector2f{ 0.f,0.f };
	m_GroundVelocity = Vector2f{ 0.f,0.f };
	m_OnGround = false;
	m_Jumping = false;
	m_OnCorkscrew = false;
	m_Hit = false;
	m_FacingRight = true;
	m_appliedGravity = 0.f;
	m_GroundAngle = 0.f;
	m_DirectionMode = ground;
	m_Dead = false;
	setPosition(m_SpawnPoint);
}

void PlayerInput::HitSpring(int springAngle)
{
	Vector2f newVelocity{ m_MaxHorizontalGroundVelocity * cosf(float(springAngle) / 180.f * float(M_PI)),
							m_MaxHorizontalGroundVelocity * sinf(float(springAngle) / 180.f * float(M_PI)) };
	if (springAngle % 90 == 0 || springAngle == 0)
	{
		if (springAngle == 0 || springAngle == 180)
		{
			m_GroundVelocity.x = newVelocity.x;
		}
		else
		{
			m_GroundVelocity.y = newVelocity.y;
			m_appliedGravity = 0.f;
			m_ActionState = Actionstate::corkScrew;
		}
	}
	else
	{
		m_GroundVelocity.x = (newVelocity.x / abs(newVelocity.x)) * m_MaxHorizontalGroundVelocity;
		m_GroundVelocity.y = (newVelocity.y / abs(newVelocity.y)) * m_MaxHorizontalGroundVelocity + (m_JumpVelocity * m_MaxJumpTime/2.f);
		m_appliedGravity = 0.f;

		m_ActionState = Actionstate::bouncedUp;
	}
	
	if (springAngle > 90 && springAngle < 270)
	{
		m_FacingRight = false;
	}
	else
	{
		m_FacingRight = true;
	}

	m_HitSpring = true;
	m_Skidding = false;
	m_DirectionMode = ground;
	SoundManager::GetSoundManager()->PlaySound(SoundManager::springHit, 0);
}

bool PlayerInput::IsJumping() const
{
	return m_Jumping;
}

bool PlayerInput::HasFinished() const
{
	return m_Finished;
}

bool PlayerInput::StatsUpdated()
{
	if (m_StatsUpdated)
	{
		m_StatsUpdated = false;
		return true;
	}
	else
		return false;
}

bool PlayerInput::IsAlive() const
{
	return !m_Dead;
}

bool PlayerInput::GameOver() const
{
	return m_GameOver;
}

bool PlayerInput::IsInvincible() const
{
	return m_Invincible;
}

const PlayerInput::PlayerStats& PlayerInput::GetPlayerStats() const
{
	return m_PlayerStats;
}


void PlayerInput::handlePlayerJump(float elapsedSec, const Uint8* pState)
{
	if (pState[SDL_SCANCODE_W] && (m_JumpTime < m_MaxJumpTime))
	{
		if (!m_JumpSoundPlayed)
		{
			SoundManager::GetSoundManager()->PlaySound(SoundManager::jump, 0);
			m_JumpSoundPlayed = true;
		}

		m_GroundVelocity.y += m_JumpVelocity * elapsedSec;
		m_JumpTime += elapsedSec;
		m_Jumping = true;
	}
	if (m_OnGround)
	{
		m_JumpTime = 0.f;
		m_JumpSoundPlayed = false;
	}
}

void PlayerInput::handlePlayerMove(float elapsedSec, const Uint8* pState)
{
	const float maxGroundVelocity{ 600.f };
	bool pressing{ false };
	//code based on example provided for running physics in https://info.sonicretro.org/SPG:Running
	//but adjusted to work time based 
	if (m_ControlLockTimer == 0.f)
	{
		if (pState[SDL_SCANCODE_D] || m_Finished)
		{
			pressing = true;

			if (m_GroundVelocity.x < 0)
			{
				if (abs(m_GroundVelocity.x) > 400.f) m_Skidding = true;

				m_GroundVelocity.x += m_DecelerationVelocity * elapsedSec;

				if (m_GroundVelocity.x >= 0)
				{
					m_Skidding = false;
					m_GroundVelocity.x = 5.f;
				}
			}
			else if (m_GroundVelocity.x < maxGroundVelocity)
			{
				m_FacingRight = true;
				m_GroundVelocity.x += m_Acceleration.x * elapsedSec;
				if (m_GroundVelocity.x >= maxGroundVelocity)
				{
					m_GroundVelocity.x = maxGroundVelocity;
				}
			}
		}
		if (pState[SDL_SCANCODE_A] && !m_Finished)
		{
			pressing = true;

			if (m_GroundVelocity.x > 0)
			{
				if (abs(m_GroundVelocity.x) >= 400.f) m_Skidding = true;
			
				m_GroundVelocity.x -= m_DecelerationVelocity * elapsedSec;

				if (m_GroundVelocity.x <= 0)
				{
					m_Skidding = false;
					m_GroundVelocity.x = -5.f;
				}
			}
			else if (m_GroundVelocity.x > -maxGroundVelocity)
			{
				m_FacingRight = false;
				m_GroundVelocity.x -= m_Acceleration.x * elapsedSec;
				if (abs(m_GroundVelocity.x) >= maxGroundVelocity)
				{
					m_GroundVelocity.x = -maxGroundVelocity;
				}
			}
		}
	}
	if (!pressing && !m_Jumping && (m_ControlLockTimer == 0.f))
	{
		m_Skidding = false;
		if (m_GroundVelocity.x != 0)
		{
			m_GroundVelocity.x -= std::min(abs(m_GroundVelocity.x), m_Acceleration.x * elapsedSec) * (m_GroundVelocity.x / abs(m_GroundVelocity.x));
		}
	}

}

std::vector<Point2f> PlayerInput::getRotatedVertices(float angle,const std::vector<Point2f>& originalVertices, float xTranslate, float yTranslate) const
{
	Matrix2x3 resetMatrix{ Matrix2x3::CreateTranslationMatrix(xTranslate, yTranslate) };
	Matrix2x3 rotateMatrix{ Matrix2x3::CreateRotationMatrix(angle) };
	Matrix2x3 centerMatrix{ Matrix2x3::CreateTranslationMatrix(-xTranslate,-yTranslate) };

	Matrix2x3 compoundMatrix{ resetMatrix * rotateMatrix * centerMatrix};
	return compoundMatrix.Transform(originalVertices);
}

void PlayerInput::UpdateHitBoxOrientation(std::vector<Point2f>& hitBoxVertices) const
{
	float groundAngle{ 0.f };
	switch (m_DirectionMode)
	{
	case PlayerPhysics::ground:
		groundAngle = 0.f;
		break;
	case PlayerPhysics::rightWall:
		groundAngle = 90.f;
		break;
	case PlayerPhysics::ceiling:
		groundAngle = 180.f;
		break;
	case PlayerPhysics::leftWall:
		groundAngle = 270.f;
		break;
	default:
		break;
	}
	hitBoxVertices = getRotatedVertices(groundAngle, hitBoxVertices,
		GetCenter().x, GetCenter().y);
}

