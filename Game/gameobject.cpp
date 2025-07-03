#include "pch.h"
#include "gameobject.h"
#include "GameData.h"
#include <iostream>

GameObject::GameObject()
{
	//set the Gameobject to the origin with no rotation and unit scaling 
	m_pos = Vector3::Zero;
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	m_scale = Vector3::One;

	m_worldMat = Matrix::Identity;
	m_fudge = Matrix::Identity;

	m_gravity = Vector3(0, -40, 0);
}

GameObject::~GameObject()
{

}

void GameObject::Tick(GameData* _GD)
{
	if (m_physicsOn)
	{
		// adds gravity
		m_acc += m_gravity;

		Vector3 newVel = m_vel + _GD->m_dt * (m_acc - m_drag*m_vel);
		Vector3 newPos = m_pos + _GD->m_dt * m_vel;

		m_vel = newVel;
		m_pos = newPos;
	}
	updateWorldMat();

	//zero acceleration ready for the next time round
	m_acc = Vector3::Zero;
}

void GameObject::updateWorldMat()
{
	//build up the world matrix depending on the new position of the GameObject
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this
	Matrix  scaleMat = Matrix::CreateScale(m_scale);

	// recreates the rotation matrix from roll pitch yaw if enabled
	if (m_useRollPitchYaw)
		m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll); //possible not the best way of doing this!

	Matrix  transMat = Matrix::CreateTranslation(m_pos);

	// if this object has a parent, apply its world matrix after this objects (to move with the parent)
	if (!m_parent.expired())
	{
		m_worldMat = m_fudge * scaleMat * m_rotMat * transMat * m_parent.lock()->GetWorldMat();
	}
	else
		m_worldMat = m_fudge * scaleMat * m_rotMat * transMat;
}

void GameObject::SetParent(std::shared_ptr<GameObject> new_parent)
{
	m_parent = new_parent;

	// adds this object to the parents vector of child objects
	new_parent->addChild(this);

	// sets the rotation matrix to be relative to the parents
	m_rotMat = m_rotMat * m_parent.lock()->GetRotMat().Invert();

	// sets the position to be relative to the parents
	m_pos = Vector3::Transform(m_pos, m_parent.lock()->GetWorldMat().Invert());

	// sets the parents position to be relative to the parents
	m_scale = m_scale / m_parent.lock()->GetScale();

	// cannot use roll pitch yaw anymore as that would reset the rotation matrix
	m_useRollPitchYaw = false;
}
