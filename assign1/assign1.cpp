// assign1.cpp : Defines the entry point for the console application.
//

/*
CSCI 420 Computer Graphics
Assignment 1: Height Fields
C++ starter code
*/

#include "stdafx.h"
#include <pic.h>
#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

int g_iMenuId;

int g_vMousePos[2] = { 0, 0 };
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = { 0.0, 0.0, 0.0 };
float g_vLandTranslate[3] = { 0.0, 0.0, 0.0 };
float g_vLandScale[3] = { 0.01, 0.01, 0.01 };

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

bool topographicMode = false;

/* Write a screenshot to the specified filename */
void saveScreenshot(char *filename)
{
	int i, j;
	Pic *in = NULL;

	if (filename == NULL)
		return;

	/* Allocate a picture buffer */
	in = pic_alloc(640, 480, 3, NULL);

	printf("File to save to: %s\n", filename);

	for (i = 479; i >= 0; i--) {
		glReadPixels(0, 479 - i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
			&in->pix[i*in->nx*in->bpp]);
	}

	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);
}

void createScreenshot(int i){
	char myFilename[2048];
	sprintf_s(myFilename, "animations/anim%04d.jpg", i);
	saveScreenshot(myFilename);
}

void myinit()
{
	/* setup gl view here */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
}

float ridgeOffset = 50.0f;

void createTriStrips(){
	for (int i = 0; i<g_pHeightData->ny - 1; i++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < g_pHeightData->nx; j++) {
			float indx0 = PIC_PIXEL(g_pHeightData, j, i, 0); // 'top' vertex
			float indx1 = PIC_PIXEL(g_pHeightData, j, i + 1, 0); // 'bottom' vertex
			float z1 = indx0 / 255; //divide by 255 as PIC_PIXEL gives values from 0-255 but glColor3f takes in values from 0-1
			float z2 = indx1 / 255;

			float offset = (g_pHeightData->ny) / 2; //divide height of image by 2 to get offset to center image
			// sequential top,bottom vert pairs generates a tri-strip

			if (topographicMode){
				if (z1 <= 1 && z1 > .5f){
					glColor3f(1, -2 * z1 + 2, 0);
				}
				else if (z1 == .5f){
					glColor3f(1, 1, 0);
				}
				else {
					glColor3f(2 * z1, 1, 0);
				}
				//glColor3f(z1, 1 - z1, 0);
			}
			else {
				glColor3f(z1, z1, z1); // top vert color
			}



			glVertex3f(j - offset, i - offset, z1 * ridgeOffset); //multiply z by ridgeOffset to get more height range
			//glVertex3f(j, i, z1 * ridgeOffset); //multiply z by ridgeOffset to get more height range

			if (topographicMode){
				if (z2 <= 1 && z2 > .5f){
					glColor3f(1, -2 * z2 + 2, 0);
				}
				else if (z2 == .5f){
					glColor3f(1, 1, 0);
				}
				else {
					glColor3f(2 * z2, 1, 0);
				}
				//glColor3f(z2, 1 - z2,0);
			}
			else {
				glColor3f(z2, z2, z2); // bottom vert color
			}

			glVertex3f(j - offset, i + 1 - offset, z2 * ridgeOffset);
			//glVertex3f(j, i + 1, z2 * ridgeOffset);
		}// next pixel in current row
		glEnd();
	}// next row
}

int frames = 0;
bool screenshotMode = false;
void display()
{
	/* replace this code with your height field implementation */
	/* you may also want to precede it with your
	rotation/translation/scaling */

	//clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//reset transformation
	glLoadIdentity();

	glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2] - 10);
	glRotatef(g_vLandRotate[0], 1, 0, 0);
	glRotatef(g_vLandRotate[1], 0, 1, 0);
	glRotatef(g_vLandRotate[2], 0, 0, 1);
	glScalef(g_vLandScale[0], -g_vLandScale[1], g_vLandScale[2]); //y is negative so that the image will be upright from start

	//colorcube();
	createTriStrips();

	if (frames < 300 && screenshotMode == true){ //take screenshots
		createScreenshot(frames);
		frames++;
	}

	glutSwapBuffers();
}

void menufunc(int value)
{
	switch (value)
	{
	case 0:
		exit(0);
		break;
	}
}

//GLfloat delta = 2.0;
//GLint axis = 0;
void doIdle()
{
	/* do some stuff... */
	//g_vLandRotate[axis] += delta;
	//if (g_vLandRotate[axis] > 45 || g_vLandRotate[axis] < -45){
	//	delta *= -1;
	//}

	/* make the screen update */
	glutPostRedisplay();
}

/* converts mouse drags into information about
rotation/translation/scaling */
void mousedrag(int x, int y)
{
	int vMouseDelta[2] = { x - g_vMousePos[0], y - g_vMousePos[1] };

	switch (g_ControlState)
	{
	case TRANSLATE:
		if (g_iLeftMouseButton)
		{
			g_vLandTranslate[0] += vMouseDelta[0] * 0.01;
			g_vLandTranslate[1] -= vMouseDelta[1] * 0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandTranslate[2] += vMouseDelta[1] * 0.01;
		}
		break;
	case ROTATE:
		if (g_iLeftMouseButton)
		{
			g_vLandRotate[0] += vMouseDelta[1];
			g_vLandRotate[1] += vMouseDelta[0];
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandRotate[2] += vMouseDelta[1];
		}
		break;
	case SCALE:
		if (g_iLeftMouseButton)
		{
			g_vLandScale[0] *= 1.0 + vMouseDelta[0] * 0.01;
			g_vLandScale[1] *= 1.0 - vMouseDelta[1] * 0.01;
		}
		if (g_iMiddleMouseButton)
		{
			g_vLandScale[2] *= 1.0 - vMouseDelta[1] * 0.01;
		}
		break;
	}
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		g_iLeftMouseButton = (state == GLUT_DOWN);
		break;
	case GLUT_MIDDLE_BUTTON:
		g_iMiddleMouseButton = (state == GLUT_DOWN);
		break;
	case GLUT_RIGHT_BUTTON:
		g_iRightMouseButton = (state == GLUT_DOWN);
		break;
	}

	switch (glutGetModifiers())
	{
	case GLUT_ACTIVE_CTRL:
		g_ControlState = TRANSLATE;
		break;
	case GLUT_ACTIVE_SHIFT:
		g_ControlState = SCALE;
		break;
	default:
		g_ControlState = ROTATE;
		break;
	}

	g_vMousePos[0] = x;
	g_vMousePos[1] = y;
}

void keyboard(unsigned char key, int x, int y)
{
	if (key == '1'){ //vertices
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	} if (key == '2'){ //wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} if (key == '3'){ //solid triangles
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} if (key == '4'){ //topographical mode toggle
		topographicMode = !topographicMode;
	} if (key == '5'){ //screenshot toggle
		screenshotMode = !screenshotMode;
	}
}

GLdouble fovy = 30;
void reshape(int w, int h)
{
	GLfloat aspect = (GLfloat)w / (GLfloat)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, 1, 20);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[])
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc<2)
	{
		printf("usage: %s heightfield.jpg\n", argv[0]);
		exit(1);
	}

	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
		printf("error reading %s.\n", argv[1]);
		exit(1);
	}

	glutInit(&argc, (char**)argv);
	/*
	create a window here..should be double buffered and use depth testing
	the code past here will segfault if you don't have a window set up....
	replace the exit once you add those calls.
	*/
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Homework 1");
	//exit(0);


	/* allow the user to quit using the right mouse button menu */
	g_iMenuId = glutCreateMenu(menufunc);
	glutSetMenu(g_iMenuId);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* replace with any animate code */
	glutIdleFunc(doIdle);

	/* callback for mouse drags */
	glutMotionFunc(mousedrag);
	/* callback for idle mouse movement */
	glutPassiveMotionFunc(mouseidle);
	/* callback for mouse button changes */
	glutMouseFunc(mousebutton);
	/* callback for keyboard */
	glutKeyboardFunc(keyboard);

	/* do initialization */
	myinit();

	//camera
	glutReshapeFunc(reshape);

	/* tells glut to use a particular display function to redraw */
	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}