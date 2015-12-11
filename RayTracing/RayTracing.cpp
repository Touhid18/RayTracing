// RayTracing.cpp : Defines the entry point for the console application.
//
#include "RayTracing.h"
#include "Macros.h"
#include "bitmap_image.hpp"
using namespace std;

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

Point3f camPos, camLookAt, camUpDir;
vector<Point3f> lightSources;

Point3f crossProduct(Point3f a, Point3f b) {
	Point3f p(a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x);
	return p;
}

Point3f diffVector(Point3f a, Point3f b) {
	Point3f p(a.x - b.x, a.y - b.y, a.z - b.z);
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



}

#pragma region Helper-methods
/*template<typename T> class Vec_3 
{ 
public: 
    T x, y, z; 
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {} 
    Vec3(T xx) : x(xx), y(xx), z(xx) {} 
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {} 

    Vec3& normalize() 
    { 
        T nor2 = length2(); 
        if (nor2 > 0) { 
            T invNor = 1 / sqrt(nor2); 
            x *= invNor, y *= invNor, z *= invNor; 
        } 
        return *this; 
    } 
    Vec3<T> operator * (const T &f) const { 
		return Vec3<T>(x * f, y * f, z * f); 
	} 
    Vec3<T> operator * (const Vec3<T> &v) const { 
		return Vec3<T>(x * v.x, y * v.y, z * v.z); 
	} 
    T dot(const Vec3<T> &v) const {
		return x * v.x + y * v.y + z * v.z; 
	} 
    Vec3<T> operator - (const Vec3<T> &v) const {
		return Vec3<T>(x - v.x, y - v.y, z - v.z); 
	} 
    Vec3<T> operator + (const Vec3<T> &v) const { 
		return Vec3<T>(x + v.x, y + v.y, z + v.z); 
	} 
    Vec3<T>& operator += (const Vec3<T> &v) { 
		x += v.x, y += v.y, z += v.z; return *this; 
	} 
    Vec3<T>& operator *= (const Vec3<T> &v) { 
		x *= v.x, y *= v.y, z *= v.z; 
		return *this; 
	} 
    Vec3<T> operator - () const { 
		return Vec3<T>(-x, -y, -z); 
	} 
    T length2() const { 
		return x * x + y * y + z * z; 
	} 
    T length() const { 
		return sqrt(length2()); 
	} 
}; 
typedef Vec3<float> Vec3f; 

class SphereR : public Sphere{
	
    Vec3f center2;                           /// position of the sphere 
    float radius2;                  /// sphere radius and radius^2 
    Vec3f surfaceColor, emissionColor;      /// surface color and emission (light) 
    float transparency, reflection;         /// surface transparency and reflectivity 
    Sphere( 
        const Vec3f &c, 
        const float &r, 
        const Vec3f &sc, 
        const float &refl = 0, 
        const float &transp = 0, 
        const Vec3f &ec = 0) : 
        center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec), 
        transparency(transp), reflection(refl) 
    {   }

	bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const 
    { 
        Vec3f l = center - rayorig; 
        float tca = l.dot(raydir); 
        if (tca < 0) return false; 
        float d2 = l.dot(l) - tca * tca; 
        if (d2 > radius2) return false; 
        float thc = sqrt(radius2 - d2); 
        t0 = tca - thc; 
        t1 = tca + thc; 
 
        return true; 
    } 
}

Vec3f traceRayOnSpheres( 
    const Vec3f &rayorig, const Vec3f &raydir, const vector<Sphere> &spheres, 
    const int &depth) 
{ 
    float tnear = INFINITY; 
    const Sphere* sphere = NULL; 
    // find intersection of this ray with the sphere in the scene
    for (unsigned i = 0; i < spheres.size(); ++i) { 
        float t0 = INFINITY, t1 = INFINITY; 
        if (spheres[i].intersect(rayorig, raydir, t0, t1)) { 
            if (t0 < 0) t0 = t1; 
            if (t0 < tnear) { 
                tnear = t0; 
                sphere = &spheres[i]; 
            } 
        } 
    } 
    // if there's no intersection return black or background color
    if (!sphere) return Vec3f(2); 
    Vec3f surfaceColor = 0; // color of the ray/surface of the object intersected by the ray 
    Vec3f phit = rayorig + raydir * tnear; // point of intersection 
    Vec3f nhit = phit - sphere->center; // normal at the intersection point 
    nhit.normalize(); // normalize normal direction 
    // If the normal and the view direction are not opposite to each other
    // reverse the normal direction. That also means we are inside the sphere so set
    // the inside bool to true. Finally reverse the sign of IdotN which we want
    // positive.
    float bias = 1e-4; // add some bias to the point from which we will be tracing 
    bool inside = false; 
    if (raydir.dot(nhit) > 0) nhit = -nhit, inside = true; 
    if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < recDepth) { 
        float facingratio = -raydir.dot(nhit); 
        // change the mix value to tweak the effect
        float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1); 
        // compute reflection direction (not need to normalize because all vectors
        // are already normalized)
        Vec3f refldir = raydir - nhit * 2 * raydir.dot(nhit); 
        refldir.normalize(); 
        Vec3f reflection = trace(phit + nhit * bias, refldir, spheres, depth + 1); 
        Vec3f refraction = 0; 
        // if the sphere is also transparent compute refraction ray (transmission)
        if (sphere->transparency) { 
            float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface? 
            float cosi = -nhit.dot(raydir); 
            float k = 1 - eta * eta * (1 - cosi * cosi); 
            Vec3f refrdir = raydir * eta + nhit * (eta *  cosi - sqrt(k)); 
            refrdir.normalize(); 
            refraction = 1.0; //trace(phit - nhit * bias, refrdir, spheres, depth + 1); 
        } 
        // the result is a mix of reflection and refraction (if the sphere is transparent)
        surfaceColor = ( 
            reflection * fresneleffect + 
            refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor; 
    } 
    else { 
        // it's a diffuse object, no need to raytrace any further
        for (unsigned i = 0; i < spheres.size(); ++i) { 
            if (spheres[i].emissionColor.x > 0) { 
                // this is a light
                Vec3f transmission = 1; 
                Vec3f lightDirection = spheres[i].center - phit; 
                lightDirection.normalize(); 
                for (unsigned j = 0; j < spheres.size(); ++j) { 
                    if (i != j) { 
                        float t0, t1; 
                        if (spheres[j].intersect(phit + nhit * bias, lightDirection, t0, t1)) { 
                            transmission = 0; 
                            break; 
                        } 
                    } 
                } 
                surfaceColor += sphere->surfaceColor * transmission * 
                std::max(float(0), nhit.dot(lightDirection)) * spheres[i].emissionColor; 
            } 
        } 
    } 
 
    return surfaceColor + sphere->emissionColor; 
}*/ 
#pragma endregion

#pragma region Offline-specific custom-drawing functions

GLenum avLights[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, 
						GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
void showDifLights(){
	glColor3f(1.0, 1.0, 1.0);
	for (vector<Point3f>::size_type iv = 0; iv < lightSources.size(); iv++) {
		glPushMatrix(); {
			Point3f p(lightSources[iv]);
			
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

#pragma region Tracing codes

ColorRGB traceScene(Ray r, Point3f p, int depth){
	if(depth>=recDepth)
		return ColorRGB(0,0,0);
	
	double mn= MAX_INT;
	int indx;
	int type=0;

	for (vector<CheckerBoard>::size_type ic = 0; ic < chBoards.size(); ic++) {
		double dist = chBoards[ic].intersect(r);
		if(dist<0)continue;
		if(dist<mn){
			mn = dist;
			indx = ic;
			type = 1; // CheckerBoard
		}
	}
	for (vector<Sphere>::size_type is = 0; is < spheres.size(); is++) {
		double dist = spheres[is].intersect(r);
		if(dist<0)continue;
		if(dist<mn){
			mn = dist;
			indx = is;
			type = 2; // Sphere
		}
	}
	/*for (vector<Triangle>::size_type it = 0; it < triangles.size(); it++) {
		double dist = triangles[it].intersect(r);
		if(dist<0)continue;
		if(dist<mn){
			mn = dist;
			indx = it;
			type = 3; // Triangle
		}
	}
	for (vector<Cylinder>::size_type iy = 0; iy < cylinders.size(); iy++) {
		double dist = cylinders[iy].intersect(r);
		if(dist<0)continue;
		if(dist<mn){
			mn = dist;
			indx = iy;
			type = 4; // Cylinder
		}
	}*/
	Vector3f origin(r.originPoint.x, r.originPoint.y, r.originPoint.z);

	if(type==1)		// the ray hits the checker-board
    {
		double dist = mn;
		CheckerBoard ground(chBoards[indx]);

		Vector3f v = origin+r.dir.mult(dist); //intersection point
		Point3f p(v.x,v.y,v.z);
		Vector3f normal = ground.getNormal();

		ColorRGB col = ground.getColorAt(p).clip().to256Range();
		// printf("\n\nGround color: %f %f %f\n\n", col.r, col.g, col.b);
		if(col.r<100 && col.g<100 && col.b>200){
			printf("CheckerBoard: how come!! RGB=%f,%f,%f :( :(\n", col.r, col.g, col.b);
		}

		col.r *= ground.ambCoeff;
		col.g *= ground.ambCoeff;
		col.b *= ground.ambCoeff;

		Vector3f ci,si,reflected,ct,st,refracted;
		ci = normal.mult((-r.dir).dot(normal));
		si = ci+r.dir;

		reflected = ci+si;

		Ray reflect_ray(p,reflected);

		ColorRGB reflect_col= traceScene(reflect_ray,p,depth+1);

		col.r += reflect_col.r;
		col.g += reflect_col.g;
		col.b += reflect_col.b;

		for (vector<Point3f>::size_type i = 0; i < lightSources.size(); i++) {
			Vector3f lm = Vector3f(lightSources[i].x - p.x,
				lightSources[i].y - p.y,
				lightSources[i].z - p.z);
			Vector3f n = normal;
			Vector3f rm = n.mult(2*(lm.dot(n)))-lm;
			Vector3f v = r.dir;

			lm.normalize();
			n.normalize();
			rm.normalize();
			v.normalize();

			col.r += ground.difCoeff*lm.dot(n) 
				+ ground.specCoeff*(pow((double)rm.dot(v),(double)ground.specExp));
			col.g += ground.difCoeff*lm.dot(n) 
				+ ground.specCoeff*(pow((double)rm.dot(v),(double)ground.specExp));
			col.b += ground.difCoeff*lm.dot(n) 
				+ ground.specCoeff*(pow((double)rm.dot(v),(double)ground.specExp));

		}
		col.r = maxV(0.0,minV(col.r,1.0));
		col.g = maxV(0.0,minV(col.g,1.0));
		col.b = maxV(0.0,minV(col.b,1.0));
		return col;
	}
	else if(type==2){//hits a sphere
		Sphere sp(spheres[indx]);
		Vector3f v = origin+r.dir.mult(mn); //intersection point
		Point3f p(v.x,v.y,v.z);
		Vector3f normal = sp.getNormalAt(p);

		ColorRGB col(sp.baseColor[0], sp.baseColor[1], sp.baseColor[2]);
		// return col;

		col.r *= sp.ambCoeff;
		col.g *= sp.ambCoeff;
		col.b *= sp.ambCoeff;

		Vector3f ci,si,reflected,ct,st,refracted;
		ci = normal.mult((-r.dir).dot(normal));
		si = ci+r.dir;

		reflected = ci+si;
		reflected.normalize();
		Ray reflect_ray(p,reflected);

		ColorRGB reflect_col = traceScene(reflect_ray,p,depth+1);

		col.r += reflect_col.r * sp.refCoeff;
		col.g += reflect_col.g * sp.refCoeff;
		col.b += reflect_col.b * sp.refCoeff;

		for (vector<Point3f>::size_type i = 0; i < lightSources.size(); i++) {
				Vector3f lm = Vector3f(lightSources[i].x - p.x,
					 lightSources[i].y - p.y,
					 lightSources[i].z - p.z);
				Vector3f n = normal;
				Vector3f rm = n.mult(2*(lm.dot(n)))-lm;
				Vector3f v = r.dir;

				lm.normalize();
				n.normalize();
				rm.normalize();
				v.normalize();

				col.r += sp.difCoeff*lm.dot(n) 
					+ sp.specCoeff*(pow((double)rm.dot(v),(double)sp.specExp));
				col.g += sp.difCoeff*lm.dot(n) 
					+ sp.specCoeff*(pow((double)rm.dot(v),(double)sp.specExp));
				col.b += sp.difCoeff*lm.dot(n) 
					+ sp.specCoeff*(pow((double)rm.dot(v),(double)sp.specExp));
			}

			col.r = maxV(0.0,minV(col.r,1.0));
			col.g = maxV(0.0,minV(col.g,1.0));
			col.b = maxV(0.0,minV(col.b,1.0));


			return col;
	}
	else if(type==3)
	{//hits a triangle
		Triangle tr(triangles[indx]);
		Vector3f v = origin+r.dir.mult(mn); //intersection point
		Point3f p(v.x,v.y,v.z);
		Vector3f normal = tr.getNormalAt(p);

		ColorRGB col(tr.baseColor[0], tr.baseColor[1], tr.baseColor[2]);
		//return col;

		/*col.r *= sp.ambCoeff;
		col.g *= sp.ambCoeff;
		col.b *= sp.ambCoeff;

		Vector3f ci,si,reflected,ct,st,refracted;
		ci = normal.mult((-r.dir).dot(normal));
		si = ci+r.dir;

		reflected = ci+si;
		reflected.normalize();
		Ray reflect_ray(p,reflected);

		ColorRGB reflect_col = traceScene(reflect_ray,p,depth+1);

		col.r += reflect_col.r * sp.refCoeff;
		col.g += reflect_col.g * sp.refCoeff;
		col.b += reflect_col.b * sp.refCoeff;

		for (vector<Point3f>::size_type i = 0; i < lightSources.size(); i++) {
				Vector3f lm = Vector3f(lightSources[i].x - p.x,
					 lightSources[i].y - p.y,
					 lightSources[i].z - p.z);
				Vector3f n = normal;
				Vector3f rm = n.mult(2*(lm.dot(n)))-lm;
				Vector3f v = r.dir;

				lm.normalize();
				n.normalize();
				rm.normalize();
				v.normalize();

				col.r += sp.difCoeff*lm.dot(n) 
					+ sp.specCoeff*(pow((double)rm.dot(v),(double)sp.specExp));
				col.g += sp.difCoeff*lm.dot(n) 
					+ sp.specCoeff*(pow((double)rm.dot(v),(double)sp.specExp));
				col.b += sp.difCoeff*lm.dot(n) 
					+ sp.specCoeff*(pow((double)rm.dot(v),(double)sp.specExp));
			}

			col.r = maxV(0.0,minV(col.r,1.0));
			col.g = maxV(0.0,minV(col.g,1.0));
			col.b = maxV(0.0,minV(col.b,1.0));


			return col;*/
	}/**/

	return ColorRGB(0,0,0);
}

void traceFullWorld(){
	printf("\nTracing the world ...\n");
	bitmap_image image(pixel, pixel);
	Point3f cop = camPos;

	Vector3f up(camUpDir.x, camUpDir.y, camUpDir.z);
	// n = eye-look, 
	// u = look X up =right, 
	// v = right X look = up
    Vector3f n(camPos.x - camLookAt.x, camPos.y - camLookAt.y, camPos.z - camLookAt.z); // make n
    Vector3f u(up.cross(n)); // make u = up X n
    n.normalize(); u.normalize(); // make them unit length
    Vector3f v(n.cross(u)); // make v = n X u
	
	for(int i=0; i<pixel; i++){
		for(int j=0; j<pixel; j++){
			double xNorm = 1.0 * i / pixel;
			double zNorm = 1.0 * j / pixel;

			
			Vector3f camdir = -n;
			Vector3f right = u;
			Vector3f up = v;
			Vector3f dir = camdir 
							+ right.mult(xNorm)
							+ up.mult(zNorm);
			dir.normalize();
			
			Ray r(cop,dir);
			ColorRGB col = traceScene(r,cop,0).to256Range();

			image.set_pixel(i,j,col.r,col.g,col.b);
		}
	}
	image.save_image("test.bmp");
	printf("************ Image traced ************\n");
}
#pragma endregion 

#pragma region Camera movements
void camStrafe(int dir) {
	Point3f alongVect = crossProduct(camUpDir, diffVector(camPos, camLookAt));
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
	Point3f d = diffVector(camPos, camLookAt);
	Point3f alongVect = crossProduct(camUpDir, d);

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

	case '1':	// Yaw (looking left) 
			  // camLookAt.y += 10;
		camYawn(1);
		break;

	case '2':	 // Yaw (looking right)
			  // camLookAt.y -= 10;
		camYawn(-1);
		break;

	case '3':	
		// Pitch (looking up) 
		camLookAt.z += 10;
		break;

	case '4': 
		// Pitch (looking down) 
		camLookAt.z -= 10;
		break;

	case '5': //	Roll (twisting left)
		camLookAt.x += 10;
		break;

	case '6':// Roll (twisting right)
		camLookAt.x -= 10;
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
		camPos.z -= 10;
		camLookAt.z -= 10;
		break;

	case GLUT_KEY_DOWN: // Walk (backward w.r.t. current position)
						// cameraRadius += 10; // 
		camPos.z += 10;
		camLookAt.z += 10;
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
			traceFullWorld();
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
	camPos.x = 0;
	camPos.y = 100;
	camPos.z = 150;
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

	//setupDiffuseLightEffect();
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
	GLfloat unset[] = { 0,0,0,1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, unset);

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

Point3f parseLightSource(char* line){
	float x, y, z;
	char garbage[30];
	sscanf(line, "%s %f %f %f", &garbage, &x, &y, &z);
	return Point3f(x,y,z);
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
			lightSources.push_back(Point3f(x, y, z));
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