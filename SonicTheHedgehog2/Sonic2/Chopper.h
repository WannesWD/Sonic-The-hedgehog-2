#pragma once
#include "Enemy.h"
class Texture;

class Chopper final : public Enemy
{
public:
	Chopper(const Point2f& centerPos);
	~Chopper();

	Chopper(const Chopper& other) = delete;
	Chopper(Chopper&& other) noexcept = delete;
	Chopper& operator=(const Chopper& other) = delete;
	Chopper& operator=(Chopper&& other) noexcept = delete;

	void Update(float elapsedSec) override;
private:
	static Texture* SPRITESHEETTEXTUREPTR;
	static int INSTANCECOUNTER;	

	static const float 
		ROTATIONSPEED,
		GRAVITY,
		JUMPVELOCITY;

	Point2f m_SpawnPos;
	float m_YVelocity;
	float m_spriteAngle;

};

