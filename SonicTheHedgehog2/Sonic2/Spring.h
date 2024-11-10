#pragma once
#include "Interactable.h"

class Spring final: public Interactable
{
public:
	Spring(const Point2f& centerPos, int angle);
	~Spring();

	Spring(const Spring& other) = delete;
	Spring(Spring&& other) noexcept = delete;
	Spring& operator=(const Spring& other) = delete;
	Spring& operator=(Spring&& other) noexcept = delete;
	
	void Draw() const override;
	void Update(float elapsedSec) override;
	Rectf GetHitbox() override;

	void SpringHit();
	int GetSpringAngle();
private:
	static Texture* ANGLEDSPRINGSPRITESHEET;
	static Texture* STRAIGHTSPRINGSPRITESHEET;
	static int INSTANCECOUNTER;
	 
	const float m_MaxSprungTime;
	const float m_FrameTime;
	float m_SprungTime;

	const int m_Angle;
	const int m_MaxTimesAnimated;
	int m_TimesAnimated;

	bool m_Angled;
};

