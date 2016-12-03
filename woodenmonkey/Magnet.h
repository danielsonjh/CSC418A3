#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Magnet {
public:

	float strength = 400.0f;
	float speed = 25;
	glm::vec3 position;

	Magnet(glm::vec3 _position) {
		position = _position;
	}
};