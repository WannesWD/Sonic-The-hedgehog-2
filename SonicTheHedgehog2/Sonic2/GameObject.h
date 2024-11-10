#pragma once
class Level;

class GameObject
{
public:
	GameObject() = default;
	virtual ~GameObject() = default;

	GameObject(const GameObject& other) = delete;
	GameObject(GameObject&& other) noexcept = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject& operator=(GameObject&& other) noexcept = delete;

	virtual void Update(float elapsedSec, Level* level) = 0;
	virtual void Draw() const = 0;
private:
};

