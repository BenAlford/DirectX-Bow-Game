#pragma once
#include "CMOGO.h"
class Spinner : public CMOGO
{
public:
	Spinner(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, float _spin_speed);
	void Tick(GameData* _GD) override;

private:
	float m_spinSpeed = 0;
};

