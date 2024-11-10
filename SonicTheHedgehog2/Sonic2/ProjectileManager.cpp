#include "pch.h"
#include "ProjectileManager.h"
#include "Projectile.h"
#include "Stinger.h"
#include "Coconut.h"
#include "Level.h"
#include "utils.h"
#include <iostream>

ProjectileManager::ProjectileManager() : 
	m_ProjectilePtrVector{}
{
}

ProjectileManager* ProjectileManager::GetInstance()
{
	static ProjectileManager* INSTANCE;

	if (INSTANCE == nullptr)
	{
		INSTANCE = new ProjectileManager{};
	}

	return INSTANCE;
}

ProjectileManager::~ProjectileManager()
{
	for (Projectile*& projectile : m_ProjectilePtrVector)
	{
		delete projectile;
		projectile = nullptr;
	}
}

void ProjectileManager::AddProjectile(const ProjectileType& projectileType, const Point2f& targetCenter, const Point2f& spawnPos)
{	
	switch (projectileType)
	{
	case ProjectileManager::stinger:
		AddProjectileOfType<Stinger>(targetCenter, spawnPos);
		break;
	case ProjectileManager::coconut:
		AddProjectileOfType<Coconut>(targetCenter, spawnPos);
		break;
	default:
		break;
	}
}

void ProjectileManager::Update(float elapsedSec, Level* level)
{
	for (Projectile* projectile : m_ProjectilePtrVector)
	{
		if (projectile->IsActive() && 
				utils::IsPointInRect(projectile->GetCenterPos(), level->GetBoundaries()))
		{
			projectile->Update(elapsedSec);
		}
		else
		{
			projectile->SetInActive();
		}
	}
}

void ProjectileManager::Draw() const
{
	for (Projectile* projectile : m_ProjectilePtrVector)
	{
		if (projectile->IsActive())
		{
			projectile->Draw();
		}
	}
}

bool ProjectileManager::ProjectileHit(const Rectf& playerHitBox)
{
	if (m_ProjectilePtrVector.size() < 1)
	{
		return false;
	}
	for (Projectile* projectile : m_ProjectilePtrVector)
	{
		if (utils::IsPointInRect(projectile->GetCenterPos(), playerHitBox))
		{
			if (projectile->IsActive())
			{
				projectile->SetInActive();
				return true;
			}
		}
	}

	return false;
}

