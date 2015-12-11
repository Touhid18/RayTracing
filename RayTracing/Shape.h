#pragma once
class Shape
{
public:
	float ambCoeff, difCoeff, refCoeff, specCoeff, specExp;
	float baseColor[3];
	Shape();
	~Shape();

	void toString();
};

