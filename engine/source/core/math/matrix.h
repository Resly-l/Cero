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
	template <typename T>
	struct Matrix2x2
	{
		union
		{
			Vector2d<T> v_[2];
			struct
			{
				T _11, _12;
				T _21, _22;
			};
		};

		Matrix2x2()
			: _11(T(0)), _12(T(0))
			, _21(T(0)), _22(T(0))
		{}

		static Matrix2x2 Identity()
		{
			Matrix2x2 mat{};
			mat.v_[0] = Vector2d(T(1), T(0));
			mat.v_[1] = Vector2d(T(0), T(1));
			return mat;
		}

		static Matrix2x2 Rotation(const T _angle)
		{
			const T sinTheta = sin(_angle);
			const T cosTheta = cos(_angle);

			Matrix2x2 mat;
			mat.v_[0] = Vector2d(T(cosTheta), T(sinTheta));
			mat.v_[1] = Vector2d(-T(sinTheta), T(cosTheta));
			return mat;
		}
	};

	template <typename T>
	struct Matrix3x3
	{
		union
		{
			Vector3d<T> v_[3];
			struct
			{
				T _11, _12, _13;
				T _21, _22, _23;
				T _31, _32, _33;
			};
		};

		Matrix3x3()
			: _11(T(0)), _12(T(0)), _13(T(0))
			, _21(T(0)), _22(T(0)), _23(T(0))
			, _31(T(0)), _32(T(0)), _33(T(0))
		{}

		Matrix3x3& operator =(const Matrix2x2<T>& _matrix2d)
		{
			_11 = _matrix2d._11;
			_12 = _matrix2d._12;
			_21 = _matrix2d._21;
			_22 = _matrix2d._22;
			return *this;
		}

		static Matrix3x3 Identity()
		{
			Matrix3x3 mat{};
			mat.v_[0] = Vector3d(T(1), T(0), T(0));
			mat.v_[1] = Vector3d(T(0), T(1), T(0));
			mat.v_[2] = Vector3d(T(0), T(0), T(1));
			return mat;
		}

		static Matrix3x3 RotationX(const T _angle)
		{
			const T sinTheta = sin(_angle);
			const T cosTheta = cos(_angle);

			Matrix3x3 mat;
			mat.v_[0] = Vector3d(T(1), T(0), T(0));
			mat.v_[1] = Vector3d(T(0), T(cosTheta), T(sinTheta));
			mat.v_[2] = Vector3d(T(0), -T(sinTheta), T(cosTheta));
			return mat;
		}
		static Matrix3x3 RotationY(const T _angle)
		{
			const T sinTheta = sin(_angle);
			const T cosTheta = cos(_angle);

			Matrix3x3 mat;
			mat.v_[0] = Vector3d(T(cosTheta), T(0), -T(sinTheta));
			mat.v_[1] = Vector3d(T(0), T(1), T(0));
			mat.v_[2] = Vector3d(T(sinTheta), T(0), T(cosTheta));
			return mat;
		}
		static Matrix3x3 RotationZ(const T _angle)
		{
			const T sinTheta = sin(_angle);
			const T cosTheta = cos(_angle);

			Matrix3x3 mat;
			mat.v_[0] = Vector3d(T(cosTheta), T(sinTheta), T(0));
			mat.v_[1] = Vector3d(-T(sinTheta), T(cosTheta), T(0));
			mat.v_[2] = Vector3d(T(0), T(0), T(1));
			return mat;
		}
	};

	template <typename T>
	struct Matrix
	{
		union
		{
			Vector<T> v_[4];
			struct
			{
				T _11, _12, _13, _14;
				T _21, _22, _23, _24;
				T _31, _32, _33, _34;
				T _41, _42, _43, _44;
			};
		};

		Matrix()
			: _11(T(0)), _12(T(0)), _13(T(0)), _14(T(0))
			, _21(T(0)), _22(T(0)), _23(T(0)), _24(T(0))
			, _31(T(0)), _32(T(0)), _33(T(0)), _34(T(0))
			, _41(T(0)), _42(T(0)), _43(T(0)), _44(T(0))
		{}

		Matrix& operator =(const Matrix2x2<T>& _matrix2d)
		{
			_11 = _matrix2d._11;
			_12 = _matrix2d._12;
			_21 = _matrix2d._21;
			_22 = _matrix2d._22;
			return *this;
		}
		Matrix& operator =(const Matrix3x3<T>& _matrix3d)
		{
			_11 = _matrix3d._11;
			_12 = _matrix3d._12;
			_13 = _matrix3d._13;
			_21 = _matrix3d._21;
			_22 = _matrix3d._22;
			_23 = _matrix3d._23;
			_31 = _matrix3d._31;
			_32 = _matrix3d._32;
			_33 = _matrix3d._33;
			return *this;
		}

		static Matrix Identity()
		{
			Matrix mat{};
			mat.v_[0] = Vector(T(1), T(0), T(0), T(0));
			mat.v_[1] = Vector(T(0), T(1), T(0), T(0));
			mat.v_[2] = Vector(T(0), T(0), T(1), T(0));
			mat.v_[3] = Vector(T(0), T(0), T(0), T(1));
			return mat;
		}

		static Matrix RotationX(const T _angle)
		{
			Matrix<T> mat = Identity();
			mat = Matrix3x3<T>::RotationX(_angle);
			return mat;
		}
		static Matrix RotationY(const T _angle)
		{
			Matrix<T> mat = Identity();
			mat = Matrix3x3<T>::RotationY(_angle);
			return mat;
		}
		static Matrix RotationZ(const T _angle)
		{
			Matrix<T> mat = Identity();
			mat = Matrix3x3<T>::RotationZ(_angle);
			return mat;
		}

		static Matrix Projection(const T _near, const T _far, const T _horizontalFoV, const T _aspectRatio)
		{
			// depth division occurs in graphics api pipeline
			// projection matrix is to convert world space point into ndc space point

			const T tanHorizontal = tan(_horizontalFoV * T(0.5));
			const T tanVertical = tanHorizontal / _aspectRatio;
			const T invFmN = T(_far) / (_far - _near);
			const T beta = -_near * invFmN;

			Matrix<T> mat;
			mat.v_[0] = Vector<T>(T(T(1) / (tanHorizontal)), T(0), T(0), T(0));
			mat.v_[1] = Vector<T>(T(0), T(T(1) / (tanVertical)), T(0), T(0));
			mat.v_[2] = Vector<T>(T(0), T(0), T(invFmN), T(1));
			mat.v_[3] = Vector<T>(T(0), T(0), T(beta), T(0));
			return mat;
		}
	};
}