

// Include standard headers
#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <string.h>
#include <Windows.h> //for the caps lock

#define 	GLFW_KEY_Q   81
#define GLFW_KEY_SPACE   32


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"
#include "objloader.hpp"

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}



// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	return true;
}



int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(800, 800, "SOLAR SYSTEM", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 800 / 2, 800 / 2);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");



	// Load the textures
	//for the sun

	int width, height, nrChannels;
	unsigned char* data = stbi_load("sun.jpg", &width, &height, &nrChannels, 0);

	//for the planet p
	int width1, height1, nrChannels1;
	unsigned char* data1 = stbi_load("planet.jpg", &width1, &height1, &nrChannels1, 0);

	//for the planet mars
	int width3, height3, nrChannels3;
	unsigned char* data3 = stbi_load("2k_mars.jpg", &width3, &height3, &nrChannels3, 0);


	//for the meteor
	int width2, height2, nrChannels2;
	unsigned char* data2 = stbi_load("meteor.jpg", &width2, &height2, &nrChannels2, 0);

	if (data || data1 || data2 || data3)
	{

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");


	//for the sun
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	bool res = loadOBJ("sun.obj", vertices, uvs, normals);

	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);


	///////////////////////////////////////////for the planet
	GLuint textureID1;
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Read our .obj file
	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec3> normals1;
	std::vector<glm::vec2> uvs1;
	bool res1 = loadOBJ("planet.obj", vertices1, uvs1, normals1);

	// Load it into a VBO
	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);

	GLuint uvbuffer1;
	glGenBuffers(1, &uvbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
	glBufferData(GL_ARRAY_BUFFER, uvs1.size() * sizeof(glm::vec2), &uvs1[0], GL_STATIC_DRAW);

	/////////////////////////////////////////////////////////////for the meteor
	GLuint textureID2;
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Read our .obj file
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec3> normals2;
	std::vector<glm::vec2> uvs2;
	bool res2 = loadOBJ("meteor.obj", vertices2, uvs2, normals2);

	// Load it into a VBO
	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);

	GLuint uvbuffer2;
	glGenBuffers(1, &uvbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);
	//**************************BONUS************************************************
	/////////////////////////////////////////////////////////////for the planet mars 
	GLuint textureID3;
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Read our .obj file
	std::vector<glm::vec3> vertices3;
	std::vector<glm::vec3> normals3;
	std::vector<glm::vec2> uvs3;
	bool res3 = loadOBJ("planet.obj", vertices3, uvs3, normals3);

	// Load it into a VBO
	GLuint vertexbuffer3;
	glGenBuffers(1, &vertexbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
	glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &vertices3[0], GL_STATIC_DRAW);

	GLuint uvbuffer3;
	glGenBuffers(1, &uvbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer3);
	glBufferData(GL_ARRAY_BUFFER, uvs3.size() * sizeof(glm::vec2), &uvs3[0], GL_STATIC_DRAW);

	glm::mat4 ModelMatrix = glm::mat4(1.0); //for the sun 
	glm::mat4 ModelMatrix1 = glm::mat4(1.0); //for the planet
	glm::mat4 ModelMatrix2 = glm::mat4(1.0); //for the metor
	glm::mat4 ModelMatrix3 = glm::mat4(1.0); //for the planet mars bonus


	//here we create deltaTime to use it for the rotation of the planet p
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime); //we create deltatime to use it for the rotation of the planet

	ModelMatrix1 = glm::translate(ModelMatrix1, glm::vec3(25.0f, 0.0f, 0.0f)); //for the planet
	ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(45.0f, 0.0f, 0.0f)); //for the planet mars bonus
	ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(60.0f, 0.0f, 60.0f)); //for the meteor apo to shmeio tou parathrhth
	int meteor = 0;

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID); // Use our shader

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;



		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);


		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(TextureID, 0);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//for the planet 
		//////////////////////////////////////////////////

		glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);



		glBindTexture(GL_TEXTURE_2D, textureID1);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices1.size());

		//movement of the planet p around the sun 
		ModelMatrix1 = glm::translate(ModelMatrix1, glm::vec3(0.0f, 0.0f, -0.02f));
		ModelMatrix1 = glm::rotate(ModelMatrix1, glm::radians(30.0f) * deltaTime * 650, glm::vec3(0, 1, 0));


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//************************BONUS*******************
		///////////////////for the planet mars
		glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);



		glBindTexture(GL_TEXTURE_2D, textureID3);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer3);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices3.size());

		//here we create the movement of the planet mars
		ModelMatrix3 = glm::translate(ModelMatrix3, glm::vec3(0.0f, 0.0f, -0.04f));
		ModelMatrix3 = glm::rotate(ModelMatrix3, glm::radians(30.0f) * deltaTime * 650, glm::vec3(0, 4, 0));


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		//for the meteor
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || meteor == 1) {
			meteor = 1;

			glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
			glBindTexture(GL_TEXTURE_2D, textureID2);
			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
			glVertexAttribPointer(
				0,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
			glVertexAttribPointer(
				1,                                // attribute
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, vertices2.size());
			ModelMatrix2 = glm::translate(ModelMatrix2, glm::vec3(-0.02f, 0.0f, -0.02f));




			//elegxos sygkroushs
			/*if ((MVP1[0][0] + 5) - (MVP2[0][0] + 2) > 0) { //meteor me hlio

			}
			else if ((MVP1[0][0] + 5) - (MVP2[0][0] + 2) <= 0 ){        //collision
				glm::mat4 ProjectionMatrix = getProjectionMatrix();
				glm::mat4 ViewMatrix = getViewMatrix();
				glm::mat4 ModelMatrix = glm::mat4(1.0);
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

			}*/

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);


		}
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();


	}
	//press	Q to close the window
	while (((glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS) || (GetKeyState(VK_CAPITAL) & 0x0001) == 0) &&
		(glfwWindowShouldClose(window) == 0));





	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);

	//delete buffer for the planet p
	glDeleteBuffers(1, &vertexbuffer1);
	glDeleteBuffers(1, &uvbuffer1);

	//delete buffers for the meteor
	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteBuffers(1, &uvbuffer2);

	//delete buffers for the planet mars
	glDeleteBuffers(1, &vertexbuffer3);
	glDeleteBuffers(1, &uvbuffer3);

	glDeleteProgram(programID);

	//delete texture for the sun
	glDeleteTextures(1, &textureID);

	//delete texture for the planet p
	glDeleteTextures(1, &textureID1);

	//delete texture for the meteor
	glDeleteTextures(1, &textureID2);

	//delete texture for the planet mars
	glDeleteTextures(1, &textureID3);


	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}