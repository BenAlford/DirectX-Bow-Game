#include "pch.h"
#include "LevelViewCam.h"
#include "GameData.h"
#include <iostream>

LevelViewCam::LevelViewCam(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up, Vector3 _target, float _rotSpeed, Vector3 _dpos) : Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up, _target)
{
	m_rotSpeed = _rotSpeed;
	m_dpos = _dpos;
}

void LevelViewCam::LateTick(GameData* _GD)
{
	// rotates around the centre of the world
	m_yaw += m_rotSpeed * _GD->m_dt;
	Matrix rotCam = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.0f);
	m_pos = m_target + Vector3::Transform(m_dpos, rotCam);
	Camera::LateTick(_GD);
}
