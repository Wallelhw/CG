#pragma once
#ifndef MATERIAL
#define MATERIAL

#include "global.h"

struct intersection;

class Materail
{
public:
	virtual bool scatter( const ray& r_out,const intersection& inter, ray& incident_ray,color&  attenuation) const = 0;
	virtual double get_bsdf(const vec3& wi, const vec3& wo, const vec3& n)const = 0;
	virtual double get_pdf(const vec3& wi, const vec3& wo, const vec3& n) const = 0;
};


class LambertianDiffuse : public Materail {
public:
	color albedo;
	double kd;
public:
	LambertianDiffuse(vec3 _albedo = vec3(0.5),double _kd= 0.91) :albedo(_albedo),kd(_kd) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		point3 incident_ray_direction = random_in_unit_hemisphere(n);
		incident_ray = ray(inter.p, incident_ray_direction);
		attenuation = albedo;
		auto cos_theta = dot(incident_ray.direction(),n);
		double eval = get_bsdf(incident_ray.direction(), r_out.direction(), n);
		double pdf = get_pdf(incident_ray.direction(), r_out.direction(), n);
		attenuation *= cos_theta * eval * (1 / pdf);
		return true;
	}

	virtual double get_pdf(const vec3& wi, const vec3& wo, const vec3& n) const override {
		return 1 / (2 * pi);
	}
	virtual double get_bsdf(const vec3& wi, const vec3& wo, const vec3& n)const override {
		return kd / pi;
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
		auto cos_theta = 1;
		double eval = get_bsdf(incident_ray.direction(), r_out.direction(), n);
		double pdf = get_pdf(incident_ray.direction(), r_out.direction(), n);
		attenuation *= cos_theta * eval * (1 / pdf);
		return true;
	}
	virtual double get_pdf(const vec3& wi, const vec3& wo, const vec3& n) const override {
		return 1;
	}
	virtual double get_bsdf(const vec3& wi, const vec3& wo, const vec3& n)const override {
		return 1;
	}
};

class Metal:public Materail {
public:
	color albedo;
	double fuzz;
	double kd;
public:
	Metal(vec3 _albedo = vec3(1),double _fuzz=0.5f,double _kd =0.55 ) :albedo(_albedo),fuzz(_fuzz),kd(_kd) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		point3 incident_ray_direction = reflect(r_out.direction(), n)+fuzz*random_in_unit_sphere();
		incident_ray = ray(inter.p, incident_ray_direction);
		attenuation = albedo;

		return true;

	}
	virtual double get_pdf(const vec3& wi, const vec3& wo, const vec3& n) const override {
		return 1 / (2 * pi);
	}
	virtual double get_bsdf(const vec3& wi, const vec3& wo, const vec3& n)const override {
		return kd / pi;
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
	virtual double get_pdf(const vec3& wi, const vec3& wo, const vec3& n) const override {
		return 1 / (2 * pi);
	}
	virtual double get_bsdf(const vec3& wi, const vec3& wo, const vec3& n)const override {
		return 1;
	}
};


class HG_materail : public Materail {
public:
	double attenuation;
	double HG_g;

public:
	HG_materail(double _attenuation = 1., double _HG_g = 0.5) :attenuation(_attenuation), HG_g(_HG_g) {};

	virtual bool scatter(const ray& r_out, const intersection& inter, ray& incident_ray, color& attenuation) const override {
		vec3 n = inter.normal;
		point3 incident_ray_target = random_in_unit_sphere();
		incident_ray = ray(inter.p, incident_ray_target);
		auto cos_theta = dot(incident_ray.direction(), r_out.direction());


		attenuation = vec3(1);

		return true;
	}
	virtual double get_pdf(const vec3& wi, const vec3& wo, const vec3& n) const override {
		return 1 / (2 * pi);
	}
	virtual double get_bsdf(const vec3& wi, const vec3& wo, const vec3& n)const override {
		return 1;
	}
};
#endif // !MATERIAL
