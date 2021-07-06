#include "Sence.h"

bool Sence::hit(const ray& r, double t_min, double t_max, intersection& inter) const
{
	intersection temp;
	bool hit_anything = false;
	double closet_t = t_max;

	for (const auto& object : objects) {
		if (object->hit(r, t_min, closet_t, temp)) {
			hit_anything = true;
			closet_t = temp.t;
			inter = temp;
		}
	}
	return hit_anything;
}
