#pragma once
#include "Enemy.h"

class Texture;

class Aiaiai final : public Enemy
{
public:
	Aiaiai(const Point2f& centerPos);
	~Aiaiai();

	Aiaiai(const Aiaiai& other) = delete;
	Aiaiai(Aiaiai&& other) noexcept = delete;
	Aiaiai& operator=(const Aiaiai& other) = delete;
	Aiaiai& operator=(Aiaiai&& other) noexcept = delete;
	void Update(float elapsedSec) override;

	void Attack(const Rectf& playerHitBox) override;
private:
	static Texture* SPRITESHEETTEXTUREPTR;
	static int INSTANCECOUNTER;

	static const float 
		MOVEMENTDISTANCE,
		MAXLERPTIME,
		MAXCOOLDOWNTIME,
		DETECTIONRADIUS;

	Point2f m_SpawnPos;

	float 
		m_LerpTime,
		m_LerpFactor,
		m_CooldownTime;

	bool m_OnCooldown;
};
