#include "ColorRGB.h"
#include "Macros.h"

ColorRGB::ColorRGB(void)
{
	r = g = b = 0;
}

ColorRGB::ColorRGB(double _r, double _g, double _b){
	r = _r;
	g = _g;
	b = _b;
}

ColorRGB::~ColorRGB(void)
{
}

ColorRGB ColorRGB::clip(){
	double tot = r+g+b;
	double exces = tot-3;
	if(exces>0){
		r += exces*r/tot;
		g += exces*g/tot;
		b += exces*b/tot;
	}
	r = minV(1.0, maxV(0.0,r));
	g = minV(1.0, maxV(0.0,g));
	b = minV(1.0, maxV(0.0,b));
	return ColorRGB(r,g,b);
}

ColorRGB ColorRGB::to256Range(){
	/*if(r>2.0 || g>2.0 || b>2.0)
		return *this;*/
	return ColorRGB( (int)(r*255), (int)(g*255), (int)(b*255) );
}

ColorRGB ColorRGB::to1Range(){
	return ColorRGB( (r/255.0), (g/255.0), (b/255.0) );
}
