#pragma once
#include <vector>

class Texture;
class Sprite;

class Bridge final
{
public:
	enum NumLogs
	{
		eight,
		ten,
		twelve,
		sixteen
	};

	Bridge(const Point2f& centerPos, const NumLogs& numLogs);
	~Bridge();

	Bridge(const Bridge& other) = delete;
	Bridge(Bridge&& other) noexcept = delete;
	Bridge& operator=(const Bridge& other) = delete;
	Bridge& operator=(Bridge&& other) noexcept = delete;

	void Draw();
	void Update(const Rectf& playerHitbox, bool playerOnground);

private:
	void InitSpriteSheetVector();
	void InitBridgeSpriteAndHitbox(const Point2f& centerPos);
	void InitHitbox(const Point2f centerPos, float spriteWidth, float spriteHeight);

	static std::vector<Texture*> m_BridgeSpriteSheetVector;

	static int INSTANCECOUNTER;
	static const int NUMBRIDGEVARIANTS;
	
	Rectf m_Hitbox;

	const NumLogs m_NumLogs;

	Sprite* m_BridgeSprite;
};


