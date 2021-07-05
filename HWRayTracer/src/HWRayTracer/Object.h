#pragma once
#ifndef  HITTABLE_H
#define HITTABLE_H
#include "ray.h"

struct intersection
{
	point3 p;
	vec3 normal;
	double t;
	bool front_face;

	inline void set_face_normal (const vec3& outward_normal,const ray& r)  {
		front_face = dot(normal, outward_normal) < 0;
		normal = front_face ? normal : (-normal);
	}
};

class Object {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, intersection& inter) const = 0;
};

#endif // ! HITTABLE_H
