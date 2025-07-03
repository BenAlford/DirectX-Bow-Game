#include "pch.h"
#include "BowUI.h"
#include "GameData.h"
#include <iostream>

BowUI::BowUI(string _fileName, ID3D11Device* _GD, float time_to_charge) : ImageGO2D(_fileName, _GD)
{
	m_chargeTime = time_to_charge;
	m_scale = { 2.1,2.1 };
}

void BowUI::Tick(GameData* _GD)
{
	// if it is visible, slowly shrink (shows the shot charging)
	if (m_show)
	{
		m_scale.x -= _GD->m_dt * (2 / m_chargeTime);
		m_scale.y -= _GD->m_dt * (2 / m_chargeTime);

		// if the scale is 0, hide the object
		if (m_scale.x < 0)
		{
			Deactivate();
		}
	}
}

void BowUI::Draw(DrawData2D* _DD)
{
	// doesn't draw this if the bow isn't charging
	if (m_show)
	{
		ImageGO2D::Draw(_DD);
	}
}

void BowUI::Activate()
{
	m_show = true;
}

void BowUI::Deactivate()
{
	// hides and resets the image
	m_show = false;
	m_scale = { 2.1,2.1 };
}
