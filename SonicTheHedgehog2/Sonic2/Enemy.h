#pragma once
#include "Interactable.h"

class Enemy : public Interactable
{
public:
	virtual ~Enemy();

	Enemy(const Enemy& other) = delete;
	Enemy(Enemy&& other) noexcept = delete;
	Enemy& operator=(const Enemy& other) = delete;
	Enemy& operator=(Enemy&& other) noexcept = delete;

	virtual void Attack(const Rectf& playerHitbox) {};
protected:
	Enemy(const Point2f& centerPos,
		float spriteWidth, float spriteHeight,
		float spriteFrameTime, int spriteRows, int spriteCols);
private:
	static Texture* m_ParticleSpriteSheetTexture;
	static int m_InstanceCounter;
};
