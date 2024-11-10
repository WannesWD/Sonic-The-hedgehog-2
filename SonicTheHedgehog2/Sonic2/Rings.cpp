#include "pch.h"
#include "Rings.h"
#include "Texture.h"

Texture* Rings::m_SpriteSheetTexture{ nullptr };
Texture* Rings::m_PickupParticleSpriteSheetTexture{ nullptr };

int Rings::m_InstanceCounter{ 0 };

Rings::Rings(const Point2f& centerPos) : 
	Interactable(centerPos, 16.f, 16.f, 0.166f, 1, 4, 16.f, 16.f, 0.0166f, 1, 4)
{
	if (m_InstanceCounter == 0)
	{
		m_SpriteSheetTexture = new Texture{ "SpriteSheets/CommonObjects/Rings/RotatingRings.png" };
		m_PickupParticleSpriteSheetTexture = new Texture{ "SpriteSheets/CommonObjects/Rings/RingPickupParticle.png" };
	}

	++m_InstanceCounter;

	Interactable::setSpriteTextures(m_SpriteSheetTexture, m_PickupParticleSpriteSheetTexture);

}

Rings::~Rings()
{
	if (--m_InstanceCounter < 1)
	{
		delete m_SpriteSheetTexture;
		m_SpriteSheetTexture = nullptr;
		delete m_PickupParticleSpriteSheetTexture;
		m_PickupParticleSpriteSheetTexture = nullptr;
	}
}
