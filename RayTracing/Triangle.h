#pragma once
#include "Macros.h"

class Triangle : public Shape
{
public:
	float a[3], b[3], c[3], refractiveIndex;
	Triangle();
	~Triangle();

	void toString();
	double intersect(Ray r);
	Vector3f getNormalAt(Point3f p);
};

