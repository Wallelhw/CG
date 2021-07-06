// HWRayTracer.cpp: 定义应用程序的入口点。
//
#include <iostream>
#include "global.h"
#include "HWRayTracer.h"

#include "Sphere.cpp"
#include "Sence.cpp"
#include "Camera.h"

using namespace std;

color ray_color( ray& r,Sence s,int max_depth) {
	if (max_depth < 1)return color(0.0);

	intersection inter = intersection();
	if (s.hit(r, 0.001, INFINITY, inter)) {
		point3 target = inter.p + inter.normal + random_in_unit_sphere();
		return 0.5 * ray_color(ray(inter.p, target - inter.p), s, max_depth - 1);
	}

	auto t = 0.5 * r.direction().y() + 0.5;
	color res = color(0.52+t*0.43,0.91+t*0.07,1);
	return res;
}

int main()
{
	//for (int i = 20; i > 0; i--) {
	//	cout << random_in_unit_sphere().norm() << endl;
	//}
	
	//_____________________________________render part____________________________________________//
	//Sence
	Sence sence = Sence();
	Sphere s1 = Sphere(point3(0, 0, -1), 0.5);
	Sphere s2 = Sphere(point3(1, 0, -1), 0.5);
	Sphere s3 = Sphere(point3(-1, 0, -1), 0.5);
	Sphere s0 = Sphere(point3(0, -100.5, -1), 100);
	sence.add(make_shared<Sphere>(s1));
	sence.add(make_shared<Sphere>(s2));
	sence.add(make_shared<Sphere>(s3));
	sence.add(make_shared<Sphere>(s0));

	//Camera
	Camera camera = Camera();

	//Image
	const double aspect = camera.getaspect();
	const int image_width = 400;
	const int image_height =static_cast<int> (image_width / aspect);
	
	//Render
	int antialiasing_sample_nums = 100;
	int ray_trace_maxdepth = 50;

	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	for (int i = image_height; i >= 0;i--) {
		std::cerr << "\rScanlines remaining: " << image_height - i << ' ' << std::flush;
		for (int j = 0; j < image_width; j++) {
			color pixel_color = color(0.0);
			for (int s = 0; s < antialiasing_sample_nums; s++) {
				auto u = double(j + random_double()) / double(image_width - 1.0);
				auto v = double(i - random_double()) / double(image_height);
				ray r = camera.get_ray(u,v);
			    pixel_color += ray_color(r, sence,ray_trace_maxdepth);
			}
			write_color(std::cout,pixel_color,antialiasing_sample_nums);
		}
	}
	std::cerr << "Done!\n";
	//___________________________________________________________________________________________________//
}
