#pragma once
#ifndef GLOBAL
#define GLOBAL

#include <cstdlib>

const double pi = 3.1415926535897932385;

inline double degree_to_radian(double degree) {
	return degree / 180 * pi;
}

//return random double in[0,1)
inline double random_double() {
	return rand()/ (RAND_MAX+1.0);
}

inline double random_double(double min, double max) {
	return min + random_double() * (max - min);
}

inline double clamp(double value, double min, double max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

//common headers

#include "ppm.h"
#include "ray.h"
#include "vec3.h"

#endif // !GLOBAL