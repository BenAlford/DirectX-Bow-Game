#include "pch.h"
#include "PhysicsCube.h"

PhysicsCube::PhysicsCube(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	SetPhysicsOn(true);
	SetDrag(0.5);
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	updateWorldMat();
}

void PhysicsCube::Hit(Vector3 normal)
{
	if (normal.y > 0.5)
	{
		m_grounded = true;
	}
}

void PhysicsCube::Tick(GameData* _GD)
{
	// simulates friction by having higher drag when grounded
	if (m_grounded)
		SetDrag(3);
	else
		SetDrag(0.5);

	m_grounded = false;
	CMOGO::Tick(_GD);
}
