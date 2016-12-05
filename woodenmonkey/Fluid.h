#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Magnet.h"

const float fluid_c = 15;
const float decay_per_sec = 0.4f;
const int fluid_i_count = 50;
const int fluid_j_count = 50;
const float init_avg_u = 30;

class Fluid {

public:
	float u[fluid_i_count][fluid_j_count];

	Fluid(Light* _light) {
		light = _light;

		// Initialize fluid state
		for (int i = 0; i < fluid_i_count; i++) {
			for (int j = 0; j < fluid_j_count; j++) {
				u[i][j] = init_avg_u;
				v[i][j] = 0;
			}
		}
	}

	void update(float deltaTime, Magnet magnet) {
		float u1 = 0;
		float u2 = 0;
		float u3 = 0;
		float u4 = 0;
		float u_xx = 0;
		float r;
		avg_u = 0;
		// Get new height values before applying them, since updates depend on nearby heights.
		for (int i = 0; i < fluid_i_count; i++) {
			for (int j = 0; j < fluid_j_count; j++) {
				u1 = i > 0 ? u[i - 1][j] : u[0][j];
				u2 = i < fluid_i_count - 1 ? u[i + 1][j] : u[fluid_i_count - 1][j];
				u3 = j > 0 ? u[i][j - 1] : u[i][0];
				u4 = j < fluid_j_count - 1 ? u[i][j + 1] : u[i][fluid_j_count - 1];
				u_xx = u1 + u2 + u3 + u4 - 4 * u[i][j];
				u_xx = glm::min(u_xx, 1.0f); // Prevent wave force from exploding
				r = glm::length(magnet.position - glm::vec3(i - fluid_i_count / 2, u[i][j], j - fluid_j_count / 2));
				r = glm::max(r, 1.0f); // Prevent magnetic force from exploding
				f = pow(fluid_c, 2) * u_xx + magnet.strength / pow(r, 2);
				v[i][j] += f * deltaTime;
				v[i][j] *= powf(decay_per_sec, deltaTime); // Decay the velocity over time
				u_new[i][j] = u[i][j] + v[i][j] * deltaTime;
				u_new[i][j] = glm::max(u_new[i][j], 0.0f);
				avg_u += u_new[i][j];
			}
		}
		avg_u /= fluid_i_count + fluid_j_count;

		for (int i = 0; i < fluid_i_count; i++) {
			for (int j = 0; j < fluid_j_count; j++) {
				// Update heights, making sure to keep the same amount of fluid volume.
				u[i][j] = u_new[i][j] + (init_avg_u - avg_u) / (fluid_i_count + fluid_j_count);
			}
		}
	}

	void draw() {
		light->lightingShader.Use();
		GLint modelLoc = glGetUniformLocation(light->lightingShader.Program, "model");

		glBindVertexArray(shapes.cubeVAO);

		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.ambient"), 0.0f, 0.5f, 0.75f);
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.diffuse"), 0.0f, 0.5f, 0.75f);
		glUniform3f(glGetUniformLocation(light->lightingShader.Program, "material.specular"), 0.0f, 0.1f, 0.15f);
		glUniform1f(glGetUniformLocation(light->lightingShader.Program, "material.shininess"), 1.0f);
		glm::mat4 model;
		for (int i = 0; i < fluid_i_count; i++) {
			for (int j = 0; j < fluid_j_count; j++) {
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(i - fluid_i_count / 2, 0, j - fluid_j_count / 2));
				model = glm::scale(model, glm::vec3(1, glm::max(u[i][j], 0.01f), 1)); // Prevent 0 height water
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		glBindVertexArray(0);
	}

private:
	float u_new[fluid_i_count][fluid_j_count];
	float v[fluid_i_count][fluid_j_count];
	float f = 0;
	float avg_u = 0;

	Light* light;
};