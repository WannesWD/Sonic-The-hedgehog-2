#pragma once
#include "Enemy.h"

class Buzzer final : public Enemy
{
public:
	Buzzer(const Point2f& centerPos);
	~Buzzer();

	Buzzer(const Buzzer& other) = delete;
	Buzzer(Buzzer&& other) noexcept = delete;
	Buzzer& operator=(const Buzzer& other) = delete;
	Buzzer& operator=(Buzzer&& other) noexcept = delete;

	void Update(float elapsedSec) override;
	void Attack(const Rectf& playerHitbox) override;
private:
	static Texture* SPRITESHEETTEXTUREPTR;	
	static int INSTANCECOUNTER;

	static const float 
		VELOCITY, 
		MAXMOVEMENTDISTANCE, 
		MAXLERPTIME, 
		MAXCOOLDOWNTIME, 
		DETECTIONRADIUS;

	float 
		m_LerpTime, 
		m_CooldownTime;
	
	bool 
		m_OnCooldown, 
		m_Flipped;

	const Point2f m_SpawnPos;
};


