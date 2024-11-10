#pragma once
#include "Projectile.h"

class Coconut final : public Projectile
{
public:
	Coconut(const Point2f& targetCenter, const Point2f centerPos);
	~Coconut();

	Coconut(const Coconut& other) = delete;
	Coconut(Coconut&& other) noexcept = delete;
	Coconut& operator=(const Coconut& other) = delete;
	Coconut& operator=(Coconut&& other) noexcept = delete;

	void Update(float elapsedSec) override;
	void ResetProjectile(const Point2f& targetCenter, const Point2f centerPos);
private:
	static Texture* PROJECTILESPRITESHEETTEXTUREPTR;
	static int INSTANCECOUNTER;
	static float PROJECTILEVELOCITY;
	static const float GRAVITY;
	static const float TRAVELTIME;

	float m_XVelocity;
	float m_YVelocity;

	float m_DeltaTime;
	Point2f m_SpawnPos;

	void SetVelocity();
};

