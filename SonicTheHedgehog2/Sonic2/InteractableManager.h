#pragma once
#include "GameObject.h"

#include <vector>
#include <string>

class Interactable;
class Level;
class PlayerInput;

class InteractableManager final : public GameObject
{
public:
	InteractableManager(const std::vector<std::string>& SVGPathVector, const std::vector<int>& springAngle);
	~InteractableManager();

	InteractableManager(const InteractableManager& other) = delete;
	InteractableManager(InteractableManager&& other) noexcept = delete;
	InteractableManager& operator=(const InteractableManager& other) = delete;
	InteractableManager& operator=(InteractableManager&& other) noexcept = delete;

	void Update(float elapsedSec, Level* level) override;
	void Draw() const override;
	void InteractablePlayerInteraction(PlayerInput* playerPtr);

	enum class InteractableTypes
	{
		ring,
		aiaiai,
		chopper,
		checkPoint,
		buzzer,
		endSign,
		spring
	};

private:
	std::vector<Interactable*> m_pLevelInteractables;
};

