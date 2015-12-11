#include "Macros.h"


CheckerBoard::CheckerBoard()
{
}

CheckerBoard::CheckerBoard(float baseColor[], float secColor[],
	float ambCoeff, float difCoeff, float refCoeff,
	float specCoeff, float specExp)
{
	this->baseColor[0] = baseColor[0];
	this->baseColor[1] = baseColor[1];
	this->baseColor[2] = baseColor[2];

	this->secColor[0] = secColor[0];
	this->secColor[1] = secColor[1];
	this->secColor[2] = secColor[2];

	this->ambCoeff = ambCoeff;
	this->difCoeff = difCoeff;
	this->refCoeff = refCoeff;
	this->specCoeff = specCoeff;
	this->specExp = specExp;
}


CheckerBoard::~CheckerBoard()
{
}

void CheckerBoard::toString() {
	printf("CheckerBoard {\n");
	printf("\t colorOne %.1f %.1f %.1f\n", baseColor[0], baseColor[1], baseColor[2]);
	printf("\t colorTwo %.1f %.1f %.1f\n", secColor[0], secColor[1], secColor[2]);
	printf("\t ambCoeff %.1f\n\t difCoeff %.1f\n\t refCoeff %.1f\n\t specCoeff %.1f\n\t specExp %.1f\n}\n",
		ambCoeff, difCoeff, refCoeff, specCoeff, specExp);
}

Vector3f CheckerBoard::getNormal(){
	return Vector3f(0,1,0);
}
double CheckerBoard::intersect(Ray r){
	Point3f o(0, 0, 0);
	Vector3f normal(getNormal());

	double d = (r.dir.dot(normal));
	if(d<eps)return -1;
	Vector3f pointOnPlane(o.x, o.y, o.z);
	Vector3f rayOrigin(r.originPoint.x,r.originPoint.y,r.originPoint.z);
	
	return -(pointOnPlane.dot(normal)-rayOrigin.dot(normal))/d;
}

ColorRGB CheckerBoard::getColorAt(Point3f p){
	int i = p.x / 10;
	int j = p.z / 10;
	int k = i+j;
	if(k<0) k *= -1;

	if(k%2 == 1)
		return ColorRGB(baseColor[0], baseColor[1], baseColor[2]);
	else 
		return ColorRGB(secColor[0], secColor[1], secColor[2]);
}