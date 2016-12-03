#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Light {

public:
	Shader lampShader;
	Shader lightingShader;
	
	Light(glm::vec3 _position, Camera _camera) {
		position = _position;
		camera = _camera;
	}

	void setup() {
		lampShader = Shader("shaders/lamp.vert", "shaders/lamp.frag");
		lightingShader = Shader("shaders/default.vert", "shaders/default.frag");
		setupLightingShader();
	}

	void draw(glm::mat4 view, glm::mat4 projection) {
		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		GLint modelLoc = glGetUniformLocation(lampShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lampShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(shapes.cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

private:
	glm::vec3 position;
	Camera camera;

	void setupLightingShader() {
		lightingShader.Use();
		GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(lightPosLoc, position.x, position.y, position.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
		// Set lights properties
		glm::vec3 lightColor;
		lightColor.x = 0;
		lightColor.y = 0.75;
		lightColor.z = 1;
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.9f); // Decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.4f); // Low influence
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.ambient"), ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.diffuse"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "light.specular"), 0.25f, 0.25f, 0.25f);
	}
};