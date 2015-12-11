#pragma once
#include "Shape.h"
class Sphere : public Shape
{
public:
	float center[3];
	float radius;
	Sphere();
	~Sphere();

	void toString();
};

