#include "Point3f.h"
#include "Vector3f.h"

#pragma once

class Ray
{
public:
	Point3f originPoint;
	Vector3f dir;

	Ray(Point3f _originPoint, Vector3f _dir);
	Ray(void);
	~Ray(void);
};
