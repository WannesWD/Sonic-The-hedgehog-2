#pragma once
#include "PlayerCharacter.h"
class Texture;
class Sprite;
class StartMenu final
{
public:
	StartMenu(float screenWith, float screenHeight, PlayerCharacter::CharacterName characterName = PlayerCharacter::Sonic);
	~StartMenu();

	StartMenu(const StartMenu& other) = delete;
	StartMenu(StartMenu&& other) noexcept = delete;
	StartMenu& operator=(const StartMenu& other) = delete;
	StartMenu& operator=(StartMenu&& other) noexcept = delete;

	void Update(float elapsedSec, bool afterLevel);
	void Draw() const;

	void Reset();
	bool MenuDrawn();
	void SetMenuCharacter(PlayerCharacter::CharacterName characterName);
private:
	const int m_NumLogoFrames;
	const int m_NumMenuFrames;
	
	const float m_ScreenWidth;
	const float m_ScreenHeight;
	const float m_MenuScale;
	const float m_LogoScale;
	const float m_LogoFrameTime;
	const float m_MenuFrameTime;
	const float m_MaxFadeInTime;
	
	float m_LogoAnimationTime;
	float m_FadeInTime;
	float m_MenuAnimationTime;

	bool m_AfterLevel;
	bool m_MenuDrawn;
	bool m_PlayedLogoSound;
	bool m_PlayedMenuMusic;

	PlayerCharacter::CharacterName m_CharacterName;

	const Color4f m_MenuColor;
	Color4f m_FadeInColor;

	Texture* m_MenuBackground;
	Texture* m_SonicMenuSpriteTexturePtr;
	Texture* m_ShadowMenuSpriteTexturePtr;
	Texture* m_SegaLogoSpriteSheetPtr;
	Sprite* m_SegaLogoSpritePtr;
	Sprite* m_SonicMenuSpritePtr;
	Sprite* m_ShadowMenuSpritePtr;

	void DrawGameStartUp() const;
	void DrawMenu() const;
	void UpdateGameStartUp(float elapsedSec);
	void UpdateMenu(float elapsedSec);
	void ResetMenuAnimation();
};

