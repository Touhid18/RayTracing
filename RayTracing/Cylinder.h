#pragma once
#include "Shape.h"

class Cylinder : public Shape
{
public:
	int type;
	float xCenter, zCenter, radius, yMin, yMax;
	Cylinder();
	~Cylinder();

	void toString();
};

