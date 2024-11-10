#pragma once
#include "SVGParser.h"
#include "Vector2f.h"
#include "utils.h"
#include <vector>	
class Texture;
class Sprite;
class Bridge;
class FloatingPlatform;
class PlayerInput;

class Level final
{
public:
	Level();
	~Level();

	Level(const Level& other) = delete;
	Level(Level&& other) noexcept = delete;
	Level& operator=(const Level& other) = delete;
	Level& operator=(Level&& other) noexcept = delete;

	void Draw(bool drawBackground) const;
	void Update(float elapsedSec, bool playerOnGround, PlayerInput* playerHitbox);
	void DrawParallax(const Point2f& camPos, float viewportWidth, float viewportHeight, float zoomScale) const;

	bool GetHitInfo(const Vector2f& playerVelocity, const Vector2f& playerGroundVelocity, 
					const Point2f& rayP1, const Point2f& rayP2, 
					utils::HitInfo& hitInfo, bool characterOnBackground, 
					bool& detectedOnBackground, bool& detectedOnCrokscrew, 
					bool wallDetection) const;

	bool GetHitInfo(const Vector2f& playerVelocity, const Vector2f& playerGroundVelocity, 
					const Point2f& rayP1, const Point2f& rayP2,
					utils::HitInfo& hitInfo, bool characterOnBackground = false, 
					bool wallDetection = false) const;
	bool LevelFinished();

	void reset();

	const Rectf& GetBoundaries() const;
private:
	typedef std::vector<std::vector<Point2f>> SVG;

	const int m_NumParallaxTextures;
	const int m_NumFlowers;
	int m_FirstFlippedSpikeIdx;

	const float m_MaxFlowerIdleTime;
	float m_ParallaxOffset;
	float m_FlowerIdleTime;

	bool m_Finshed;

	SVG m_Vertices, m_LoopForeGroundVertices, m_LoopBackGroundVertices,
		 m_CorkscrewVertices, m_BridgeVertices, m_FloatingPlatformVertices, m_SpikeVertices, m_SpikeLocations;

	Texture* m_pBackGroundTexturePtr;
	Texture* m_pForeGroundTexturePtr;
	Texture* m_SpikeTexturePtr;

	std::vector<Texture*> m_ParallaxTextureVector;
	std::vector<Texture*> m_FlowerSpriteSheetVector;
	std::vector<Sprite*> m_FlowerSpriteVector;
	std::vector<Bridge*> m_BridgeVector;
	std::vector<FloatingPlatform*> m_FloatingPlatformVector;
	std::vector<Rectf> m_SpikeHitboxVector;

	Rectf m_Boundaries;

	void InitParallaxTextureVector();
	void InitFlowerSpriteVector();
	void InitBridgeVector();
	void InitFloatingPlatformVector();
	void InitSpikeVertices();

	void updateFlowers(float elapsedSec);
	void updateFloatingPlatforms(float elapsedSec);
	void DrawFlowers() const;
	void DrawBridges() const;
	void DrawFloatingPlatforms() const;
	void DrawSpikes() const;
};

