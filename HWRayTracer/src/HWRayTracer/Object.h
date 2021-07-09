#pragma once
#ifndef  HITTABLE_H
#define HITTABLE_H
#include "ray.h"

class Materail;

struct intersection
{
	point3 p;
	vec3 normal;
	double t;
	bool out_face;
	shared_ptr<Materail> materail;

	inline void set_face_normal (const vec3& outward_normal,const ray& r)  {
	    out_face = dot(r.direction(), outward_normal) < 0;
		normal = out_face ? normal : (-normal);
	}
};

class Object {
public:
	shared_ptr<Materail> materail;

public:
	virtual bool hit(const ray& r, double t_min, double t_max, intersection& inter) const = 0;
};

#endif // ! HITTABLE_H
