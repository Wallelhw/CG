#pragma once
#ifndef  HITTABLE_H
#define HITTABLE_H
#include "ray.h"

struct intersection
{
	point3 p;
	vec3 normal;
	double t;
};

class Object {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, intersection& inter) const = 0;
};

#endif // ! HITTABLE_H
