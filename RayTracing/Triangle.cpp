#include "stdafx.h"
#include "Triangle.h"


Triangle::Triangle()
{
}


Triangle::~Triangle()
{
}

void Triangle::toString() {
	printf("Triangle {\n");
	printf("\t a %.1f %.1f %.1f\n", a[0], a[1], a[2]);
	printf("\t b %.1f %.1f %.1f\n", b[0], b[1], b[2]);
	printf("\t c %.1f %.1f %.1f\n", c[0], c[1], c[2]);
	printf("\t color %.1f %.1f %.1f\n", baseColor[0], baseColor[1], baseColor[2]);
	printf("\t ambCoeff %.1f\n\t difCoeff %.1f\n\t refCoeff %.1f\n\t specCoeff %.1f\n\t specExp %.1f\n\t refractiveIndex %.1f\n}\n",
		ambCoeff, difCoeff, refCoeff, specCoeff, specExp, refractiveIndex);
}

double Triangle::intersect(Ray r){
	/*Vector3f v(r.originPoint.x-center[0],
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
	return root;*/
	Vector3f ab( a[0]-b[0], a[1]-b[1], a[2]-b[2] );
	Vector3f ac( a[0]-c[0], a[1]-c[1], a[2]-b[2] );
	Vector3f n( ab.cross(ac) );
	Vector3f po(a[0],a[01],a[2]);
	Vector3f org( r.originPoint.x, r.originPoint.y, r.originPoint.z );

	double t = ( po.dot(n) - org.dot(n) ) / r.dir.dot(n);
	return t;

}

Vector3f Triangle::getNormalAt(Point3f p){
	Vector3f av(p.x-a[0], p.y-a[1], p.z-a[2]);
	Vector3f bv(p.x-b[0], p.y-b[1], p.z-b[2]);
	Vector3f tn(av.cross(bv));
	tn.normalize();
	return tn;
}