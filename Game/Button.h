#pragma once
#include "CMOGO.h"
class Button : public CMOGO
{
public:
	Button(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 _pos, float _pitch, float _yaw, float _roll, Vector3 _scale, std::shared_ptr<GameObject> _target, Vector3 _moveAmount, float _timeToMove);
	void Tick(GameData* _GD) override;
	void TriggerEntered(std::shared_ptr<CMOGO> other) override;

private:
	bool m_activate = false;
	bool m_depleted = false;
	std::shared_ptr<GameObject> m_target;
	Vector3 m_moveAmount;
	float m_timeToMove;
	float m_timer = 0;
};

