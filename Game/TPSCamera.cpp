#include "pch.h"
#include "TPSCamera.h"
#include "GameData.h"
#include "Player.h"

TPSCamera::TPSCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, std::shared_ptr<GameObject> _target, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_targetObject = _target;
	m_dpos = _dpos;
}

TPSCamera::~TPSCamera()
{

}

void TPSCamera::LateTick(GameData* _GD)
{
	if (_GD->m_GS == GS_PLAY_TPS_CAM)
	{
		float rotSpeed = 0.25f * _GD->m_dt;
		if (_GD->m_MS.y != 0)
		{
			m_pitch -= rotSpeed * _GD->m_MS.y;
			float convertion = 3.1415 / 180;
			if (m_pitch < -60 * convertion)
			{
				m_pitch = -60 * convertion;
			}
			else if (m_pitch > 60 * convertion)
			{
				m_pitch = 60 * convertion;
			}
		}
	}
	//Set up position of camera and target position of camera based on new position and orientation of target object
	Matrix rotCam = Matrix::CreateFromYawPitchRoll(m_targetObject->GetYaw(), m_pitch, 0.0f);
	m_target = std::static_pointer_cast<Player>(m_targetObject)->getHeadPos();
	m_pos = m_target + Vector3::Transform(m_dpos, rotCam);

	//and then set up proj and view matrices
	Camera::LateTick(_GD);
}

