#pragma once
#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>

using namespace std;
using std::sqrt;


//define Vecter3D
class vec3{
public:
	double e[3];

public:
	vec3() :e{ 0,0,0 } {}
	vec3(double x, double y, double z) :e{ x,y,z } {}
	vec3(double t) :e{ t,t,t } {}

	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[1]); }

	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v) {
		this->e[0] += v.e[0];
		this->e[1] += v.e[1];
		this->e[2] += v.e[2];
		return *this;
		};

	vec3& operator*=(const double t) {
		this->e[0] *= t;
		this->e[1] *= t;
		this->e[2] *= t;
		return *this;
	}

	vec3& operator/=(const double t) {
		return *this *= 1 / t;
	}

	double length_squared() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	double length() const {
		return sqrt(length_squared());
	}
};

using point3 = vec3;
using color = vec3;

//Vector3 Utility Functions

inline std::ostream& operator <<(std::ostream& os, const vec3& v) {
	return os << "{" << v.x() << " ," << v.y() << " ," << v.z() << " }" << endl;
}

inline vec3 operator+ (const vec3& v0, const vec3& v1) {
	return vec3(v0.e[0] + v1[0], v0.e[1] + v1.e[1], v0.e[2] + v0.e[3]);
}

inline vec3 operator- (const vec3& v0, const vec3& v1) {
	return vec3(v0.e[0] - v1[0], v0.e[1] - v1.e[1], v0.e[2] - v0.e[3]);
}

inline vec3 operator*(const double& d, const vec3& v0) {
	return vec3(v0.e[0] * d, v0.e[1] * d, v0.e[2] * d);
}

inline vec3 operator*(const vec3& v0,const double& d) {
	return d * v0;
}

inline vec3 operator/(const vec3& v0, const double& d) {
	return v0 * (1 / d);
}

inline double dot(const vec3& v0,const vec3& v1) {
	return v0.e[0] * v1.e[0] + v0.e[1] * v1.e[1] + v0.e[2] * v1.e[2];
}

inline vec3 cross(const vec3& v0, const vec3& v1) {
	return vec3(v0.e[1] * v1.e[2] - v1.e[1] * v0.e[2], v0.e[2] * v1.e[0] - v0.e[0] * v1.e[2], v0.e[0] * v1.e[1] - v1.e[0] * v0.e[1]);
}
#endif
