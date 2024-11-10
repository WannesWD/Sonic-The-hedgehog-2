#pragma once
#include "GameObject.h"
#include "SoundStream.h"
#include "SoundEffect.h"
#include "PlayerCharacter.h"
#include <vector>

class SoundManager final
{
public:
	static SoundManager* GetSoundManager();
	~SoundManager();

	SoundManager(const SoundManager& other) = delete;
	SoundManager(SoundManager&& other) noexcept = delete;
	SoundManager& operator=(const SoundManager& other) = delete;
	SoundManager& operator=(SoundManager&& other) noexcept = delete;

	enum EffectName
	{
		ringPickup,
		ringLoss,
		springHit,
		jump,
		skidding,
		enemykilled,
		segaLogo,
		checkpoint,
		introSound,
		levelComplete,
		menu

	};

	void PlaySound(EffectName effectName, int numLoops);
	void StopAllSound();
	void StopMusic();
	void PlayMusic(PlayerCharacter::CharacterName characterName);
private:
	SoundManager();
	std::vector<SoundEffect*> m_SoundEffectVector;
	SoundStream* m_SonicBackgroundMusic;
	SoundStream* m_ShadowBackgroundMusic;

	
};

