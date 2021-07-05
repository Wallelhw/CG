#include "Sphere.h"

 bool Sphere::hit(const ray& r, double t_min, double t_max, intersection& inter) const {
	 vec3 distance = r.origin() - center;
	 
	 double a = r.direction().norm_squared();
	 double half_b = dot(2 * r.direction(), distance) / 2;
	 double c = distance.norm_squared() - radius * radius;

	 double discriminant_square = half_b * half_b - a * c;
	 if (discriminant_square < 0) return false;

	 double discriminant = std::sqrt(discriminant_square);
	 double t = (-half_b - discriminant) / a;

	 if (t<t_min || t>t_max) {
		 double t = (-half_b + discriminant) / a;
		 if (t<t_min || t>t_max) return false;
	 }

	 inter.t = t;
	 inter.p = r.at(t);
	 inter.normal = (inter.p - center)/radius;

	 return true;
}