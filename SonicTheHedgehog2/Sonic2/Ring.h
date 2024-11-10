#pragma once
class Sprite;
class Texture;

class Ring final
{
public:
	Ring(Point2f centerPos);
	~Ring();

	Ring(const Ring& other) = delete;
	Ring(Ring&& other) noexcept = delete;
	Ring& operator=(const Ring& other) = delete;
	Ring& operator=(Ring&& other) noexcept = delete;

	void Update(float elapsedSec);
	void Draw() const;
	Rectf GetHitbox();
	void Delete();
private:

	Point2f m_Center;
	Sprite* m_pRingSprite;
	Sprite* m_pPickupParticleSprite;

	static Texture* m_RingSpriteSheetTexture;
	static Texture* m_PickupParticleSpriteSheetTexture;
	static int m_InstanceCounter;

	bool m_Hit;
	float m_ParticleFrameTime;
	float m_ParticleTime;
	float m_MaxParticleTime;
	const float m_SpriteSize;
};

