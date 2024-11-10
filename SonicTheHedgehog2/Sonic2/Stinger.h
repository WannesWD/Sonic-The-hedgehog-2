#pragma once
#include "Projectile.h"
#include "Vector2f.h"

class Texture;

class Stinger final : public Projectile
{
public:
	Stinger(const Point2f& targetCenter, const Point2f centerPos);
	~Stinger();

	Stinger(const Stinger& other) = delete;
	Stinger(Stinger&& other) noexcept = delete;
	Stinger& operator=(const Stinger& other) = delete;
	Stinger& operator=(Stinger&& other) noexcept = delete;

	void Update(float elapsedSec) override;
	void ResetProjectile(const Point2f& targetCenter, const Point2f centerPos) override;
private:
	static Texture* PROJECTILESPRITESHEETTEXTUREPTR;
	static int INSTANCECOUNTER;
	static float PROJECTILEVELOCITY;

	Vector2f m_DirectionVector;

	Vector2f GetDirectionVector(Point2f centerPos, Point2f targetCenter);
};

