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

class Mirror : public Materail {
public:
	color albedo;
public:
	Mirror(vec3 _albedo = vec3(0.5)) :albedo(_albedo) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		point3 incident_ray_direction = reflect(r_out.direction(), n);
		incident_ray = ray(inter.p, incident_ray_direction);
		attenuation = albedo;

		return true;
	}

};

class Metal:public Materail {
public:
	color albedo;
	double fuzz;
public:
	Metal(vec3 _albedo = vec3(1),double _fuzz=0.5f) :albedo(_albedo),fuzz(_fuzz) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		point3 incident_ray_direction = reflect(r_out.direction(), n)+fuzz*random_in_unit_sphere();
		incident_ray = ray(inter.p, incident_ray_direction);
		attenuation = albedo;

		return true;
	}

};

class Glass : public Materail{
public:
	double eta0;
	double eta1;
	double attenuation;

public:
	Glass(double _eta0 = 1, double _eta1 = 1.5,double _attenuation = 1) :eta0(_eta0), eta1(_eta1) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		double eta_to_etap = inter.out_face ? (eta0 / eta1) : (eta1 / eta0);
		point3 incident_ray_target = refract(r_out.direction(), n, eta_to_etap);
		incident_ray = ray(inter.p, incident_ray_target);
		attenuation = vec3(1);

		return true;
	}
};
#endif // !MATERIAL
