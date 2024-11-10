#pragma once
#include "GameObject.h"
#include "Projectile.h"
#include <vector>
class Projectile;
class Level;

class ProjectileManager final : public GameObject
{
public:
	static ProjectileManager* GetInstance();
	~ProjectileManager();

	ProjectileManager(const ProjectileManager& other) = delete;
	ProjectileManager(ProjectileManager&& other) noexcept = delete;
	ProjectileManager& operator=(const ProjectileManager& other) = delete;
	ProjectileManager& operator=(ProjectileManager&& other) noexcept = delete;

	enum ProjectileType
	{
		stinger,
		coconut
	};

	void AddProjectile(const ProjectileType& projectileType, const Point2f& targetCenter, const Point2f& spawnPos);

	void Update(float elapsedSec, Level* level) override;
	void Draw() const override;
	
	bool ProjectileHit(const Rectf& playerHitBox);
private:
	ProjectileManager();
	std::vector<Projectile*> m_ProjectilePtrVector;

	template<typename TypeOfProjectile> 
	void AddProjectileOfType(const Point2f& targetCenter, const Point2f& spawnPos);
};	

template<typename TypeOfProjectile> //THIS WORKS LMAOOO
void ProjectileManager::AddProjectileOfType(const Point2f& targetCenter, const Point2f& spawnPos)
{
	bool projectileAdded{ false };
	for (Projectile* projectile : m_ProjectilePtrVector)
	{
		TypeOfProjectile* projectileOfType{ dynamic_cast<TypeOfProjectile*>(projectile) };
		if (projectileOfType && !projectileOfType->IsActive())
		{
			projectile->ResetProjectile(targetCenter, spawnPos);
			projectileAdded = true;
			break;
		}
	}

	if (!projectileAdded)
	{
		m_ProjectilePtrVector.push_back(new TypeOfProjectile{ targetCenter, spawnPos });
	}
}