#pragma once
#ifndef MATERIAL
#define MATERIAL

#include "global.h"

struct intersection;

class Materail
{
public:
	virtual bool scatter( const ray& r_out,const intersection& inter, ray& incident_ray,color&  attenuation) const = 0;
};


class Diffuse : public Materail {
public:
	color albedo;
public:
	Diffuse(vec3 _albedo = vec3(0.5)) :albedo(_albedo) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		point3 incident_ray_direction = random_in_unit_hemisphere(n);

		incident_ray = ray(inter.p, incident_ray_direction);
		attenuation = albedo;

		return true;
	}
};
#endif // !MATERIAL
