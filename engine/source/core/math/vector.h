#pragma once
#include <cmath>

namespace math
{
	template <typename T>
	struct Vector2d
	{
		T x_{};
		T y_{};

		Vector2d() : x_(T{}), y_(T{}) {}
		Vector2d(const T& _x, const T& _y) : x_(_x), y_(_y) {}
	};

	template <typename T>
	struct Vector3d
	{
		T x_{};
		T y_{};
		T z_{};

		Vector3d() : x_(T{}), y_(T{}), z_(T{}) {}
		Vector3d(const T& _x, const T& _y, const T& _z) : x_(_x), y_(_y), z_(_z) {}
	};

	template <typename T>
	struct Vector
	{
		T x_{};
		T y_{};
		T z_{};
		T w_{};

		Vector() : x_(T{}), y_(T{}), z_(T{}), w_(T{}) {}
		Vector(const T& _x, const T& _y, const T& _z, const T& _w) : x_(_x), y_(_y), z_(_z), w_(_w) {}
	};
}