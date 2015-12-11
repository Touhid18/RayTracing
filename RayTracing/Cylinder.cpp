#include "stdafx.h"
#include "Cylinder.h"


Cylinder::Cylinder()
{
}


Cylinder::~Cylinder()
{
}

void Cylinder::toString() {
	printf("Cylinder {\n");
	printf("\t xCenter %.1f \n\t zCenter %.1f \n\t radius %.1f \n\t yMin %.1f \n\t yMax %.1f \n\t",
		xCenter, zCenter, radius, yMin, yMax);
	printf("\t color %.1f %.1f %.1f\n", baseColor[0], baseColor[1], baseColor[2]);
	printf("\t ambCoeff %.1f\n\t difCoeff %.1f\n\t refCoeff %.1f\n\t specCoeff %.1f\n\t specExp %.1f\n}\n",
		ambCoeff, difCoeff, refCoeff, specCoeff, specExp);
}
