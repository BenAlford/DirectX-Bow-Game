#include "pch.h"
#include "BlackHole.h"
#include "GameData.h"
#include <iostream>

BlackHole::BlackHole(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _influence_size, float _strength) : CMOGOSphere(_fileName, _pd3dDevice, _EF)
{
	// sets up basics
	m_pos = _pos;
	m_yaw = XM_PI;
	m_scale = Vector3(2.5,2.5,2.5);
	m_sphereCollider.Radius *= _influence_size / 2.5;
	m_strength = _strength;
	m_influenceSize = _influence_size;

	updateWorldMat(); //update my world_transform
}

void BlackHole::Tick(GameData* _GD)
{
	m_dt = _GD->m_dt;
}

void BlackHole::TriggerEntered(std::shared_ptr<CMOGO> other)
{
	// accelerates the object towards the centre, faster the closer it is

	// gets the distance and direction of object from the black holes centre
	Vector3 dir = m_pos - other->GetPos();
	float distance = dir.Length();
	dir.Normalize();

	// calculates the percentage of the blackholes distance the object has covered
	float percent = ((m_influenceSize - distance) / m_influenceSize);
	if (percent < 0)
	{
		percent = 0.001f;
	}

	// adds acceleration based on strength and distance
	other->addAcceleration(dir * m_strength * (1-m_dt) * percent);
}
