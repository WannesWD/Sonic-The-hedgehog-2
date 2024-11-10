#pragma once
#include "GameObject.h"
#include "Vector2f.h"
#include "utils.h"
#include <vector>
class Level;

class PlayerPhysics : public GameObject
{
public:

	enum DirectionMode
	{
		ground,
		rightWall,
		ceiling,
		leftWall
	};

	Point2f GetCenter() const;
	bool GetOnGround() const;
	Rectf GetHitBox() const;
protected:
	PlayerPhysics(Vector2f velocity, Rectf hitBox);
	virtual ~PlayerPhysics() = default;

	PlayerPhysics(const PlayerPhysics& other) = delete;
	PlayerPhysics(PlayerPhysics&& other) noexcept = delete;
	PlayerPhysics& operator=(const PlayerPhysics& other) = delete;
	PlayerPhysics& operator=(PlayerPhysics&& other) noexcept = delete;


	void Update(float elapsedSec, Level* level) override;

	void setPosition(Point2f centerPos);

	std::vector<Point2f> m_HitBoxVertices;
	Vector2f m_GroundVelocity;
	Vector2f m_Velocity;
	Vector2f m_GroundNormal;
	DirectionMode m_DirectionMode;
	Rectf m_HitBox;

	float m_ControlLockTimer;
	float m_GroundAngle;

	bool m_OnGround;
	bool m_FacingRight;
	bool m_Jumping;
	bool m_OnCorkscrew;
	bool m_Hit;
	bool m_HitSpring;
	bool m_Dead;
	bool m_OnBackground;
	float m_appliedGravity;

private:
	void groundedUpdate(float elapsedSec, Level* level);
	void aerialUpdate(float elapsedSec, Level* level);		

	void UpdateDirectionMode();
	void UpdateHitBoxOrientation(std::vector<Point2f>& hitBoxVertices);
	void UpdateFallingAngle(float elapsedSec, float groundAngle);
	void UpdateHitboxVertices();

	bool IsOnGround(Level* level, bool aerialCheck = false);
	bool HitboxIsValid();

	void SetAngle();
	void SetVelocity(float elapsedSec);
	void SetWallSensorEnd(Point2f& wallSensorEnd, float sensorLength, float sensorAngle, bool facingRight);

	void HandleSlopeSlipping(float elapsedSec);
	void HandleSlopeLanding(float elapsedSec);

	void HandleWallCollision(Level* level);
	void HandleCeilingCollision(Level* level);
	void HandleGroundCollision(Level* level);

	std::vector<Point2f> getRotatedVertices(float angle, const std::vector<Point2f>& originalVertices, float xTranslate, float yTranslate) const;
	
	Point2f m_Center;
	DirectionMode m_PrevDirectionMode;

	const float m_Gravity;
	const float m_GroundedHitboxHeight;
	const float m_JumpingHitboxHeight;
	float m_TotalRotationAngle;

	bool m_falling;

	bool m_HitApex;
};

