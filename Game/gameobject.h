#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

//=================================================================
//Base Game Object Class
//=================================================================

#include "CommonStates.h"
#include "SimpleMath.h"
#include <vector>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera;
struct ID3D11DeviceContext;
struct GameData;
struct DrawData;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD) = 0;
	virtual void LateTick(GameData* _GD) {};
	virtual void Hit(Vector3 normal) {};

	virtual void updateWorldMat();

	void addAcceleration(Vector3 new_acc) { m_acc += new_acc; }
	void addVelocity(Vector3 new_vel) { m_vel += new_vel; }
	void addChild(GameObject* new_child) { m_children.push_back(new_child); }

	//getters
	Vector3		GetPos() { return Vector3(m_worldMat._41, m_worldMat._42,m_worldMat._43); }
	Vector3		GetScale() { if (!m_parent.expired()) return m_scale * m_parent.lock()->GetScale(); else return m_scale; }

	float		GetPitch() { return m_pitch; }
	float		GetYaw() { return m_yaw; }
	float		GetRoll() { return m_roll; }

	Matrix& getWorldTransform() { return m_worldMat; }
	const Matrix& getWorldTransform() const { return m_worldMat; }

	bool		IsPhysicsOn() { return m_physicsOn; }
	float		GetDrag() { return m_drag; }

	Matrix GetWorldMat() { return m_worldMat; }
	Matrix GetRotMat() { if (!m_parent.expired()) return m_rotMat * m_parent.lock()->GetRotMat(); else return m_rotMat; }

	std::string GetTag() { return m_tag; }

	int GetID() { return ID; }

	Vector3 GetVelocity() { return m_vel; }
	float GetRestitution() { return m_restitution; }
	float GetMass() { return m_mass; }

	//setters
	void		SetPos(Vector3 _pos) { m_pos = _pos; updateWorldMat(); for (auto& child : m_children) { child->updateWorldMat(); } }

	void		SetScale(float _scale) { m_scale = _scale * Vector3::One; }
	void		SetScale(float _x, float _y, float _z) { m_scale = Vector3(_x, _y, _z); }
	void		SetScale(Vector3 _scale) { m_scale = _scale; }

	void		SetPitch(float _pitch) { m_pitch = _pitch; }
	void		SetYaw(float _yaw) { m_yaw = _yaw; }
	void		SetRoll(float _roll) { m_roll = _roll; }
	void		SetPitchYawRoll(float _pitch, float _yaw, float _roll) { m_pitch = _pitch; m_yaw = _yaw; m_roll = _roll; }
	void		SetRotMat(Matrix mat) { m_rotMat = mat; }

	void		SetPhysicsOn(bool _physics) { m_physicsOn = _physics; }
	void		TogglePhysics() { m_physicsOn = !m_physicsOn; }
	void		SetDrag(float _drag) { m_drag = _drag; }
	void		SetTag(std::string _tag) { m_tag = _tag; }
	void		SetID(int new_ID) { ID = new_ID; }
	void		SetParent(std::shared_ptr<GameObject> new_parent);
	void		SetVelocity(Vector3 new_vel) { m_vel = new_vel; }
	void		SetRestitution(float _restitution) { m_restitution = _restitution; }
	void		SetMass(float _mass) { m_mass = _mass; }

protected:

	//World transform/matrix of this GO and it components
	Matrix m_worldMat;
	Matrix m_rotMat;
	Matrix m_fudge;
	Vector3 m_pos;
	float m_pitch, m_yaw, m_roll;
	Vector3 m_scale;

	//very basic physics
	bool m_physicsOn = false;
	float m_drag = 0.0f;
	Vector3 m_vel = Vector3::Zero;
	Vector3 m_acc = Vector3::Zero;
	Vector3 m_gravity;

	std::string m_tag = "None";
	int ID = 0;

	std::weak_ptr<GameObject> m_parent;
	bool m_useRollPitchYaw = true;
	std::vector<GameObject*> m_children;
	float m_restitution = 0;
	float m_mass = 1;
};

#endif