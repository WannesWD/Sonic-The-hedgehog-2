#pragma once

class Sprite;
class Texture;

class Interactable 
{
public:
	virtual ~Interactable();

	Interactable(const Interactable& other) = delete;
	Interactable(Interactable&& other) noexcept = delete;
	Interactable& operator=(const Interactable& other) = delete;
	Interactable& operator=(Interactable&& other) noexcept = delete;

	virtual void Update(float elapsedSec);
	virtual void Draw() const;
	virtual Rectf GetHitbox();
	Point2f GetCenter();
	virtual void Delete();
	bool IsDeleted();
protected:
	Interactable(const Point2f& centerPos,
		float spriteWidth, float spriteHeight,
		float spriteFrameTime, int spriteRows, int spriteCols,
		float particleWidth, float particleHeight,
		float particleFrameTime, int particleRows, int particleCols);

	void setSpriteTextures(Texture* spriteSheetTexturePtr, Texture* particleSpriteSheetTexturePtr);
	void setSpriteTexture(Texture* spriteSheetTexturePtr);
	void setParticleTexture(Texture* particleSpriteSheetTexturePtr);

	bool IsHit();
	const Point2f& GetCenter() const;

	void FlipSprite();
	void SetCenter(Point2f newCenter);
	void SetAngle(float angle);
	void Sethit(bool hit);

	void UpdateSprite(float elapsedSec);
	void UpdateParticleSprite(float elapsedSec);

	void DrawSprite() const;
	void DrawParticle() const;

	Rectf GetSpriteHitbox();
	Rectf GetParticleHitbox();
private:

	Point2f m_Center;
	Sprite* m_SpritePtr;
	Sprite* m_ParticleSpritePtr;

	bool m_Hit;
	bool m_Deleted;
	bool m_Flipped;

	float m_ParticleTime;

	const float m_SpriteWidth, m_SpriteHeight, m_SpriteFrameTime, 
		m_ParticleWidth, m_ParticleHeight, m_MaxParticleTime;

};

