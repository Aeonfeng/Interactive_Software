//OBJ model loader program Dom Ralphs August 2020
#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp> //includes GLM
#include <glm/gtx/component_wise.hpp>
#include <vector>
#include <fstream>
#include <sstream>


#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0) //define key down for keyboard input
#define GLFW_KEY_SPACE   32

using namespace std;
using namespace glm;

GLuint texture1;

//struct for storing the data from the file
struct Vertex
{
	vec3 position; //postion vector of 3 which is x,y z postion of vertices
	vec2 texcoord; //textcord vector of 2 which is x,y  postion of textcords
	vec3 normal; //normals vector of 3 whihc is the x,y and z postion of normals
};

//struct for storing the temp data from the file
struct VertRef
{
	VertRef(int v, int vt, int vn) //vert ref for v = vertices, vt =  vertex textcords, vn = vertex normals
		: v(v), vt(vt), vn(vn) // used to store the data in each int.
	{ }
	int v, vt, vn;
};


vector< Vertex > LoadOBJ(istream& in) //class of vector, LoadOBJ struct for storing the data from the file
{
	vector< Vertex > verts; //set up a vector for the verts

	vector< vec4 > positions(1, vec4(0, 0, 0, 0)); //set up the positions as a vec of 4
	vector< vec3 > texcoords(1, vec3(0, 0, 0)); // set up textcords as a vector of 3
	vector< vec3 > normals(1, vec3(0, 0, 0)); // set the normals as a vector of 3


	//read the obj file
	string line; //set line as a string
	while (getline(in, line))  //while there is data, read each line
	{
		istringstream lineInfo(line); // set up the lineInfo as a stringstream using the line data
		string lineType; //set up the linetype as a string, used for checking the first letter of the line
		lineInfo >> lineType; //lineInfo into line type


		//check for vertex  
		if (lineType == "v") //check if the start of the line begains with a V
		{
			float x = 0, y = 0, z = 0, w = 1;   //set up the floats to store each data cord in
			lineInfo >> x >> y >> z >> w; // add each cord data to the floats x = vertex data x and  y  = vertex data y etc...
			positions.push_back(vec4(x, y, z, w)); //add the data of x,y,z and w into the positions vector
		}

		//check for texture cords
		if (lineType == "vt") //check if the start of the line begains with a vt
		{
			float u = 0, v = 0, w = 0; //set up the floats to store each text data cord in
			lineInfo >> u >> v >> w; // add each texture cord data to the floats u = texture data x and  v  = vertex data y etc...
			texcoords.push_back(vec3(u, v, w)); //add the data of u, v and w into the textcords vector
		}

		//check for normals cords
		if (lineType == "vn") //check if the start of the line begains with a vn
		{
			float i = 0, j = 0, k = 0; //set up the floats to store each normals data cord in
			lineInfo >> i >> j >> k;  // add each normal cord data to the floats i = normal data x and j = normal data y etc....
			normals.push_back(normalize(vec3(i, j, k))); //add the data of i, j and k into the normals vector
		}

		//check for faces
		if (lineType == "f") //check if the start of the line begains with a f
		{
			vector< VertRef > refs; //set up a struct to store the refrences for the faces
			string refStr;  //set up a string called ref string
			while (lineInfo >> refStr) //while there is data to read
			{
				istringstream ref(refStr); // set string object as a string stream
				string vStr, vtStr, vnStr;  //set up strings for vertex String vStr, texture string vtStr, normal string vnStr to split the faces data up

				getline(ref, vStr, '/');  // split the faces up with / vertex string as vStr
				getline(ref, vtStr, '/'); // split the faces up with / texture string as vtSTr
				getline(ref, vnStr, '/'); // split the faces up with / normal string as vnStr

				int v = atoi(vStr.c_str());  //copy vStr into a c-string and use atoi to convert to int
				int vt = atoi(vtStr.c_str()); //copy vtStr into a c-string and use atoi to convert to int
				int vn = atoi(vnStr.c_str()); //copy vnStr into a c-string and use atoi to convert to int

				v = (v >= 0 ? v : positions.size() + v); //ternary operator, if v >= 0 then v // else  size of vertices + v
				vt = (vt >= 0 ? vt : texcoords.size() + vt); //ternary operator, if vt >= 0 then vt // else  size of vertices + vt
				vn = (vn >= 0 ? vn : normals.size() + vn); //ternary operator, if vn >= 0 then vn // else  size of vertices + vn
				refs.push_back(VertRef(v, vt, vn)); //add element v,vt and vn to end of vector verTexRef
			}

			if (refs.size() < 3) //check if face ref is less than 3 
			{
				//if its less than 3 it isn't in the correct format or isn't a face ref so skip
				printf("err faces can't be read, not correct format \n");
				continue;
			}

			// triangulate, assuming n>3 poloygons are convex (all interier angles are less than 180 degrees) and coplanar ()
			VertRef* p[3] = { &refs[0], NULL, NULL }; //store data in p 
			for (size_t i = 1; i + 1 < refs.size(); ++i) //size_t unasigned int, for loop incrementimg before the expression is evaluated 
			{
				p[1] = &refs[i + 0]; //point to the memory location of the first FaceRefs
				p[2] = &refs[i + 1]; //point to the memory location of the second FaceRefs

				vec3 U(positions[p[1]->v] - positions[p[0]->v]); //add data to the vector
				vec3 V(positions[p[2]->v] - positions[p[0]->v]); //add data to the vector
				vec3 faceNormal = normalize(cross(U, V)); //check for crossed correlation between the vectors of U and V

				for (size_t j = 0; j < 3; ++j) //for inner loop to add the data
				{
					Vertex vert; //set vert in the vertex
					vert.position = vec3(positions[p[j]->v]); //vert position = vector of 3
					vert.texcoord = vec2(texcoords[p[j]->vt]); //vert textcords = vector of 2
					vert.normal = (p[j]->vn != 0 ? normals[p[j]->vn] : faceNormal); //if p is less or greater than  not 0 then normals p is less or greater then vn ie.. the plain is 0 else face normal
					verts.push_back(vert); // add data from vert back to the verts.
				}
			}
		}
	}

	return verts; //return the verts
}

//set up the keybpard inputs
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) { //set the esc key as exit 
		exit(0);
	}
	if (key == 32) { // set the space bar as worframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //set object as wireframe
	}
}

int btn; //set up btn as int
ivec2 startMouse; //set up the mouse 
ivec2 startRot, curRot; // set the rotation and current rotation
ivec2 startTrans, curTrans; //start transfer fopr moving object around window
void mouse(int button, int state, int x, int y) //set up the mouse movement function for the object
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) //if the mouse left button is pressed down 
	{
		btn = button;
		startMouse = ivec2(x, glutGet(GLUT_WINDOW_HEIGHT) - y); //check the height of the window
		startRot = curRot; //start the roation to be current roatation
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) //if the right button is pressed down
	{
		btn = button;
		startMouse = ivec2(x, glutGet(GLUT_WINDOW_HEIGHT) - y); //check the height of the window
		startTrans = curTrans; // start transfer as current movement of the object
	}
	
}

void motion(int x, int y) //set up motion 
{
	ivec2 curMouse(x, glutGet(GLUT_WINDOW_HEIGHT) - y); //check height of window
	if (btn == GLUT_LEFT_BUTTON)
	{
		curRot = startRot + (curMouse - startMouse); //start the mouse so object rotates direction the mouse is moved
	}
	else if (btn == GLUT_RIGHT_BUTTON)
	{
		curTrans = startTrans + (curMouse - startMouse); //start the mouse so object moves the direction the mouse is moved. 
	}
	glutPostRedisplay(); //set as displayed
}

//model 1
vector< Vertex > model; //set the vector for the object model
void display()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //set colour of model
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //set buffer


	glMatrixMode(GL_PROJECTION); //set the matrix projection
	glLoadIdentity();
	double w = glutGet(GLUT_WINDOW_WIDTH); //set center point
	double h = glutGet(GLUT_WINDOW_HEIGHT); //set centre point
	double ar = w / h;
	glTranslatef(curTrans.x / w * 2, curTrans.y / h * 2, 0);
	gluPerspective(60, ar, 0.1, 100);

	glMatrixMode(GL_MODELVIEW); //set matrix model view
	glLoadIdentity(); //load identy matrix
	glTranslatef(0, 0, -10); //set translate of matrix



	glPushMatrix(); //add matrix to pipeline
	{
		glRotatef(curRot.x % 360, 0, 1, 0); //rotate left
		glRotatef(-curRot.y % 360, 1, 0, 0);//rotate right

		// object
		glColor3ub(19, 122, 154); //set object colour
		glEnableClientState(GL_VERTEX_ARRAY); //get vertex array
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); //get texture cord array
		glEnableClientState(GL_NORMAL_ARRAY); //get normals array
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &model[0].position); //refrence vertex postion pointer
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &model[0].texcoord); //reference textcoord pointer 
		glNormalPointer(GL_FLOAT, sizeof(Vertex), &model[0].normal); //refreence normal pointer
		glDrawArrays(GL_TRIANGLES, 0, model.size()); // draw the arrays
		glDisableClientState(GL_VERTEX_ARRAY); //discard the client states to use the buffers
		glDisableClientState(GL_TEXTURE_COORD_ARRAY); //discard the client states to use the buffers
		glDisableClientState(GL_NORMAL_ARRAY); //discard the client states to use the buffers
	}
	glPopMatrix(); //remove matrix from  stack

	glutSwapBuffers(); //swap the buffers
}


//second model under testing phase at present #################
//model 2
//vector< Vertex > model2;
//void display2()
//{
//	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	double w = glutGet(GLUT_WINDOW_WIDTH);
//	double h = glutGet(GLUT_WINDOW_HEIGHT);
//	double ar = w / h;
//	glTranslatef(curTrans.x / w * 2, curTrans.y / h * 2, 0);
//	gluPerspective(60, ar, 0.1, 100);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glTranslatef(0, 0, -10);
//
//	glPushMatrix();
//	{
//		glRotatef(curRot.x % 360, 0, 1, 0);
//		glRotatef(-curRot.y % 360, 1, 0, 0);
//
//		// object
//		glColor3ub(19, 122, 154); //set object colour
//		glEnableClientState(GL_VERTEX_ARRAY);
//		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//		glEnableClientState(GL_NORMAL_ARRAY);
//		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &model2[0].position);
//		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &model2[0].texcoord);
//		glNormalPointer(GL_FLOAT, sizeof(Vertex), &model2[0].normal);
//		glDrawArrays(GL_TRIANGLES, 0, model2.size());
//		glDisableClientState(GL_VERTEX_ARRAY);
//		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//		glDisableClientState(GL_NORMAL_ARRAY);
//	}
//	glPopMatrix();
//
//	glutSwapBuffers();
//}



// return the min/max points of pts
template< typename Vec >
pair< Vec, Vec > GetExtents(const Vec* pts, size_t stride, size_t count) //findong the min and max points fro the transformations
{
	unsigned char* base = (unsigned char*)pts;
	Vec pmin(*(Vec*)base);
	Vec pmax(*(Vec*)base);
	for (size_t i = 0; i < count; ++i, base += stride)
	{
		const Vec& pt = *(Vec*)base;
		pmin = glm::min(pmin, pt); //set the min points
		pmax = glm::max(pmax, pt); //set the max points
	}

	return make_pair(pmin, pmax);
}

// Centers geometry around the origin of the object and then scales to fit in the window 
template< typename Vec >
void CenterAndScale(Vec* pts, size_t stride, size_t count, const typename Vec::value_type& size) //set up center and scale function 
{
	typedef typename Vec::value_type Scalar;

	// get min and the max extents
	pair< Vec, Vec > exts = GetExtents(pts, stride, count);

	// center and scale for the object
	const Vec center = (exts.first * Scalar(0.5)) + (exts.second * Scalar(0.5f));

	const Scalar factor = size / glm::compMax(exts.second - exts.first);
	unsigned char* base = (unsigned char*)pts;
	for (size_t i = 0; i < count; ++i, base += stride)
	{
		Vec& pt = *(Vec*)base;
		pt = ((pt - center) * factor);
	}
}

//seting up a second diaply, work in progress. 
//void display2()
//{
//
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	rectangle();
//	text();
//
//	glFlush();
//}




//#################################################################################################################
//Texture  //work in progress


//int image_width = 0;
//int image_height = 0;
//unsigned char* image = SOIL_load_image("media/textures/Creeper-obj/Creeper.obj", &image_width, &image_height, NULL, SOIL_LOAD_RGBA);
//
//GLuint texture0;
//glGenTextures(1, &texture0);
//glBindTexture(GL_TEXTURE_ID, texture0); 
//
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // s= x cord
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T= y cord
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);  //magnification 
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  //minification 
//
//glTextImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
//glGenerateMipmap(GL_TEXTURE_2D);
//
//
//glActiveTexture(0);
//glBindTexture(GL_TEXTURE_2D, 0);
//SOIL_free_image_data(image);
//###############################################################################################################



//set main program
int main(int argc, char** argv)
{
	//file locations for easy chnaging when testing 
	//ifstream myFile("media/textures/Creeper-obj/Creeper.obj"); //Creeper obj
	//ifstream myFile("media/textures/Teddy-obj/teddy.obj"); //Teddy obj
	ifstream myFile("media/textures/Porsche-obj/porsche.obj"); //Car obj
	//ifstream myFile("media/textures/pouf-obj/Pouf.obj"); //pouf obj
	//ifstream myFile("media/textures/LowPolyBoat-obj/low_poly_boat.obj"); //Boat obj

//########################## adding a file path fetaure, work in progress
	//string path = NULL;
	//printf("please enter file path");
	//cin >> path;
	//ifstream ifile(path);
//###############################
	model = LoadOBJ(myFile);
	//model2 = LoadOBJ(myFile2); //second model for testing, work in progress
	CenterAndScale(&model[0].position, sizeof(Vertex), model.size(), 5); //size of the model 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); //set diasplay mode
	glutInitWindowSize(800, 800); //set window size 
	glutCreateWindow("Model Loader"); //set window name 
	glutDisplayFunc(display); //set display 
//################################# testing a second model display. work in progress
	//glutDisplayFunc(display2);
//################################
	glutMouseFunc(mouse); //start the mouse function function
	glutMotionFunc(motion); //start the mouse function motion
	glutKeyboardFunc(keyboard);
	glEnable(GL_DEPTH_TEST); //enable and use the depth test to store z cords and check depth so triangle render in the correct places
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //fill object with colour
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	glutMainLoop(); //start main loop
	
	return 0;
} //end of main