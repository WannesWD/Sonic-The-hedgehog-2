#pragma once
#include "Sprite.h"

class AnimatedSprite : public Sprite
{
public:
	virtual ~AnimatedSprite() = default;

	AnimatedSprite(const AnimatedSprite& other) = delete;
	AnimatedSprite(AnimatedSprite&& other) noexcept = delete;
	AnimatedSprite& operator=(const AnimatedSprite& other) = delete;
	AnimatedSprite& operator=(AnimatedSprite&& other) noexcept = delete;

	void Update(float elapsedSec);
	void Draw(const Texture& sprite) const;
 
protected:
	AnimatedSprite(Point2f position, float frameTime, int numCols, int numRows);

	float m_FrameTime;
	int m_NumCols;
	int m_NumRows;
	float m_TimeAnimated;
	int m_CurrentFrame;
};

