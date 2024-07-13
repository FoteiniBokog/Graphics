// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"
#define 	GLFW_KEY_W   87
#define 	GLFW_KEY_A   65
#define 	GLFW_KEY_X   88
#define 	GLFW_KEY_D   68
#define 	GLFW_KEY_MINUS   45 
#define 	GLFW_KEY_KP_ADD   334

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;


glm::mat4 getViewMatrix() {
	return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}


// Initial position 
float x = 60.0f;
float y = 0.0f;
float z = 60.0f;

glm::vec3 position = glm::vec3(x, y, z); //initialize the position of the camera

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;

// Initial vertical angle : none
float verticalAngle = 0.0f;

// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second




void computeMatricesFromInputs() {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);


	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Strafe right around x axis up
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		x += 0.5;
		y += 0.0;
		z += 0.0;
		position = glm::vec3(x, y, z);
		position += up * deltaTime * speed;

	}

	// Strafe left around x axis down

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		x -= 0.5;
		y -= 0.0;
		z -= 0.0;
		position = glm::vec3(x, y, z);
		position -= deltaTime * speed;

	}

	// Strafe right around y axis up
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		x += 0.0;
		y += 0.1;
		z += 0.0;
		position = glm::vec3(x, y, z);
		position += up * deltaTime * speed;

	}

	// Strafe left around y axis down

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		x -= 0.0;
		y -= 0.1;
		z -= 0.0;
		position = glm::vec3(x, y, z);
		position -= deltaTime * speed;

	}
	// zoom in
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
		x += 0.5;
		y += 0.0;
		z += 0.5;
		position = glm::vec3(x, y, z);
		position += up * speed * deltaTime;

	}
	// zoom out
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
		x -= 0.5;
		y = 0.0;
		z -= 0.5;
		position = glm::vec3(x, y, z);
		position -= up * deltaTime * speed;
	}


	
	


	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);


	// Camera matrix for the sun
	ViewMatrix = glm::lookAt(
		position,
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)

	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}