#pragma once
#ifndef PPM_H
#define PPM_H

#include"vec3.h"
#include <iostream>
#include <string>

string write_color(std::ostream& out, color pixel_color,double sample_num = 1.0)
{
	//muiltisample
	pixel_color /= sample_num;
	//gamma
	pixel_color.e[0] = sqrt(pixel_color.e[0]);
	pixel_color.e[1] = sqrt(pixel_color.e[1]);
	pixel_color.e[2] = sqrt(pixel_color.e[2]);
	auto r = static_cast<int>(256 * clamp(pixel_color.x(), 0.0, 0.999));
	auto g = static_cast<int>(256 * clamp(pixel_color.y(), 0.0, 0.999));
	auto b = static_cast<int>(256 * clamp(pixel_color.z(), 0.0, 0.999));
//	out << r << ' ' << g << ' ' << b << '\n';
	string res = to_string(r)+ ' ' + to_string(g) + ' ' + to_string(b) + '\n';
	return res;
}

#endif // !PPM_H
