#pragma once
#ifndef SPHERE
#define SPHERE

#include<cmath>
#include "Object.h"

class Sphere : public Object
{
public:
	point3 center;
	double radius;

public:
	Sphere() {};
	Sphere(point3 _center, double _radius, shared_ptr<Materail> _m) : center(_center), radius(_radius) { materail = _m; };

    virtual bool hit(const ray& r, double t_min, double t_max, intersection& inter) const override;
};

#endif // !SPHERE
