#include "pch.h"
#include "StartMenu.h"
#include "SoundManager.h"
#include "Texture.h"
#include "StaticAnimatedSprite.h"
#include "PlayerCharacter.h"
#include "utils.h"

StartMenu::StartMenu(float screenWidth, float screenHeight, PlayerCharacter::CharacterName characterName) :
	m_NumLogoFrames{ 43 },
	m_NumMenuFrames{ 12 },
	m_ScreenWidth{ screenWidth },
	m_ScreenHeight{ screenHeight },
	m_MenuScale{ 1.8f },
	m_LogoScale{ 3.f },
	m_LogoFrameTime{ 0.025f },
	m_MenuFrameTime{ 0.1f },
	m_AfterLevel{ false },
	m_MenuDrawn{ false },
	m_PlayedLogoSound{ false },
	m_PlayedMenuMusic{ false },
	m_CharacterName{ characterName },
	m_MenuColor{ 0.f,0.025f,0.778f,1.f },
	m_FadeInColor{ 0.f,0.f,0.f,0.f },
	m_MaxFadeInTime{ 1.f },
	m_MenuAnimationTime{},
	m_LogoAnimationTime{},
	m_FadeInTime{},
	m_MenuBackground{ new Texture{"SpriteSheets/StartMenu/MenuBackground.png"} },
	m_SonicMenuSpriteTexturePtr{ new Texture{ "SpriteSheets/StartMenu/Sonic/SonicMenu.png" } },
	m_ShadowMenuSpriteTexturePtr{ new Texture{ "SpriteSheets/StartMenu/Shadow/ShadowMenu.png" } },
	m_SonicMenuSpritePtr{ new StaticAnimatedSprite{ Point2f{},m_MenuFrameTime,m_NumMenuFrames,1,m_SonicMenuSpriteTexturePtr } },
	m_ShadowMenuSpritePtr{ new StaticAnimatedSprite{ Point2f{},m_MenuFrameTime,m_NumMenuFrames,1,m_ShadowMenuSpriteTexturePtr } },
	m_SegaLogoSpriteSheetPtr{ new Texture{"SpriteSheets/StartMenu/SegaLogo.png"} },
	m_SegaLogoSpritePtr{ new StaticAnimatedSprite{Point2f{},m_LogoFrameTime,1,m_NumLogoFrames,m_SegaLogoSpriteSheetPtr} }
{
	const float logoHeight{ m_SegaLogoSpriteSheetPtr->GetHeight() / m_NumLogoFrames };
	const float logoWidth{ m_SegaLogoSpriteSheetPtr->GetWidth() };
	m_SegaLogoSpritePtr->SetScale(m_LogoScale, m_LogoScale);
	m_SegaLogoSpritePtr->SetPos(Point2f{ m_ScreenWidth/2.f - logoWidth/2.f, (m_ScreenHeight / 2.f) - logoHeight / 2.f });

	Point2f centeredLogoPos{ m_ScreenWidth / 2.f - ((m_SonicMenuSpriteTexturePtr->GetWidth() / m_NumMenuFrames)) / 2.f,
								m_ScreenHeight / 2.f - ((m_SonicMenuSpriteTexturePtr->GetHeight() / m_NumMenuFrames)) / 2.f };
	
	m_SonicMenuSpritePtr->SetScale(m_MenuScale, m_MenuScale);
	m_SonicMenuSpritePtr->SetPos(centeredLogoPos);
	m_ShadowMenuSpritePtr->SetScale(m_MenuScale, m_MenuScale);
	m_ShadowMenuSpritePtr->SetPos(centeredLogoPos);

}

StartMenu::~StartMenu()
{
	delete m_SegaLogoSpriteSheetPtr;
	m_SegaLogoSpriteSheetPtr = nullptr;
	delete m_SegaLogoSpritePtr;
	m_SegaLogoSpritePtr = nullptr;
	delete m_MenuBackground;
	m_MenuBackground = nullptr;
	delete m_SonicMenuSpritePtr;
	m_SonicMenuSpritePtr = nullptr;
	delete m_SonicMenuSpriteTexturePtr;
	m_SonicMenuSpriteTexturePtr = nullptr;
	delete m_ShadowMenuSpritePtr;
	m_ShadowMenuSpritePtr = nullptr;
	delete m_ShadowMenuSpriteTexturePtr;
	m_ShadowMenuSpriteTexturePtr = nullptr;
}

void StartMenu::Update(float elapsedSec, bool afterLevel)
{
	if (!afterLevel)
	{
		UpdateGameStartUp(elapsedSec);
	}
	else
	{
		if (!m_AfterLevel && afterLevel)
		{
			m_FadeInTime = 0.f;
			m_AfterLevel = afterLevel;
		}
		UpdateMenu(elapsedSec);
	}

}

void StartMenu::Draw() const
{
	if (!m_AfterLevel)
	{
		DrawGameStartUp();
	}
	else
	{
		DrawMenu();
	}
}

void StartMenu::Reset()
{
	ResetMenuAnimation();
	m_FadeInColor.a = 0.f;
	m_FadeInTime = 0.f;
	m_MenuAnimationTime = 0.f;
	m_MenuDrawn = false;
	m_PlayedMenuMusic = false;
}

bool StartMenu::MenuDrawn()
{
	return m_MenuDrawn;
}

void StartMenu::SetMenuCharacter(PlayerCharacter::CharacterName characterName)
{
	m_CharacterName = characterName;

	//did not work, dont know why
	//if (m_MenuSpriteTexturePtr != nullptr && m_MenuSpritePtr != nullptr)
	//{
	//	delete m_MenuSpriteTexturePtr;
	//	m_MenuSpriteTexturePtr = nullptr;
	//	delete m_MenuSpritePtr;
	//	m_MenuSpritePtr = nullptr;
	//}
	//m_MenuSpriteTexturePtr = new Texture{ "SpriteSheets/StartMenu/" + characterName + "/" + characterName + "Menu.png" };
	//m_MenuSpritePtr = new StaticAnimatedSprite{ Point2f{},m_MenuFrameTime,m_NumMenuFrames,1,m_MenuSpriteTexturePtr };
}

void StartMenu::UpdateGameStartUp(float elapsedSec)
{
	if (m_LogoAnimationTime < (m_LogoFrameTime * (m_NumLogoFrames - 1)))
	{
		m_SegaLogoSpritePtr->Update(elapsedSec);
		m_LogoAnimationTime += elapsedSec;
	}
	if (m_LogoAnimationTime > (m_LogoFrameTime * (m_NumLogoFrames - 1)) / 2.f)
	{
		if (!m_PlayedLogoSound)
		{
			SoundManager::GetSoundManager()->PlaySound(SoundManager::segaLogo, 0);
			m_PlayedLogoSound = true;
		}
	}
	if (m_LogoAnimationTime >= (m_LogoFrameTime * (m_NumLogoFrames - 1)))
	{
		UpdateMenu(elapsedSec);
	}
}

void StartMenu::DrawGameStartUp() const
{
	utils::SetColor(Color4f{ 1.f,1.f,1.f,1.f });
	utils::FillRect(0.f, 0.f, m_ScreenWidth, m_ScreenHeight);
	m_SegaLogoSpritePtr->Draw();
	if (m_LogoAnimationTime >= (m_LogoFrameTime * (m_NumLogoFrames - 1)))
	{
		DrawMenu();
	}
}

void StartMenu::DrawMenu() const
{
	if (m_FadeInTime > m_MaxFadeInTime)
	{
		m_MenuBackground->Draw(Rectf{ 0.f,0.f,m_ScreenWidth,m_ScreenHeight });
		switch (m_CharacterName)
		{
		case PlayerCharacter::Sonic:
			m_SonicMenuSpritePtr->Draw();
			break;
		case PlayerCharacter::Shadow:
			m_ShadowMenuSpritePtr->Draw();
			break;
		default:
			break;
		}
	}

	utils::SetColor(m_FadeInColor);
	utils::FillRect(0.f, 0.f, m_ScreenWidth, m_ScreenHeight);
}

void StartMenu::UpdateMenu(float elapsedSec)
{
	if (m_FadeInTime < m_MaxFadeInTime)
	{
		m_FadeInTime += elapsedSec;
		m_FadeInColor.a = (m_FadeInTime / m_MaxFadeInTime);
	}
	else if (m_FadeInTime < (m_MaxFadeInTime * 2.f))
	{
		if (!m_PlayedMenuMusic)
		{
			SoundManager::GetSoundManager()->PlaySound(SoundManager::menu, 0);
			m_PlayedMenuMusic = true;
		}
		m_MenuDrawn = true;
		m_FadeInTime += elapsedSec;
		m_FadeInColor.a = (1 - ((m_FadeInTime - m_MaxFadeInTime) / m_MaxFadeInTime));
	}
	else if (m_MenuAnimationTime < (m_MenuFrameTime * (m_NumMenuFrames - 1)))
	{
		m_MenuAnimationTime += elapsedSec;
		switch (m_CharacterName)
		{
		case PlayerCharacter::Sonic:
			m_SonicMenuSpritePtr->Update(elapsedSec);
			break;
		case PlayerCharacter::Shadow:
			m_ShadowMenuSpritePtr->Update(elapsedSec);
			break;
		}
	}
}

void StartMenu::ResetMenuAnimation()
{
	float totalTimeAdjustment{ (m_MenuFrameTime * m_NumMenuFrames) - m_MenuAnimationTime };
	float AdjustedTime{};
	while (AdjustedTime < totalTimeAdjustment)
	{
		switch (m_CharacterName)
		{
		case PlayerCharacter::Sonic:
			m_SonicMenuSpritePtr->Update(m_MenuFrameTime);
			break;
		case PlayerCharacter::Shadow:
			m_ShadowMenuSpritePtr->Update(m_MenuFrameTime);
			break;
		default:
			break;
		}
		AdjustedTime += m_MenuFrameTime;
	}
}
