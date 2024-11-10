#pragma once
#include "Texture.h"
#include "AnimatedSprite.h"
class StaticAnimatedSprite final : public AnimatedSprite
{
public:
	StaticAnimatedSprite(Point2f bottomLeftPosition, float frameTime, int numCols, int numRows, Texture* spriteSheetTexturePtr);
	~StaticAnimatedSprite() = default;

	void Update(float elapsedSec) override;
	void UpdateFrame(int frameNumber);
	void Draw() const override;

	StaticAnimatedSprite(const StaticAnimatedSprite& other) = delete;
	StaticAnimatedSprite(StaticAnimatedSprite&& other) noexcept = delete;
	StaticAnimatedSprite& operator=(const StaticAnimatedSprite& other) = delete;
	StaticAnimatedSprite& operator=(StaticAnimatedSprite&& other) noexcept = delete;

	void SetSpriteSheetTexturePtr(Texture* spriteSheet);
private:
	Texture* m_pSpriteSheet;
};


