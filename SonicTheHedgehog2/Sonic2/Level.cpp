#include "pch.h"
#include "Level.h"
#include "Texture.h"
#include "utils.h"
#include "Bridge.h"
#include "FloatingPlatform.h"
#include "SimpleSprite.h"
#include "StaticAnimatedSprite.h"
#include "PlayerInput.h"
#include <iostream>


Level::Level() :
	m_NumParallaxTextures{ 18 },
	m_NumFlowers{ 4 },
	m_pBackGroundTexturePtr{ new Texture{"FullMap/Level/Background.png"} },
	m_pForeGroundTexturePtr{ new Texture{"FullMap/Level/ForeGround.png"} },
	m_SpikeTexturePtr{ new Texture{ "FullMap/Spikes/Spikes.png" } },

	m_Boundaries{ Rectf{0.f, 0.f, m_pBackGroundTexturePtr->GetWidth(), m_pBackGroundTexturePtr->GetHeight()} },
	m_ParallaxOffset{ 0.f },
	m_FlowerIdleTime{ 0.f },
	m_MaxFlowerIdleTime{ 2.f },
	m_FloatingPlatformVertices{},
	m_Finshed{ false }
{
	InitParallaxTextureVector();
	InitFlowerSpriteVector();
	InitBridgeVector();
	InitFloatingPlatformVector();
	InitSpikeVertices();

	SVGParser::GetVerticesFromSvgFile("FullMap/Level/Ground.svg", m_Vertices);
	SVGParser::GetVerticesFromSvgFile("FullMap/Level/ForegroundLoop.svg", m_LoopForeGroundVertices);
	SVGParser::GetVerticesFromSvgFile("FullMap/Level/BackGroundLooop.svg", m_LoopBackGroundVertices);
	SVGParser::GetVerticesFromSvgFile("FullMap/Level/Corkscrew.svg", m_CorkscrewVertices);
	SVGParser::GetVerticesFromSvgFile("FullMap/Level/Bridges.svg", m_BridgeVertices);
}

Level::~Level()
{
	delete m_pBackGroundTexturePtr;
	m_pBackGroundTexturePtr = nullptr;
	delete m_pForeGroundTexturePtr;
	m_pForeGroundTexturePtr = nullptr;

	for (int textureIdx = 0; textureIdx < m_NumParallaxTextures; textureIdx++)
	{
		delete m_ParallaxTextureVector[textureIdx];
		m_ParallaxTextureVector[textureIdx] = nullptr;
	}
	for (Texture*& spriteSheet : m_FlowerSpriteSheetVector)
	{
		delete spriteSheet;
		spriteSheet = nullptr;
	}
	for (Sprite*& sprite : m_FlowerSpriteVector)
	{
		delete sprite;
		sprite = nullptr;
	}
	for (Bridge*& bridge : m_BridgeVector)
	{
		delete bridge;
		bridge = nullptr;
	}
	for (FloatingPlatform*& platform : m_FloatingPlatformVector)
	{
		delete platform;
		platform = nullptr;
	}

	delete m_SpikeTexturePtr;
	m_SpikeTexturePtr = nullptr;
}

void Level::Draw(bool drawBackground) const
{
	if (drawBackground)
	{
		m_pBackGroundTexturePtr->Draw();

		DrawSpikes();
		DrawFlowers();
		DrawBridges();
		DrawFloatingPlatforms();
	}
	else
	{
		m_pForeGroundTexturePtr->Draw();
	}

}

void Level::Update(float elapsedSec, bool playerOnGround, PlayerInput* playerPtr)
{
	updateFlowers(elapsedSec);
	if (!m_Finshed)
	{
		updateFloatingPlatforms(elapsedSec);
		const Rectf playerHitbox{ playerPtr->GetHitBox() };
		for (Bridge* bridge : m_BridgeVector)
		{
			bridge->Update(playerHitbox, playerOnGround);
		}
		if (!utils::IsOverlapping(playerHitbox, m_Boundaries))
		{
			if (playerPtr->HasFinished())
			{
				if (playerHitbox.left > (m_Boundaries.left + m_Boundaries.width + playerHitbox.width))
				{
					m_Finshed = true;
				}
			}
			else if (playerHitbox.left < (m_Boundaries.left + m_Boundaries.width))
			{
				playerPtr->Respawn();
			}
		}
		for (Rectf& SpikeHitbox : m_SpikeHitboxVector)
		{

			if (utils::IsOverlapping(playerPtr->GetHitBox(), SpikeHitbox))
			{
				playerPtr->Hit();
			}
		}
	}
}

void Level::DrawParallax(const Point2f& camPos, float viewportWidth, float viewportHeight, float zoomScale) const
{
	const float screenWidth{ viewportWidth / zoomScale };
	const float xOffset{ ((camPos.x) / (m_Boundaries.width - screenWidth)) * (m_ParallaxTextureVector[0]->GetWidth() - screenWidth) };
	float prevTextureBottom{ camPos.y + (viewportHeight / zoomScale) };

	glPushMatrix();
	for (int textureIdx = 0; textureIdx < m_NumParallaxTextures; textureIdx++)
	{
		prevTextureBottom -= m_ParallaxTextureVector[textureIdx]->GetHeight();
		Point2f TexturePos{ camPos.x - (xOffset * (textureIdx + 1)) , prevTextureBottom };

		if (TexturePos.x + m_ParallaxTextureVector[textureIdx]->GetWidth() < camPos.x)
		{
			TexturePos.x += m_ParallaxTextureVector[textureIdx]->GetWidth();
		}

		m_ParallaxTextureVector[textureIdx]->Draw(TexturePos);
		float endOfTexture{ TexturePos.x + m_ParallaxTextureVector[textureIdx]->GetWidth() };
		while (endOfTexture < (camPos.x + viewportWidth))
		{
			m_ParallaxTextureVector[textureIdx]->Draw(Point2f{ endOfTexture, TexturePos.y });
			endOfTexture += m_ParallaxTextureVector[textureIdx]->GetWidth();
		}
	}
	glPopMatrix();
}

bool Level::GetHitInfo(const Vector2f& playerVelocity, const Vector2f& playerGroundVelocity, 
						const Point2f& rayP1, const Point2f& rayP2, utils::HitInfo& hitInfo, 
						bool characterOnBackground, bool& detectedOnBackground, 
						bool& detectedOnCrokscrew, bool wallDetection) const
{
	const float minCorkScrewVelocity{ 400.f };
	utils::HitInfo groundHitInfo{};
	utils::HitInfo loopForegroundHitInfo{};
	utils::HitInfo loopBackgroundHitInfo{};
	utils::HitInfo corkscrewHitInfo{};
	utils::HitInfo bridgeHitInfo{};
	utils::HitInfo platformHitInfo{};


	for (int svgIdx = 0; svgIdx < m_CorkscrewVertices.size(); svgIdx++)
	{
		if (utils::Raycast(m_CorkscrewVertices[svgIdx], rayP1, rayP2, corkscrewHitInfo))
		{
			if (!characterOnBackground && abs(playerGroundVelocity.x) > minCorkScrewVelocity &&
				playerGroundVelocity.y == 0.f && !wallDetection)
			{
				hitInfo = corkscrewHitInfo;
				detectedOnBackground = false;
				detectedOnCrokscrew = true;
				return true;
			}
		}
	}

	for (int svgIdx = 0; svgIdx < m_BridgeVertices.size(); svgIdx++)
	{
		if (utils::Raycast(m_BridgeVertices[svgIdx], rayP1, rayP2, bridgeHitInfo))
		{
			detectedOnBackground = false;
			detectedOnCrokscrew = false;
			if (!characterOnBackground && playerVelocity.y < 1.f)
			{
				hitInfo = bridgeHitInfo;
				hitInfo.normal = Vector2f{ 0.f,1.f };
				return true;
			}
		}
	}


	for (int svgIdx = 0; svgIdx < m_Vertices.size(); svgIdx++)
	{
		if (utils::Raycast(m_Vertices[svgIdx], rayP1, rayP2, groundHitInfo))
		{
			hitInfo = groundHitInfo;
			detectedOnBackground = false;
			detectedOnCrokscrew = false;
			if (!characterOnBackground)
			{
				return true;
			}
		}
	}

	for (int loopIdx = 0; loopIdx < m_LoopForeGroundVertices.size(); loopIdx++)
	{
		if (utils::Raycast(m_LoopForeGroundVertices[loopIdx], rayP1, rayP2, loopForegroundHitInfo))
		{
			hitInfo = loopForegroundHitInfo;
			detectedOnBackground = false;
			detectedOnCrokscrew = false;
			if (!characterOnBackground)
			{
				return true;
			}
		}
	}

	for (int loopIdx = 0; loopIdx < m_LoopBackGroundVertices.size(); loopIdx++)
	{
		if (utils::Raycast(m_LoopBackGroundVertices[loopIdx], rayP1, rayP2, loopBackgroundHitInfo))
		{
			hitInfo = loopBackgroundHitInfo;
			detectedOnBackground = true;
			detectedOnCrokscrew = false;
			if (!wallDetection)
			{
				return true;
			}
		}
	}

	for (int svgIdx = 0; svgIdx < m_FloatingPlatformVertices.size(); svgIdx++)
	{
		if (utils::Raycast(m_FloatingPlatformVertices[svgIdx], rayP1, rayP2, platformHitInfo))
		{
			detectedOnBackground = false;
			detectedOnCrokscrew = false;
			if (!characterOnBackground && playerVelocity.y < 1.f)
			{
				hitInfo = platformHitInfo;
				return true;
			}
		}
	}

	for (int svgIdx = 0; svgIdx < m_SpikeVertices.size(); svgIdx++)
	{
		if (utils::Raycast(m_SpikeVertices[svgIdx], rayP1, rayP2, groundHitInfo))
		{
			detectedOnBackground = false;
			detectedOnCrokscrew = false;
			if (!characterOnBackground && playerVelocity.y < 1.f)
			{
				hitInfo = groundHitInfo;
				return true;
			}
		}
	}

	return false;
}

bool Level::GetHitInfo(const Vector2f& playerVelocity, const Vector2f& playerGroundVelocity, const Point2f& rayP1, const Point2f& rayP2, utils::HitInfo& hitInfo, bool characterOnBackground, bool wallDetection) const
{
	bool uselessBool{};
	return GetHitInfo(playerVelocity, playerGroundVelocity, rayP1, rayP2, hitInfo, characterOnBackground, uselessBool, uselessBool, wallDetection);
}

bool Level::LevelFinished()
{
	return m_Finshed;
}

void Level::reset()
{
	m_Finshed = false;
}

const Rectf& Level::GetBoundaries() const
{
	return m_Boundaries;
}

void Level::InitParallaxTextureVector()
{
	for (int textureIdx = 0; textureIdx < m_NumParallaxTextures; textureIdx++)
	{
		m_ParallaxTextureVector.push_back(new Texture{ "FullMap/Parallax/ParallaxBackground" +
														std::to_string(textureIdx) + ".png" });
	}
}

void Level::InitFlowerSpriteVector()
{
	for (int flowerIdx = 0; flowerIdx < m_NumFlowers; flowerIdx++)
	{
		m_FlowerSpriteSheetVector.push_back(new Texture{ "FullMap/Flowers/Flower" +
														std::to_string(flowerIdx) + ".png" });
	}

	std::vector<SVG> flowerLocationsVector;
	for (int flowerIdx = 0; flowerIdx < m_NumFlowers; flowerIdx++)
	{
		SVG flowerLocations;
		SVGParser::GetVerticesFromSvgFile("FullMap/Flowers/Flower" +
			std::to_string(flowerIdx) + "Locations.svg",
			flowerLocations);
		flowerLocationsVector.push_back(flowerLocations);
	}

	const float flowerFrameTime{ 0.166f };
	const int numFlowerCols{ 1 };
	const int numFlowerRows{ 2 };
	for (int flowerIdx = 0; flowerIdx < flowerLocationsVector.size(); flowerIdx++)
	{
		for (int svgIdx = 0; svgIdx < flowerLocationsVector[flowerIdx].size(); svgIdx++)
		{
			for (int locationIdx = 0; locationIdx < flowerLocationsVector[flowerIdx][svgIdx].size(); locationIdx++)
			{
				m_FlowerSpriteVector.push_back(new StaticAnimatedSprite{ flowerLocationsVector[flowerIdx][svgIdx][locationIdx],
												flowerFrameTime,numFlowerCols,numFlowerRows,m_FlowerSpriteSheetVector[flowerIdx] });
			}
		}
	}
}

void Level::InitBridgeVector()
{
	const int numBridgeVariants{ 4 };
	std::vector<SVG> bridgeLocationsVector;
	for (int bridgeVariantIdx{0}; bridgeVariantIdx < numBridgeVariants ; ++bridgeVariantIdx)
	{
		int numLogs{};
		switch (Bridge::NumLogs(bridgeVariantIdx))
		{
		case Bridge::eight:
			numLogs = 8;
			break;
		case Bridge::ten:
			numLogs = 10;
			break;
		case Bridge::twelve:
			numLogs = 12;
			break;
		case Bridge::sixteen:
			numLogs = 16;
			break;
		default:
			break;
		}

		SVG bridgeLocations;
		SVGParser::GetVerticesFromSvgFile("FullMap/Bridges/" +
			std::to_string(numLogs) + "LogBridgeLocations.svg", bridgeLocations);
		bridgeLocationsVector.push_back(bridgeLocations);
	}

	for (int bridgeVariant = 0; bridgeVariant < bridgeLocationsVector.size(); bridgeVariant++)
	{
		for (int svgIdx = 0; svgIdx < bridgeLocationsVector[bridgeVariant].size(); svgIdx++)
		{
			for (int locationIdx = 0; locationIdx < bridgeLocationsVector[bridgeVariant][svgIdx].size(); locationIdx++)
			{
				m_BridgeVector.push_back(new Bridge{bridgeLocationsVector[bridgeVariant][svgIdx][locationIdx],Bridge::NumLogs(bridgeVariant)});
			}
		}
	}
}

void Level::InitFloatingPlatformVector()
{
	const int numPlatformTypes{ 2 };
	std::vector<SVG>platformLocatoinsVector{};
	for (int platformType = 0; platformType < numPlatformTypes; platformType++)
	{
		SVG platformLocations{};
		
		std::string svgPath{ (platformType == 0) ? "FullMap/FloatingPlatforms/VerticalFloatingPlatformLocations.svg" :
							"FullMap/FloatingPlatforms/HorizontalFloatingPlatformLocations.svg" };

		SVGParser::GetVerticesFromSvgFile(svgPath, platformLocations);
		platformLocatoinsVector.push_back(platformLocations);

	}

	for (int platformVariant = 0; platformVariant < platformLocatoinsVector.size(); platformVariant++)
	{
		for (int svgIdx = 0; svgIdx < platformLocatoinsVector[platformVariant].size(); svgIdx++)
		{
			for (int platformIdx = 0; platformIdx < platformLocatoinsVector[platformVariant][svgIdx].size(); platformIdx++)
			{
				FloatingPlatform::PlatformType platformType{ FloatingPlatform::PlatformType(platformVariant) };
				bool oppositMovement{ false };
				if (platformType == FloatingPlatform::horizontal && ((platformIdx % 2 == 0) || platformIdx == 0))
				{
					oppositMovement = true;
				}

				m_FloatingPlatformVector.push_back(new FloatingPlatform{ platformType,
									platformLocatoinsVector[platformVariant][svgIdx][platformIdx], oppositMovement });
			}
		}
	}

	for (FloatingPlatform* platform : m_FloatingPlatformVector)
	{
		m_FloatingPlatformVertices.push_back(platform->GetPlatformSVG());
	}

	SVG staticFloatingPlatformVertices;
	SVGParser::GetVerticesFromSvgFile("FullMap/FloatingPlatforms/StaticFloatingPlatforms.svg", staticFloatingPlatformVertices);

	for (std::vector<Point2f> vertices : staticFloatingPlatformVertices)
	{
		m_FloatingPlatformVertices.push_back(vertices);
	}

}

void Level::InitSpikeVertices()
{
	const float margin{ 1.f };

	SVGParser::GetVerticesFromSvgFile("FullMap/Spikes/SpikeLocations.svg", m_SpikeLocations);
	m_FirstFlippedSpikeIdx = int(m_SpikeLocations.size());
	SVG flippedSpikeLocations;
	SVGParser::GetVerticesFromSvgFile("FullMap/Spikes/FlippedSpikeLocations.svg", flippedSpikeLocations);
	for (std::vector<Point2f> flippedSpikePos : flippedSpikeLocations)
	{
		m_SpikeLocations.push_back(flippedSpikePos);
	}

	for (int svgIdx = 0; svgIdx < m_SpikeLocations.size(); svgIdx++)
	{
		for (int spikeIdx = 0; spikeIdx < m_SpikeLocations[svgIdx].size(); spikeIdx++)
		{
			Point2f bottomLeft{};
			Point2f topLeft{};
			Point2f bottomRight{};
			Point2f topRight{};
			if (svgIdx < m_FirstFlippedSpikeIdx)
			{
				bottomLeft = m_SpikeLocations[svgIdx][spikeIdx];
				topLeft = Point2f{ bottomLeft.x, bottomLeft.y + m_SpikeTexturePtr->GetHeight() };
				bottomRight = Point2f{bottomLeft.x + m_SpikeTexturePtr->GetWidth(), bottomLeft.y};
				topRight = Point2f{bottomRight.x, topLeft.y};
			}
			else
			{
				topRight = m_SpikeLocations[svgIdx][spikeIdx];
				bottomRight = Point2f{ topRight.x, topRight.y - m_SpikeTexturePtr->GetHeight() };
				topLeft = Point2f{ topRight.x - m_SpikeTexturePtr->GetWidth(), topRight.y };
				bottomLeft = Point2f{ topLeft.x, bottomRight.y };
			}


			m_SpikeVertices.push_back(std::vector<Point2f>{Point2f{ topLeft.x - margin, topLeft.y }, 
															Point2f{ bottomLeft.x - margin, bottomLeft.y }});
			m_SpikeVertices.push_back(std::vector<Point2f>{Point2f{ topRight.x + margin, topRight.y },
															Point2f{ bottomRight.x + margin, bottomRight.y }});

			m_SpikeHitboxVector.push_back(Rectf{ bottomLeft.x, bottomLeft.y, 
													m_SpikeTexturePtr->GetWidth(), m_SpikeTexturePtr->GetHeight() });
		}
	}
}

void Level::updateFlowers(float elapsedSec)
{
	if (m_FlowerIdleTime >= m_MaxFlowerIdleTime)
	{
		for (Sprite*& flower : m_FlowerSpriteVector)
		{
			flower->Update(elapsedSec);
		}
		if (m_FlowerIdleTime >= m_MaxFlowerIdleTime + m_MaxFlowerIdleTime * (1.f / 2.f))
		{
			m_FlowerIdleTime = 0.f;
		}
	}
	m_FlowerIdleTime += elapsedSec;
}

void Level::updateFloatingPlatforms(float elapsedSec)
{
	for (int platformIdx = 0; platformIdx < m_FloatingPlatformVector.size(); platformIdx++)
	{
		m_FloatingPlatformVector[platformIdx]->Update(elapsedSec);
		m_FloatingPlatformVertices[platformIdx] = m_FloatingPlatformVector[platformIdx]->GetPlatformSVG();
	}
}

void Level::DrawFlowers() const
{
	for (Sprite* flower : m_FlowerSpriteVector)
	{
		flower->Draw();
	}
}

void Level::DrawBridges() const
{
	for (Bridge* bridge : m_BridgeVector)
	{
		bridge->Draw();
	}
}

void Level::DrawFloatingPlatforms() const
{
	for (FloatingPlatform* platform : m_FloatingPlatformVector)
	{
		platform->Draw();
	}
}

void Level::DrawSpikes() const
{
	for (int svgIdx = 0; svgIdx < m_SpikeLocations.size(); svgIdx++)
	{
		for (int spikeIdx = 0; spikeIdx < m_SpikeLocations[svgIdx].size(); spikeIdx++)
		{
			Point2f spikeLocation{ m_SpikeLocations[svgIdx][spikeIdx] };

			glPushMatrix();
			
			if (svgIdx >= m_FirstFlippedSpikeIdx)
			{
				glTranslatef(spikeLocation.x, spikeLocation.y, 0.f);
				glRotatef(180, 0, 0, 1);
				glTranslatef(-spikeLocation.x, -spikeLocation.y, 0.f);
			}

			m_SpikeTexturePtr->Draw(spikeLocation);
			glPopMatrix();
		}
	}
}



