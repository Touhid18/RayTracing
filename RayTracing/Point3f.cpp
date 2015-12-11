#include "Point3f.h"

Point3f::Point3f(void)
{
	x = y = z = 0; 
}
Point3f::Point3f(float xx, float yy, float zz)
{
	x = xx;
	y = yy;
	z = zz;
}

Point3f::~Point3f(void)
{
}

void Point3f::set(double a, double b, double c){
	x=a;y=b;z=c;
}
void Point3f::set(Point3f a){
	x=a.x;
	y=a.y;
	z=a.z;
}
