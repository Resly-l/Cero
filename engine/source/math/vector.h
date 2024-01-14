#pragma once
#include "thirdparty/glm/vec3.hpp"
#include "thirdparty/glm/vec4.hpp"
#include "thirdparty/glm/geometric.hpp"

namespace math
{
	template <typename T>
	struct Vec2
	{
		T x_{};
		T y_{};

		Vec2() : x_(T{}), y_(T{}) {}
		Vec2(const T& _x, const T& _y) : x_(_x), y_(_y) {}
	};

	template <typename T>
	struct Vec3
	{
		T x_{};
		T y_{};
		T z_{};

		Vec3() : x_(T{}), y_(T{}), z_(T{}) {}
		Vec3(const T& _x, const T& _y, const T& _z) : x_(_x), y_(_y), z_(_z) {}
	};

	template <typename T>
	struct Vec4
	{
		T x_{};
		T y_{};
		T z_{};
		T w_{};

		Vec4() : x_(T{}), y_(T{}), z_(T{}), w_(T{}) {}
		Vec4(const T& _x, const T& _y, const T& _z, const T& _w) : x_(_x), y_(_y), z_(_z), w_(_w) {}
	};
}