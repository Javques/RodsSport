#pragma once
#include <glew/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
//basic usage of this .h file
	// initialize number of nots
	// create rods between two nots and assign the rod to each not
	//update the points of individual rods and nots 
	//simulate the physics of the entire system


using namespace std;
using glm::vec3;
//Constants
const float YOUNGMOD = 0.1;
const float ROD_RADIUS = 0.5;
const float PI = 3.1415926;
const float baseLength = 1.;
const float BENDING_MODULUS = 0.2f;
const float MASS = 100.f;
const float MIN_LENGTH = 0.25f;
const float timeStep = 0.05f;
const float K = 1.f;
const float DELTA = 0.01f;
const int nbOfNots = 25;
const float damping = 0.01f;


class Not;
class Rod {

private:

	//mass of a point of the rod
	float mass;
	//length of entire rod
	float currentLength;
	//offset for bending forces since we base our force calculations off of estimate derivatives
	vector<vec3> offsetBending;
	//initial position of all points of the rod
	vector<vec3> presetPoints;
	//initial edges between all initial points of the rod
	vector<vec3> presetEdges;
	//current position of all points of the rod
	vector<vec3> currentPoints;
	//current edges between all current points of the rod
	vector<vec3> currentEdges;
	//velocity of all current points
	vector<vec3> currentVel;
	
	//positiong of first point (first not)
	vec3 beginningEdge;
	//position of last point (last not)
	vec3 endingEdge;
	//constant jump between each point
	vec3 iteraror;



	
public:
	//nb of points per rod
	int nbOfPoints;
	//constructor with default nbPoints and mass
	Rod(vec3 begininngEdge, vec3 endingEdge, vec3 iterator,int nbPoints=10, float mass = MASS);
	//unused hack for decrementing initial static forces even more
	void decrementOffset(int index, vec3 off);
	//used for derivative estimation
	void addAndUpdate(bool start,float delta,char c);
	//set preset edges
	void setDefaultEdges();
	//set an offset to compensate with the estimative derivatives
	void setOffSet();
	//calculate the ebsolum of integral rod
	float ebsolum();
	//change a specific point
	void setCurrentPoint(int i, vec3 newPos);
	//get entire stretching energy
	float stretchingEnergy();
	//get initial point at index i
	vec3 GetPresetPoint(int i);
	//get stretch force at index i
	vec3 stretchForce(int i);
	//get stretch force at first nod
	vec3 StretchForceAt0();
	//get stretch force at final nod
	vec3 StretchForceAtEnd();
	//update the nods of the rod
	void updateEnds(int start,vec3 pos, vec3 velocity);

	//calculate the k of edge i
	vec3 k(int i);
	//calculate the bending of edge i
	float bendingEnergy(int i);
	//force at point i
	vec3 forceAtPoint(int i);
	//get edge at index i
	vec3 GetCurrentEdge(int i);
	//get preset edge at index i
	vec3 GetPresetEdge(int i);
	//time integration euler explicit
	void EulerExplicit(int i);
	//recalculate edges 
	void updateCurrentEdges();
	//print points position
	void displayCurrentPoints();
	//print energy
	void displayEnergy();
	//called every frame to simulate physics
	void update();
	//return point at index i
	vec3 getCurrentPoint(int i);

};
//Not is one of the extremities of a rod
class Not {
private: 
	//mass of not
	float mass;
	//clamping 
	bool clamped;
	//position of not
	 vec3 position;

	 //updates are broadcasting their changes to associated rods
	 //change the x component
	 void updateX(bool add);
	 //change y component
	 void updateY(bool add);
	 //change the z component
	 void updateZ(bool add);
	 //rod of not
	 vec3 velocity;
	 //reference to the grid of rods the not is part of
	 vector<Rod>* rods;
	 //force offset needed after derivative estimation
	 vec3 OffSet;
public:
	//change mass
	void setMass(float ms);
	//set ref to integral grid
	void setVectorRef(vector<Rod>* rod);
	//initialize not with mass
	Not(float mass = MASS);
	//get vel of not
	vec3 getVelocity();
	//index of each rod in the rod grid that is connected to the not
	int leftRod;
	int rightRod;
	int upRod;
	int downRod;
	//constructor with pos, clamping and mass
	Not(vec3 pos, bool stuck,float mass=MASS);
	//set the index of connecting rods
	void setLeft(int index);
	void setRight(int index);
	void setUp(int index);
	void setDown(int index);
	//get force on the specific not
	vec3 GetTotalForce();
	//bending energy of the not
	float getBendingEnergy();
	//stretch force of not
	vec3 getStretchForce();
	//time integration using forward euler
	void EulerExplicit();
	//broadcast position to connected rods
	void BroadCastInfo();
	//get curvature values using connected rods
	vec3 KVertical();
	vec3 KHorizontal();
	//return position
	vec3 getPosition();
	//set position
	void setPosition(vec3 pos);
	//set and get clamping
	void setClamp(bool isClamped);
	bool getClamped();
	//euler and broadcast
	void update();
	//initialize offset when whole system is at rest
	void setOffSet();
};