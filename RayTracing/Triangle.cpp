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