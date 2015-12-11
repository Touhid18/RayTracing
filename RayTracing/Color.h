#include <GL/glut.h>

//should be called before calling any function
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals
	//glShadeModel(GL_SMOOTH); //Enable smooth shading
}
//color
void setAmbientColor(float r,float g,float b){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//~ glLoadIdentity();
	//~ glTranslatef(0.0f, 0.0f, -8.0f);
	GLfloat ambientColor[] = {r, g, b, 1.0f}; //Color white
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
}
//position and color
void setDiffuseSource(float px,float  py,float  pz,
					  float  r,float  g,float  b,
					  GLenum lightId){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//~ glLoadIdentity();
	GLfloat lightColor0[] = {r, g, b, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {px, py, pz, 1.0f}; //Positioned at (4, 0, 8)
    //~ glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    //~ glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightfv(lightId, GL_DIFFUSE, lightColor0);
    glLightfv(lightId, GL_POSITION, lightPos0);
    //this function can be called only once 
    //if second call is needed then GL_LIGHT1 will be needed
}


void setSpotLight(float lpx,float lpy,float lpz,float drx,float dry,float drz){
	//~ GLfloat dirVector0[]={ x, y, z, 0.0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	GLfloat qaDiffuseLight[]    = {1, 0, 0, 1.0};
    // Light source position
	GLfloat qaLightPosition[]    = {lpx, lpy, lpz, 1};
	GLfloat dirVector0[]={ drx, dry, drz, 0.0};
	
	glLightfv(GL_LIGHT1, GL_DIFFUSE, qaDiffuseLight);
    glLightfv(GL_LIGHT1, GL_POSITION, qaLightPosition);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);// set cutoff angle
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dirVector0); 
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 1); // 
	
}