#pragma once
#include "CMOGO.h"
class Projectile : public CMOGO
{
public:
	Projectile(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 pos_start, Vector3 direction_start, float speed);
	void Tick(GameData* _GD) override;
	void TriggerEntered(std::shared_ptr<CMOGO> other) override;

	void updateRotation();
	void setLastShot(bool _last_shot) { m_lastShot = _last_shot; }

protected:
	bool m_deleteTrigger = false;
	bool m_lastShot = false;
};

