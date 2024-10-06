#pragma once
#include "vector.h"
#include "DirectXMath.h"

//-- about rotation --
	// angle is mesurement which represents amount of rotation
	// degree is just an arbtrary number ancient civilization used with no particular mathemathical reason
	// pi is length of half circonference relative to radious which is same for any size of circles
	// angle mesurement of pi is called radian
	//
	// we can get x and y coorinates of a point on a circle using trigonometry
	// x = r * cos theta, y = r * sin theta
	//
	// when there's an arbtrary point p0 on a circle and we want a rotated point coordinate(p1) of this p0 by certain angle phi,
	// we can get the coordinates of p1 by calculating [ p0's angle from the unit vector + phi ]
	// calculating p0's angle with arc trigs is very slow. thankfully, thanks to trigonometric identities,
	// these can be avoided by rewriting arc trigs with different expressions and makeing them simple
	// sin(alpha + beta) = sin(alpha)*cos(beta) + cos(alpha)*sin(beta)
	// cos(alpha + beta) = cos(alpha)*cos(beta) - sin(alpha)*sin(beta)
	// so in result,
	// p1.x = cos(theta)*cos(phi) - sin(theta)*sin(phi)
	// = p0.x*cos(phi) - p0.y*sin(phi)
	// p1.y = p0.y*cos(phi) + p0.x*sin(phi)

namespace math
{
	struct Matrix
	{
		union
		{
			Vector v_[4];
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
		};

		Matrix()
			: _11(0.0f), _12(0.0f), _13(0.0f), _14(0.0f)
			, _21(0.0f), _22(0.0f), _23(0.0f), _24(0.0f)
			, _31(0.0f), _32(0.0f), _33(0.0f), _34(0.0f)
			, _41(0.0f), _42(0.0f), _43(0.0f), _44(0.0f)
		{}

		Matrix operator *(const Matrix& _other) const
		{
			Matrix m;
			m._11 = (_11 * _other._11) + (_12 * _other._21) + (_13 * _other._31) + (_14 * _other._41);
			m._12 = (_11 * _other._12) + (_12 * _other._22) + (_13 * _other._32) + (_14 * _other._42);
			m._13 = (_11 * _other._13) + (_12 * _other._23) + (_13 * _other._33) + (_14 * _other._43);
			m._14 = (_11 * _other._14) + (_12 * _other._24) + (_13 * _other._34) + (_14 * _other._44);

			m._21 = (_21 * _other._11) + (_22 * _other._21) + (_23 * _other._31) + (_24 * _other._41);
			m._22 = (_21 * _other._12) + (_22 * _other._22) + (_23 * _other._32) + (_24 * _other._42);
			m._23 = (_21 * _other._13) + (_22 * _other._23) + (_23 * _other._33) + (_24 * _other._43);
			m._24 = (_21 * _other._14) + (_22 * _other._24) + (_23 * _other._34) + (_24 * _other._44);

			m._31 = (_31 * _other._11) + (_32 * _other._21) + (_33 * _other._31) + (_34 * _other._41);
			m._32 = (_31 * _other._12) + (_32 * _other._22) + (_33 * _other._32) + (_34 * _other._42);
			m._33 = (_31 * _other._13) + (_32 * _other._23) + (_33 * _other._33) + (_34 * _other._43);
			m._34 = (_31 * _other._14) + (_32 * _other._24) + (_33 * _other._34) + (_34 * _other._44);

			m._41 = (_41 * _other._11) + (_42 * _other._21) + (_43 * _other._31) + (_44 * _other._41);
			m._42 = (_41 * _other._12) + (_42 * _other._22) + (_43 * _other._32) + (_44 * _other._42);
			m._43 = (_41 * _other._13) + (_42 * _other._23) + (_43 * _other._33) + (_44 * _other._43);
			m._44 = (_41 * _other._14) + (_42 * _other._24) + (_43 * _other._34) + (_44 * _other._44);

			return m;
		}

		Matrix& operator *=(const Matrix& _other)
		{
			return *this = *this * _other;
		}

		static Matrix Identity()
		{
			Matrix mat{};
			mat.v_[0] = Vector(1.0f, 0.0f, 0.0f, 0.0f);
			mat.v_[1] = Vector(0.0f, 1.0f, 0.0f, 0.0f);
			mat.v_[2] = Vector(0.0f, 0.0f, 1.0f, 0.0f);
			mat.v_[3] = Vector(0.0f, 0.0f, 0.0f, 1.0f);
			return mat;
		}

		static Matrix RotationX(const float _angle)
		{
			const float sinTheta = sin(_angle);
			const float cosTheta = cos(_angle);

			Matrix mat = Identity();
			mat.v_[0] = Vector(1.0f, 0.0f, 0.0f);
			mat.v_[1] = Vector(0.0f, cosTheta, sinTheta);
			mat.v_[2] = Vector(0.0f, -sinTheta, cosTheta);
			return mat;
		}
		static Matrix RotationY(const float _angle)
		{
			const float sinTheta = sin(_angle);
			const float cosTheta = cos(_angle);

			Matrix mat = Identity();
			mat.v_[0] = Vector(cosTheta, 0.0f, -sinTheta);
			mat.v_[1] = Vector(0.0f, 1.0f, 0.0f);
			mat.v_[2] = Vector(sinTheta, 0.0f, cosTheta);
			return mat;
		}
		static Matrix RotationZ(const float _angle)
		{
			const float sinTheta = sin(_angle);
			const float cosTheta = cos(_angle);

			Matrix mat = Identity();
			mat.v_[0] = Vector(cosTheta, sinTheta, 0.0f);
			mat.v_[1] = Vector(-sinTheta, cosTheta, 0.0f);
			mat.v_[2] = Vector(0.0f, 0.0f, 1.0f);
			return mat;
		}

		static Matrix Rotation(const Vector& _pitchYawRoll)
		{
			return RotationZ(_pitchYawRoll.z_) * RotationX(_pitchYawRoll.x_) * RotationY(_pitchYawRoll.y_);
		}

		static Matrix Scale(const float _scale)
		{
			Matrix mat = Identity();
			mat.v_[0].x_ = _scale;
			mat.v_[1].y_ = _scale;
			mat.v_[2].z_ = _scale;
			return mat;
		}

		static Matrix Translation(const Vector& _offset)
		{
			Matrix translation = Identity();
			translation._41 = _offset.x_;
			translation._42 = _offset.y_;
			translation._43 = _offset.z_;
			return translation;
		}

		static Matrix Projection(const float _near, const float _far, const float _horizontalFoV, const float _aspectRatio, bool _flipY = false)
		{
			// depth division occurs in graphics api pipeline
			// projection matrix is to convert world space point into ndc space point

			const float tanHorizontal = tan(_horizontalFoV * 0.5f);
			const float tanVertical = tanHorizontal / _aspectRatio;
			const float invFmN = _far / (_far - _near);
			const float beta = -_near * invFmN;

			Matrix mat;
			mat.v_[0] = Vector(1.0f / (tanHorizontal), 0.0f, 0.0f, 0.0f);
			mat.v_[1] = Vector(0.0f, (_flipY ? -1.0f : 1.0f) / (tanVertical), 0.0f, 0.0f);
			mat.v_[2] = Vector(0.0f, 0.0f, invFmN, 1.0f);
			mat.v_[3] = Vector(0.0f, 0.0f, beta, 0.0f);

			return mat;
		}
	};
}