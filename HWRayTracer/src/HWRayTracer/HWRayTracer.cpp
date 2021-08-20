// HWRayTracer.cpp: 定义应用程序的入口点。
//

#include "HWRayTracer.h"

using namespace std;

color ray_color( ray& r,Sence s,int max_depth) {
	if (max_depth < 1)return color(0.0);

	intersection inter = intersection();

	if (s.hit(r, 0.001, INFINITY, inter)) {
		ray& incident_ray = ray();
		color& attenuation = color();
		if (inter.materail->scatter(r, inter, incident_ray, attenuation)) {
			
			return array_multiply(attenuation,ray_color(incident_ray,s,max_depth-1));
		}
		return color(0);
	}

	auto t = 0.5 * r.direction().y() + 0.5;
	color res = color(0.52 + t * 0.43, 0.91 + t * 0.07, 1);
	//color res = color(1);
	return res;
}

int main()
{	
	//_____________________________________render part____________________________________________//
	//Sence
	sence = Sence();
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

	GetSystemInfo(&sysInfo);
	int cores_num = sysInfo.dwNumberOfProcessors;
	//Camera
	 camera = Camera(point3(-2,2,1),point3(0,0,-1));

	//Image
	const double aspect = camera.getaspect();
	image_width = 400;
	image_height =static_cast<int> (image_width / aspect);
	remains_lines = image_height;
	
	//Render
	antialiasing_sample_nums = 100;
	ray_trace_maxdepth = 50;

	out_ppm_list = vector<string>(cores_num);
	vector<thread> thread_list(cores_num);
	out_ppm_list[0].append("P3\n" + to_string(image_width) + " " + to_string(image_height) + "\n255\n");

	//Show process
	auto t_log = thread(Showprogress);

	//Mult_thread 
	int range = ceil((double)image_height / cores_num);
	for (int i = 0; i < cores_num; i++) {
		int range_index = cores_num - i;
		int start_height = (range * range_index) > image_height ? image_height : (range * range_index) - 1;
		int end_height = range * (range_index - 1);
		cout << start_height << "  " << end_height << endl;
		thread_list[i] = thread(RenderThread,start_height,end_height,i);
	}

	for (auto& t : thread_list) {
		t.join();
	}
	for (auto& str : out_ppm_list) {
		cout << str;
	}

	system("pause");
}

int RenderThread(int start_height,int end_height, int thread_id)
{
	for (int i = start_height; i >= end_height; i--) {
		for (int j = 0; j < image_width; j++) {
			color pixel_color = color(0.0);
			for (int s = 0; s < antialiasing_sample_nums; s++) {
				auto u = double(j + random_double()) / double(image_width - 1.0);
				auto v = double(i - random_double()) / double(image_height);
				ray r = camera.get_ray(u, v);
				pixel_color += ray_color(r, sence, ray_trace_maxdepth);
			}
			string value = write_color(std::cout, pixel_color, antialiasing_sample_nums);
			out_ppm_list[thread_id].append(value);
		}
		remains_lines--;
	}
	return 0;
}

void Showprogress(){
	while (remains_lines >= 0) {
		cerr << "\n Total remains_lines is:" << remains_lines << flush;
		Sleep(500);
	}
}