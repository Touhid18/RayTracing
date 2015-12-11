#pragma once

class Point3f
{
public:
	float x, y, z;
	Point3f(void);
	Point3f(float xx, float yy, float zz);
	~Point3f(void);

	void set(double a, double b, double c);
	void set(Point3f a);

};
