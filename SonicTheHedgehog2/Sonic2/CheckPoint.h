#pragma once
#include "Interactable.h"

class CheckPoint final : public Interactable
{
public:
	CheckPoint(const Point2f& centerPos);
	~CheckPoint();

	CheckPoint(const CheckPoint& other) = delete;
	CheckPoint(CheckPoint&& other) noexcept = delete;
	CheckPoint& operator=(const CheckPoint& other) = delete;
	CheckPoint& operator=(CheckPoint&& other) noexcept = delete;

	void Update(float elapsedSec) override;
	void Delete() override;

private:
	static Texture* SPRITESHEETTEXTUREPTR;
	static Texture* PARTICLETEXTUREPTR;
	static int INSTANCECOUNTER;
	static const float MAXANIMATIONTIME;

	bool m_Collected;
	float m_AnimationTime;
};


