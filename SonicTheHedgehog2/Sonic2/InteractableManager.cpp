#include "pch.h"
#include "Sprite.h"
#include "InteractableManager.h"
#include "SoundManager.h"

#include "Rings.h"
#include "Aiaiai.h"
#include "Chopper.h"
#include "Buzzer.h"
#include "CheckPoint.h"
#include "EndSign.h"
#include "Spring.h"

#include "PlayerInput.h"
#include "utils.h"
#include "StaticAnimatedSprite.h"
#include "SVGParser.h"

#include <string>
#include <vector>

InteractableManager::InteractableManager(const std::vector<std::string>& SVGPathVector, const std::vector<int>& springAngle)
{
	int springIdx{};

	typedef std::vector<std::vector<Point2f>> SVG;
	std::vector<SVG> interactablePositionsVector;

	for (int positionArrayIndex = 0; positionArrayIndex < SVGPathVector.size(); positionArrayIndex++)
	{
		SVG interactablePositions;
		SVGParser::GetVerticesFromSvgFile(SVGPathVector[positionArrayIndex], interactablePositions);
		interactablePositionsVector.push_back(interactablePositions);

		for (int svgIdx = 0; svgIdx < interactablePositionsVector[positionArrayIndex].size(); svgIdx++)
		{
			for (int interactableIdx = 0; interactableIdx < interactablePositionsVector[positionArrayIndex][svgIdx].size(); interactableIdx++)
			{
				Point2f SpawnPos{ interactablePositionsVector[positionArrayIndex][svgIdx][interactableIdx] };
				
				switch (InteractableTypes(positionArrayIndex))
				{
				case InteractableTypes::ring:
					m_pLevelInteractables.push_back(new Rings{ SpawnPos });
					break;
				case InteractableTypes::aiaiai:
					m_pLevelInteractables.push_back(new Aiaiai{ SpawnPos });
					break;
				case InteractableTypes::chopper:
					m_pLevelInteractables.push_back(new Chopper{ SpawnPos });
					break;
				case InteractableTypes::checkPoint:
					m_pLevelInteractables.push_back(new CheckPoint{ SpawnPos });
					break;
				case InteractableTypes::buzzer:
					m_pLevelInteractables.push_back(new Buzzer{ SpawnPos });
					break;
				case InteractableTypes::endSign:
					m_pLevelInteractables.push_back(new EndSign{ SpawnPos });
					break;
				case InteractableTypes::spring:
					m_pLevelInteractables.push_back(new Spring{ SpawnPos, springAngle[springIdx++] });
				default:
					break;
				}
			}
		}
	}
}

InteractableManager::~InteractableManager()
{
	for (Interactable*& interactable : m_pLevelInteractables)
	{
		delete interactable;
		interactable = nullptr;
	}
}

void InteractableManager::Update(float elapsedSec, Level* level)
{
	for (Interactable*& interactable : m_pLevelInteractables)
	{
		if (interactable != nullptr)
		{
			interactable->Update(elapsedSec);
			if (interactable->IsDeleted())
			{
				delete interactable;
				interactable = nullptr;
			}
		}
	}
}

void InteractableManager::Draw() const
{
	for (const Interactable* interactable : m_pLevelInteractables)
	{
		if (interactable != nullptr)
		{
			interactable->Draw();
		}
	}
}

void InteractableManager::InteractablePlayerInteraction(PlayerInput* playerPtr)
{
	for (Interactable* interactable : m_pLevelInteractables)
	{
		EndSign* endSign{ dynamic_cast<EndSign*>(interactable) };
		Enemy* enemy{ dynamic_cast<Enemy*>(interactable) };
		if (interactable && enemy)
		{
			enemy->Attack(playerPtr->GetHitBox());
		}

		if (interactable &&
			utils::IsOverlapping(interactable->GetHitbox(), playerPtr->GetHitBox()))
		{
			if (dynamic_cast<Enemy*>(interactable))
			{
				if (playerPtr->IsJumping())
				{
					playerPtr->KilledEnemy();
					interactable->Delete();
				}
				else
				{
					playerPtr->Hit();
				}
			}

			else if (dynamic_cast<Rings*>(interactable))
			{
				playerPtr->RingCollected();
				interactable->Delete();	
			}

			else if(dynamic_cast<CheckPoint*>(interactable))
			{
				if (!interactable->IsDeleted())
				{
					SoundManager::GetSoundManager()->PlaySound(SoundManager::checkpoint, 0);
				}
				playerPtr->SetSpawnPoint(interactable->GetCenter());
				interactable->Delete();
			}

			else if (endSign)
			{
				interactable->Delete();
			}

			else if (Spring* spring{ dynamic_cast<Spring*>(interactable) })
			{
				spring->SpringHit();
				playerPtr->HitSpring(spring->GetSpringAngle());

			}

		}
		
		if (endSign && endSign->HasFinished())
		{
			playerPtr->SetFinished(true);
			SoundManager::GetSoundManager()->StopMusic();
			SoundManager::GetSoundManager()->PlaySound(SoundManager::levelComplete, 0);
		}
	}
}


