#pragma once
#ifndef CAMERA
#define CAMERA

#include "global.h"

class Camera
{
private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	double aspect;

public:
	Camera(	point3 _lookfrom = vec3(0.0),
			point3 _lookat = vec3(0, 0, -1),
			vec3 _vup = vec3(0, 1, 0), 
			double _aspect = 16.0 / 9.0,
			double fov = 90
		) {
		origin = _lookfrom;
		aspect = _aspect;
		auto theta = degree_to_radian(fov);
		auto viewport_height = tan(theta / 2) * 2;
		auto viewport_wight = viewport_height * aspect;

		auto w = -((_lookat - _lookfrom).normalize());
		auto u = cross(_vup, w).normalize();
		auto v = cross(w, u).normalize();

		vertical = v * viewport_height;
		horizontal = u * viewport_wight;
		lower_left_corner = origin - vertical / 2 - horizontal / 2 - w;
	}

	double getaspect() { return aspect; };

	ray get_ray(double s, double t) const{
		return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
	}
};
#endif // !CAMERA
