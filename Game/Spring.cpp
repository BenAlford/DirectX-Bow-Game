#include "pch.h"
#include "Spring.h"

Spring::Spring(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, float _height) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;
	m_height = _height;
	m_trigger = true;

	updateWorldMat(); //update my world_transform
}

void Spring::TriggerEntered(std::shared_ptr<CMOGO> other)
{
	// bounces the object that enters
	other->SetVelocity(Vector3(other->GetVelocity().x, m_height, other->GetVelocity().z));
}
