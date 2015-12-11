// RayTracing.cpp : Defines the entry point for the console application.
//

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

using namespace std;

#include<GL/glut.h>
#include "RayTracing.h"

#define BLACK 0, 0, 0
#define PI_VAL acos(-1.0)

#define CHK_BRD_SZ 1000
//#define MAX_LT_SRC_CNT 3
//#define MAX_SPHERE_CNT 4
//#define MAX_TRIANG_CNT 1
//#define MAX_CYLLINDER_CNT 3
//#define MAX_CHKR_BRD_CNT 3

int recDepth, pixel;
int nObj;
vector<CheckerBoard> chBoards;
vector<Cylinder> cylinders;
vector<Sphere> spheres;
vector<Triangle> triangles;

double cameraAngle;	//in radian
double cameraAngleDelta;
double cameraHeight;
double cameraRadius;
double difPointLightAngle; // in radian

bool canDrawGrid;
GLUquadric* IDquadric = gluNewQuadric();

struct Point3D {
	float x, y, z;

	Point3D() { x = y = z = 0; }
	Point3D(float ax, float ay, float az) {
		x = ax; y = ay; z = az;
	}
};
Point3D camPos, camLookAt, camUpDir;
vector<Point3D> lightSources;

Point3D crossProduct(Point3D a, Point3D b) {
	Point3D p(a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x);
	return p;
}

Point3D diffVector(Point3D a, Point3D b) {
	Point3D p(a.x - b.x, a.y - b.y, a.z - b.z);
	return p;
}

void glFilledCircle(float x, float y, float r, int segments)
{
	float t;
	glBegin(GL_TRIANGLE_FAN); {
		glVertex3f(x, y - 25, -88);
		for (int n = 0; n <= segments; ++n) {
			t = 2 * PI_VAL * (float)n / (float)segments;
			glVertex3f(x + sin(t) * r, y + cos(t) * r - 25, -88);
		}
	}glEnd();
}

void drawRectangle(float blx, float bly, float blz, float length, float width) {
	glPushMatrix(); {
		glBegin(GL_QUADS); {
			// glColor3f(.9, .7, .85);
			glVertex3f(blx, bly, blz);
			glVertex3f(blx + length, bly, blz);
			glVertex3f(blx + length, bly + width, blz);
			glVertex3f(blx, bly + width, blz);
		}glEnd();
	}glPopMatrix();
}

void drawRectangleXZ(float blx, float bly, float blz, float length, float width) {
	glPushMatrix(); {
		glBegin(GL_QUADS); {
			// glColor3f(.9, .7, .85);
			glVertex3f(blx, bly, blz);
			glVertex3f(blx + length, bly, blz);
			glVertex3f(blx + length, bly, blz + width);
			glVertex3f(blx, bly, blz + width);
		}glEnd();
	}glPopMatrix();
}

void drawRectangle(float blx, float bly, float blz, float tRx, float tRy, float tRz) {
	glPushMatrix(); {
		glBegin(GL_QUADS); {
			glVertex3f(blx, bly, blz);
			glVertex3f(tRx, bly, blz);
			glVertex3f(tRx, tRy, blz);
			glVertex3f(blx, tRy, blz);
		}glEnd();
	}glPopMatrix();
}

void drawGridAndAxes() {

	//some gridlines along the field
	int i;

	//WILL draw grid IF the "canDrawGrid" is true:

	if (canDrawGrid == true) {
		glColor3f(0.3, 0.65, 0.72);
		glColor3f(0.3, 0.3, 0.3);	//grey
		glBegin(GL_LINES); {
			int csz = CHK_BRD_SZ / 10;
			for (i = -csz; i <= csz; i++) {

				if (i == 0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Z-axis
				glVertex3f(i * 10, 0, -CHK_BRD_SZ);
				glVertex3f(i * 10, 0, CHK_BRD_SZ);

				//lines parallel to X-axis
				glVertex3f(-CHK_BRD_SZ, 0, i * 10);
				glVertex3f(CHK_BRD_SZ, 0, i * 10);
			}
		}glEnd();
	}

	// draw the two AXES
	glColor3f(1, 1, 1);	//100% white
	glBegin(GL_LINES); {
		//X axis
		glColor3f(1, 0, 0);
		glVertex3f(-(CHK_BRD_SZ+100), 0, 0);
		glVertex3f(CHK_BRD_SZ+100, 0, 0);

		//Y axis
		glColor3f(0, 1, 0);
		glVertex3f(0, -(CHK_BRD_SZ+100), 0);	// intentionally extended to -450 to 450, no big deal
		glVertex3f(0, CHK_BRD_SZ + 100, 0);

		// Z axis
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, -(CHK_BRD_SZ + 100));
		glVertex3f(0, 0, (CHK_BRD_SZ + 100));

	}glEnd();
}

void setupCurCamView() {
	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(BLACK, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera is looking?
	//3. Which direction is the camera's UP direction?

	//instead of CONSTANT information, we will define a circular path.
	// gluLookAt(-150,-150,150,	0,0,0,	0,0,1);
	/*camPos.x = cameraRadius*cos(cameraAngle);
	camPos.y = cameraRadius*sin(cameraAngle);
	camPos.z = cameraHeight;
	camLookAt.x = -25;		camUpDir.x = 0;
	camLookAt.y = 5;		camUpDir.y = 0;
	camLookAt.z = 40;		camUpDir.z = 1;*/

	gluLookAt(camPos.x, camPos.y, camPos.z,
		camLookAt.x, camLookAt.y, camLookAt.z,
		camUpDir.x, camUpDir.y, camUpDir.z);
	glMatrixMode(GL_MODELVIEW);
}

void setupDiffuseLightEffect()
{
	// Moving Diffuse-Point Light Source
	GLfloat position[] = { -450.0*cos(difPointLightAngle),
		-450.0*sin(difPointLightAngle),
		500, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	GLfloat difPointLightColor[] = { 0.5, 0.5, 0.5, 1.0 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, difPointLightColor);

	GLfloat light_emission[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat unset[] = { 0,0,0,1 };
	glMaterialfv(GL_FRONT, GL_EMISSION, light_emission);
	glMaterialfv(GL_FRONT, GL_SPECULAR, unset);
	glPushMatrix();
	//glTranslatef (-10.0,-10.0,5);
	glTranslatef(-450.0*cos(difPointLightAngle), -450.0*sin(difPointLightAngle), 500);
	glutSolidSphere(10, 36, 36);
	glPopMatrix();

	//effect of local light source: directed diffuse light
	GLfloat diffuseDir[] = { 0.4, 0.4, 0.4, 1.0 }; //Color (0.5, 0.2, 0.2)
	GLfloat lightDir[] = { -1.0, 0.5, 0.5, 0.0 }; //Coming from the direction (-1, 0.5, 0.5)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseDir);
	glLightfv(GL_LIGHT0, GL_POSITION, lightDir);


	//effect of local light source: point diffuse light
	/**/
	// GLfloat position[] = {-10.0, -10.0, 5.0, 1.0}; Positioned at (-10, -10, 5)

	//object being illuminated
	GLfloat mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 }; //equal to the light source
	GLfloat low_shininess[] = { 5.0 };
	GLfloat mid_shininess[] = { 50.0 };
	GLfloat high_shininess[] = { 100.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, unset);
}

#pragma region Offline-specific custom-drawing functions

GLenum avLights[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, 
						GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
void showDifLights(){
	glColor3f(1.0, 1.0, 1.0);
	for (vector<Point3D>::size_type iv = 0; iv < lightSources.size(); iv++) {
		glPushMatrix(); {
			Point3D p(lightSources[iv]);
			
			GLfloat position[] = { p.x, p.y, p.z };
			glLightfv(avLights[iv], GL_POSITION, position);

			GLfloat difPointLightColor[] = { 0.5, 0.5, 0.5, 1.0 };
			glLightfv(avLights[iv], GL_DIFFUSE, difPointLightColor);

			glTranslatef(p.x, p.y, p.z);
			glutSolidSphere(1, 20, 20);
		}glPopMatrix();
	}
}

void drawCheckerBoard() {
	for (vector<CheckerBoard>::size_type iv = 0; iv < chBoards.size(); iv++) {
		// chBoards[iv];
		glPushMatrix(); {
			int csz = CHK_BRD_SZ;
			for (int i = -csz; i <= csz; i+=10) {
				for (int j = -csz; j <= csz; j+=10) {
					int k = (i + j) / 10;
					if(k<0) k *= -1;
					if (k%2==1)
						glColor3f( chBoards[iv].baseColor[0], 
							chBoards[iv].baseColor[1], 
							chBoards[iv].baseColor[2]);
					else
						glColor3f(chBoards[iv].secColor[0], 
							chBoards[iv].secColor[1], 
							chBoards[iv].secColor[2]);
					drawRectangleXZ(i, 0, j, 10, 10);
				}
			}
		}glPopMatrix();
	}
}

void drawSpheres(){
	for (vector<Sphere>::size_type iv = 0; iv < spheres.size(); iv++) {
		// spheres[iv];
		glPushMatrix(); {
			Sphere sp(spheres[iv]);
			glTranslatef(sp.center[0], sp.center[1], sp.center[2]);
			glColor3f(sp.baseColor[0], sp.baseColor[1], sp.baseColor[2]);
			glutSolidSphere(sp.radius, 20, 20);
		}glPopMatrix();
	}
}

void drawCylinder(){
	GLUquadricObj *qobj = gluNewQuadric();

	for (vector<Cylinder>::size_type iv = 0; iv < cylinders.size(); iv++) {
		glPushMatrix(); {
			Cylinder cy(cylinders[iv]);
			
			glTranslatef(cy.xCenter, (cy.yMin+cy.yMax)/2, cy.zCenter);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glColor3f(cy.baseColor[0],cy.baseColor[1],cy.baseColor[2]);

			gluQuadricNormals(qobj, GLU_SMOOTH);
			gluQuadricTexture(qobj, GLU_TRUE);
			gluCylinder(qobj, cy.radius, cy.radius, cy.yMax-cy.yMin, 20, 20);
		}glPopMatrix();
	}
}

void drawTriangle(){
	for (vector<Triangle>::size_type iv = 0; iv < triangles.size(); iv++) {
		glPushMatrix(); {
			Triangle tr(triangles[iv]);

			glColor3f(tr.baseColor[0], tr.baseColor[1], tr.baseColor[2]);
			glBegin(GL_TRIANGLES);{
				glVertex3f(tr.a[0], tr.a[1], tr.a[2]);
				glVertex3f(tr.b[0], tr.b[1], tr.b[2]);
				glVertex3f(tr.c[0], tr.c[1], tr.c[2]);
			}glEnd();
		}glPopMatrix();
	}
}

#pragma endregion


#pragma region Camera movements
void camStrafe(int dir) {
	Point3D alongVect = crossProduct(camUpDir, diffVector(camPos, camLookAt));
	alongVect.x *= .1;
	alongVect.y *= .1;
	alongVect.z *= .1;

	camPos.x += (dir* alongVect.x);
	camPos.y += (dir*  alongVect.y);
	camPos.z += (dir*  alongVect.z);

	camLookAt.x += (dir*  alongVect.x);
	camLookAt.y += (dir*  alongVect.y);
	camLookAt.z += (dir*  alongVect.z);
}

void camFly(int dir) {

	camPos.x += (2 * dir* camUpDir.x);
	camPos.y += (2 * dir*  camUpDir.y);
	camPos.z += (2 * dir*  camUpDir.z);

	camLookAt.x += (2 * dir*  camUpDir.x);
	camLookAt.y += (2 * dir*  camUpDir.y);
	camLookAt.z += (2 * dir*  camUpDir.z);
}

void camYawn(int dir) {
	Point3D d = diffVector(camPos, camLookAt);
	Point3D alongVect = crossProduct(camUpDir, d);

	float dx = camPos.x - camLookAt.x;
	float dy = camPos.y - camLookAt.y;
	float dz = camPos.z - camLookAt.z;
	float r = sqrt(dx*dx + dy*dy + dz*dz);
	float t = 5 / 180;
	camLookAt.x = (dir * (camPos.x
		+ r * cos(t) * d.x
		+ r * sin(t) * alongVect.x));
	camLookAt.y = (dir * (camPos.y
		+ r * cos(t) * d.y
		+ r * sin(t) * alongVect.y));
	camLookAt.z = (dir * (camPos.z
		+ r * cos(t) * d.z
		+ r * sin(t) * alongVect.z));
}
#pragma endregion

#pragma region Listeners
void keyboardListener(unsigned char key, int x, int y) {
	switch (key) {

	case '1':	// Pitch (looking up) 
				// cameraAngleDelta = -cameraAngleDelta;
		camLookAt.z += 10;
		break;

	case '2':	// Pitch (looking down) 
				// cameraAngleDelta *= 1.1;
		camLookAt.z -= 10;
		break;

	case '3':	//	Roll (twisting left)
				// cameraAngleDelta /= 1.1;
		camLookAt.x += 10;
		break;

	case '4': // Roll (twisting right)
		camLookAt.x -= 10;
		break;

	case '5': // Yaw (looking left) 
			  // camLookAt.y += 10;
		camYawn(1);
		break;

	case '6': // Yaw (looking right)
			  // camLookAt.y -= 10;
		camYawn(-1);
		break;

	case '9':
		canDrawGrid = !canDrawGrid;
		break;

	case 27:	//ESCAPE KEY -- simply exit
		exit(0);
		break;

	default:
		break;
	}
}

void specialKeyListener(int key, int x, int y) {
	switch (key) {

	case GLUT_KEY_UP: // Walk (forward w.r.t. current position)
					  /*if (cameraRadius > 10)
					  cameraRadius -= 10;*/
		camPos.x -= 10;
		camLookAt.x -= 10;
		break;

	case GLUT_KEY_DOWN: // Walk (backward w.r.t. current position)
						// cameraRadius += 10; // 
		camPos.x += 10;
		camLookAt.x += 10;
		break;

	case GLUT_KEY_LEFT: // Strafing (left side w.r.t. current position) 
						// cameraAngle -= (2 * cameraAngleDelta);
		camStrafe(-1);
		break;
	case GLUT_KEY_RIGHT: // Strafing (right side w.r.t. current position) 
						 // cameraAngle += (2 * cameraAngleDelta);
		camStrafe(1);
		break;

	case GLUT_KEY_PAGE_UP: // Fly (look up w.r.t. current position)
		camFly(1);
		break;
	case GLUT_KEY_PAGE_DOWN: // Fly (look down w.r.t. current position)
		camFly(-1);
		break;

	case GLUT_KEY_INSERT:
		break;

	case GLUT_KEY_HOME:
		break;
	case GLUT_KEY_END:
		break;

	case GLUT_KEY_F1:
		printf("Key F1");
		// glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_day);
		break;


	case GLUT_KEY_F2:
		difPointLightAngle += .04; // Diffuse-Point light : acw movement
		break;
	case GLUT_KEY_F3:
		difPointLightAngle -= .04; // Diffuse-Point light : cw movement
		break;
	case GLUT_KEY_F4: // Diffuse-Point light : Color (white/blue) chooser
					  // TODO
		/* if (difPointLightColor[0] > 0.1) // white -> blue
		{
			difPointLightColor[0] = 0.0;
			difPointLightColor[1] = 0.0;
			difPointLightColor[2] = 1.0;
		}
		else
		{
			difPointLightColor[0] = 0.5;
			difPointLightColor[1] = 0.5;
			difPointLightColor[2] = 0.5;
		}*/
		break;


	case GLUT_KEY_F5:
		glEnable(GL_LIGHT0); //Enable light #0
		break;
	case GLUT_KEY_F6:
		glDisable(GL_LIGHT0); //Disable light #0
		break;

	case GLUT_KEY_F7:
		glEnable(GL_LIGHT1); //Enable light #1
		break;
	case GLUT_KEY_F8:
		glDisable(GL_LIGHT1); //Disable light #1
		break;

	case GLUT_KEY_F9:
		glEnable(GL_LIGHT2); //Enable light #2
		// isSpotLightEanbled = true;
		break;
	case GLUT_KEY_F10:
		glDisable(GL_LIGHT2); //Disable light #2
		// isSpotLightEanbled = false;
		break;

	case GLUT_KEY_F11:
		break;
	case GLUT_KEY_F12:
		printf("Key F12");
		break;

	default:
		break;
	}
}

void mouseListener(int button, int state, int x, int y) {
	//x, y is the x-y of the screen (2D)
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			// TODO Render & generate image
		}
		break;

	case GLUT_RIGHT_BUTTON:
		//........
		break;

	case GLUT_MIDDLE_BUTTON:
		//........
		break;

	default:
		break;
	}
}
#pragma endregion

void display() {
	setupCurCamView();

	// setupDiffuseLightEffect();
	showDifLights();

	drawGridAndAxes();

	drawCheckerBoard();

	drawSpheres();

	drawCylinder();

	drawTriangle();

#pragma region Test-region
	/*glPushMatrix();
	{
	drawVolume(200, -200, 20, 30, 20, 40);
	}glPopMatrix();*/
#pragma endregion

	glutSwapBuffers();
}

void animate() {
	glutPostRedisplay();	//this will call the display AGAIN
}

#pragma region init
void init() {
	//codes for initialization
	//cameraAngle = -.45;	//// init the cameraAngle
	//cameraAngleDelta = 0.015;
	//canDrawGrid = true;
	// cameraHeight = 100;
	//cameraRadius = 75;
	canDrawGrid = true;
	camPos.x = 200;
	camPos.y = 100;
	camPos.z = 50;
	camLookAt.x = 0;		camUpDir.x = 0;
	camLookAt.y = 0;		camUpDir.y = 1;
	camLookAt.z = 0;		camUpDir.z = 0;

	//clear the screen
	glClearColor(BLACK, 0);

	//set the shading model
	glShadeModel(GL_SMOOTH);

	//Add global ambient light
	GLfloat ambient_day[] = { .6, .6, .6, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_day);

	setupDiffuseLightEffect();

	glEnable(GL_NORMALIZE); //Automatically normalize normals needed by the illumination model
	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_LIGHT2); //Enable light #2

	glEnable(GL_COLOR_MATERIAL);
	// glColorMaterial(GL_AMBIENT_AND_DIFFUSE);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(70, 1, 0.1, 10000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

								//ADD keyboard listener:
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	//ADD mouse listener:
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL
}
#pragma endregion

#pragma region  SDL-parsing codes
CheckerBoard cb;
void parseCheckerBoard(char objLines[15][30]) {
	char garbage[30];
	for (int i = 0; i < 15; i++) {
		if (objLines[i][0] == 'o' && objLines[i][3] == 'E')
			break;

		if (objLines[i][0] == 'c' && objLines[i][5] == 'O')
			sscanf(objLines[i], "%s %f %f %f", 
				&garbage, &cb.baseColor[0], &cb.baseColor[1], &cb.baseColor[2]);
		else if (objLines[i][0] == 'c' && objLines[i][5] == 'T')
			sscanf(objLines[i], "%s %f %f %f",
				&garbage, &cb.secColor[0], &cb.secColor[1], &cb.secColor[2]);
		else if (objLines[i][0] == 'a')
			sscanf(objLines[i], "%s %f", &garbage, &cb.ambCoeff);
		else if (objLines[i][0] == 'd')
			sscanf(objLines[i], "%s %f", &garbage, &cb.difCoeff);
		else if (objLines[i][0] == 'r')
			sscanf(objLines[i], "%s %f", &garbage, &cb.refCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'C')
			sscanf(objLines[i], "%s %f", &garbage, &cb.specCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'E')
			sscanf(objLines[i], "%s %f", &garbage, &cb.specExp);
	}
	cb.toString();
	// return cb;
}

Sphere parseSphere(char objLines[15][30]) {
	Sphere sp;
	char garbage[30];
	for (int i = 0; i < 15; i++) {
		if (objLines[i][0] == 'o' && objLines[i][3] == 'E')
			break;

		if (objLines[i][0] == 'c' && objLines[i][1] == 'o')
			sscanf(objLines[i], "%s %f %f %f",
				&garbage, &sp.baseColor[0], &sp.baseColor[1], &sp.baseColor[2]);
		else if (objLines[i][0] == 'c' && objLines[i][1] == 'e')
			sscanf(objLines[i], "%s %f %f %f",
				&garbage, &sp.center[0], &sp.center[1], &sp.center[2]);
		else if (objLines[i][0] == 'r' && objLines[i][1] == 'a')
			sscanf(objLines[i], "%s %f", &garbage, &sp.radius);
		else if (objLines[i][0] == 'a')
			sscanf(objLines[i], "%s %f", &garbage, &sp.ambCoeff);
		else if (objLines[i][0] == 'd')
			sscanf(objLines[i], "%s %f", &garbage, &sp.difCoeff);
		else if (objLines[i][0] == 'r')
			sscanf(objLines[i], "%s %f", &garbage, &sp.refCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'C')
			sscanf(objLines[i], "%s %f", &garbage, &sp.specCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'E')
			sscanf(objLines[i], "%s %f", &garbage, &sp.specExp);
	}
	sp.toString();
	return sp;
}

Triangle parseTriangle(char objLines[15][30]) {
	Triangle tr;
	char garbage[30];
	for (int i = 0; i < 15; i++) {
		if (objLines[i][0] == 'o' && objLines[i][3] == 'E')
			break;

		if (objLines[i][0] == 'c' && objLines[i][1] == 'o')
			sscanf(objLines[i], "%s %f %f %f",
				&garbage, &tr.baseColor[0], &tr.baseColor[1], &tr.baseColor[2]);
		else if (objLines[i][0] == 'a' && objLines[i][1] == ' ')
			sscanf(objLines[i], "%s %f %f %f", &garbage, &tr.a[0], &tr.a[1], &tr.a[2]);
		else if (objLines[i][0] == 'b' && objLines[i][1] == ' ')
			sscanf(objLines[i], "%s %f %f %f", &garbage, &tr.b[0], &tr.b[1], &tr.b[2]);
		else if (objLines[i][0] == 'c' && objLines[i][1] == ' ')
			sscanf(objLines[i], "%s %f %f %f", &garbage, &tr.c[0], &tr.c[1], &tr.c[2]);
		else if (objLines[i][0] == 'r' && objLines[i][3] == 'r')
			sscanf(objLines[i], "%s %f", &garbage, &tr.refractiveIndex);
		else if (objLines[i][0] == 'a')
			sscanf(objLines[i], "%s %f", &garbage, &tr.ambCoeff);
		else if (objLines[i][0] == 'd')
			sscanf(objLines[i], "%s %f", &garbage, &tr.difCoeff);
		else if (objLines[i][0] == 'r')
			sscanf(objLines[i], "%s %f", &garbage, &tr.refCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'C')
			sscanf(objLines[i], "%s %f", &garbage, &tr.specCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'E')
			sscanf(objLines[i], "%s %f", &garbage, &tr.specExp);
	}
	tr.toString();
	return tr;
}

Cylinder parseCylinder(char objLines[15][30]) {
	Cylinder cy;
	char garbage[30];
	for (int i = 0; i < 15; i++) {
		if (objLines[i][0] == 'o' && objLines[i][3] == 'E')
			break;

		if (objLines[i][0] == 'c' && objLines[i][1] == 'o')
			sscanf(objLines[i], "%s %f %f %f",
				&garbage, &cy.baseColor[0], &cy.baseColor[1], &cy.baseColor[2]);
		
		else if (objLines[i][0] == 'r' && objLines[i][1] == 'a')
			sscanf(objLines[i], "%s %f", &garbage, &cy.radius);
		
		else if (objLines[i][0] == 'x' && objLines[i][1] == 'C')
			sscanf(objLines[i], "%s %f", &garbage, &cy.xCenter);
		else if (objLines[i][0] == 'z' && objLines[i][1] == 'C')
			sscanf(objLines[i], "%s %f", &garbage, &cy.zCenter);
		else if (objLines[i][0] == 'y' && objLines[i][3] == 'n')
			sscanf(objLines[i], "%s %f", &garbage, &cy.yMin);
		else if (objLines[i][0] == 'y' && objLines[i][3] == 'x')
			sscanf(objLines[i], "%s %f", &garbage, &cy.yMax);

		else if (objLines[i][0] == 'a')
			sscanf(objLines[i], "%s %f", &garbage, &cy.ambCoeff);
		else if (objLines[i][0] == 'd')
			sscanf(objLines[i], "%s %f", &garbage, &cy.difCoeff);
		else if (objLines[i][0] == 'r')
			sscanf(objLines[i], "%s %f", &garbage, &cy.refCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'C')
			sscanf(objLines[i], "%s %f", &garbage, &cy.specCoeff);
		else if (objLines[i][0] == 's' && objLines[i][4] == 'E')
			sscanf(objLines[i], "%s %f", &garbage, &cy.specExp);
	}
	cy.toString();
	return cy;
}

Point3D parseLightSource(char* line){
	float x, y, z;
	char garbage[30];
	sscanf(line, "%s %f %f %f", &garbage, &x, &y, &z);
	return Point3D(x,y,z);
}

void parseSDLFile() {
	FILE *fp = fopen("Spec.txt", "r");
	char line[30];
	size_t read;

	char garbage[350];

	if (fp == NULL)
		exit(EXIT_FAILURE);

	
	while (fgets(line, 30, fp) != NULL) {
		if(strlen(line)<6)continue;
		// printf("%s", line);

		if (line[0] == 'r' && line[1] == 'e' && line[2] == 'c')
			sscanf(line, "%s %d", &garbage, &recDepth);
		else if(line[0]=='p')
			sscanf(line, "%s %d", &garbage, &pixel);
		else if (line[0] == 'l' && line[1] == 'i' && line[2] == 'g') {
			float x, y, z;
			sscanf(line, "%s %f %f %f", &garbage, &x, &y, &z);
			lightSources.push_back(Point3D(x, y, z));
		}
		else {
			// Parse light
			if(line[0]=='l')
				lightSources.push_back( parseLightSource(line) );
			// Parse an object
			char objLines[15][30];
			int lc = 0;
			while (fgets(objLines[lc], 30, fp) != NULL) {
				if (objLines[lc][0] == 'o' && objLines[lc][3] == 'E')
					break;
				lc++;
			}
			if (line[9] == 'S')
				spheres.push_back(parseSphere(objLines)); // TODO Parse Sphere
			else if (line[9] == 'C' && line[10]=='H'){
				parseCheckerBoard(objLines);
				chBoards.push_back( cb ); // TODO Parse CheckerBoard
			}
			else if (line[9] == 'T')
				triangles.push_back(parseTriangle(objLines)); // TODO Parse Triangle
			else if (line[9] == 'C')
				cylinders.push_back(parseCylinder(objLines)); // TODO Parse Cyllinder
		}
	}
	fclose(fp);

	printf("Rec. depth = %d, pixel = %d", recDepth, pixel);
}
#pragma endregion

int main(int argc, char **argv) {
	parseSDLFile();

	glutInit(&argc, argv);
	glutInitWindowSize(pixel, pixel);
	glutInitWindowPosition(0, 0);
	//Depth, Double buffer, RGB color
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	glutCreateWindow("CSE-410 Offline-3 : Ray Tracing");

	init();

	return 0;
}