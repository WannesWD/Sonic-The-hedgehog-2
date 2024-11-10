#pragma once

class Texture;

class Sprite
{
public:
	Sprite(Point2f position);
	Sprite(Point2f position, float angle, float xScale, float yScale);
	virtual ~Sprite() = default;

	Sprite(const Sprite& other) = delete;
	Sprite(Sprite&& other) noexcept = delete;
	Sprite& operator=(const Sprite& other) = delete;
	Sprite& operator=(Sprite&& other) noexcept = delete;

	virtual void Draw() const = 0;
	virtual void Update(float elapsedSec) = 0;

	virtual void SetPos(Point2f newPos);
	void SetScale(float xScale, float yScale);
	void SetAngle(float angle);

protected:
	Point2f m_Position;
	float m_Angle;
	float m_XScale;
	float m_YScale;

	void ApplyTRS() const;

};

