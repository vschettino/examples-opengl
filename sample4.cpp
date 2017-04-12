#include <math.h> //For expf()
#include <GL/gl.h> //GLUT library
#include <GL/freeglut.h>
#include "Quad.h" //the Quad class definition
#include "Point3d.h"



float Xmin	= -5, Xmax	= 5; //Range of variable x that we sample
float Ymin	= -5, Ymax	= 5; //Range of variable y that we sample
int   Nx	= 30, Ny	= 30; //Number of samples taken along the x and y axes

float dx = (Xmax-Xmin)/(Nx-1); //Size of a cell along the x axis
float dy = (Ymax-Ymin)/(Ny-1); //Size of a cell along the y axis


float fov = 60;	//Perspective projection parameters
float aspect = 1; 
float z_near = 0.1; 
float z_far  = 30;

float eye_x = 10, eye_y = 10, eye_z = 10; //Modelview (camera extrinsic) parameters
float c_x   = 0,  c_y   = 0,  c_z   = 0;
float up_x  = 0,  up_y  = 0,  up_z  = 1;


float f(float x, float y) //A simple two-variable function to plot 
{	//The function is samples 
	float ret = 8*expf(-(x*x+y*y)/5);
	return ret;
}

Point3d computeNormal(const Point3d& p)	 //Given a point p on the function's domain, computes the normal 
{	//of the graph of the function at p
	Point3d pr(p.x+dx,p.y,f(p.x+dx,p.y)); //For this, we compute four normals of the triangles formed by p and neighbor
	Point3d pl(p.x-dx,p.y,f(p.x-dx,p.y)); //grid points (pr,pl,pu,pd), and next average them.
	Point3d pu(p.x,p.y+dy,f(p.x,p.y+dy));
	Point3d pd(p.x,p.y-dy,f(p.x,p.y-dy));

	Point3d n1 = (pr-p).cross(pu-p); n1.normalize(); //Compute the four normals of the above triangles
	Point3d n2 = (pu-p).cross(pl-p); n2.normalize();
	Point3d n3 = (pl-p).cross(pd-p); n3.normalize();
	Point3d n4 = (pd-p).cross(pr-p); n4.normalize();
	
	Point3d n = n1+n2+n3+n4; //Compute the average normal
	n.normalize(); //Normalize the result, since it should be unit length

	return n;
}


void viewing(int W, int H) //Window resize function, sets up viewing parameters (GLUT callback function)
{
	glMatrixMode(GL_MODELVIEW); //1. Set the modelview matrix (including the camera position and view direction)
	glLoadIdentity ();										
	gluLookAt(eye_x,eye_y,eye_z,c_x,c_y,c_z,up_x,up_y,up_z);

	glMatrixMode (GL_PROJECTION); //2. Set the projection matrix
	glLoadIdentity ();
	gluPerspective(fov,float(W)/H,z_near,z_far);

	glViewport(0,0,W,H); //3. Set the viewport to the entire size of the rendering window
}



void draw() //Render the height plot (GLUT callback function)
{
	glClearColor(1,1,1,1); //1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glShadeModel(GL_SMOOTH); //5. Set shading parameters: use flat shading
	float light[4] = {1,1,1,0}; //Enable and set one directional light
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_POSITION,light);
    glEnable(GL_COLOR_MATERIAL); //Tell OpenGL to use the values passed by glColor() as material properties
	glColor3f(0,1,0); //Draw all quads filled with green
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for(float x=Xmin;x<=Xmax-dx;x+=dx) //2. Render the height plot as a set of quads
	 for(float y=Ymin;y<=Ymax-dy;y+=dy)	
	 {
		Point3d p1(x,y,f(x,y));	//Compute the four vertices of the quad
		Point3d p2(x+dx,y,f(x+dx,y));	
		Point3d p3(x+dx,y+dy,f(x+dx,y+dy));	
		Point3d p4(x,y+dy,f(x,y+dy));	
		
		Point3d n1 = computeNormal(p1);	//Compute the four normals at the quad vertices
		Point3d n2 = computeNormal(p2);
		Point3d n3 = computeNormal(p3);
		Point3d n4 = computeNormal(p4);
		
		Quad q1; //Draw the quad. For each vertex, first add the normal, then the vertex
		q1.addNormal(n1.data);							
		q1.addPoint(p1.x,p1.y,p1.z);					
		q1.addNormal(n2.data);							
		q1.addPoint(p2.x,p2.y,p2.z); 
		q1.addNormal(n3.data);							
		q1.addPoint(p3.x,p3.y,p3.z); 
		q1.addNormal(n4.data);							
		q1.addPoint(p4.x,p4.y,p4.z); 
		q1.draw();
	 }	
	  
	glutSwapBuffers(); //3. Rendering ready, ask GLUT to show the back buffer
}



int main(int argc,char* argv[])	//Main program
{
   glutInit(&argc, argv); //1. Initialize the GLUT toolkit
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);	//2. Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
   glutInitWindowSize(500,500);	//3. Tell GLUT how large are the windows we want to create next
   glutCreateWindow("4. Smooth shaded graph"); //4. Create our window
   
   glutDisplayFunc(draw); //5. Add a drawing callback to the window	
   glutReshapeFunc(viewing); //6. Add a resize callback to the window
   glutMainLoop(); //7. Start the event loop that displays the graph and handles window-resize events
   
   return 0;
}




