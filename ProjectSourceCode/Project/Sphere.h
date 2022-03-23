#pragma once
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
using namespace std;
using glm::vec3;
class Sphere {
private: 
	float radius;
	vec3 position;
	float mass;
	vec3 velocity;
	vec3 force;
	float timeStep = 0.05f;

public:
	Sphere(float radius, vec3 position, float mass, vec3 velocity, vec3 force);
	vec3 getPosition();
	vec3 getVelocity();
	void setVelocity(vec3 velocity);
	void EulerUpdate();
	
	float getRadius();
	void setForce(vec3 force);

	void setPosition(vec3 newPos);
	

};