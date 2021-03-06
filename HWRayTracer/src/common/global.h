#pragma once
#ifndef GLOBAL
#define GLOBAL

#include <cstdlib>

#include "ray.h"
#include "vec3.h"
const double pi = 3.1415926535897932385;

inline double degree_to_radian(double degree) {
	return degree / 180 * pi;
}//return random double in[0,1)

inline double random_double() {
	return rand()/ (RAND_MAX+1.0);
}

inline double random_double(double min, double max) {
	return min + random_double() * (max - min);
}

inline static vec3 random() {
	return vec3(random_double(), random_double(), random_double());
}

inline static vec3 random(double min, double max) {
	return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}


inline double clamp(double value, double min, double max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

inline static vec3 random_in_unit_sphere() {
	while (true) {
		vec3 temp = random(-1.0, 1.0);
		if (temp.norm() > 1) continue;
		return temp;
	}
}

inline static vec3 random_in_unit_hemisphere(vec3& normal) {
	vec3 res = random_in_unit_sphere();
	return dot(res, normal) > 0 ? res : (-res);
}

inline static double reflectance(double cosine, double ref_idx) { //Schlick approximation for fresnel
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

inline static vec3 reflect(const vec3& r_d,const vec3& n) {
	return r_d - 2.0 * dot(r_d, n) * n;
}

inline static vec3 refract(const vec3& r_d, const vec3& n,double eta_to_etap) {
	auto cos_theta = dot(-r_d, n);
	bool cant_refract = eta_to_etap * sqrt(1 - cos_theta * cos_theta) > 1 ;

	if (cant_refract || reflectance(cos_theta, eta_to_etap) > random_double()) {
		return reflect(r_d, n);
	}
	auto r_out_perp = eta_to_etap * (r_d + n * cos_theta);
	auto r_out_parallel = (-n) * sqrt(abs( 1.0 - r_out_perp.norm_squared()));
	return r_out_parallel + r_out_perp;
}


//common headers
#include "ppm.h"
#endif // !GLOBAL