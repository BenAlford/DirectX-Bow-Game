#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "CMOGO.h"
#include "BowUI.h"

//=================================================================
//Base Player Class (i.e. a GO the player controls)
//=================================================================

class Player : public CMOGO
{

public:
	Player(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, int _shots, Vector3 _head_offset);
	~Player();

	virtual void Tick(GameData* _GD) override;
	virtual void Hit(Vector3 direction) override;
	virtual void LateTick(GameData* _GD) override;

	void setCamera(std::shared_ptr<GameObject> _camera);
	void setBowUI(std::shared_ptr<BowUI> _bowUI);
	void addArrowUI(std::shared_ptr<ImageGO2D> _arrow) { m_arrowUI.push_back(_arrow); }
	Vector3 getHeadPos();

	int getShots() { return m_shots; }
	void setShots(int _shots) { for (int i = 0; i < m_shots - _shots; i++) { m_arrowUI.pop_back(); } m_shots = _shots; }


protected:
	bool m_inAir;
	bool m_jumpStarted;
	float m_jumpCD;
	float m_jumpCDTimer;
	bool m_canJump;

	bool m_chargingShot;
	bool m_fullyCharged;
	bool m_canShoot;
	float m_shotChargeTime;
	float m_shotChargeTimer;
	float m_minShotSpeed;
	float m_maxShotSpeed;
	float m_shots;
	Vector3 m_head_offset;

	float m_maxHorizontalSpeed;
	float m_jumpHeight;


	std::weak_ptr<GameObject> m_camera;
	std::weak_ptr<BowUI> m_bowUI;
	std::vector<std::shared_ptr<ImageGO2D>> m_arrowUI;
};

#endif