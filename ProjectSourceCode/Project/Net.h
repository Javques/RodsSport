#pragma once
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Rod.h"
using namespace std;
using glm::vec3;
const int nbOfRods = 200;


class Net{
private:
	Rod presetPoints[nbOfRods];
	

public:
	Net();


	void setDefaultEdges();

	float ebsolum();

	float stretchingEnergy();

	float stretchEnergy(int i);

	vec3 k(int i);

	float bendingEnergy(int i);

	vec3 forceAtPoint(int i);



	void EulerExplicit(int i);

	void updateCurrentEdges();

	void displayCurrentPoints();

	void displayEnergy();

	void update();
	vec3 getCurrentPoint(int i);

};