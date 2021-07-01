#pragma once
#ifndef PPM_H
#define PPM_H

#include"vec3.h"

#include<iostream>

void write_color(std::ostream& out, color pixel_color)
{
	out << static_cast<int>(255.999 * pixel_color.x()) << ' '
		<< static_cast<int>(255.999 * pixel_color.y()) << ' '
		<< static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

#endif // !PPM_H
