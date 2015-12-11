#include "Ray.h"

Ray::Ray(void)
{
}
Ray::Ray(Point3f _originPoint, Vector3f _dir)
{
	originPoint = _originPoint;
	dir = _dir;
	dir.normalize();
}

Ray::~Ray(void)
{
}
