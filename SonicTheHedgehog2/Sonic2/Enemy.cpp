#include "pch.h"
#include "Enemy.h"
#include "Texture.h"
Texture* Enemy::m_ParticleSpriteSheetTexture{ nullptr };
int Enemy::m_InstanceCounter{ 0 };

Enemy::Enemy(const Point2f& centerPos,
	float spriteWidth, float spriteHeight, float spriteFrameTime, int spriteRows, int spriteCols)
	: Interactable(centerPos, spriteWidth, spriteHeight, spriteFrameTime, spriteRows, spriteCols,
		32.f, 32.f, 0.166f, 1, 5)

{
	if (m_InstanceCounter == 0)
	{
		m_ParticleSpriteSheetTexture = new Texture{ "SpriteSheets/CommonObjects/EnemyExplosion/EnemyExplosion.png" };
	}
	++m_InstanceCounter;
	setParticleTexture(m_ParticleSpriteSheetTexture);
}

Enemy::~Enemy()
{
	if (--m_InstanceCounter < 1)
	{
		delete m_ParticleSpriteSheetTexture;
		m_ParticleSpriteSheetTexture = nullptr;
	}
}
