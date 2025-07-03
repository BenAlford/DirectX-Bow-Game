#pragma once
#include "CMOGO.h"
class Spring : public CMOGO
{
public:
	Spring(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, float _height);
	void TriggerEntered(std::shared_ptr<CMOGO> other) override;
private:
	float m_height = 0;
};

