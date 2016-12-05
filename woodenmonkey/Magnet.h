#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Magnet {

public:
	float strength = 350.0f;
	float speed = 25;
	glm::vec3 position;

	Magnet(glm::vec3 _position, Light* _light) {
		position = _position;
		light = _light;
	}

	void update_movement(bool keys[], float deltaTime) {
		if (keys[GLFW_KEY_W])
			position += glm::vec3(0, 0, -1) * speed * deltaTime;
		if (keys[GLFW_KEY_S])
			position += glm::vec3(0, 0, 1) * speed * deltaTime;
		if (keys[GLFW_KEY_A])
			position += glm::vec3(-1, 0, 0) * speed * deltaTime;
		if (keys[GLFW_KEY_D])
			position += glm::vec3(1, 0, 0) * speed * deltaTime;
	}

	void draw() {
		light->lightingShader.Use();
		GLint modelLoc = glGetUniformLocation(light->lightingShader.Program, "model");

		glBindVertexArray(shapes.cubeVAO);

		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.ambient"), 0.0f, 0.25f, 0.3f);
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.diffuse"), 0.25f, 0.25f, 0.25f);
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(light->lightingShader.Program, "material.shininess"), 32.0f);
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(2, 2, 2));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
	}

private:
	Light* light;
};