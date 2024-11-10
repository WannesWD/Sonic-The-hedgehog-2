#pragma once
#include "Interactable.h"
class Texture;

class EndSign final: public Interactable
{
public:
	EndSign(const Point2f& centerPos);
	~EndSign();

	EndSign(const EndSign& other) = delete;
	EndSign(EndSign&& other) noexcept = delete;
	EndSign& operator=(const EndSign& other) = delete;
	EndSign& operator=(EndSign&& other) noexcept = delete;

	void Update(float elapsedSec) override;
	void Delete() override;
	bool HasFinished() const;
private:
	static Texture* STATIONARYSPRITEPTR;
	static Texture* ROTATINGSPRITEPTR;
	static int INSTANCECOUNTER;

	const float m_StationaryFrameTime;
	const float m_MaxRotatingTime;
	float m_AnimationTime;
	bool m_Collected;
	bool m_Finished;
};

