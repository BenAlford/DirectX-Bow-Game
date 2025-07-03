#pragma once
#include "CMOGO.h"
class Target : public CMOGO
{
public:
	Target(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale);

	void TriggerEntered(std::shared_ptr<CMOGO> other) override;
	void Tick(GameData* _GD) override;

private:
	bool m_hit = false;
};

