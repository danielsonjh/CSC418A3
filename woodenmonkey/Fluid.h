#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Magnet.h"

const float fluid_c = 15;
const float decay_per_sec = 0.5f;
const int fluid_i_count = 50;
const int fluid_j_count = 50;
const float init_avg_u = 30;

class Fluid {
public:
	float u[fluid_i_count][fluid_j_count];


	Fluid() {
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
		for (int i = 0; i < fluid_i_count; i++) {
			for (int j = 0; j < fluid_j_count; j++) {
				u1 = i > 0 ? u[i - 1][j] : u[0][j];
				u2 = i < fluid_i_count - 1 ? u[i + 1][j] : u[fluid_i_count - 1][j];
				u3 = j > 0 ? u[i][j - 1] : u[i][0];
				u4 = j < fluid_j_count - 1 ? u[i][j + 1] : u[i][fluid_j_count - 1];
				u_xx = u1 + u2 + u3 + u4 - 4 * u[i][j];
				u_xx = u_xx < 1 ? u_xx : 1; // Prevent wave force from exploding
				r = glm::length(magnet.position - glm::vec3(i - fluid_i_count / 2, u[i][j], j - fluid_j_count / 2));
				r = r > 1 ? r : 1.0f; // Prevent magnetic force from exploding
				f = pow(fluid_c, 2) * u_xx + magnet.strength / pow(r, 2);
				v[i][j] += f * deltaTime;
				v[i][j] *= powf(decay_per_sec, deltaTime);
				u_new[i][j] = u[i][j] + v[i][j] * deltaTime;
				avg_u += u_new[i][j];
			}
		}
		avg_u /= fluid_i_count + fluid_j_count;

		for (int i = 0; i < fluid_i_count; i++) {
			for (int j = 0; j < fluid_j_count; j++) {
				u[i][j] = u_new[i][j] + (init_avg_u - avg_u) / (fluid_i_count + fluid_j_count);
			}
		}
	}
private:
	float u_new[fluid_i_count][fluid_j_count];
	float v[fluid_i_count][fluid_j_count];
	float f = 0;
	float avg_u = 0;
};