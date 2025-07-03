#include "pch.h"
#include "GoToCamera.h"
#include "GameData.h"
#include <iostream>

GoToCamera::GoToCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, std::shared_ptr<GameObject> _target, Vector3 _up, Vector3 _startPos) : Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_targetObject = _target;
	m_startPos = _startPos;
	m_pos = _startPos;

	updateWorldMat();
}

void GoToCamera::LateTick(GameData* _GD)
{
	// moves towards the target position until it reaches it
	Vector3 direction = m_targetObject->GetPos() - m_pos;
	if (direction.Length() > 10)
	{
		direction.Normalize();
		m_pos = m_pos + direction * _GD->m_dt * 50;
	}
	m_target = m_targetObject->GetPos();
	Camera::LateTick(_GD);
}
