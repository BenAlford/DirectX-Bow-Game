#pragma once

#include <DirectXCollision.h>
#include <array>
#include <limits>
#include <algorithm>

#include "CMOGO.h"

using namespace DirectX;

namespace Collision
{
	namespace OBB
	{
		struct MinMax
		{
			float min;
			float max;
		};

		void getOBBFaceNormals(const BoundingOrientedBox& worldspace_box, std::array<XMFLOAT3, 8>& corners_world, XMVECTOR* out_axes)
		{
			//std::array<XMFLOAT3, 8> corners_world;

			worldspace_box.GetCorners(&corners_world[0]);

			const auto origin	= XMLoadFloat3(&corners_world[4]);
			const auto x_dir	= XMLoadFloat3(&corners_world[5]) - origin;
			const auto y_dir	= XMLoadFloat3(&corners_world[7]) - origin;
			const auto z_dir	= XMLoadFloat3(&corners_world[0]) - origin;

			out_axes[0]			= XMVector3Normalize(XMVector3Cross(x_dir, y_dir));
			out_axes[1]			= XMVector3Normalize(XMVector3Cross(y_dir, z_dir));
			out_axes[2]			= XMVector3Normalize(XMVector3Cross(z_dir, x_dir));
		}


		MinMax projectBoxOnAxes(std::array<XMFLOAT3, 8>& corners, const XMVECTOR& ax)
		{
			// Find projection on axes
			float min = std::numeric_limits<float>::infinity();
			float max = -std::numeric_limits<float>::infinity();

			for (const auto& vertex : corners)
			{
				float projection;
				XMStoreFloat(&projection, XMVector3Dot(XMLoadFloat3(&vertex), ax));
				if (projection < min) min = projection;
				if (projection > max) max = projection;
			}

			return MinMax{ min, max };
		}
	}

	XMFLOAT3 ejectionCMOGO(CMOGO& go1, CMOGO& go2)
	{
		// Create & Populate Collision Axes
		std::array<XMVECTOR, 6 > collision_axes;

		BoundingOrientedBox worldbox_1, worldbox_2;
		std::array<XMFLOAT3, 8> corners_world_1, corners_world_2;

		go1.getCollider().Transform(worldbox_1, go1.getWorldTransform());
		OBB::getOBBFaceNormals(worldbox_1, corners_world_1, &collision_axes[0]);

		go2.getCollider().Transform(worldbox_2, go2.getWorldTransform());
		OBB::getOBBFaceNormals(worldbox_2, corners_world_2, &collision_axes[3]);


		float pen_dist = numeric_limits<float>::infinity();
		XMVECTOR pen_vector;

		for (const auto& ax : collision_axes)
		{
			// Get Min and Max overlaps of box vertixes on axes
			const auto minmax_1 = OBB::projectBoxOnAxes(corners_world_1, ax);
			const auto minmax_2 = OBB::projectBoxOnAxes(corners_world_2, ax);

			// Calculate collision depth
			float overlap = min(minmax_1.max, minmax_2.max) - max(minmax_1.min, minmax_2.min);

			// Check if this overlap is the smallest ejection thus far
			if (overlap < pen_dist)
			{
				pen_dist = overlap;
				pen_vector = ax;

				XMFLOAT3 v;
				XMStoreFloat3(&v, ax);
			}
		}

		XMFLOAT3 out;
		XMStoreFloat3(&out, pen_vector);

		auto diff = worldbox_1.Center - worldbox_2.Center;
		float orientation;
		XMStoreFloat(&orientation, XMVector3Dot(XMLoadFloat3(&diff), pen_vector));

		if (orientation > 0) out = out * -1;

		if (go1.IsPhysicsOn())
		{
			// reflects the gameobject off of the collider object

			// finds the vector component of b reflected that is parallel to the face collided with
			Vector3 b = -go1.GetVelocity();
			Vector3 perpb = b - (Vector3(out).Dot(b) * out);
			Vector3 new_vel = -perpb;

			// finds the vector component of b reflected that is perpendicular to the face collided with
			Vector3 parb = (Vector3(out).Dot(b) * out);

			// applies none of the perp velocity if it is too small (it would jitter)
			if (parb.Length() > 5)
			{
				// applies a percentage of the perpendicular velocity based off of the restitution of both objects
				new_vel += parb * go1.GetRestitution() * go2.GetRestitution();
			}

			// sets the new velocity
			go1.SetVelocity(new_vel);
		}

		// calls hit for both objects
		go2.Hit(out);
		go1.Hit(out * -1);

		return out * pen_dist;
	}

	void PhysicsToPhysics(CMOGO& go1, CMOGO& go2)
	{
		// handles physics for 2 physics objects colliding

		// first convert each objects velocity into components parralel and perpendicular to the collision axis (the line drawn between the 2 objects centre)
		Vector3 to_one = go1.GetPos() - go2.GetPos();
		to_one.Normalize();
		Vector3 par2 = (Vector3(to_one).Dot(go2.GetVelocity()) * to_one);
		Vector3 perp2 = go2.GetVelocity() - par2;
		float m2 = go2.GetMass();
		float u2 = par2.Length();

		Vector3 to_two = go2.GetPos() - go1.GetPos();
		to_two.Normalize();
		Vector3 par1 = (Vector3(to_two).Dot(go1.GetVelocity()) * to_two);
		Vector3 perp1 = go1.GetVelocity() - par1;
		float m1 = go1.GetMass();
		float u1 = -par1.Length();

		// calculates coefficient of restitution for this collision
		float Cr = go1.GetRestitution() * go2.GetRestitution();

		// calculates the resulting velocity after the collision for both objects in 1 dimension
		float v1 = ((Cr * m2 * (u2 - u1)) + (m1 * u1) + (m2 * u2)) / (m1 + m2);
		float v2 = ((Cr * m1 * (u1 - u2)) + (m1 * u1) + (m2 * u2)) / (m1 + m2);


		// converts each velocity to 3D and re-adds the perpendicular component that has no part in the collision
		Vector3 new_vel_1 = (to_one * v1) + perp1;
		go1.SetVelocity(new_vel_1);

		Vector3 new_vel_2 = (to_one * v2) + perp2;
		go2.SetVelocity(new_vel_2);


		// separates the 2 objects by half the ejection distance

		// Create & Populate Collision Axes
		std::array<XMVECTOR, 6 > collision_axes;

		BoundingOrientedBox worldbox_1, worldbox_2;
		std::array<XMFLOAT3, 8> corners_world_1, corners_world_2;

		go1.getCollider().Transform(worldbox_1, go1.getWorldTransform());
		OBB::getOBBFaceNormals(worldbox_1, corners_world_1, &collision_axes[0]);

		go2.getCollider().Transform(worldbox_2, go2.getWorldTransform());
		OBB::getOBBFaceNormals(worldbox_2, corners_world_2, &collision_axes[3]);


		float pen_dist = numeric_limits<float>::infinity();
		XMVECTOR pen_vector;

		for (const auto& ax : collision_axes)
		{
			// Get Min and Max overlaps of box vertixes on axes
			const auto minmax_1 = OBB::projectBoxOnAxes(corners_world_1, ax);
			const auto minmax_2 = OBB::projectBoxOnAxes(corners_world_2, ax);

			// Calculate collision depth
			float overlap = min(minmax_1.max, minmax_2.max) - max(minmax_1.min, minmax_2.min);

			// Check if this overlap is the smallest ejection thus far
			if (overlap < pen_dist)
			{
				pen_dist = overlap;
				pen_vector = ax;

				XMFLOAT3 v;
				XMStoreFloat3(&v, ax);
			}
		}

		XMFLOAT3 out;
		XMStoreFloat3(&out, pen_vector);

		auto diff = worldbox_1.Center - worldbox_2.Center;
		float orientation;
		XMStoreFloat(&orientation, XMVector3Dot(XMLoadFloat3(&diff), pen_vector));

		if (orientation > 0) out = out * -1;

		go1.SetPos(go1.GetPos() - (out * pen_dist / 1.5));
		go2.SetPos(go2.GetPos() + (out * pen_dist / 1.5));
	}
}