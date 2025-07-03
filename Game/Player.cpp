#include "pch.h"
#include "Player.h"
#include <dinput.h>
#include "GameData.h"
#include <iostream>
#include "Projectile.h"

Player::Player(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, int _shots, Vector3 _head_offset) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.y = 20.0f;

	SetDrag(3);
	SetPhysicsOn(true);
	m_shots = _shots;
	m_head_offset = _head_offset;
	m_draw = false;

	m_inAir = true;
	m_jumpStarted = false;
	m_jumpCD = 0.5;
	m_jumpCDTimer = 0;
	m_canJump = true;

	m_chargingShot = false;
	m_fullyCharged = false;
	m_canShoot = true;
	m_shotChargeTime = 1.5;
	m_shotChargeTimer = 0;
	m_minShotSpeed = 10;
	m_maxShotSpeed = 100;

	m_maxHorizontalSpeed = 12;
	m_jumpHeight = 20;
}

Player::~Player()
{
	//tidy up anything I've created
}

void Player::setCamera(std::shared_ptr<GameObject> _camera)
{
	m_camera = _camera;
}

void Player::setBowUI(std::shared_ptr<BowUI> _bowUI)
{
	m_bowUI = _bowUI;
}

Vector3 Player::getHeadPos()
{
	// returns the position the head is at
	return GetPos() + m_head_offset;
}


void Player::Tick(GameData* _GD)
{
	if (_GD->m_GS == GS_PLAY_TPS_CAM)
	{
		// limits the players horizontal velocity
		Vector2 horizontal_vel = Vector2(m_vel.x, m_vel.z);
		if (horizontal_vel.Length() > m_maxHorizontalSpeed)
		{
			horizontal_vel.Normalize();
			horizontal_vel *= m_maxHorizontalSpeed;
			m_vel.x = horizontal_vel.x;
			m_vel.z = horizontal_vel.y;
		}

		// resets the players position if it drops too low
		if (GetPos().y < -10)
		{
			SetPos(Vector3(0, 20, 0));
			m_vel = Vector3::Zero;
		}
		m_acc.y -= 20;

		// simulates friction by having higher drag when grounded
		if (m_inAir)
		{
			SetDrag(0.5);
		}
		else
		{
			SetDrag(3);
		}

		// sets the player to be in the air to stop the players ability to jump (set to true when colliding with ground
		m_inAir = true;

		// stops the player being able to jump instantly after initiating a jump with a short cooldown
		if (!m_canJump)
		{
			m_jumpCDTimer += _GD->m_dt;
			if (m_jumpCDTimer > m_jumpCD)
			{
				m_canJump = true;
				m_jumpCDTimer = 0;
			}
		}

		// Moves the player forward/backwards if w/s is pressed;
		Vector3 forwardMove = 40.0f * Vector3::Forward;
		Matrix rotMove = Matrix::CreateRotationY(m_yaw);
		forwardMove = Vector3::Transform(forwardMove, rotMove);
		if (_GD->m_KBS.W)
		{
			m_acc += forwardMove;
		}
		if (_GD->m_KBS.S)
		{
			m_acc -= forwardMove;
		}

		// Moves the player left/right if a/d is pressed;
		Vector3 rightMove = 40.0f * Vector3::Right;
		Matrix rotMove2 = Matrix::CreateRotationY(m_yaw);
		rightMove = Vector3::Transform(rightMove, rotMove);
		if (_GD->m_KBS.D)
		{
			m_acc += rightMove;
		}
		if (_GD->m_KBS.A)
		{
			m_acc -= rightMove;
		}

		//change yaw of player based on mouse movement
		float rotSpeed = 0.25f * _GD->m_dt;
		if (_GD->m_MS.x != 0)
		{
			m_yaw -= rotSpeed * _GD->m_MS.x;
		}

		// initiates charging a shot
		if (m_shots > 0 && _GD->m_MS.leftButton && !m_chargingShot)
		{
			m_chargingShot = true;
			m_bowUI.lock()->Activate();
		}

		// tries to shoot if left click is released while charging
		else if (!_GD->m_MS.leftButton && m_chargingShot)
		{
			// shoots if the bow is at least half charged
			if (m_shotChargeTimer > (m_shotChargeTime / 2))
			{
				// gets the direction the arrow will start with
				Vector3 direction = getHeadPos() - m_camera.lock()->GetPos();
				direction.Normalize();

				// calculates the arrows initial speed
				float speed = (m_shotChargeTimer / m_shotChargeTime) * (m_maxShotSpeed - m_minShotSpeed) + m_minShotSpeed;

				// creates the arrow
				std::shared_ptr<Projectile> new_proj = std::make_shared<Projectile>("arrowfinal3", _GD->m_d3dDevice.Get(), _GD->m_fxFactory, getHeadPos(), direction, speed);
				new_proj->SetScale(1.2, 1.2, 1.2);
				new_proj->SetID(_GD->m_objectNumber);
				_GD->m_objectNumber++;
				new_proj->setIsTrigger(true);
				new_proj->SetTag("arrow");
				new_proj->SetMass(0.5);
				_GD->m_GameObjects.push_back(new_proj);
				_GD->m_PhysicsObjects.push_back(new_proj);

				// makes it so there is no shot limit in the last level (playground)
				if (!_GD->m_wonGame)
				{
					// removes an arrow UI indicator
					m_arrowUI.back()->SetPos(Vector2(-100, -100));
					m_arrowUI.pop_back();

					// removes a shot
					m_shots -= 1;

					// lets the arrow know if it is the last shot
					if (m_shots <= 0)
					{
						new_proj->setLastShot(true);
					}
				}
			}

			// makes the player able to shoot again
			m_chargingShot = false;
			m_shotChargeTimer = 0;
			m_fullyCharged = false;

			// resets the moving part of the reticle
			m_bowUI.lock()->Deactivate();
		}

		// increases charge amount
		if (m_chargingShot && !m_fullyCharged)
		{
			m_shotChargeTimer += _GD->m_dt;
			if (m_shotChargeTimer > m_shotChargeTime)
			{
				m_shotChargeTimer = m_shotChargeTime;
				m_fullyCharged = true;
			}
		}

		// makes the player jump
		if (m_jumpStarted)
		{
			m_vel.y = m_jumpHeight;
			m_jumpStarted = false;
		}
	}

	//apply my base behaviour
	CMOGO::Tick(_GD);
}

void Player::Hit(Vector3 direction)
{
	// if the face hit has a positive enough y component then set the player to be able to jump (on the ground)
	if (direction.y > 0.5)
	{
		m_inAir = false;
	}
}

void Player::LateTick(GameData* _GD)
{
	// if the player collided with ground and pressed space and the jump is not on cooldown, jump
	if (_GD->m_KBS.Space)
	{
		if (!m_inAir && m_canJump)
		{
			m_jumpStarted = true;
			m_canJump = false;
		}
	}
}