
#pragma once

// @TODO: This needs to go in a shared location so can be used by multiple gems using common shared engine-agnostic types.

#include "k_core.h"
#include "k_math.h"

/*
#include <Cry_Vector3.h>
#include <Cry_Quat.h>
#include <Cry_Matrix33.h>
*/
#include <Cry_Geo.h>
#include <Cry_Color.h>


namespace kantan_ly
{
	inline Vec3 k_to_cry(const kantan::k_vec3& v)
	{
		const auto v_f = v.cast< float >();
		return Vec3(v_f[0], v_f[1], v_f[2]);
	}

	inline Vec2 k_to_cry(const kantan::k_vec2& v)
	{
		const auto v_f = v.cast< float >();
		return Vec2(v_f[0], v_f[1]);
	}

	inline Quat k_to_cry(const kantan::k_quat& q)
	{
		const auto q_f = q.cast< float >();
		return Quat(q_f.w(), q_f.x(), q_f.y(), q_f.z());
	}

/*	inline Matrix33 k_to_cry(const kantan::k_quat& v)
	{
		return;
	}
	*/

	inline ColorF k_to_cry(const kantan::k_color& c)
	{
		const auto c_f = c.cast< float >();
		return ColorF(c_f[0], c_f[1], c_f[2], c_f[3]);
	}
}


