// WoTProject1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#define GLEW_STATIC
#include <algorithm>
#include <Windows.h>
#include <cstdio>
#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <gl/glut.h>
#include <gl/GL.h>
#include <gl/GLU.h>
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "btBulletDynamicsCommon.h"


#include "C_Tank.h"

const float pi=acos(-1);
// GLUT CALLBACK functions
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void idleCB();
void keyboardCB(unsigned char key, int x, int y);
void keyboardSpecialCB(int key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);
void mousewheelCB(int button, int dir, int x, int y);
void mousePassiveMotionCB(int x, int y);

// CALLBACK function when exit() called ///////////////////////////////////////
void exitCB();

void initGL();
int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void showInfo();
void toOrtho();
void toPerspective();
void drawAxis(float size=2.5f);
void drawGrid(float size=100.0f, float step=1.0f);
void drawModel();
void anglesToMatrix(const glm::vec3 angles, glm::mat4 &matrix);
glm::mat4 setFrustum(float l, float r, float b, float t, float n, float f);
glm::mat4 setFrustum(float fovY, float aspectRatio, float front, float back);
glm::mat4 setOrthoFrustum(float l, float r, float b, float t, float n, float f);

// constantsbullet/
const int   SCREEN_WIDTH    = 800;//400;
const int   SCREEN_HEIGHT   = 640;//300;
const float CAMERA_DISTANCE = 15.0f;
const int   TEXT_WIDTH      = 8;
const int   TEXT_HEIGHT     = 13;
const float DEG2RAD         = 3.141593f / 180;
const GLfloat g_ViewDistance=900.0;

// global variables
void *font = GLUT_BITMAP_8_BY_13;
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseMidleDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraX;
float cameraY;
float cameraDistance;
int drawMode = 0;
HWND g_hWnd;
glm::mat4 matrixView;
glm::mat4 matrixModel;
glm::mat4 matrixModelView;    // = matrixView * matrixModel
glm::mat4 matrixProjection;

//
//GLuint Chassis_tex_ID,Hull_tex_ID,Turret_01_tex_ID,Gun_01_tex_ID;
GLuint Clouds_tex_ID, Map_tex_ID;


class C_Player {
public:
	GLfloat x, y, z, rotx, trotx, troty, a_yaw, a_pitch, a_roll;
	C_Player():x(0), y(0), z(0), rotx(0), a_yaw(0), a_pitch(0), a_roll(0) {}
	void forward() {
		x-=0.05*sin(pi*(rotx)/180);
		z-=0.05*cos(pi*(rotx)/180);
	}
	void backward() {
		x+=0.05*sin(pi*(rotx)/180);
		z+=0.05*cos(pi*(rotx)/180);
	}
	void turnleft() {
		rotx+=2;
	}
	void turnright() {
		rotx-=2;
	}
	void turretturnleft() {
		//if(trotx<=KV2.leftAng) { trotx+=0.4f; }
		if(trotx>=180) { trotx-=360; }
	}
	void turretturnright() {
		//if(trotx>=KV2.rightAng) { trotx-=0.4f; }
		if(trotx<=-180) { trotx+=360; }
	}
	void turretturnup() {
		//        troty+=0.1;
		//        printf("%f %f\n",troty,KV2.upAng);
		//        if()
		//troty=std::min(troty+0.1f, KV2.upAng);
	}
	void turretturndown() {
		//        troty-=0.1;
		//        printf("%f %f\n",troty,KV2.upAng);
		//troty=std::max(troty-0.1f, KV2.downAng);
	}
	void rotturret() {
		if(abs(a_yaw-(trotx))>0.2) {
			if((a_yaw-(trotx))>=((trotx)-a_yaw)) { turretturnleft(); } else { turretturnright(); }
		}
	}
	void lookleft(float ang) {
		a_yaw=a_yaw+ang;
		if(a_yaw>=180) { a_yaw-=360; }
		if(a_yaw<-180) { a_yaw+=360; }
	}
	void lookup(float ang) {
		a_pitch=std::max(std::min(a_pitch+ang, 60.f), -60.f);
	}
	void lookright(float ang) {
		a_yaw-=ang;
	}
	void lookdown(float ang) {
		a_pitch-=ang;
	}
} Player;
GLUquadricObj *qobj= gluNewQuadric();

#include <SOIL.h>
/// //////
int main(int argc, char **argv) {

	// init global vars
	initSharedMem();
	// register exit callback
	atexit(exitCB);
	// init GLUT and GL
	initGLUT(argc, argv);
	initGL();

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	//const GLchar* fragmentShaderSource =
	//    "#version 330 core\n"
	//    "out vec4 color;\n"
	//    "void main()\n"
	//    "{\n"
	//        "color = vec4(0.0f, 0.5f, 0.2f, 1.0f);\n"
	//        "if(gl_FragCoord.x < 400)\n"
	//            "color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
	//        "else\n"
	//            "color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
	//    "}\n\0";
	//    GLint success;
	//    GLchar infoLog[512];
	//
	//    GLuint vertexShader;
	//    // Fragment shader
	//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//    glCompileShader(fragmentShader);
	//    // Check for compile time errors
	//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	//    if (!success)
	//    {
	//        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	//        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	//    }
	//    GLuint shaderProgram = glCreateProgram();
	//    glAttachShader(shaderProgram, fragmentShader);
	//    glLinkProgram(shaderProgram);
	//    // Check for linking errors
	//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	//    if (!success) {
	//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	//        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	//    }
	//    glDeleteShader(fragmentShader);

	int t_timestart;
	printf("%d,", t_timestart=GetTickCount());
	Clouds_tex_ID = SOIL_load_OGL_texture(
		".\\cloud_ring2.bmp",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS
		| SOIL_FLAG_DDS_LOAD_DIRECT
		);
	//Map_tex_ID = SOIL_load_OGL_texture(
	//	".\\Color_Tex.dds",
	//	SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS
	//	| SOIL_FLAG_DDS_LOAD_DIRECT
	//	);
	//    Chassis_tex_ID = SOIL_load_OGL_texture(
	//            "F:\\Workspace\\GLrefs\\WotConverter\\vehicles_level_06.pkg\\vehicles\\russian\\tracks\\KV_track_2014_AM.dds",
	//            SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS
	//                                   //| SOIL_FLAG_MULTIPLY_ALPHA
	//                                   //| SOIL_FLAG_COMPRESS_TO_DXT
	//                                   | SOIL_FLAG_DDS_LOAD_DIRECT
	//                                   //| SOIL_FLAG_NTSC_SAFE_RGB
	//                                   //| SOIL_FLAG_CoCg_Y
	//                                   //| SOIL_FLAG_TEXTURE_RECTANGLE
	//                                  );
	printf("%d %d\n", GetTickCount(), GetTickCount()-t_timestart);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricTexture(qobj, GL_TRUE);

	//    glutFullScreen();
	// the last GLUT call (LOOP)
	// window will be shown and display callback is triggered by events
	// NOTE: this call never return main().
	glutMainLoop(); /* Start GLUT event-processing loop */
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// draw a grid on XZ-plane
///////////////////////////////////////////////////////////////////////////////
void drawGrid(float size, float step) {
	// disable lighting
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	// 20x20 grid
	glBegin(GL_LINES);
	glColor3f(0.5f, 0.5f, 0.5f);
	for(float i=step; i <= 20; i+= 2*step) {
		glVertex3f(-size, 0, i);   // lines parallel to X-axis
		glVertex3f(size, 0, i);
		glVertex3f(-size, 0, -i);   // lines parallel to X-axis
		glVertex3f(size, 0, -i);
		glVertex3f(i, 0, -size);    // lines parallel to Z-axis
		glVertex3f(i, 0, size);
		glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
		glVertex3f(-i, 0, size);
	}
	// x-axis
	glColor3f(1, 0, 0);
	glVertex3f(-g_ViewDistance, 0, 0);
	glVertex3f(g_ViewDistance, 0, 0);
	// y-axis
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, -g_ViewDistance);
	glVertex3f(0, 0, g_ViewDistance);
	// z-axis
	glColor3f(0, 0, 1);
	glVertex3f(0, -g_ViewDistance, 0);
	glVertex3f(0, g_ViewDistance, 0);
	glEnd();
	// enable lighting back
	glEnable(GL_LIGHTING);
}

///////////////////////////////////////////////////////////////////////////////
// draw the local axis of an object
///////////////////////////////////////////////////////////////////////////////
void drawAxis(float size) {
	//    glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
	glDisable(GL_LIGHTING);
	// draw axis
	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(size, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, size);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, size, 0);
	glEnd();
	glLineWidth(1);
	// draw arrows(actually big square dots)
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(size, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, size);
	glColor3f(0, 0, 1);
	glVertex3f(0, size, 0);
	glEnd();
	glPointSize(1);
	// restore default settings
	glEnable(GL_LIGHTING);
	//    glDepthFunc(GL_LEQUAL);
}
void drawArc(double x, double y, double start_angle, double end_angle, double delta_angle, double radius, bool fill) {
	if(fill) {
		glBegin(GL_TRIANGLE_FAN);
	} else {
		glBegin(GL_LINE_STRIP);
	}
	for(double i=start_angle; i<=end_angle; i+=delta_angle) {
		double vx=x+radius * cos(i);
		double vy=y+radius*sin(i);
		glVertex2d(vx, vy);
	}
	glEnd();
}

///////////////////////////////////////////////////////////////////////////////
// draw a model (tetrahedron)
///////////////////////////////////////////////////////////////////////////////

void drawModel() {
	//    glBindTexture(GL_TEXTURE_2D,Chassis_tex_ID);
	//    glEnable(GL_TEXTURE_2D);
	//    glDisable(GL_TEXTURE_2D);
	//    glutSolidTorus(3,5,10,20);
	if(1) { //天空
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		static float r=0.0;
		r+=0.08;
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		//glFrontFace(GL_CCW);
		glColor3f(1,.5,.6);
		glCullFace(GL_FRONT);
		glBindTexture(GL_TEXTURE_2D, Clouds_tex_ID);
		//        glTranslatef(0,310,0);
		glTranslatef(0, 375, 0);
		glRotatef(r, 0.0f, 1.0f, 0.0f);
		glRotatef(90, 1, 0, 0);
		glColor4f(1, 1, 1, 1);
		gluSphere(qobj, 500, 32, 32);
		//        gluCylinder(qobj,0,200,400,32,32);
		glCullFace(GL_BACK);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		/////
		//glEnable(GL_TEXTURE_2D);
		//glPushMatrix();
		////glFrontFace(GL_CCW);
		//glCullFace(GL_FRONT);
		//glBindTexture(GL_TEXTURE_2D, Map_tex_ID);
		//glColor4f(1, 1, 1, 1);
		//glBegin(GL_QUADS);
		//glTexCoord2f(0, 0);
		//glVertex3f(-220, 0, -220);
		//glTexCoord2f(1, 0);
		//glVertex3f(220, 0, -220);
		//glTexCoord2f(1, 1);
		//glVertex3f(220, 0, 220);
		//glTexCoord2f(0, 1);
		//glVertex3f(-220, 0, 220);
		//glEnd();
		//glCullFace(GL_BACK);
		//glPopMatrix();
		//glDisable(GL_TEXTURE_2D);
		//glEnable(GL_DEPTH_TEST);
	}
	//glEnable(GL_LIGHTING);
	//glColor3f(1, 1, 1);
	//static float r=0.0;
	//r+=0.08;
	////    double offset1[]= {0,0,0};
	////    glutSolidSierpinskiSponge(4,offset1,5);
	///// ////////////
	//////////////////////////////////////////////////////
	//glDisable(GL_LIGHTING);
	//glLineWidth(2);
	//glBegin(GL_LINE_STRIP);
	//glColor3f(1, 0, 0.5);
	//glVertex3f(0, 1, 0);
	//glVertex3f(0, 1, 0);
	//glEnd();
	//glLineWidth(1);
	//glEnable(GL_LIGHTING);
}

///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv) {
	// GLUT stuff for windowing
	// initialization openGL window.
	// it is called before any other GLUT routine
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode
	glutInitWindowSize(screenWidth, screenHeight);  // window size
	glutInitWindowPosition(900, 100);           // window location
												// finally, create a window with openGL context
												// Window will not displayed until glutMainLoop() is called
												// it returns a unique ID
	int handle = glutCreateWindow("GLWoT");//argv[0]);     // param is the title of window
	g_hWnd=FindWindowA(NULL, "GLWoT");
	// register GLUT callback functions
	glutDisplayFunc(displayCB);
	glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
	//glutTimerFunc(16, inputcalcCB, 16);             // independent with redraw, to have a sooth control
//    glutIdleFunc(idleCB);                       // redraw whenever system is idle
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(keyboardSpecialCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	//glutMouseWheelFunc(mousewheelCB);
	glutPassiveMotionFunc(mousePassiveMotionCB);
	return handle;
}

///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL() {
	glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
												// enable /disable features
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);
	initLights();
}

///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);
	glColor4fv(color);          // set text color
	glRasterPos2i(x, y);        // place text position
								// loop all characters in the string
	while(*str) {
		glutBitmapCharacter(font, *str);
		++str;
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font) {
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);
	glColor4fv(color);          // set text color
	glRasterPos3fv(pos);        // place text position
								// loop all characters in the string
	while(*str) {
		glutBitmapCharacter(font, *str);
		++str;
	}
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem() {
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;
	mouseLeftDown = mouseRightDown = false;
	mouseX = mouseY = 0;
	cameraAngleX = cameraAngleY = 0;
	cameraDistance = CAMERA_DISTANCE;
	drawMode = 0; // 0:fill, 1: wireframe, 2:points
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// clean up shared memory
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem() {
}

///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights() {
	// set up light colors (ambient, diffuse, specular)
	GLfloat lightKa[] ={.82f, .82f, .82f, 1.0f};  // 环境光ambient light
	GLfloat lightKd[] ={.7f, .7f, .7f, 1.0f};  // 漫反光diffuse light
	GLfloat lightKs[] ={1, 1, 1, 1};           // 镜面光specular light
											   //    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
											   //    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
											   //    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
											   //    // position the light
											   //    GLfloat lightPos[4] = {0, 0, 20, 0}; // positional light
											   //    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
											   //    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
	{
		GLfloat lightKa[] ={1.f, 1.f, 1.f, 1.0f};  // 环境光ambient light
		GLfloat lightKd[] ={1.7f, 1.7f, 1.7f, 1.0f};  // 漫反光diffuse light
		GLfloat lightKs[] ={1, 1, 1, 1};           // 镜面光specular light
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightKa);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightKd);
		glLightfv(GL_LIGHT1, GL_SPECULAR, lightKs);
		// position the light
		GLfloat lightPos[4] ={0, 0, 20, 1}; // positional light
		glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
		glEnable(GL_LIGHT1);                        // MUST enable each light source after configuration
	}
}

///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
//void setCamera(float eyeX, float eyeY, float eyeZ, float targetX, float targetY, float targetZ)
//{
//    Vector3 eye(eyeX, eyeY, eyeZ);
//    Vector3 target(targetX, targetY, targetZ);
//    Vector3 up(0, 1, 0); // assume the camera is always straight up (roll=0)
//    // compute the forward, left  and up vectors
//    // NOTE: the direction is reversed (target to camera pos) because of camera transform
//    Vector3 forward = eye - target;
//    float distance = forward.length();  // remember the distance
//    forward /= distance;    // normalize
//    Vector3 left = up.cross(forward).normalize();
//    up = forward.cross(left); // up vector is unit length here
//    // set inverse of rotation matrix: M^-1 = M^T if it is Euclidean transform
//    matrixView.identity();
//    matrixView.setRow(0, left);
//    matrixView.setRow(1, up);
//    matrixView.setRow(2, forward);
//    // set translation column
//    Vector3 trans(0, 0, -distance);
//    matrixView.setColumn(3, trans);
//}

///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo() {
	// backup current model-view matrix
	glPushMatrix();                     // save current modelview matrix
	glLoadIdentity();                   // reset modelview matrix
										// set to 2D orthogonal projection
	glMatrixMode(GL_PROJECTION);        // switch to projection matrix
	glPushMatrix();                     // save current projection matrix
	glLoadIdentity();                   // reset projection matrix
	gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
	float color[4] ={1, 1, 1, 1};
	char buf[100];
	sprintf(buf, "X%.2fY%.2fZ%.2f", Player.x, Player.y, Player.z);
	drawString(buf, 0, screenHeight-TEXT_HEIGHT, color, font);
	sprintf(buf, "R%.2fT%.2fY%.2f", Player.rotx, Player.trotx, Player.troty);
	drawString(buf, 0, screenHeight-2*TEXT_HEIGHT, color, font);
	sprintf(buf, "CR%.2fCT%.2f", Player.a_yaw, Player.a_pitch);
	drawString(buf, 0, screenHeight-3*TEXT_HEIGHT, color, font);
	//    ss << "[" << std::setw(8) << matrixView[0] << std::setw(8) << matrixView[4] << std::setw(8) << matrixView[8] << std::setw(8) << matrixView[12] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, screenHeight-(2*TEXT_HEIGHT), color, font);
	//    ss.str("");
	//    ss << "[" << std::setw(8) << matrixView[1] << std::setw(8) << matrixView[5] << std::setw(8) << matrixView[9] << std::setw(8) << matrixView[13] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, screenHeight-(3*TEXT_HEIGHT), color, font);
	//    ss.str("");
	//    ss << "[" << std::setw(8) << matrixView[2] << std::setw(8) << matrixView[6] << std::setw(8) << matrixView[10]<< std::setw(8) << matrixView[14] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, screenHeight-(4*TEXT_HEIGHT), color, font);
	//    ss.str("");
	//    ss << "[" << std::setw(8) << matrixView[3] << std::setw(8) << matrixView[7] << std::setw(8) << matrixView[11]<< std::setw(8) << matrixView[15]<< "]" << ends;
	//    drawString(ss.str().c_str(), 0, screenHeight-(5*TEXT_HEIGHT), color, font);
	//    ss.str("");
	//    drawString("=== Model Matrix ===", 0, 4*TEXT_HEIGHT, color, font);
	//    ss << "[" << std::setw(8) << matrixModel[0] << std::setw(8) << matrixModel[4] << std::setw(8) << matrixModel[8] << std::setw(8) << matrixModel[12] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, 3*TEXT_HEIGHT, color, font);
	//    ss.str("");
	//    ss << "[" << std::setw(8) << matrixModel[1] << std::setw(8) << matrixModel[5] << std::setw(8) << matrixModel[9] << std::setw(8) << matrixModel[13] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, 2*TEXT_HEIGHT, color, font);
	//    ss.str("");
	//    ss << "[" << std::setw(8) << matrixModel[2] << std::setw(8) << matrixModel[6] << std::setw(8) << matrixModel[10]<< std::setw(8) << matrixModel[14] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, TEXT_HEIGHT, color, font);
	//    ss.str("");
	//    ss << "[" << std::setw(8) << matrixModel[3] << std::setw(8) << matrixModel[7] << std::setw(8) << matrixModel[11]<< std::setw(8) << matrixModel[15] << "]" << ends;
	//    drawString(ss.str().c_str(), 0, 0, color, font);
	//    ss.str("");
	//    // unset floating format
	//    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
	// restore projection matrix
	glPopMatrix();                   // restore to previous projection matrix
									 // restore modelview matrix
	glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
	glPopMatrix();                   // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
///////////////////////////////////////////////////////////////////////////////
glm::mat4 setFrustum(float l, float r, float b, float t, float n, float f) {
	glm::mat4 mat;
	mat[0][0]  =  2 * n / (r - l);
	mat[1][1]  =  2 * n / (t - b);
	mat[2][0]  = (r + l) / (r - l);
	mat[2][1]  = (t + b) / (t - b);
	mat[2][2] = -(f + n) / (f - n);
	mat[2][3] = -1;
	mat[3][2] = -(2 * f * n) / (f - n);
	mat[3][3] =  0;
	return mat;
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
glm::mat4 setFrustum(float fovY, float aspectRatio, float front, float back) {
	float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
	float height = front * tangent;           // half height of near plane
	float width = height * aspectRatio;       // half width of near plane
											  // params: left, right, bottom, top, near, far
	return setFrustum(-width, width, -height, height, front, back);
}



//=============================================================================
// CALLBACKS
//=============================================================================

inline void CalculateFrameRate() {
	static float framesPerSecond=0.0f;
	static float lastTime=0.0f;
	static char strFrameRate[50];
	float currentTime=GetTickCount()*0.001f;
	++framesPerSecond;
	if(currentTime - lastTime > 1.0f) {
		lastTime = currentTime;
		std::sprintf(strFrameRate, "Current FPS:%d", int(framesPerSecond));
		SetWindowTextA(g_hWnd, strFrameRate);
		framesPerSecond = 0;
	}
}
void displayCB() {
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(Player.x, Player.y+4.5, Player.z-0.35,
			  Player.x-10*sin(pi*Player.a_yaw/180), Player.y+4.5+10*tan(pi*(Player.a_pitch)/180), Player.z-10*cos(pi*Player.a_yaw/180)-0.35,
			  0, 1, 0);
	// tramsform camera
	//    matrixView.identity();
	//    matrixView.translate(-cameraX, 0.0,-cameraY);
	//    matrixView.rotate(cameraAngleY, 0, 1, 0);
	//    matrixView.rotate(cameraAngleX, 1, 0, 0);
	//    matrixView.translate(0, 0, -cameraDistance);
	//@@ the equivalent code for using OpenGL routine is:
	//@@ glTranslatef(0, 0, -cameraDistance);
	//@@ glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	//@@ glRotatef(cameraAngleY, 0, 1, 0);   // heading
	// copy view matrix to OpenGL
	//    glLoadMatrixf(matrixView.get());
	drawGrid();                         // draw XZ-grid with default size
										// compute model matrix
										//    matrixModel.identity();
										//matrixModel.rotateZ(45);        // rotate 45 degree on Z-axis
										///matrixModel.rotateY(45);        // rotate 45 degree on Y-axis
										//matrixModel.rotateX(45);        // rotate 45 degree on Y-axis
										//matrixModel.translate(0, 1, 0); // move 2 unit up
										// compute modelview matrix
										//    matrixModelView = matrixView * matrixModel;
										// copy modelview matrix to OpenGL
										//    glLoadMatrixf(matrixModelView.get());
	drawAxis();
	drawModel();
	// draw info messages
	showInfo();
	glFlush();
	glPopMatrix();
	glutSwapBuffers();
	CalculateFrameRate();
}

void reshapeCB(int w, int h) {
	screenWidth = w;
	screenHeight = h;
	// set viewport to be the entire window
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	matrixProjection = setFrustum(45, (float)w/h, 1.0f, g_ViewDistance);
	glLoadMatrixf(&matrixProjection[0][0]);
	//@@ the equivalent OpenGL call
	//@@ gluPerspective(45.0f, (float)(w)/h, 1.0f, 100.0f); // FOV, AspectRatio, NearClip, FarClip
	// DEBUG
	///    std::cout << "===== Projection Matrix =====\n";
	///    std::cout << matrixProjection << std::endl;
	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
}

const GLfloat mouse_sensitivity=0.5;
void timerCB(int millisec) {
	//
	RECT t_rect; POINT t_point;
	GetClientRect(g_hWnd, &t_rect);
	GetCursorPos(&t_point);
	int midx=t_rect.left+(t_rect.right-t_rect.left)>>1;
	int midy=t_rect.top+(t_rect.bottom-t_rect.top)>>1;
	int curx=t_point.x, cury=t_point.y;
	float dx=midx-curx;
	if(fabs(dx)>=2) { Player.lookleft(mouse_sensitivity*dx); }
	float dy=midy-cury;
	if(fabs(dy)>=2) { Player.lookup(mouse_sensitivity*dy); }
	SetCursorPos(midx, midy);
	Player.rotturret();
	glutTimerFunc(millisec, timerCB, millisec);
	glutPostRedisplay();
}

void idleCB() {
	glutPostRedisplay();
}

void keyboardCB(unsigned char key, int x, int y) {
	switch(key) {
		case 27: // ESCAPE
			exit(0);
			break;
		case ' ':
			break;
		case 'W':case 'w':
		{
			Player.forward();
		} break;
		case 'S':case 's':
		{
			Player.backward();
		} break;
		case 'A':case 'a':
		{
			Player.turnleft();
		} break;
		case 'D':case 'd':
		{
			Player.turnright();
		} break;
		case 'z': // switch rendering modes (fill -> wire -> point)
		case 'Z':
			drawMode = ++drawMode % 3;
			if(drawMode == 0) {      // fill mode
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);
			} else if(drawMode == 1) { // wireframe mode
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			} else {                // point mode
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
			}
			break;
		case '+':
		{
			cameraDistance -=1;
		} break;
		case '-':
		{
			cameraDistance +=1;
		} break;
		default:
			;
	}
}
void keyboardSpecialCB(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
		{
			Player.turretturnup();
		} break;
		case GLUT_KEY_LEFT:
		{
			Player.turretturnleft();
		} break;
		case GLUT_KEY_DOWN:
		{
			Player.turretturndown();
		} break;
		case GLUT_KEY_RIGHT:
		{
			Player.turretturnright();
		} break;
		default:
			break;
	}
}

void mouseCB(int button, int state, int x, int y) {
	mouseX = x;
	mouseY = y;
	if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) { mouseLeftDown = true; } else if(state == GLUT_UP) { mouseLeftDown = false; }
	} else if(button == GLUT_RIGHT_BUTTON) {
		if(state == GLUT_DOWN) { mouseRightDown = true; } else if(state == GLUT_UP) { mouseRightDown = false; }
	} else if(button == GLUT_MIDDLE_BUTTON) {
		if(state == GLUT_DOWN) { mouseMidleDown = true; } else if(state == GLUT_UP) { mouseMidleDown = false; }
	}
}

void mouseMotionCB(int x, int y) {
	mouseX = x;
	mouseY = y;
	if(mouseLeftDown) {
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if(mouseRightDown) {
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
	if(mouseMidleDown) {
		//        cameraX += (x - mouseX);
		//        cameraY -= (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	//    if(dx<0){}
	//    else if(dx>0){Player.lookright();}
}
void mousePassiveMotionCB(int x, int y) {
	mouseX = x;
	mouseY = y;
	if(mouseLeftDown) {
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if(mouseRightDown) {
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
	if(mouseMidleDown) {
		//        cameraX += (x - mouseX);
		//        cameraY -= (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	//    if(dx<0){}
	//    else if(dx>0){Player.lookright();}
}
void mousewheelCB(int button, int dir, int x, int y) {
	if(dir>0) {
		cameraDistance -=1;
	} else {
		cameraDistance +=1;
	}
	return;
}
void exitCB() {
	//    glDeleteTextures(1,&Chassis_tex_ID);
	clearSharedMem();
}

/*
#include <iostream>  

void init();
void displayCB();
void idle();
void keyboardCB(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void mouseCB(int button, int state, int x, int y);
void mouseMotionCB(int x, int y);
void quitProgram(int quitStatus);

GLuint initShader(const GLenum shaderType, const char *shaderPath);
GLuint initProgram(GLuint vertexShader, GLuint fragmentShader);

void saveScreenShot(std::string fname);
void quitProgram();

const int WindowPosX = 0;
const int WindowPosY = 0;
const float DEG2RAD         = 3.141593f / 180;
extern int MainWindow = 0;
extern int WindowWidth = 450;
extern int WindowHeight = 450;

///////////////////////////////////////////////////////////////////////////////
// set a perspective frustum with 6 params similar to glFrustum()
// (left, right, bottom, top, near, far)
///////////////////////////////////////////////////////////////////////////////
glm::mat4 setFrustum(float l, float r, float b, float t, float n, float f) {
	glm::mat4 mat;
	mat[0][0]  =  2 * n / (r - l);
	mat[1][1]  =  2 * n / (t - b);
	mat[2][0]  = (r + l) / (r - l);
	mat[2][1]  = (t + b) / (t - b);
	mat[2][2] = -(f + n) / (f - n);
	mat[2][3] = -1;
	mat[3][2] = -(2 * f * n) / (f - n);
	mat[3][3] =  0;
	return mat;
}

///////////////////////////////////////////////////////////////////////////////
// set a symmetric perspective frustum with 4 params similar to gluPerspective
// (vertical field of view, aspect ratio, near, far)
///////////////////////////////////////////////////////////////////////////////
glm::mat4 setFrustum(float fovY, float aspectRatio, float front, float back) {
	float tangent = tanf(fovY/2 * DEG2RAD);   // tangent of half fovY
	float height = front * tangent;           // half height of near plane
	float width = height * aspectRatio;       // half width of near plane
											  // params: left, right, bottom, top, near, far
	return setFrustum(-width, width, -height, height, front, back);
}
void reshape(int w, int h) {
	WindowWidth = w;
	WindowHeight = h;
	// set render area  
	glViewport(0, 0, w, h);
	// set projection matrix  
	glMatrixMode(GL_PROJECTION);
	//glm::lookAt()
	glm::mat4 projectionMatrix = setFrustum(90.0f, (float)w / (float)h, 0.1f, 99.0f);
	glLoadMatrixf(&projectionMatrix[0][0]);
}


void quitProgram(int quitStatus) {
	glutDestroyWindow(MainWindow);
	exit(quitStatus);
}

int main(int argc, char *argv[]) {
	// 初始化FreeImage  
	// 初始化glut  
	glutInit(&argc, argv);
	// 设置glut显示方式  
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// 创建窗口  
	std::string windowName = argv[0];
	windowName = windowName.substr(windowName.find_last_of("/\\") + 1, windowName.length() - windowName.find_last_of("/\\") - 1);
	// 窗口创建方式1  
	{
		glutInitWindowPosition(WindowPosX, WindowPosY);
		glutInitWindowSize(WindowWidth, WindowHeight);
		MainWindow = glutCreateWindow(windowName.c_str());
	}
	// 窗口创建方式2  
	/*{
	MainWindow = glutCreateWindow(windowName.c_str());
	glutPositionWindow(WindowPosX, WindowPosY);
	glutReshapeWindow(WindowWidth, WindowHeight);
	}
	// 初始化Glew  
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(GLEW_OK != err) {
		std::cerr << "Error: " << glewGetString(err) << std::endl;
	}
	// 绑定显示函数  
	glutDisplayFunc(displayCB);
	// 绑定空闲函数  
	glutIdleFunc(displayCB);
	// 绑定键盘响应函数  
	glutKeyboardFunc(keyboardCB);
	// 绑定键盘Special响应函数  
	//glutSpecialFunc(specialKeyboard);
	// 绑定鼠标响应函数  
	glutMouseFunc(mouseCB);
	// 绑定鼠标移动函数  
	glutMotionFunc(mouseMotionCB);
	// 绑定Reshape函数  
	glutReshapeFunc(reshape);

	// 初始化  
	//init();

	// 开始主循环  
	glutMainLoop();

	// never come here  
	glutDestroyWindow(MainWindow);

	return 0;
}

void displayCB() {
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// save the initial ModelView matrix before modifying ModelView matrix
	//glPushMatrix();
	//glLoadIdentity();
	//gluLookAt(Player.x, Player.y+4.5, Player.z-0.35,
	//		  Player.x-10*sin(pi*Player.a_yaw/180), Player.y+4.5+10*tan(pi*(Player.a_pitch)/180), Player.z-10*cos(pi*Player.a_yaw/180)-0.35,
	//		  0, 1, 0);
	// tramsform camera
	//    matrixView.identity();
	//    matrixView.translate(-cameraX, 0.0,-cameraY);
	//    matrixView.rotate(cameraAngleY, 0, 1, 0);
	//    matrixView.rotate(cameraAngleX, 1, 0, 0);
	//    matrixView.translate(0, 0, -cameraDistance);
	//@@ the equivalent code for using OpenGL routine is:
	//@@ glTranslatef(0, 0, -cameraDistance);
	//@@ glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	//@@ glRotatef(cameraAngleY, 0, 1, 0);   // heading
	// copy view matrix to OpenGL
	//    glLoadMatrixf(matrixView.get());
	//drawGrid();                         // draw XZ-grid with default size
										// compute model matrix
										//    matrixModel.identity();
										//matrixModel.rotateZ(45);        // rotate 45 degree on Z-axis
										///matrixModel.rotateY(45);        // rotate 45 degree on Y-axis
										//matrixModel.rotateX(45);        // rotate 45 degree on Y-axis
										//matrixModel.translate(0, 1, 0); // move 2 unit up
										// compute modelview matrix
										//    matrixModelView = matrixView * matrixModel;
										// copy modelview matrix to OpenGL
										//    glLoadMatrixf(matrixModelView.get());
	//drawAxis();
	//drawModel();
	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 100, 0);
	glVertex3f(666, 200, 0);
	glVertex3f(400, 300, 0);
	glEnd();
	// draw info messages
	//showInfo();
	glFlush();
	glPopMatrix();
	glutSwapBuffers();
	//CalculateFrameRate();
}

bool mouseLeftDown;
bool mouseMidleDown;
bool mouseRightDown;
float mouseX, mouseY;
void mouseCB(int button, int state, int x, int y) {
	mouseX = x;
	mouseY = y;
	if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) { mouseLeftDown = true; } else if(state == GLUT_UP) { mouseLeftDown = false; }
	} else if(button == GLUT_RIGHT_BUTTON) {
		if(state == GLUT_DOWN) { mouseRightDown = true; } else if(state == GLUT_UP) { mouseRightDown = false; }
	} else if(button == GLUT_MIDDLE_BUTTON) {
		if(state == GLUT_DOWN) { mouseMidleDown = true; } else if(state == GLUT_UP) { mouseMidleDown = false; }
	}
}

void mouseMotionCB(int x, int y) {
	mouseX = x;
	mouseY = y;
	//if(mouseLeftDown) {
	//	cameraAngleY += (x - mouseX);
	//	cameraAngleX += (y - mouseY);
	//	mouseX = x;
	//	mouseY = y;
	//}
	//if(mouseRightDown) {
	//	cameraDistance -= (y - mouseY) * 0.2f;
	//	mouseY = y;
	//}
	//if(mouseMidleDown) {
	//	//        cameraX += (x - mouseX);
	//	//        cameraY -= (y - mouseY);
	//	mouseX = x;
	//	mouseY = y;
	//}
	//    if(dx<0){}
	//    else if(dx>0){Player.lookright();}
}
void keyboardCB(unsigned char key, int x, int y) {
	switch(key) {
		case 27: // ESCAPE
			exit(0);
			break;
		case ' ':
			break;
		//case 'W':case 'w':
		//{
		//	Player.forward();
		//} break;
		//case 'S':case 's':
		//{
		//	Player.backward();
		//} break;
		//case 'A':case 'a':
		//{
		//	Player.turnleft();
		//} break;
		//case 'D':case 'd':
		//{
		//	Player.turnright();
		//} break;
		//case 'z': // switch rendering modes (fill -> wire -> point)
		//case 'Z':
		//	drawMode = ++drawMode % 3;
		//	if(drawMode == 0) {      // fill mode
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//		glEnable(GL_DEPTH_TEST);
		//		glEnable(GL_CULL_FACE);
		//	} else if(drawMode == 1) { // wireframe mode
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//		glDisable(GL_DEPTH_TEST);
		//		glDisable(GL_CULL_FACE);
		//	} else {                // point mode
		//		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		//		glDisable(GL_DEPTH_TEST);
		//		glDisable(GL_CULL_FACE);
		//	}
		//	break;
		//case '+':
		//{
		//	cameraDistance -=1;
		//} break;
		//case '-':
		//{
		//	cameraDistance +=1;
		//} break;
		default:
			;
	}
}

/*

//#include "Camera.h"
//#include "Shader.h"
#include "PlayerCamera.h"
#include "ShaderProgramBase.h"
#include "SimpleModelShader.h"
#include "Model.h"




void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void Do_Movement();
PlayerCamera camera;// (glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLuint screenWidth = 800, screenHeight = 600;
HWND g_hWnd;
inline void CalculateFrameRate() {
	static float framesPerSecond=0.0f;
	static float lastTime=0.0f;
	static char strFrameRate[50];
	float currentTime=GetTickCount()*0.001f;
	++framesPerSecond;
	if(currentTime - lastTime > 1.0f) {
		lastTime = currentTime;
		std::sprintf(strFrameRate, "Current FPS:%d", int(framesPerSecond));
		SetWindowTextA(g_hWnd, strFrameRate);
		framesPerSecond = 0;
	}
}

GLuint loadTexture(GLchar* path) {
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_AUTO);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;

}
int main(int argc, char **argv) {

	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);
	g_hWnd=FindWindow(NULL, _T("LearnOpenGL"));
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);
	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	//装载shader
	Shader::SimpleModelShader shader;
	//装载模型与贴图
	Model ourModel("F:/Workspace/GLWOT/bin/nanosuit/nanosuit.obj");
	//地图
	GLuint Clouds_tex_ID, Map_tex_ID;
	GLuint planeVAO, planeVBO, planeEBO;
	{
		struct Vertex {
			// Position
			glm::vec3 Position;
			// Normal
			glm::vec3 Normal;
			// TexCoords
			glm::vec2 TexCoords;
		};
		Vertex mmap[]={
			{{ 5.0f, -0.5f,  5.0f},{1.0f, 1.0f,0.0f},{1.0f, 0.0f}},
			{{-5.0f, -0.5f,  5.0f},{1.0f, 1.0f,0.0f},{0.0f, 0.0f}},
			{{-5.0f, -0.5f, -5.0f},{1.0f, 1.0f,0.0f},{0.0f, 1.0f}},
			{{ 5.0f, -0.5f, -5.0f},{1.0f, 1.0f,0.0f},{1.0f, 1.0f}},
		};

		GLfloat planeVerticesOrder[] ={
			0,1,2,3
		};
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glGenBuffers(1, &planeEBO);

		glBindVertexArray(planeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);



		glBufferData(GL_ARRAY_BUFFER, 4, mmap, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), planeVerticesOrder, GL_STATIC_DRAW);


		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
		Clouds_tex_ID = loadTexture("Medium_Mark_I.bmp");
	}
	//// Game loop
	glfwSwapInterval(2);
	while(!glfwWindowShouldClose(window)) {
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check and call events
		glfwPollEvents();
		Do_Movement();
		// Clear the colorbuffer
		glClearColor(0.05f, 0.25f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.bind();   // <-- Don't forget this one!
						// Transformation matrices
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene

		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));   // It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
		ourModel.Draw(shader);

		{
		// Floor

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Clouds_tex_ID);
			glBindVertexArray(planeVAO);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		CalculateFrameRate();
		// Swap the buffers
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	//return 0;


	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	printf("hello world\n");
	//getchar();
	return 0;
}
// Moves/alters the camera positions based on user input
void Do_Movement() {
	//// Camera controls
	if(keys[GLFW_KEY_W]) { camera.ProcessKeyboard(FORWARD, deltaTime); }
	if(keys[GLFW_KEY_S]) { camera.ProcessKeyboard(BACKWARD, deltaTime); }
	if(keys[GLFW_KEY_A]) { camera.ProcessKeyboard(LEFT, deltaTime); }
	if(keys[GLFW_KEY_D]) { camera.ProcessKeyboard(RIGHT, deltaTime); }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, GL_TRUE); }
	if(action == GLFW_PRESS) { keys[key] = true; } else if(action == GLFW_RELEASE) { keys[key] = false; }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
	static bool firstMouse = true;
	if(firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

*/