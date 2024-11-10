#pragma once
#include <vector>
class Texture;
class FloatingPlatform final
{
public:
	enum PlatformType
	{
		vertical,
		horizontal
	};

	FloatingPlatform(const PlatformType& platformType, 
		const Point2f& bottomLeftPos, bool oppositMovement = false);
	~FloatingPlatform();

	FloatingPlatform(const FloatingPlatform& other) = delete;
	FloatingPlatform(FloatingPlatform&& other) noexcept = delete;
	FloatingPlatform& operator=(const FloatingPlatform& other) = delete;
	FloatingPlatform& operator=(FloatingPlatform&& other) noexcept = delete;

	void Update(float elapsedSec);
	void Draw() const;

	std::vector<Point2f> GetPlatformSVG();
private:
	static Texture* PLATFORMTEXTUREPTR;
	static int INSTANCECOUNTER;

	static const float
		MAXLERPTIME,
		MAXLERPDISTANCE;

	const Point2f m_SpawnPos;
	Point2f m_CurrentPos;

	float
		m_LerpTime,
		m_LerpFactor;

	PlatformType m_PlatformType;
	bool m_OppositeMovement;
};

