#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Camera.h"
#include "Shader.h"
#include "Shapes.h"
#include "Light.h"
#include "Fluid.h"
#include "Magnet.h"


// Function prototypes
GLFWwindow* initGlfw();
void drawFluid(GLint modelLoc);
void drawMagnet(GLint modelLoc);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(0.0f, 50.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0, -45.0);
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];
glm::mat4 view;
glm::mat4 projection;

// Light
glm::vec3 lightPos(3.0f, 10.0f, 4.0f);
Light light = Light(lightPos, camera);

// Fluid
Fluid fluid = Fluid();

// Magnet attributes
Magnet magnet = Magnet(glm::vec3(0.0f, 7.0f, 0.0f));

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
	GLFWwindow* window = initGlfw();

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	shapes.setup();
	light.setup();

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Create camera transformations
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(light.lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(light.lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(light.lightingShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		fluid.update(deltaTime, magnet);

		drawFluid(modelLoc);

		drawMagnet(modelLoc);

		light.draw(view, projection);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

GLFWwindow* initGlfw() {
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Wooden Monkey", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	return window;
}

void drawFluid(GLint modelLoc) {
	light.lightingShader.Use();
	glBindVertexArray(shapes.cubeVAO);

	glUniform3f(glGetUniformLocation(light.lightingShader.Program, "material.ambient"), 0.75f, 0.75f, 0.75f);
	glUniform3f(glGetUniformLocation(light.lightingShader.Program, "material.diffuse"), 0.75f, 0.75f, 0.75f);
	glUniform3f(glGetUniformLocation(light.lightingShader.Program, "material.specular"), 0.15f, 0.15f, 0.15f);
	glUniform1f(glGetUniformLocation(light.lightingShader.Program, "material.shininess"), 1.0f);
	glm::mat4 model;
	for (int i = 0; i < fluid_i_count; i++) {
		for (int j = 0; j < fluid_j_count; j++) {
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(i - fluid_i_count / 2, 0, j - fluid_j_count / 2));
			model = glm::scale(model, glm::vec3(1, fluid. u[i][j], 1));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	glBindVertexArray(0);
}

void drawMagnet(GLint modelLoc) {
	light.lightingShader.Use();
	glBindVertexArray(shapes.cubeVAO);

	glUniform3f(glGetUniformLocation(light.lightingShader.Program, "material.ambient"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(light.lightingShader.Program, "material.diffuse"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(light.lightingShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(light.lightingShader.Program, "material.shininess"), 32.0f);
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, magnet.position);
	model = glm::scale(model, glm::vec3(2, 2, 2));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Magnet controls
	if (keys[GLFW_KEY_W])
		magnet.position += glm::vec3(0, 0, -1) * magnet.speed * deltaTime;
	if (keys[GLFW_KEY_S])
		magnet.position += glm::vec3(0, 0, 1) * magnet.speed * deltaTime;
	if (keys[GLFW_KEY_A])
		magnet.position += glm::vec3(-1, 0, 0) * magnet.speed * deltaTime;
	if (keys[GLFW_KEY_D])
		magnet.position += glm::vec3(1, 0, 0) * magnet.speed * deltaTime;
}