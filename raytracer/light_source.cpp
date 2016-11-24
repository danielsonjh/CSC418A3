/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray, bool ambient_only) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  
	
	if (ray.intersection.none) {
		return;
	}

	Vector3D normal = ray.intersection.normal;
	normal.normalize();

	Vector3D incident_ray = _pos - ray.intersection.point;
	incident_ray.normalize();

	Vector3D reflected_ray = 2 * (normal.dot(incident_ray))*normal - incident_ray;
	reflected_ray.normalize();

	Vector3D camera = -ray.dir;
	camera.normalize();

	Material* mat = ray.intersection.mat;

	Colour ambient = mat->ambient * _col_ambient;

	Colour diffuse = fmax(0.0, (incident_ray.dot(normal))) * (mat->diffuse * _col_diffuse);

	Colour specular = fmax(0.0, pow((reflected_ray.dot(camera)), mat->specular_exp)) * (mat->specular * _col_specular);

	if (ambient_only) {
		ray.col = ambient;
	}
	else {
		ray.col = ambient + diffuse + specular;
	}

	ray.col.clamp();
}

