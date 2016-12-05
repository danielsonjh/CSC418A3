#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "Light.h"
#include "Fluid.h"

class Turtle {

public:

	Turtle(glm::vec3 _root_position, Fluid* _fluid, Light* _light) {
		root_position = _root_position;
		fluid = _fluid;
		light = _light;
	}

	void update_movement(bool keys[], float deltaTime) {

		if (keys[GLFW_KEY_UP]) {
			speed += acceleration * deltaTime;
		}
		else if (keys[GLFW_KEY_DOWN]) {
			speed -= acceleration * deltaTime;
		}
		else {
			speed *= powf(speed_decay_per_sec, deltaTime);
		}

		speed = glm::clamp(speed, -max_speed, max_speed);
		root_position += glm::rotate(glm::vec3(0, 0, 1), root_angle, glm::vec3(0, 1, 0)) * speed * deltaTime;
		root_position.x = glm::clamp(root_position.x, 0.0f, (float)fluid_i_count-1);
		root_position.z = glm::clamp(root_position.z, 0.0f, (float)fluid_j_count-1);

		if (keys[GLFW_KEY_LEFT]) {
			root_angle += angle_speed * deltaTime;
		}
		if (keys[GLFW_KEY_RIGHT]) {
			root_angle -= angle_speed * deltaTime;
		}
	}

	void draw() {
		light->lightingShader.Use();
		GLint modelLoc = glGetUniformLocation(light->lightingShader.Program, "model");

		// Set the color
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.ambient"), 0.4f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.diffuse"), 0.4f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(light->lightingShader.Program, "material.shininess"), 1.0f);

		// ROOT
		glm::mat4 model = glm::mat4();
		matrixStack.push(model);
			int i = round(root_position.x);
			int j = round(root_position.z);
			model = glm::translate(model, glm::vec3(
				root_position.x - fluid_i_count / 2,
				(fluid->u[i][j] + 0.5*sin(3*glfwGetTime())) / 2.0f,
				root_position.z - fluid_j_count / 2));
			model = glm::rotate(model, root_angle, glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(2.0f, 1.5f, 3.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			shapes.drawCube();
			model = glm::scale(model, glm::vec3(1/2.0f, 1/1.5f, 1/3.0f));

			glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.ambient"), 0.0f, 0.2f, 0.1f);
			glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.diffuse"), 0.0f, 0.4f, 0.1f);
			glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.specular"), 0.0f, 0.1f, 0.0f);
			glUniform1f(glGetUniformLocation(light->lightingShader.Program, "material.shininess"), 1.0f);

			// HEAD
			matrixStack.push(model);
			model = glm::translate(model, glm::vec3(0, 0.5, 2));
			model = glm::scale(model, glm::vec3(1, 1, 1));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			shapes.drawCube();
			model = matrixStack.top();
			matrixStack.pop();

			// ARM 1
			matrixStack.push(model);
				model = glm::rotate(model, 0.3f*sinf(3*glfwGetTime()), glm::vec3(0, 1, 0));
				model = glm::translate(model, glm::vec3(1, 0, 1));
				model = glm::scale(model, glm::vec3(1, 0.5f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				shapes.drawCube();
			model = matrixStack.top();
			matrixStack.pop();

			// ARM 2
			matrixStack.push(model);
				model = glm::rotate(model, -0.3f*sinf(3 * glfwGetTime()), glm::vec3(0, 1, 0));
				model = glm::translate(model, glm::vec3(-1, 0, 1));
				model = glm::scale(model, glm::vec3(1, 0.5f, 0.5f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				shapes.drawCube();
			model = matrixStack.top();
			matrixStack.pop();

			// LEG 1
			matrixStack.push(model);
				model = glm::translate(model, glm::vec3(-0.5, 0, -1.5));
				model = glm::scale(model, glm::vec3(0.5, 0.5, 1));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				shapes.drawCube();
			model = matrixStack.top();
			matrixStack.pop();

			// LEG 2
			matrixStack.push(model);
				model = glm::translate(model, glm::vec3(0.5, 0, -1.5));
				model = glm::scale(model, glm::vec3(0.5, 0.5, 1));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				shapes.drawCube();
			model = matrixStack.top();
			matrixStack.pop();

		model = matrixStack.top();
		matrixStack.pop();
	}
	
private:
	Fluid* fluid;
	Light* light;

	glm::vec3 root_position;
	std::stack<glm::mat4> matrixStack;

	float acceleration = 8.0f;
	float max_speed = 8.0f;
	float speed = 0.0f;
	float speed_decay_per_sec = 0.75f;
	float root_angle;
	float angle_speed = 1.0f;
};