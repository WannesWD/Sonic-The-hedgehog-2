#include "pch.h"
#include "Game.h"
#include "GameObject.h"
#include "PlayerInput.h"
#include "PlayerCharacter.h"
#include "InterActableManager.h"
#include "ProjectileManager.h"
#include "SoundManager.h"
#include "Level.h"
#include "Camera.h"
#include "HUD.h"
#include "StartMenu.h"
#include <string>
#include <vector>
#include <iostream>



Game::Game(const Window& window)
	:BaseGame{ window },
	m_KonamiCode{ std::vector<int>{int(SDLK_UP),int(SDLK_UP),int(SDLK_DOWN),int(SDLK_DOWN),int(SDLK_LEFT),int(SDLK_RIGHT),int(SDLK_LEFT),int(SDLK_RIGHT),int(SDLK_b),int(SDLK_a)} },
	m_Level{ new Level{} },
	m_GameObjects{ new PlayerCharacter{},
					new InteractableManager{std::vector<std::string>({"FullMap/InteractableLocations/RingLocations.svg",
												"FullMap/InteractableLocations/AiaiaiLocations.svg",
												"FullMap/InteractableLocations/ChopperLocations.svg",
												"FullMap/InteractableLocations/CheckPointLocations.svg",
												"FullMap/InteractableLocations/BuzzerLocations.svg",
												"FullMap/InteractableLocations/EndSignLocation.svg",
												"FullMap/InteractableLocations/SpringLocations.svg"}),
											std::vector<int>{0,90,45,90,0,135,0,135,270,90,45,45,135,0}},
					ProjectileManager::GetInstance() },
	m_Camera{ new Camera{GetViewPort().width, GetViewPort().height} },
	m_HUD{ new HUD{GetViewPort().width, GetViewPort().height} },
	m_StartMenu{ new StartMenu{GetViewPort().width,GetViewPort().height} },
	m_SoundManager{ SoundManager::GetSoundManager() },
	m_GameStarted{ false },
	m_Reset{ false },
	m_PlayMusic{ false },
	m_CorrectCode{ false },
	m_TimeStartupSound{ 0.f }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup();
}

void Game::Initialize( )
{
	m_Camera->SetLevelBoundries(m_Level->GetBoundaries());
}

void Game::Cleanup( )
{
	for (GameObject*& gameObject : m_GameObjects)
	{
		delete gameObject;
		gameObject = nullptr;
	}
	delete m_Level;
	m_Level = nullptr;
	delete m_Camera;
	m_Camera = nullptr;
	delete m_HUD;
	m_HUD = nullptr;
	delete m_StartMenu;
	m_StartMenu = nullptr;
	delete m_SoundManager;
	m_SoundManager = nullptr;
}

void Game::Update( float elapsedSec )
{
	if (m_GameStarted)
	{
		UpdateGame(elapsedSec);

		if (m_HUD->FinalHudDrawn() || m_GameOver)
		{
			if (m_GameOver) m_SoundManager->StopMusic();

			m_GameStarted = false;
			m_Reset = true;
			m_Restarted = false;
			m_TimeStartupSound = 0.f;
			m_StartMenu->Reset();
			m_StartMenu->Update(elapsedSec, m_Reset);
		}
	}
	else
	{
		if (m_StartMenu->MenuDrawn())
		{
			if (!m_GameStarted)
			{
				CheckCode();
				if (m_CorrectCode)
				{
					Restart();
					for (GameObject* gameObject : m_GameObjects)
					{
						if (PlayerCharacter * playerPtr{ dynamic_cast<PlayerCharacter*>(gameObject) })
						{
							m_StartMenu->SetMenuCharacter(playerPtr->GetCharacterName());
							break;
						}
					}
				}
			}
			if (!m_Restarted)
			{
				Restart();
				m_Restarted = true;
			}
		}

		m_StartMenu->Update(elapsedSec, m_Reset);
	}

	PlayStartupAndMusic(elapsedSec);

	
}

void Game::Draw( ) const
{
	ClearBackground();
	
	if (m_GameStarted)
	{
		DrawGame();
	}
	else
	{
		if (m_Reset)
		{
			DrawGame();
		}

		m_StartMenu->Draw();	
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
	if (!m_GameStarted)
	{
		switch (e.keysym.sym)
		{
		case SDLK_SPACE:
			m_SoundManager->StopAllSound();
			m_GameStarted = true;
			m_PlayMusic = true;
			break;
		default:
			break;
		}
	}

	switch (e.keysym.sym)
	{
	case SDLK_i:
		std::cout << "Move Left: A\nMove Right: D\nJump: W\nRespawn: R\nSpace(only in menu): Start/Restart Game";
		break;
	default:
		break;
	}
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	if (!m_StartMenu->MenuDrawn() && !m_GameStarted)
	{
		switch (e.keysym.sym)
		{
		case SDLK_UP:
			m_EnteredCode.push_back(int(SDLK_UP));
			break;
		case SDLK_DOWN:
			m_EnteredCode.push_back(int(SDLK_DOWN));
			break;
		case SDLK_LEFT:
			m_EnteredCode.push_back(int(SDLK_LEFT));
			break;
		case SDLK_RIGHT:
			m_EnteredCode.push_back(int(SDLK_RIGHT));
			break;
		case SDLK_b:
			m_EnteredCode.push_back(int(SDLK_b));
			break;
		case SDLK_a:
			m_EnteredCode.push_back(int(SDLK_a));
			break;
		default:
			break;
		}
	}
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.f, 0.5f, 1.f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::PlayStartupAndMusic(float elapsedSec)
{

	bool playDefaultSounds{};
	const float totalStartupSoundTime{ 4.1f };

	PlayerCharacter* playerCharacterPtr{ GetPlayerCharacterPtr() };
	if (playerCharacterPtr)
	{
		switch (playerCharacterPtr->GetCharacterName())
		{
		case PlayerCharacter::Sonic:
			playDefaultSounds = true;
			break;
		case PlayerCharacter::Shadow:
			playDefaultSounds = false;
		default:
			break;
		}
	}

	if (m_GameStarted && m_PlayMusic)
	{
		if (playDefaultSounds)
		{
			if (m_TimeStartupSound <= 0.f)
			{
				m_SoundManager->PlaySound(SoundManager::introSound, 0);
			}
			if (m_TimeStartupSound < totalStartupSoundTime)
			{
				m_TimeStartupSound += elapsedSec;
			}
			if (m_TimeStartupSound >= totalStartupSoundTime)
			{
				m_SoundManager->PlayMusic(PlayerCharacter::Sonic);
				m_PlayMusic = false;
			}
		}
		else
		{
			m_SoundManager->PlayMusic(playerCharacterPtr->GetCharacterName());
			m_PlayMusic = false;
		}
	}
}

void Game::CheckCode()
{
	if (m_EnteredCode.size() != m_KonamiCode.size())
	{
		m_CorrectCode = false;
		return;
	}
	else
	{
		for (int digitIdx = 0; digitIdx < m_KonamiCode.size(); digitIdx++)
		{
			if (m_KonamiCode[digitIdx] != m_EnteredCode[digitIdx])
			{
				m_CorrectCode = false;
				return;
			}
		}
	}
	m_CorrectCode = true;
}

PlayerCharacter* Game::GetPlayerCharacterPtr()
{
	for (GameObject* gameObject : m_GameObjects)
	{
		if (PlayerCharacter* playerCharacterPtr{dynamic_cast<PlayerCharacter*>(gameObject)})
		{
			return playerCharacterPtr;
		}
	}
	return nullptr;
}

void Game::ManageObjectCollision()
{
	Rectf playerHitbox;
	PlayerInput* playerPtr{ nullptr };
	for (GameObject* gameObject : m_GameObjects)
	{
		if (PlayerInput* player{ dynamic_cast<PlayerInput*>(gameObject) })
		{
			playerPtr = player;
			playerHitbox = player->GetHitbox();
		}

		if (InteractableManager* interactableManager{ dynamic_cast<InteractableManager*>(gameObject) })
		{
			interactableManager->InteractablePlayerInteraction(playerPtr);
		}

		if (ProjectileManager* projectileManager{ dynamic_cast<ProjectileManager*>(gameObject) })
		{
			if (projectileManager->ProjectileHit(playerHitbox))
			{
				playerPtr->Hit();
			}
		}
	}


}

void Game::UpdateGame(float elapsedSec)
{
	if (elapsedSec < 0.05f)
	{
		for (GameObject* gameObject : m_GameObjects)
		{
			gameObject->Update(elapsedSec, m_Level);
			PlayerInput* playerPtr{ dynamic_cast<PlayerInput*>(gameObject) };
			if (playerPtr)
			{
				m_GameOver = playerPtr->GameOver();
				if (!m_Level->LevelFinished() && !m_GameOver)
				{
					if (playerPtr->IsAlive()) m_Camera->Update(elapsedSec, playerPtr->GetCenter());

					m_Level->Update(elapsedSec, playerPtr->GetOnGround(), playerPtr);
				}
				playerPtr->StatsUpdated() ? m_HUD->Update(elapsedSec, playerPtr->HasFinished(), playerPtr->GetPlayerStats(), dynamic_cast<PlayerCharacter*>(playerPtr)->GetCharacterName())
											: m_HUD->Update(elapsedSec, playerPtr->HasFinished(), dynamic_cast<PlayerCharacter*>(playerPtr)->GetCharacterName());
			}
		}
		ManageObjectCollision();
	}
}

void Game::DrawGame() const
{
	glPushMatrix();
	m_Camera->Transform();
	m_Level->DrawParallax(m_Camera->getCameraBottomLeft(), GetViewPort().width, GetViewPort().height, m_Camera->getZoomScale());
	m_Level->Draw(true);

	for (const GameObject* gameObject : m_GameObjects)
	{
		if (const InteractableManager * interactableManager{ dynamic_cast<const InteractableManager*>(gameObject) })
		{
			interactableManager->Draw();
		}
	}

	for (const GameObject* gameObject : m_GameObjects)
	{
		if (const PlayerCharacter * sonic{ dynamic_cast<const PlayerCharacter*>(gameObject) })
		{
			sonic->Draw();
		}
	}

	for (const GameObject* gameObject : m_GameObjects)
	{
		if (const ProjectileManager * projcetileManager{ dynamic_cast<const ProjectileManager*>(gameObject) })
		{
			projcetileManager->Draw();
		}
	}

	m_Level->Draw(false);
	m_Camera->Draw();
	glPopMatrix();
	m_HUD->Draw();
} 

void Game::Restart()
{
	m_Level->reset();

	delete m_HUD;
	m_HUD = new HUD{ GetViewPort().width, GetViewPort().height };

	int nullPtrIdx{};

	for (GameObject*& gameObject : m_GameObjects)
	{
		if (!dynamic_cast<ProjectileManager*>(gameObject))
		{
			delete gameObject;
			gameObject = nullptr;
			
			switch (nullPtrIdx++)
			{
			case 0:
				gameObject = new PlayerCharacter{ (m_CorrectCode) ? PlayerCharacter::Shadow : PlayerCharacter::Sonic };
				break;
			case 1:
				gameObject = new InteractableManager{ std::vector<std::string>({
												"FullMap/InteractableLocations/RingLocations.svg",
												"FullMap/InteractableLocations/AiaiaiLocations.svg",
												"FullMap/InteractableLocations/ChopperLocations.svg",
												"FullMap/InteractableLocations/CheckPointLocations.svg",
												"FullMap/InteractableLocations/BuzzerLocations.svg",
												"FullMap/InteractableLocations/EndSignLocation.svg",
												"FullMap/InteractableLocations/SpringLocations.svg"}),
												std::vector<int>{0,90,45,90,0,135,0,135,270,90,45,45,135,0} };

				break;
			default:
				break;
			}
		}
	}

}