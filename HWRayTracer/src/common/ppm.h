#pragma once
#ifndef PPM_H
#define PPM_H

#include"vec3.h"

#include<iostream>

void write_color(std::ostream& out, color pixel_color,double sample_num = 1.0)
{
	//muiltisample
	pixel_color /= sample_num;
	//gamma
	pixel_color.e[0] = sqrt(pixel_color.e[0]);
	pixel_color.e[1] = sqrt(pixel_color.e[1]);
	pixel_color.e[2] = sqrt(pixel_color.e[2]);
	out << static_cast<int>(256 * clamp(pixel_color.x(), 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(pixel_color.y(), 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(pixel_color.z(), 0.0, 0.999)) << '\n';
}

#endif // !PPM_H
