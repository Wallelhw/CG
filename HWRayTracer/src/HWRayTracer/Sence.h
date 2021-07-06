#pragma once
#ifndef SENCE

#include <vector>
#include "Object.h"

class Sence : public Object
{
public:
	std::vector<shared_ptr<Object>> objects;

public:
	Sence() {};
	Sence(std::vector<shared_ptr<Object>> _objects) :objects(_objects) {};
	
	void add(shared_ptr<Object> _object) { objects.push_back(_object); };
	void clear() { objects.clear(); };

	virtual bool hit(const ray& r, double t_min, double t_max, intersection& inter) const override;
};


#endif // !SENCE
