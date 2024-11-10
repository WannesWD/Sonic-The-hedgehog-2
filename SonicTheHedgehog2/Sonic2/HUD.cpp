#include "pch.h"
#include "HUD.h"
#include "StaticAnimatedSprite.h"
#include "SimpleSprite.h"
#include "Texture.h"
#include "utils.h"
#include <iostream>

HUD::HUD(float screenWidth, float screenHeight) :
	m_Stats{},
	m_CharacterName{PlayerCharacter::Sonic},
	m_Scale{ 2.3f },
	m_LargeNumbersTexturePtr{ new Texture{"SpriteSheets/HUD/LargeNumbers.png"} },
	m_SmallNumbersTexturePtr{ new Texture{"SpriteSheets/HUD/SmallNumbers.png"} },
	m_LevelCompleteTexturePtr{ new Texture{"SpriteSheets/HUD/LevelComplete.png"} },

	m_LargeNumeralWidth{ m_LargeNumbersTexturePtr->GetWidth() / 10.f },
	m_GapToLargeNumeral{ 1.f * m_Scale },
	m_Time{},
	m_PlayerFinished{ false },
	m_FinishHudBox{},
	m_AmountNumerals{ 10.f },
	m_MaxDrawDelay{ 0.5f },
	m_DrawDelay{},
	m_FinalHudDrawn{ false }
{
	SetStatSprites(screenHeight);
	SetFinishHudTextures();
	SetFinishHud(screenWidth, screenHeight);
}

HUD::~HUD()
{
	for (Sprite*& sprite : m_StatSprites)
	{
		delete sprite;
		sprite = nullptr;
	}

	delete m_LargeNumbersTexturePtr;
	m_LargeNumbersTexturePtr = nullptr;
	delete m_SmallNumbersTexturePtr;
	m_SmallNumbersTexturePtr = nullptr;
	delete m_LevelCompleteTexturePtr;
	m_LevelCompleteTexturePtr = nullptr;
	
	for (Texture*& texture : m_FinishHudTextures)
	{
		delete texture;
		texture = nullptr;
	}
}

void HUD::Draw() const
{
	for (int spriteIdx = 0; spriteIdx < m_StatSprites.size(); spriteIdx++)
	{
		m_StatSprites[spriteIdx]->Draw();

		Point2f locationLastNumeral{ dynamic_cast<SimpleSprite*>(m_StatSprites[spriteIdx])->GetPos() };
		float spriteWidth{ dynamic_cast<SimpleSprite*>(m_StatSprites[spriteIdx])->GetSpriteWidth() * m_Scale };
		float spriteHeight{ dynamic_cast<SimpleSprite*>(m_StatSprites[spriteIdx])->GetSpriteHeight() * m_Scale };

		locationLastNumeral = Point2f{ locationLastNumeral.x + spriteWidth, locationLastNumeral.y };

		switch (spriteIdx)
		{
		case 0:
			DrawNumber(m_Stats.score, locationLastNumeral);
			break;
		case 1:
			DrawTime(locationLastNumeral);
			break;
		case 2:
			DrawNumber(m_Stats.ringCount, locationLastNumeral);
			break;
		case 3:
			DrawNumber(m_Stats.lives, locationLastNumeral, true);
			break;
		default:
			break;
		}
	} 
	if (m_PlayerFinished)
	{
		DrawFinishHud();
	}
}

void HUD::Update(float elapsedSec, bool playerFinished, PlayerCharacter::CharacterName characterName)
{
	UpdateCharacterName(characterName);

	if (!playerFinished)
	{
		m_Time += elapsedSec;
	}
	else
	{
		m_PlayerFinished = true;
		UpdateFinishHud(elapsedSec);
	}
}

void HUD::Update(float elapsedSec, bool playerFinished, const PlayerInput::PlayerStats& playerStats, PlayerCharacter::CharacterName characterName)
{
	if (!playerFinished)
	{
		Update(elapsedSec, playerFinished, characterName);
		m_Stats.lives = playerStats.lives;
		m_Stats.ringCount = playerStats.ringCount;
		m_Stats.score = playerStats.score;
	}
	else
	{
		m_PlayerFinished = true;
		UpdateFinishHud(elapsedSec);
	}
}

bool HUD::FinalHudDrawn()
{
	return m_FinalHudDrawn;
}

void HUD::SetStatSprites(float screenHeight)
{
	m_StatSprites.push_back(new SimpleSprite{ Point2f{},
				new Texture{ "SpriteSheets/HUD/Score.png" } });

	m_StatSprites.push_back(new SimpleSprite{ Point2f{},
				new Texture{ "SpriteSheets/HUD/Time.png" } });

	m_StatSprites.push_back(new SimpleSprite{ Point2f{},
				new Texture{ "SpriteSheets/HUD/Rings.png" } });

	m_StatSprites.push_back(new SimpleSprite{ Point2f{},
				new Texture{ "SpriteSheets/HUD/SonicLives.png"} });

	const float gapToWindow{ 2.f * m_Scale };
	float spriteHeight{ dynamic_cast<SimpleSprite*>(m_StatSprites[0])->GetSpriteHeight() * m_Scale };
	const int numTopDownStats{ 3 };
	for (int spriteIdx{}; spriteIdx < m_StatSprites.size(); spriteIdx++)
	{
		m_StatSprites[spriteIdx]->SetScale(m_Scale, m_Scale);
		if (spriteIdx < numTopDownStats)
		{
			m_StatSprites[spriteIdx]->SetPos(Point2f{ 0.f,screenHeight - ((spriteHeight + gapToWindow) * (spriteIdx + 1)) });
		}
		else
		{
			m_StatSprites[spriteIdx]->SetPos(Point2f{ 0.f,gapToWindow });
		}
	}

}

void HUD::SetFinishHudTextures()
{
	m_FinishHudTextures.push_back(new Texture{ "SpriteSheets/HUD/TimeBonus.png" });
	m_FinishHudTextures.push_back(new Texture{ "SpriteSheets/HUD/RingBonus.png" });
	m_FinishHudTextures.push_back(new Texture{ "SpriteSheets/HUD/Total.png" });
}

void HUD::SetFinishHud(float screenWidth, float screenHeight)
{
	m_FinishHudBox.width = screenWidth / 2.f;
	m_FinishHudBox.height = screenHeight / 3.f;
	m_FinishHudBox.bottom = screenHeight / 5.f;
	m_FinishHudBox.left = -m_FinishHudBox.width/2.f;
	m_FinishHudEndX = screenWidth / 2.f - m_FinishHudBox.width / 2.f;

	m_LevelCompletePos = Point2f{ (screenWidth / 2.f - (m_LevelCompleteTexturePtr->GetWidth() * m_Scale) / 2.f),
									(screenHeight + (m_LevelCompleteTexturePtr->GetHeight() * m_Scale)) };

	m_LevelCompleteEndY = (m_FinishHudBox.bottom + m_FinishHudBox.height) + ((m_LevelCompleteTexturePtr->GetHeight() * m_Scale));
}

void HUD::DrawTime(Point2f locationLastNumeral) const
{
	int seconds{ int(m_Time) };
	int minutes{0};
	while (seconds > 59)
	{
		++minutes;
		seconds -= 60;
	}

	DrawNumber(seconds, locationLastNumeral);
	if (seconds < 10)
	{
		locationLastNumeral = Point2f{ (locationLastNumeral.x - (m_LargeNumeralWidth * 2.f + m_GapToLargeNumeral * 2.f) ) ,locationLastNumeral.y };
		DrawNumber(0, locationLastNumeral);
		locationLastNumeral = Point2f{ (locationLastNumeral.x - (m_LargeNumeralWidth * 4.f + m_GapToLargeNumeral * 4.f) ) ,locationLastNumeral.y };
	}
	else
		locationLastNumeral = Point2f{ (locationLastNumeral.x - (m_LargeNumeralWidth * 6.f + m_GapToLargeNumeral * 6.f)) ,locationLastNumeral.y };
	
	DrawNumber(minutes, locationLastNumeral);
}

void HUD::DrawNumber(int number, Point2f locationLastNumeral, bool small) const
{
	int numNumerals{ 0 };
	bool rangeDetermined{ false };
	do
	{
		int value{ (number / int(powf(10.f, float(numNumerals)))) };
		if ((number / int(powf(10.f, float(numNumerals)))) != 0)
		{
			++numNumerals;
		}
		else
			rangeDetermined = true;
	} while (!rangeDetermined);

	std::vector<int> numerals;

	if (number == 0)
	{
		numerals.push_back(0);
	}
	else
	{
		for (int numeralIdx = (numNumerals - 1); numeralIdx >= 0; numeralIdx--)
		{
			numerals.push_back(number / int(powf(10.f, float(numeralIdx))));
			number -= (number / int(powf(10.f, float(numeralIdx)))) * int(powf(10.f, float(numeralIdx)));
		}
	}

	Texture* numberPtr{ (small) ? m_SmallNumbersTexturePtr : m_LargeNumbersTexturePtr };

	const float srcRectWidth{ numberPtr->GetWidth() / m_AmountNumerals };
	int numeral{};
	for (int numeralIdx{ int(numerals.size()) - 1 }; numeralIdx >= 0; numeralIdx--)
	{
		Rectf srcRect{ srcRectWidth * numerals[numeralIdx] ,0.f, srcRectWidth, numberPtr->GetHeight()};
		glPushMatrix();
		glTranslatef(locationLastNumeral.x - (srcRect.width * m_Scale), locationLastNumeral.y, 0.f);
		glScalef(m_Scale, m_Scale, 0.f);
		numberPtr->Draw(Point2f{ -(m_GapToLargeNumeral + srcRect.width) * numeral, 0.f}, srcRect);
		glPopMatrix();
		++numeral;
	}
}

void HUD::DrawFinishHud() const
{
	const float finishHudBoxTopY{ m_FinishHudBox.bottom + m_FinishHudBox.height };
	for (int textureIdx{ 0 }; textureIdx < m_FinishHudTextures.size(); textureIdx++)
	{
		bool finalTexture{ textureIdx >= (m_FinishHudTextures.size() - 1) };

		float textureXPos{ (!finalTexture) ? m_FinishHudBox.left :
							m_FinishHudBox.left + m_FinishHudTextures[textureIdx]->GetWidth() };
		float textureYPos{ (finalTexture) ? m_FinishHudBox.bottom :
							finishHudBoxTopY - (m_FinishHudTextures[textureIdx]->GetHeight() * m_Scale) * (textureIdx + 1) };

		glPushMatrix();

		glTranslatef(textureXPos, textureYPos, 0.f);
		glScalef(m_Scale, m_Scale, 0.f);
		m_FinishHudTextures[textureIdx]->Draw();
		glPopMatrix();

		if (m_FinishHudBox.left >= m_FinishHudEndX && (m_DrawDelay >= m_MaxDrawDelay * (textureIdx + 1)))
			DrawScores(textureIdx, textureYPos);
	}
	DrawLevelComplete();
}

void HUD::DrawLevelComplete() const
{
	glPushMatrix();
	glTranslatef(m_LevelCompletePos.x, m_LevelCompletePos.y, 1.f);
	glScalef(m_Scale, m_Scale, 0.f);
	m_LevelCompleteTexturePtr->Draw();
	glPopMatrix();
}

void HUD::DrawScores(int textureIdx, float textureYPos) const
{
	const float lastNumeralX{ m_FinishHudBox.left + m_FinishHudBox.width -
								m_LargeNumbersTexturePtr->GetWidth() / m_AmountNumerals };
	Point2f locationLastNumeral{ lastNumeralX,textureYPos };

	int number{};

	const int maxTimeBonus{ 10'000 };
	const int multiplier{ 5 };
	const int maxTimeForMaxBonus{ 30 };
	int timeBonus{ (int(m_Time) < maxTimeForMaxBonus) ? maxTimeBonus * multiplier : maxTimeBonus - (int(m_Time) * 100) };
	int ringBonus{ m_Stats.ringCount * 100 };

	switch (textureIdx)
	{
	case 0:
		number = timeBonus;
		break;
	case 1:
		number = ringBonus;
		break;
	case 2:
		number = timeBonus + ringBonus + m_Stats.score;
	default:
		break;
	}

	DrawNumber(number, locationLastNumeral);
}

void HUD::UpdateFinishHud(float elapsedSec)
{
	const float velocity{ 400.f };
	if (m_FinishHudBox.left < m_FinishHudEndX)
	{
		m_FinishHudBox.left += velocity * elapsedSec;
	}
	else
	{
		if (m_DrawDelay < (m_MaxDrawDelay * m_FinishHudTextures.size()))
		{
			m_DrawDelay += elapsedSec;
		}
		else
		{
			m_FinalHudDrawn = true;
		}
	}

	if (m_LevelCompletePos.y > m_LevelCompleteEndY)
	{
		m_LevelCompletePos.y -= velocity * elapsedSec;
	}

}

void HUD::UpdateCharacterName(PlayerCharacter::CharacterName characterName)
{
	if (m_CharacterName != characterName)
	{

		delete m_StatSprites[(m_StatSprites.size() - 1)];
		std::string characterNameString{};
		switch (characterName)
		{
		case PlayerCharacter::Sonic:
			characterNameString = "Sonic";
			break;
		case PlayerCharacter::Shadow:
			characterNameString = "Shadow";
			break;
		default:
			break;
		}

		const float gapToWindow{ 2.f * m_Scale };
		m_StatSprites[(m_StatSprites.size() - 1)] = new SimpleSprite{ Point2f{}, new Texture{"SpriteSheets/HUD/" +
																		characterNameString + "Lives.png"} };
		m_StatSprites[(m_StatSprites.size() - 1)]->SetScale(m_Scale, m_Scale);
		m_StatSprites[(m_StatSprites.size() - 1)]->SetPos(Point2f{ 0.f,gapToWindow });

		m_CharacterName = characterName;
	}
}
