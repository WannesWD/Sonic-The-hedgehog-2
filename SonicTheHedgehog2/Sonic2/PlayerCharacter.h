#pragma once
#include "PlayerInput.h"
#include "Sprite.h"
#include <vector>

class Level;
class Texture;

class PlayerCharacter final : public PlayerInput
{
public:
	enum CharacterName
	{
		Sonic,
		Shadow
	};

	PlayerCharacter(PlayerCharacter::CharacterName characterName = Sonic, Point2f startPos = Point2f{ 30.f,400.f });
	~PlayerCharacter();

	PlayerCharacter(const PlayerCharacter& other) = delete;
	PlayerCharacter(PlayerCharacter&& other) noexcept = delete;
	PlayerCharacter& operator=(const PlayerCharacter& other) = delete;
	PlayerCharacter& operator=(PlayerCharacter&& other) noexcept = delete;

	void Draw() const override;
	void Update(float elapsedSec, Level* level) override;
	PlayerCharacter::CharacterName GetCharacterName();
private:
	CharacterName m_CharacterName;
	Sprite* m_pSpriteSheets;
	const float m_MaxFlickerTime;
	float m_FlickerTime{};
	
	std::vector<Texture*> GenSprites();
	void UpdateFlickerTime(float elapsedSec);
	void GetSpriteSheets();
};

