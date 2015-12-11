#include "Macros.h"


Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}

void Sphere::toString() {
	printf("Sphere {\n");
	printf("\t center %.1f %.1f %.1f \n\t radius %.1f \n", center[0], center[1], center[2], radius);
	printf("\t color %.1f %.1f %.1f\n", baseColor[0], baseColor[1], baseColor[2]);
	printf("\t ambCoeff %.1f\n\t difCoeff %.1f\n\t refCoeff %.1f\n\t specCoeff %.1f\n\t specExp %.1f\n}\n",
		ambCoeff, difCoeff, refCoeff, specCoeff, specExp);
}
//bool Sphere::intersect(const Vector3f &rayorig, const Vector3f &raydir, float &t0, float &t1) const 
//{ 
//	Vector3f l(center[0]-rayorig.x, center[1]-rayorig.y, center[2]-rayorig.z); 
//    float tca = l.dot(raydir); 
//    if (tca < 0) return false; 
//    float d2 = l.dot(l) - tca * tca; 
//	float radius2 = radius * radius;
//
//    if (d2 > radius2) return false; 
//    float thc = sqrt(radius2 - d2); 
//    t0 = tca - thc; 
//    t1 = tca + thc; 
//
//    return true; 
//}

double Sphere::intersect(Ray r){
	Vector3f v(r.originPoint.x-center[0],
		r.originPoint.y-center[1],
		r.originPoint.z-center[2]);
	//~ v.normalize();
	double a = 1;
	double b = 2*v.dot(r.dir);
	double c = v.x*v.x+v.y*v.y+v.z*v.z - radius*radius;

	double d = b*b - 4*c*a;

	if(d<0)return -1;

	double root = (-b-sqrt(d))/(2*a);
	if(root<0)root = (-b+sqrt(d))/(2*a);
	if(root<0)return -1;
	return root;

}

Vector3f Sphere::getNormalAt(Point3f p){
	Vector3f tmp(p.x,p.y,p.z);
	Vector3f t2(center[0], center[1], center[2]);
	t2 = tmp-t2;
	t2.normalize();
	return t2;
}