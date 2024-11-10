#pragma once
class Camera
{
public:
	Camera(float width, float height);
	~Camera() = default;

	Camera(const Camera& other) = delete;
	Camera(Camera&& other) noexcept = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other) noexcept = delete;

	void Update(float elapsedSec, const Point2f& target);
	void SetLevelBoundries(const Rectf& levelBoundaries);

	void Transform() const;
	Point2f getCameraBottomLeft() const;
	float getZoomScale() const;
	void Draw() const;
private:
	Point2f Track(float elapseSec, const Point2f& targetCenter, const Rectf& margin);
	void Clamp(Point2f& bottomLeftPos) const;

	const float m_ZoomScale;
	const float m_CamVelocity;
	float m_Width;
	float m_Height;

	bool m_NotCenteredX;
	bool m_NotCenteredY;

	Rectf m_LevelBoundaris;
	Point2f m_CamPos;
};

