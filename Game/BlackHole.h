#pragma once
#include "CMOGOSphere.h"

class BlackHole : public CMOGOSphere
{
public:
	BlackHole(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _influence_size, float _strength);

	void Tick(GameData* _GD) override;
	void TriggerEntered(std::shared_ptr<CMOGO> other) override;

protected:
	float m_strength;
	float m_dt = 0;
	float m_influenceSize;
};

