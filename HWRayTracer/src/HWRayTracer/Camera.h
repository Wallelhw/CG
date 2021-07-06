#pragma once
#ifndef CAMERA
#define CAMERA

#include "global.h"

class Camera
{
private:
	point3 origin;
	double aspect;
	double focal_length;
	double viewport_height;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;

public:
	Camera(point3 _origin = vec3(0.0), double _aspect = 16.0/9.0,double _focal = 1.0,double _viewport_height = 2.0) {
		origin = _origin;
		aspect = _aspect;
		focal_length = _focal;
		viewport_height = _viewport_height;
		auto viewport_width = viewport_height * _aspect;
		horizontal = vec3(viewport_width, 0.0, 0.0);
		vertical = vec3(0.0, viewport_height, 0.0);
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
	}

	double getaspect() { return aspect; };

	ray get_ray(double u, double v) const{
		return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}
};
#endif // !CAMERA
