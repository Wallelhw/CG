#pragma once
#ifndef RAY_H
#define RAY_H
#include "vec3.h"

struct ray {
public:
	point3 orig;
	vec3 dir;
	double time;

public:
	ray() {}
	ray(const vec3& o, const vec3& d) :orig(o), dir(d), time( 0) {}
	ray(const vec3& o, const vec3& d, const double t = 0.0) :orig(o), dir(d), time( t ) {}

	point3 at() {
		return orig + time * dir;
	}

	vec3 direction() const {
		return dir;
	}

	vec3 origin() const {
		return orig;
	}
};

#endif 