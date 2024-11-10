#pragma once

class Texture;
class Sprite;

class Projectile
{
public:
	virtual ~Projectile();

	Projectile(const Projectile& other) = delete;
	Projectile(Projectile&& other) noexcept = delete;
	Projectile& operator=(const Projectile& other) = delete;
	Projectile& operator=(Projectile&& other) noexcept = delete;

	virtual void Update(float elapsedSec);
	void Draw() const;

	const Point2f& GetCenterPos();
	bool IsActive();

	void SetInActive();
	virtual void ResetProjectile(const Point2f& targetCenter, const Point2f centerPos);
protected:
	Projectile(const Point2f& targetCenter, const Point2f& centerPos, float width, float height,
				float frameTime, int numCols, int numRows);

	void setProjectileTexture(Texture* newProjectileTexturePtr);
	void SetCenterPos(Point2f newPos);
	const Point2f& GetTargetPos();
private:
	Point2f m_Target;
	Point2f m_Position;
	Sprite* m_ProjectileSpritePtr;

	bool m_Active;

	const float m_SpriteWidth, m_SpriteHeight;
};