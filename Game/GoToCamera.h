#pragma once
#include "camera.h"
class GoToCamera :
    public Camera
{
public:
    GoToCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, std::shared_ptr<GameObject> _target, Vector3 _up, Vector3 _startPos);
    void LateTick(GameData* _GD) override;

private:
    std::shared_ptr<GameObject> m_targetObject;
    Vector3 m_startPos;
};

