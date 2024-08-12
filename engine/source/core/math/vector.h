#pragma once
#include <cmath>

namespace math
{
	struct Float2
	{
		float x_ = 0.0f;
		float y_ = 0.0f;

		Float2() = default;
		Float2(float _x, float _y) : x_(_x), y_(_y) {}
	};
	struct Float3 : public Float2
	{
		float z_ = 0.0f;

		Float3() = default;
		Float3(float _x, float _y, float _z) : Float2(_x, _y), z_(_z) {}
	};
	struct Float4 : public Float3
	{
		float w_ = 0.0f;

		Float4() = default;
		Float4(float _x, float _y, float _z, float _w) : Float3(_x, _y, _z), w_(_w) {}
	};

	struct Vector
	{
		float x_ = 0.0f;
		float y_ = 0.0f;
		float z_ = 0.0f;
		float w_ = 0.0f;

		Vector() = default;
		Vector(float _value);
		Vector(Float2 _float2);
		Vector(Float3 _float3);
		Vector(Float4 _float4);
		Vector(float _x, float _y, float _z, float _w = 0.0f);


	};
}