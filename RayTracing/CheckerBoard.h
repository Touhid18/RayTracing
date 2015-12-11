#pragma once
#include "Shape.h"
class CheckerBoard : public Shape
{
public:
	float secColor[];
	CheckerBoard();
	CheckerBoard(float baseColor[], float secColor[], float ambCoeff, float difCoeff,
		float refCoeff, float specCoeff, float specExp);
	~CheckerBoard();

	void toString();
};

