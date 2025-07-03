#include "pch.h"
#include "Target.h"
#include <iostream>
#include "GameData.h"

Target::Target(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos = _pos;
	m_pitch = _pitch;
	m_roll = _roll;
	m_yaw = _yaw;
	m_scale = _scale;

	GameObject::Tick(nullptr); //update my world_transform
}

void Target::TriggerEntered(std::shared_ptr<CMOGO> other)
{
	// only accepts an arrow hitting it
	if (other->GetTag() == "arrow")
	{
		// if the arrow hit the circle part of the target, set hit to true
		float distance = (other->GetPos() - GetPos()).Length();
		if (distance <= m_collider.Extents.x * 0.95 * GetScale().x)
		{
			m_hit = true;
		}
	}
}

void Target::Tick(GameData* _GD)
{
	// if the target was hit, tell game
	if (m_hit)
	{
		_GD->m_GS = GS_LEVEL_WON;
		m_hit = false;
	}
	CMOGO::Tick(_GD);
}
