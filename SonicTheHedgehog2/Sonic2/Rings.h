#pragma once
#include "Interactable.h"

class Rings final : public Interactable
{
public:
	Rings(const Point2f& centerPos);
	~Rings();

	Rings(const Rings& other) = delete;
	Rings(Rings&& other) noexcept = delete;
	Rings& operator=(const Rings& other) = delete;
	Rings& operator=(Rings&& other) noexcept = delete;

private:
	static Texture* m_SpriteSheetTexture;
	static Texture* m_PickupParticleSpriteSheetTexture;
	static int m_InstanceCounter;
};


