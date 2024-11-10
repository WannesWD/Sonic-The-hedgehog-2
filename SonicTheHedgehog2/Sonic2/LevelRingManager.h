#pragma once
#include <vector>

class Ring;
class Sprite;

class LevelRingManager final
{
public:
	LevelRingManager(const std::string& ringLocationSVG);
	~LevelRingManager();

	LevelRingManager(const LevelRingManager& other) = delete;
	LevelRingManager(LevelRingManager&& other) noexcept = delete;
	LevelRingManager& operator=(const LevelRingManager& other) = delete;
	LevelRingManager& operator=(LevelRingManager&& other) noexcept = delete;

	void Update(float elapsedSec);
	void Draw() const;
	bool RingHit(const Rectf& otherHitbox);
private:
	std::vector<std::vector<Point2f>> m_RingPositions;
	std::vector<Ring*> m_pLevelRings;
};

