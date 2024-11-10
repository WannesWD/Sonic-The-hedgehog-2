#pragma once
#include "AnimatedSprite.h"
#include<vector>

class Texture;

class Avatar final : public AnimatedSprite
{
public:
	Avatar(const std::vector<Texture*>& animationSprites,
		const std::vector<float>& frameTimePerSprite,
		const std::vector<int>& numColsPerSprite,
		const std::vector<int>& numRowsPerSprite,
		int currentSpriteIndex = 0);		

	~Avatar();

	Avatar(const Avatar& other) = delete;
	Avatar(Avatar&& other) noexcept = delete;
	Avatar& operator=(const Avatar& other) = delete;
	Avatar& operator=(Avatar&& other) noexcept = delete;

	void Draw() const override;
	void UpdateFrameTime(float newFrameTime);
	void UpdateAnimationSpriteIndex(int newSpriteIndex);
	void Update(float elapsedSec) override;
	void SetPos(Point2f center) override;
	Point2f GetCenter();
protected:
	std::vector<Texture*>m_pAnimationSprites;
	std::vector<float>m_FrameTimePerSprite;
	std::vector<int>m_NumColsPerSprite;
	std::vector<int>m_NumRowsPerSprite;
	int m_CurrentSpriteIndex;
};

