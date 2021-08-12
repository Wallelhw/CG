// HWRayTracer.cpp: 定义应用程序的入口点。
//
#include <iostream>
#include "global.h"
#include "HWRayTracer.h"

#include "Sphere.cpp"
#include "Sence.cpp"
#include "Camera.h"
#include "Materail.h"

using namespace std;

color ray_color( ray& r,Sence s,int max_depth) {
	if (max_depth < 1)return color(0.0);

	intersection inter = intersection();
	//befor hit scatter event
	//TODO


	if (s.hit(r, 0.001, INFINITY, inter)) {
		ray& incident_ray = ray();
		color& attenuation = color();
		if (inter.materail->scatter(r, inter, incident_ray, attenuation)) {
			
			return array_multiply(attenuation,ray_color(incident_ray,s,max_depth-1));
		}
		return color(0);
	}

	//auto t = 0.5 * r.direction().y() + 0.5;
	//color res = color(0.52 + t * 0.43, 0.91 + t * 0.07, 1);
	color res = color(1);
	return res;
}

int main()
{
	//for (int i = 20; i > 0; i--) {
	//	auto temp = random_in_unit_hemisphere(vec3(0, 0, 1));
	//	cout << temp << " " << dot(temp, vec3(0, 0, 1)) << endl;
	//}
	//_____________________________________render part____________________________________________//
	//Sence
	Sence sence = Sence();
	auto diffuse0 = LambertianDiffuse(color(0.8));
	auto mirror = Mirror(color(0.95));
	auto diffuse2 = LambertianDiffuse(color(0.2, 0.5, 0.5));
	auto diffuse3 = LambertianDiffuse(color(0.8, 0.5, 0.5));
	auto matel1 = Metal(color(1,0.8,1), 0.3);
	auto matel2 = Metal(color(1,1,0.8), 0.3);
	auto glass = Glass();


	Sphere s1 = Sphere(point3(0,0.5, -5), 1,make_shared<Mirror>(mirror));
	Sphere s2 = Sphere(point3(2, 0, -0.8), 0.5, make_shared<LambertianDiffuse>(diffuse2));
	Sphere s4 = Sphere(point3(0.8, 0, -1.5), 0.5, make_shared<Metal>(matel1));
	Sphere s5 = Sphere(point3(-0.7, 0, -1.1), 0.5, make_shared<Metal>(matel2));
	Sphere s6 = Sphere(point3(0.2, -0.1, -0.6), 0.4, make_shared<Glass>(glass));
	Sphere s3 = Sphere(point3(-2, 0, -1.2), 0.5, make_shared<LambertianDiffuse>(diffuse3));
	Sphere s0 = Sphere(point3(0, -100.5, -1), 100,make_shared<LambertianDiffuse>(diffuse0));
	sence.add(make_shared<Sphere>(s1));
	sence.add(make_shared<Sphere>(s2));
	sence.add(make_shared<Sphere>(s3));
	sence.add(make_shared<Sphere>(s4));
	sence.add(make_shared<Sphere>(s0));
	sence.add(make_shared<Sphere>(s5));
	sence.add(make_shared<Sphere>(s6));
	
	//Camera
	Camera camera = Camera(point3(-2,2,1),point3(0,0,-1));

	//Image
	const double aspect = camera.getaspect();
	const int image_width = 400;
	const int image_height =static_cast<int> (image_width / aspect);
	
	//Render
	int antialiasing_sample_nums = 20;
	int ray_trace_maxdepth = 10;

	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	for (int i = image_height ; i >= 0;i--) {
		std::cerr << "\rScanlines remaining: " << i << ' ' << std::flush;
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
