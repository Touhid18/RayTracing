#include "stdafx.h"
#include "CheckerBoard.h"


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
