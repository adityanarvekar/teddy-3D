#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1 
#pragma once

#define X .525731112119133606
#define Z .850650808352039932

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 600;


bool mPressed = true;


int i;
GLfloat light_position[] = { 0.0, 0.0, 5.0, 1.0 };
GLfloat angle = 100;
GLfloat light_direction[] = { .0, .0, -1.0 };
GLfloat light_x = .0f;
GLfloat light_y = .0f;
GLfloat light_z = 5.0f;
int phong = 0;
bool shearCondition = false;
/*material data*/
GLfloat material_color1[] = { 0.1, 0.7, .0, 1.0 };
GLfloat material_color2[] = { 1.0, 0.0, .0, 1.0 };
GLfloat material_color3[] = { .0, 1.0, 0.0, 1.0 };
GLfloat material_ambient[] = { .5, .5, .5, 1.0 };
GLfloat low_sh[] = { 5.0 };
GLfloat material_specular[] = { .2, .2, .2, 1. };

float shx = 0;
float shy = 0;

float theta_x = 0;
float theta_y = 0;
float theta_z = 0;

/* Define data */
int mouse_down = 0; //Represent the left mouse key is clicked down
int current_x = 0, current_y = 0;
static GLdouble ex = 0.0, ey = 0.0, ez = 5.0, upx = 0.0, upy = 1.0, upz = 0.0, ox = 0.0, oy = 0.0, oz = 0.0;

/*Projection matrix mode*/
int projection = 0; //0 - parallel; 1 - perspective

/*Shading Mode*/
int shading = 1;
using namespace std;

struct Vertex {
	float x;
	float y;
	float z;



	/*Mat times a vertex and return another vertex*/
	const Vertex operator * (const Vertex& vertex) const
	{
		Vertex result;
		result.x = (y * vertex.z) - (z * vertex.y);
		result.y = (z * vertex.x) - (x * vertex.z);
		result.z = (x * vertex.y) - (y * vertex.x);

		return result;
	}
};


struct CustomVector {
	float element[4];
};


struct Triangle {
	int v1;
	int v2;
	int v3;
};


std::vector<Vertex> vt_list;
std::vector<Triangle> tg_list;





struct Mat {
	float element[4][4];


	void identityMat()
	{
		for (int i = 0; i <= 3; i++)
		{
			for (int j = 0; j <= 3; j++)
			{
				if (i == j) {
					element[i][j] = 1;
				}
				else
				{
					element[i][j] = 0;
				}
			}
		}
	}


	const Mat operator * (const Mat& right) const
	{
		Mat result;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.element[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					result.element[i][j] += element[i][k] * right.element[k][j];
				}
			}
		}

		return result;
	}


	const CustomVector operator * (const CustomVector& vec) const
	{
		CustomVector result;

		for (int i = 0; i < 4; i++)
		{

			result.element[i] = 0;
			for (int k = 0; k < 4; k++)
			{
				result.element[i] += element[i][k] * vec.element[k];
			}

		}

		return result;
	}
};





void readAndFillArray() {
	float x = 0;
	float y = 0;
	float z = 0;

	int indx_a = 0;
	int indx_b = 0;
	int indx_c = 0;

	FILE * file = fopen("teddy.obj", "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
	}
	while (1) {
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}

		else if (strcmp(lineHeader, "v") == 0) {
			fscanf(file, "%f %f %f\n", &x, &y, &z);
			vt_list.push_back({ x,y,z });
		}
		else if (strcmp(lineHeader, "f") == 0) {
			fscanf(file, "%d %d %d\n", &indx_a, &indx_b, &indx_c);
			tg_list.push_back({ indx_a,indx_b,indx_c });
		}
	}


}

void initialize()
{
	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, (GLint)WINDOW_WIDTH, (GLint)WINDOW_HEIGHT);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &angle);

}




void rotateCalc()
{
	// Rotation matrix
	Mat Rx, Ry, Rz;
	Mat T;

	Rx.identityMat();
	Ry.identityMat();
	Rz.identityMat();
	T.identityMat();

	// x axis 
	Rx.element[1][1] = cos(theta_x);
	Rx.element[1][2] = -sin(theta_x);
	Rx.element[2][1] = sin(theta_x);
	Rx.element[2][2] = cos(theta_x);


	// y axis 
	Ry.element[0][0] = cos(theta_y);
	Ry.element[0][2] = sin(theta_y);
	Ry.element[2][0] = -sin(theta_y);
	Ry.element[2][2] = cos(theta_y);



	/*

	modify this when z press
	// z axis
	Rz.element[0][0] = cos(theta_z);
	Rz.element[0][1] = -sin(theta_z);
	Rz.element[1][0] = sin(theta_z);
	Rz.element[1][1] = cos(theta_z);


	mouse motion ----- theta_z += static_cast<float>(y - current_y) / 100.f;
	at the end theta_z = 0;
	*/



	Mat P = T * Rx * Ry;


	for (int i = 0; i < vt_list.size(); i++)
	{
		CustomVector cur_pt, updated_pt;
		cur_pt.element[0] = vt_list[i].x;
		cur_pt.element[1] = vt_list[i].y;
		cur_pt.element[2] = vt_list[i].z;
		cur_pt.element[3] = 1.0f;
		updated_pt = P * cur_pt;
		vt_list[i].x = updated_pt.element[0] / updated_pt.element[3];
		vt_list[i].y = updated_pt.element[1] / updated_pt.element[3];
		vt_list[i].z = updated_pt.element[2] / updated_pt.element[3];

	}


	theta_x = 0;
	theta_y = 0;
}





void onDisplay()
{

	//for each iteration, clear the canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*glMatrixMode(GL_PROJECTION);*/
	glMatrixMode(GL_PROJECTION); //define the camera matrix model
								 //
	glLoadIdentity(); //generate a matrix


	if (projection == 0) //parallel projection
	{

		glOrtho(-40, 40, -40, 40, -100, 100);


	}
	else  //perspective
		gluPerspective(60.0, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, .1, 100.0);

	/* About Shading */
	if (shading == 1)
	{
		light_position[0] = light_x;
		light_position[1] = light_y;
		light_position[2] = light_z;
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glMaterialfv(GL_FRONT, GL_AMBIENT, material_color3);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);

		if (phong == 1)
		{
			glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.2);
			glMaterialfv(GL_FRONT, GL_AMBIENT, material_color1);
			glMaterialfv(GL_FRONT, GL_SPECULAR, material_color3);
			glMaterialfv(GL_FRONT, GL_SHININESS, low_sh);
		}
		else {
		}


	}
	else {
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(ex, ey, ez, ox, oy, oz, upx, upy, upz);

	if (mPressed) {
		glBegin(GL_LINE_LOOP);
	}
	else {
		glBegin(GL_TRIANGLES);
	}

	glColor3f(1, 1, 0);

	if (mouse_down == 1) {
		rotateCalc();
	}

	for (int arraysize = 0; arraysize < tg_list.size(); arraysize++) {

		glVertex3d(vt_list[tg_list[arraysize].v1 - 1].x + (vt_list[tg_list[arraysize].v1 - 1].y*shx), vt_list[tg_list[arraysize].v1 - 1].y + (vt_list[tg_list[arraysize].v1 - 1].x*shy), vt_list[tg_list[arraysize].v1 - 1].z);
		glVertex3d(vt_list[tg_list[arraysize].v2 - 1].x + (vt_list[tg_list[arraysize].v2 - 1].y*shx), vt_list[tg_list[arraysize].v2 - 1].y + (vt_list[tg_list[arraysize].v2 - 1].x*shy), vt_list[tg_list[arraysize].v2 - 1].z);
		glVertex3d(vt_list[tg_list[arraysize].v3 - 1].x + (vt_list[tg_list[arraysize].v3 - 1].y*shx), vt_list[tg_list[arraysize].v3 - 1].y + (vt_list[tg_list[arraysize].v3 - 1].x*shy), vt_list[tg_list[arraysize].v3 - 1].z);
	}


	glEnd();

	glutSwapBuffers();




}

void onMouse(int button, int state, int x, int y)
{

	GLint specialKey = glutGetModifiers();
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			mouse_down = 1;
			current_x = x;
			current_y = y;
			if (specialKey == GLUT_ACTIVE_SHIFT)
			{
				shearCondition = true;
			}
			else
			{
				shearCondition = false;

			}

		}
		else if (state == GLUT_UP)
		{
			mouse_down = 0;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)

			break;

	default:
		break;

	}

}


void onResize(GLint w, GLint h)
{
	//glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective (65.0, (GLfloat) w/(GLfloat) h, 1.0, 100.0);

}


void onIdle()
{

}


void onMouseMotion(int x, int y)
{

	if (mouse_down == 1 && shearCondition == true)
	{



		shx += static_cast<float>(x - current_x) / 100.f;
		shy += static_cast<float>(y - current_y) / 100.f;

		ex += static_cast<float>(x - current_x) / 100.f;
		ey += static_cast<float>(y - current_y) / 100.f;


		current_x = x;
		current_y = y;
	}

	else if (mouse_down == 1)
	{


		//	ex += static_cast<float>(x - current_x) / 100.f;
		//	ey += static_cast<float>(y - current_y) / 100.f;


		theta_y += static_cast<float>(x - current_x) / 100.f;
		theta_x += static_cast<float>(y - current_y) / 100.f;







		current_x = x;
		current_y = y;
	}

	glutPostRedisplay();
}





void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(1);

		break;
	case 'p':
		projection = (projection == 0) ? 1 : 0;
		glutPostRedisplay();
		break;
	case 'z':
		ez += 0.2;
		glutPostRedisplay();
		break;
	case 'x':
		ez -= 0.2;
		glutPostRedisplay();
		break;
	case 's':
		shading = (shading == 0) ? 1 : 0;
		glutPostRedisplay();
		break;

	case 'o':
		phong = (phong == 0) ? 1 : 0;
		glutPostRedisplay();
		break;
	case 'm':
		if (mPressed) {
			mPressed = false;
		}
		else {
			mPressed = true;
		}
		glutPostRedisplay();
		break;
	case 'M':
		if (mPressed) {
			mPressed = false;
		}
		else {
			mPressed = true;
		}
		glutPostRedisplay();
		break;
		break;
	default:
		break;
	}
}




void processSpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case 27:
		break;
	case 100:
		light_x -= 1.2;
		glutPostRedisplay();

		break;
	case 102:
		light_x += 1.2;
		glutPostRedisplay();
		break;
	case 101:
		light_y += 1.2;
		glutPostRedisplay();
		break;
	case 103:
		light_y -= 1.2;
		glutPostRedisplay();
		break;
	}

}


int main(int argc, char** argv)
{
	//Initialization functions
	readAndFillArray();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, WINDOW_HEIGHT / 2);
	glutCreateWindow("Draw Teddy");

	initialize(); //optional

	//Call-back functions
	glutDisplayFunc(onDisplay);
	glutKeyboardFunc(onKeyboard);  //waitKey(500)
	glutSpecialFunc(processSpecialKeys);
	glutMouseFunc(onMouse); //mouse click
	glutMotionFunc(onMouseMotion); //mouse movement
	glutReshapeFunc(onResize);
	glutIdleFunc(onIdle); //run something in the backgrond


	//Infinite Loop
	glutMainLoop(); //== while(1)
	return 1;
}