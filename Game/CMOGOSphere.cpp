#include "pch.h"
#include "CMOGOSphere.h"

CMOGOSphere::CMOGOSphere(string _fileName, ID3D11Device* _pd3dDevice, std::shared_ptr<IEffectFactory> _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	// calculates the size of the sphere that includes everything
	std::vector<XMFLOAT3> bbVerts(8 * m_model->meshes.size());

	for (int i = 0; i < m_model->meshes.size(); i++)
	{
		const auto& bbox = m_model->meshes[i]->boundingBox;

		const auto& bbox_ctr = bbox.Center;
		const auto& bbox_ext = bbox.Extents;

		bbVerts[8 * i + 0] = bbox.Center + XMFLOAT3(-1, -1, -1) * bbox_ext;
		bbVerts[8 * i + 1] = bbox.Center + XMFLOAT3(+1, -1, -1) * bbox_ext;
		bbVerts[8 * i + 2] = bbox.Center + XMFLOAT3(-1, +1, -1) * bbox_ext;
		bbVerts[8 * i + 3] = bbox.Center + XMFLOAT3(+1, +1, -1) * bbox_ext;
		bbVerts[8 * i + 4] = bbox.Center + XMFLOAT3(-1, -1, +1) * bbox_ext;
		bbVerts[8 * i + 5] = bbox.Center + XMFLOAT3(+1, -1, +1) * bbox_ext;
		bbVerts[8 * i + 6] = bbox.Center + XMFLOAT3(-1, +1, +1) * bbox_ext;
		bbVerts[8 * i + 7] = bbox.Center + XMFLOAT3(+1, +1, +1) * bbox_ext;
	}

	// Set up minmax floats
	auto minmax_x = std::minmax_element(bbVerts.begin(), bbVerts.end(), [](const XMFLOAT3& a, const XMFLOAT3& b) { return a.x < b.x; });
	auto minmax_y = std::minmax_element(bbVerts.begin(), bbVerts.end(), [](const XMFLOAT3& a, const XMFLOAT3& b) { return a.y < b.y; });
	auto minmax_z = std::minmax_element(bbVerts.begin(), bbVerts.end(), [](const XMFLOAT3& a, const XMFLOAT3& b) { return a.z < b.z; });

	// Get min and max values
	float minX = minmax_x.first->x;
	float maxX = minmax_x.second->x;
	float minY = minmax_y.first->y;
	float maxY = minmax_y.second->y;
	float minZ = minmax_z.first->z;
	float maxZ = minmax_z.second->z;

	// Get center values
	float centerX = (maxX + minX) / 2;
	float centerY = (maxY + minY) / 2;
	float centerZ = (maxZ + minZ) / 2;

	// Get Extents
	float extX = (maxX - minX) / 2;
	float extY = (maxY - minY) / 2;
	float extZ = (maxZ - minZ) / 2;

	m_sphereCollider.Center = { centerX, centerY, centerZ };
	
	// picks the biggest of the extents and makes that the radius
	if (extX > extY && extX > extZ)
	{
		m_sphereCollider.Radius = extX;
	}
	else if (extY > extZ)
	{
		m_sphereCollider.Radius = extY;
	}
	else
	{
		m_sphereCollider.Radius = extZ;
	}
}	