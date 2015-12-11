#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<vector>
#include "CheckerBoard.h"
#include "Cylinder.h"
#include "Shape.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Point3f.h"
#include "Vector3f.h"
#include "ColorRGB.h"
#include "Ray.h"
#include<GL/glut.h>

#define BLACK 0, 0, 0
#define WHITE 1.0, 1.0, 1.0
#define RED 1.0, 0.0, 0.0
#define GREEN 0.0, 1.0, 0.0
#define BLUE 0.0, 0.0, 1.0

#define PI_VAL acos(-1.0)
#define INFINITY 1e8 
#define eps 1e-4
#define MIN_INT -2147483648
#define MAX_INT 2147483647

#define CHK_BRD_SZ 1000

#define minV(a, b) (a)<(b)? (a):(b)
#define maxV(a, b) (a)>(b)? (a):(b)