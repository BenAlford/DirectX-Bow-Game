#pragma once

#include "CMOGO.h"

using namespace DirectX;
using namespace std;

class CMOGOSphere : public CMOGO
{

public:
	CMOGOSphere(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF);
	BoundingSphere& getSphereCollider()		noexcept { return m_sphereCollider; }
	const BoundingSphere& getSphereCollider() const noexcept { return m_sphereCollider; }

protected:
	BoundingSphere m_sphereCollider;
};
