#pragma once
#include "Sprite.h"
#include "Texture.h"

class Texture;

class SimpleSprite : public Sprite
{
public:
	SimpleSprite(Point2f position, Texture* sprite, float xScale = 1.f, float yScale = 1.f, float angle = 0.f);
	virtual ~SimpleSprite();

	SimpleSprite(const SimpleSprite& other) = delete;
	SimpleSprite(SimpleSprite&& other) noexcept = delete;
	SimpleSprite& operator=(const SimpleSprite& other) = delete;
	SimpleSprite& operator=(SimpleSprite&& other) noexcept = delete;

	void Draw() const override;

	float GetSpriteHeight() const;
	float GetSpriteWidth() const;
	Point2f GetPos() const;
private:
	Texture* m_Sprite;
	float m_XScale;
	float m_YScale;
	float m_Angle;

	void Update(float elapsedSec) override {}
};

