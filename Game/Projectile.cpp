#include "pch.h"
#include "Projectile.h"
#include <iostream>
#include "GameData.h"

Projectile::Projectile(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF, Vector3 pos_start, Vector3 direction_start, float speed) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_useRollPitchYaw = false;
	m_pos = pos_start;

	// make its hitbox smaller so it can penetrate its target more before deactivating
	m_collider.Extents = m_collider.Extents / 2;
	SetDrag(0.3);
	SetPhysicsOn(true);
	m_vel = direction_start * speed;
	updateRotation();
}

void Projectile::updateRotation()
{
	// if the arrow hasn't landed
	if (m_physicsOn)
	{
		// rotates the arrow's yaw to face its direction
		Matrix yaw_mat = Matrix::CreateFromAxisAngle(Vector3::Up, atan2(m_vel.x, m_vel.z) + XM_PI);
		Vector3 forward = m_vel;
		forward.Normalize();

		// calculates the local right of the arrow (to rotate around)
		Vector3 right = forward.Cross(Vector3::Up);

		// sets the rotation matrix so the arrow points in the direction it is travelling
		m_rotMat = yaw_mat * Matrix::CreateFromAxisAngle(right, atan2(m_vel.y, sqrt(m_vel.x * m_vel.x + m_vel.z * m_vel.z)));
	}
}

void Projectile::Tick(GameData* _GD)
{
	// if the arrow is too low, deactivate and hide it
	if (m_draw && m_pos.y < -50)
	{
		m_deleteTrigger = true;
		m_physicsOn = false;
		m_draw = false;
	}

	if (m_deleteTrigger)
	{
		// if this was the last shot, let game know it has finished its flight
		if (m_lastShot)
		{
			_GD->m_last_shot_hit = true;
		}
		m_deleteTrigger = false;

		// removes arrow from physics objects to stop colliding with objects
		int index = -1;
		for (int i = 0; i < _GD->m_PhysicsObjects.size(); i++)
		{
			if (_GD->m_PhysicsObjects[i]->GetID() == ID)
			{
				index = i;
				break;
			}
		}
		if (index != -1)
			_GD->m_PhysicsObjects.erase(std::next(_GD->m_PhysicsObjects.begin(), index));
	}
	updateRotation();
	CMOGO::Tick(_GD);
}

void Projectile::TriggerEntered(std::shared_ptr<CMOGO> other)
{
	// if the object hit is not a trigger and is not the player, stick in it my freezing and becoming parented to that object
	if (m_physicsOn && !other->isTrigger() && other->GetTag() != "Player")
	{
		m_physicsOn = false;
		m_deleteTrigger = true;
		SetParent(other);

		// if the parent is a physics object add the arrows velocity to it (adjusted for mass)
		if (other->IsPhysicsOn())
		{
			other->addVelocity((m_vel) * (m_mass / other->GetMass()) );
		}
	}
}