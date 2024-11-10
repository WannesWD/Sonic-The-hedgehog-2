#pragma once
#include "BaseGame.h"
#include <vector>

class GameObject;
class Level;
class Camera;
class HUD;
class StartMenu;
class SoundManager;
class PlayerCharacter;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw() const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:
	std::vector<GameObject*> m_GameObjects;
	Level* m_Level;
	Camera* m_Camera;
	HUD* m_HUD;
	StartMenu* m_StartMenu;
	SoundManager* m_SoundManager;

	bool m_GameStarted;
	bool m_Reset;
	bool m_Restarted;
	bool m_PlayMusic;
	bool m_GameOver;
	bool m_CorrectCode;

	float m_TimeStartupSound;
	std::vector<int>m_KonamiCode;
	std::vector<int>m_EnteredCode;
	// FUNCTIONS
	void Initialize();
	void Restart();
	void Cleanup( );
	
	void ClearBackground( ) const;
	void PlayStartupAndMusic(float elapsedSec);
	void CheckCode();
	PlayerCharacter* GetPlayerCharacterPtr();

	void ManageObjectCollision();
	void UpdateGame(float elapsedSec);
	void DrawGame() const;
};