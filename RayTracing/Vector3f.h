#pragma once

class Vector3f
{
public:
	double x, y, z;

    Vector3f(double a, double b, double c);
	Vector3f(void);
	~Vector3f(void);

	double dot(Vector3f a);
	Vector3f cross(Vector3f a);
	void normalize();

	double mod();
	Vector3f operator+(Vector3f v);
	Vector3f operator-(Vector3f v);

	Vector3f mult(double val);
	void set(double a, double b, double c);
	void set(Vector3f a);
	Vector3f operator-();

};
