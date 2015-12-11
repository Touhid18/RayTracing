#pragma once
#include "Shape.h"

class Triangle : public Shape
{
public:
	float a[3], b[3], c[3], refractiveIndex;
	Triangle();
	~Triangle();

	void toString();
};

