#include "stdafx.h"
#include "Sphere.h"


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