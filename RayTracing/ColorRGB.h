#pragma once

class ColorRGB
{
public:
	double r, g, b;

	ColorRGB(double _r, double _g, double _b);
	ColorRGB(void);
	~ColorRGB(void);

	ColorRGB clip();
	ColorRGB to256Range();
	ColorRGB to1Range();
};
