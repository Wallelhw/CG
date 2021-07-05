﻿// HWRayTracer.cpp: 定义应用程序的入口点。
//
#include <iostream>
#include "HWRayTracer.h"
#include "vec3.h"
#include "ppm.h"
#include "ray.h"
#include "Sphere.cpp"

using namespace std;

color ray_color( ray& r) {  
	auto t = 0.5 * r.direction().y() + 0.5;
	color res = color(0.52+t*0.43,0.91+t*0.07,1);

	Sphere s1 = Sphere(point3(0, 0, -1), 0.5);
	intersection inter = intersection();
	s1.hit(r, 0, INFINITY, inter);

	if (s1.hit(r, 0, INFINITY, inter)) {
		res = 0.5 * color(1 + inter.normal.x(), 1 + inter.normal.y(), 1 + inter.normal.z());
	}
	return res;
}

int main()
{
	//
	//_____________________________________render part____________________________________________//
	//Viewport
	const double aspect_ratio = 16.0 / 9.0;
	const double focal_length = 1.0;

	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0,viewport_height,0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0,0,focal_length);

	////Image
	const int image_width = 1920;
	const int image_height =static_cast<int> (image_width / aspect_ratio);

	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	for (int i = image_height; i >= 0;i--) {
		std::cerr << "\rScanlines remaining: " << image_height - i << ' ' << std::flush;
		for (int j = 0; j < image_width; j++) {
			auto pixel_point = lower_left_corner + double(i) / (image_height - 1) * vertical + double(j) / (image_width - 1) * horizontal;
			ray r = ray(origin, pixel_point - origin);
			auto pixel_color = ray_color(r);
			write_color(std::cout,pixel_color);
		}
	}

	std::cerr << "Done!\n";
	//___________________________________________________________________________________________________//
}
