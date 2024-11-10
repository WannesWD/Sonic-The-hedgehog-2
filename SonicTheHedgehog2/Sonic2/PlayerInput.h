#pragma once
#include "PlayerPhysics.h"
#include <vector>
class PlayerInput : public PlayerPhysics
{
public:
	enum class Actionstate
	{
		idle,
		idleToBored,
		bored,
		basicMotion,
		fullSpeed,
		skidding,
		turn,
		roll,
		corkScrew,
		pushing,
		bouncedUp,
		hurt,

		maxStateVal
	};
	struct PlayerStats
	{
		int score;
		int ringCount;
		int lives;
	};

	Rectf GetHitbox();
	void RingCollected();
	void Hit();
	void KilledEnemy();
	void SetSpawnPoint(Point2f spawnPoint);
	void SetFinished(bool finished);
	void Respawn();
	void HitSpring(int springAngle);
	bool IsJumping() const;
	bool HasFinished() const;
	bool StatsUpdated();
	bool IsAlive() const;
	bool GameOver() const;
	const PlayerInput::PlayerStats& GetPlayerStats() const;
protected:
	PlayerInput(Vector2f velocity, Rectf hitBox);
	virtual ~PlayerInput() = default;

	PlayerInput(const PlayerInput& other) = delete;
	PlayerInput(PlayerInput&& other) noexcept = delete;
	PlayerInput& operator=(const PlayerInput& other) = delete;
	PlayerInput& operator=(PlayerInput&& other) noexcept = delete;


	void HandleInput(float elapsedSec);
	void UpdateActionState(float elapsedSec);
	void UpdatePlayerStats(float elapsedSec);
	Actionstate m_ActionState;

	bool IsInvincible() const;
private:
	void handlePlayerJump(float elapsedSec, const Uint8* pState);
	void handlePlayerMove(float elapsedSec, const Uint8* pState);

	Actionstate m_PrevActionState;
	Vector2f m_Acceleration;
	const float m_MaxHorizontalGroundVelocity, m_JumpVelocity, m_DecelerationVelocity, 
				m_MinCorkscrewVelocity, m_MaxInvincibleTime, m_MaxJumpTime;

	float m_JumpTime ,m_InvincibleTime, 
			m_StateTime;

	bool m_Skidding, m_StatsUpdated, m_Invincible, m_Finished, m_JumpSoundPlayed, m_GameOver;

	PlayerStats m_PlayerStats;
	Point2f m_SpawnPoint;

	void UpdateHitBoxOrientation(std::vector<Point2f>& hitBoxVertices) const;
	std::vector<Point2f> getRotatedVertices(float angle, const std::vector<Point2f>& originalVertices, float xTranslate, float yTranslate) const;
};

