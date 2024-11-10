#pragma once
#include "PlayerInput.h"
#include "PlayerCharacter.h"
#include <vector>

class Texture;
class Sprite;

class HUD final
{
public:
	HUD(float screenWidth, float screenHeight);
	~HUD();

	HUD(const HUD& other) = delete;
	HUD(HUD&& other) noexcept = delete;
	HUD& operator=(const HUD& other) = delete;
	HUD& operator=(HUD&& other) noexcept = delete;

	void Draw() const;
	void Update(float elapsedSec, bool playerFinished, PlayerCharacter::CharacterName characterName);
	void Update(float elapsedSec, bool playerFinished, const PlayerInput::PlayerStats& playerStats, PlayerCharacter::CharacterName characterName = PlayerCharacter::Sonic);

	bool FinalHudDrawn();
private:
	void SetStatSprites(float screenHeight);
	void SetFinishHudTextures();
	void SetFinishHud(float screenWidth, float screenHeight);

	void DrawTime(Point2f locationLastNumeral) const;
	void DrawNumber(int number, Point2f locationLastNumeral, bool small = false) const;
	void DrawFinishHud() const;
	void DrawLevelComplete() const;
	void DrawScores(int textureIdx, float textureYPos) const;

	void UpdateFinishHud(float elapsedSec);
	void UpdateCharacterName(PlayerCharacter::CharacterName characterName);

	float m_Time;
	PlayerInput::PlayerStats m_Stats;
	PlayerCharacter::CharacterName m_CharacterName;

	std::vector<Sprite*> m_StatSprites;
	Texture* m_LargeNumbersTexturePtr;
	Texture* m_SmallNumbersTexturePtr;
	Texture* m_LevelCompleteTexturePtr;
	std::vector<Texture*> m_FinishHudTextures;
	
	
	Rectf m_FinishHudBox;
	Point2f m_LevelCompletePos;

	const float m_Scale, m_LargeNumeralWidth, m_GapToLargeNumeral,
				m_AmountNumerals, m_MaxDrawDelay;
	
	float m_FinishHudEndX, m_DrawDelay, m_LevelCompleteEndY;
	
	bool m_PlayerFinished;
	bool m_FinalHudDrawn;
};

