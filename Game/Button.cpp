#include "pch.h"
#include "Button.h"
#include "GameData.h"
#include <iostream>

Button::Button(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, std::shared_ptr<GameObject> _target, Vector3 _moveAmount, float _timeToMove) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	m_trigger = true;
	m_target = _target;
	m_timeToMove = _timeToMove;
	m_moveAmount = _moveAmount;

	updateWorldMat(); //update my world_transform
}

void Button::Tick(GameData* _GD)
{
	// if the button is hit and has not finished activating yet
	if (m_activate && !m_depleted)
	{
		// move the target to the desired position
		if (m_timer + _GD->m_dt < m_timeToMove)
		{
			m_timer += _GD->m_dt;
			m_target->SetPos(m_target->GetPos() + ((m_moveAmount * _GD->m_dt) / m_timeToMove));
		}
		else
		{
			// set the target to the desired position and deactivate the button
			m_target->SetPos(m_target->GetPos() + ((m_moveAmount * (m_timeToMove - m_timer)) / m_timeToMove));
			m_depleted = true;
		}
	}
	CMOGO::Tick(_GD);
}

void Button::TriggerEntered(std::shared_ptr<CMOGO> other)
{
	// if a physics cube touches this, activate the button and flip the button (looks like the textures changing)
	if (other->GetTag() == "cube")
	{
		m_activate = true;
		m_roll = XM_PI;
	}
}
