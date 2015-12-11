#include "Macros.h"

Vector3f::Vector3f(double a, double b, double c){
	x=a;
	y=b;
	z=c;
}
Vector3f::Vector3f(void)
{
}

Vector3f::~Vector3f(void)
{
}

double Vector3f::dot(Vector3f a)
{
    return (x*a.x + y*a.y + z*a.z);
}
Vector3f Vector3f::cross(Vector3f a)
{
    a.set(y*a.z-z*a.y,z*a.x-x*a.z,x*a.y-y*a.x);
    return a;
}
void Vector3f::normalize()
{
    double d=sqrt(x*x+y*y+z*z);
    this->set(x/d,y/d,z/d);
}

	
double Vector3f::mod(){
	return sqrt(x*x+y*y+z*z);
}

Vector3f Vector3f::operator+(Vector3f v){
	return Vector3f(x+v.x,y+v.y,z+v.z);
}

Vector3f Vector3f::operator-(Vector3f v){
	return Vector3f(x-v.x,y-v.y,z-v.z);
}

Vector3f Vector3f::mult(double val){
	return Vector3f(x*val,y*val,z*val);
}

void Vector3f::set(double a, double b, double c){
	x=a;
	y=b;
	z=c;
}
void Vector3f::set(Vector3f a){
	x=a.x;
	y=a.y;
	z=a.z;
}
Vector3f Vector3f::operator-(){
	Vector3f a(-x,-y,-z);
	return a;
}