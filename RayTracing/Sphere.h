#pragma once
#include "Shape.h"
#include "Vector3f.h"
#include "Macros.h"

class Sphere : public Shape
{
public:
	float center[3];
	float radius;
	Sphere();
	~Sphere();

	void toString();

	//bool intersect(const Vector3f &rayorig, const Vector3f &raydir, float &t0, float &t1) const;
	double intersect(Ray r);
	Vector3f getNormalAt(Point3f p);
};

