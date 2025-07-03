#pragma once
#include "CMOGO.h"

class PhysicsCube : public CMOGO
{
public:
	PhysicsCube(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale);
	void Hit(Vector3 normal) override;
	void Tick(GameData* _GD) override;

private:
	bool m_grounded = false;
};

