#include "pch.h"
#include "SoundManager.h"

SoundManager::SoundManager() :
	m_SoundEffectVector{
		new SoundEffect{ "Sounds/RingPickup.wav" },
		new SoundEffect{ "Sounds/RingLoss.wav" },
		new SoundEffect{ "Sounds/SpringHit.wav" },
		new SoundEffect{ "Sounds/Jump.wav" },
		new SoundEffect{ "Sounds/Skidding.wav" },
		new SoundEffect{ "Sounds/KillSound.wav" },
		new SoundEffect{ "Sounds/SegaLogo.wav" },
		new SoundEffect{ "Sounds/CheckPoint.wav" },
		new SoundEffect{ "Sounds/IntroSound.wav" },
		new SoundEffect{ "Sounds/LevelComplete.wav" },
		new SoundEffect{ "Sounds/Menu.wav" } },
		m_SonicBackgroundMusic{ new SoundStream{ "Sounds/EmeraldHillZoneMusic.wav"} },
		m_ShadowBackgroundMusic{ new SoundStream{"Sounds/EscapeFromTheCity.wav"} }
{
	for (SoundEffect* soundEffect : m_SoundEffectVector)
	{
		soundEffect->SetVolume(5);
	}
	m_SonicBackgroundMusic->SetVolume(20);
}

SoundManager* SoundManager::GetSoundManager()
{
    static SoundManager* SOUNDINSTANCE;

	if (SOUNDINSTANCE == nullptr)
	{
		SOUNDINSTANCE = new SoundManager{};
	}

    return SOUNDINSTANCE;
}

SoundManager::~SoundManager()
{
	for (SoundEffect*& sound : m_SoundEffectVector)
	{
		delete sound;
		sound = nullptr;
	}

	delete m_SonicBackgroundMusic;
	m_SonicBackgroundMusic = nullptr;
	delete m_ShadowBackgroundMusic;
	m_ShadowBackgroundMusic = nullptr;
}

void SoundManager::PlaySound(EffectName effectName, int numLoops)
{
	m_SoundEffectVector[int(effectName)]->Play(numLoops);
}

void SoundManager::StopAllSound()
{
	SoundEffect* soundEffect{ m_SoundEffectVector[0] };
	soundEffect->StopAll();
}

void SoundManager::StopMusic()
{
	m_SonicBackgroundMusic->Stop();
}

void SoundManager::PlayMusic(PlayerCharacter::CharacterName characterName)
{
	switch (characterName)
	{
	case PlayerCharacter::Sonic:
		m_SonicBackgroundMusic->Play(true);
		break;
	case PlayerCharacter::Shadow:
		m_ShadowBackgroundMusic->Play(true);
		break;
	default:
		break;
	}
}
