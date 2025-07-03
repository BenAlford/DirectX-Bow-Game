#pragma once
#include "camera.h"
class LevelViewCam :
    public Camera
{
public:
    LevelViewCam(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up = Vector3::Up, Vector3 _target = Vector3::Zero, float _rotSpeed = 0, Vector3 _dpos = Vector3(0,0,-20));
    void LateTick(GameData* _GD);
private:
    float m_rotSpeed;
    Vector3 m_dpos;
};

