#include "pch.h"
#include "Spinner.h"
#include "GameData.h"

Spinner::Spinner(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, float _spin_speed) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	m_spinSpeed = _spin_speed * (XM_PI / 180);

	GameObject::Tick(nullptr); //update my world_transform

	m_useRollPitchYaw = false;
}

void Spinner::Tick(GameData* _GD)
{
	// spins the object about the y axis
	if (_GD->m_GS != GS_WAITING_TO_CONTINUE)
		m_rotMat *= Matrix::CreateRotationY(m_spinSpeed * _GD->m_dt);
	CMOGO::Tick(_GD);
}
