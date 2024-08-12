#include "vector.h"

namespace math
{
	Vector::Vector(float _value)
		: x_(_value)
		, y_(_value)
		, z_(_value)
		, w_(_value)
	{}

	Vector::Vector(Float2 _float2)
		: x_(_float2.x_)
		, y_(_float2.y_)
	{}

	Vector::Vector(Float3 _float3)
		: x_(_float3.x_)
		, y_(_float3.y_)
		, z_(_float3.z_)
	{}

	Vector::Vector(Float4 _float4)
		: x_(_float4.x_)
		, y_(_float4.y_)
		, z_(_float4.z_)
		, w_(_float4.w_)
	{}

	Vector::Vector(float _x, float _y, float _z, float _w)
		: x_(_x)
		, y_(_y)
		, z_(_z)
		, w_(_w)
	{}
}