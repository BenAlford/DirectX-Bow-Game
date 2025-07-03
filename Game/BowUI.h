#pragma once
#include "ImageGO2D.h"
class BowUI : public ImageGO2D
{
public:
	BowUI(string _fileName, ID3D11Device* _GD, float time_to_charge);

	void Tick(GameData* _GD) override;
	void Draw(DrawData2D* _DD) override;
	void Activate();
	void Deactivate();

private:
	bool m_show = false;
	float m_chargeTime;
};

