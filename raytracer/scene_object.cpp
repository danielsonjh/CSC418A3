/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

#include <stdio.h>

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.

	Point3D rayOriginPoint = worldToModel * ray.origin;
	Vector3D rayOriginVector = Vector3D(rayOriginPoint[0], rayOriginPoint[1], rayOriginPoint[2]);
	Vector3D rayDir = worldToModel * ray.dir;

	Vector3D normal = Vector3D(0, 0, 1);

	double numer = (-rayOriginVector).dot(normal);
	double denom = rayDir.dot(normal);
	double lambda = numer / denom;

	Point3D intersect = rayOriginPoint + (lambda * rayDir);
	double x = intersect[0];
	double y = intersect[1];
	if (lambda > 0 && fabs(intersect[0]) <= 0.5 && fabs(intersect[1]) <= 0.5 && is_intersect_closest(ray, lambda)) {
		ray.intersection.t_value = lambda;
		ray.intersection.point = modelToWorld * intersect;
		ray.intersection.normal = transNorm(worldToModel, normal);
		ray.intersection.normal.normalize();
		ray.intersection.none = false;
		return true;
	}

	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	
	Point3D rayOriginPoint = worldToModel * ray.origin;
	Vector3D rayOriginVector = Vector3D(rayOriginPoint[0], rayOriginPoint[1], rayOriginPoint[2]);
	Vector3D rayDir = worldToModel * ray.dir;

	double A = rayDir.dot(rayDir);
	double B = rayDir.dot(rayOriginVector);
	double C = rayOriginVector.dot(rayOriginVector) - 1;
	double D = B*B - A*C;
	if (D >= 0) {
		double lambda1 = - B / A + sqrt(D) / A;
		double lambda2 = - B / A - sqrt(D) / A;
		Point3D intersect1 = rayOriginPoint + (lambda1 * rayDir);
		Vector3D normal1 = Vector3D(intersect1[0], intersect1[1], intersect1[2]);
		Point3D intersect2 = rayOriginPoint + (lambda2 * rayDir);
		Vector3D normal2 = Vector3D(intersect2[0], intersect2[1], intersect2[2]);
		
		// Both intersectiones behind
		if (lambda1 < 0 && lambda2 < 0) {
			return false;
		}
		// lambda1 behind OR both in front and lambda2 is closer
		else if ((lambda1 < 0 || lambda1 >= lambda2) && is_intersect_closest(ray, lambda2)) {
			ray.intersection.t_value = lambda2;
			ray.intersection.point = modelToWorld * intersect2;
			ray.intersection.normal = transNorm(worldToModel, normal2);
			ray.intersection.normal.normalize();
			ray.intersection.none = false;
			return true;
		}
		// lambda2 behind OR both in front and lambda1 is closer
		else if ((lambda2 < 0 || lambda1 <= lambda2) && is_intersect_closest(ray, lambda1)) {
			ray.intersection.t_value = lambda1;
			ray.intersection.point = modelToWorld * intersect1;
			ray.intersection.normal = transNorm(worldToModel, normal1);
			ray.intersection.normal.normalize();
			ray.intersection.none = false;
			return true;
		}
	}

	return false;
	
}

bool is_intersect_closest(Ray3D& ray, double lambda) {
	return ray.intersection.none || lambda < ray.intersection.t_value;
}