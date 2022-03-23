#include "Rod.h"
#include <iostream>

	Rod::Rod(vec3 begEdge, vec3 endEdge, vec3 it, int nb, float ms) {
		nbOfPoints = nb;
		mass = ms;
		offsetBending.resize(nbOfPoints, vec3(0));
		presetPoints.resize(nbOfPoints, vec3(0));
		presetEdges.resize(nbOfPoints-1, vec3(0));
		currentPoints.resize(nbOfPoints, vec3(0));
		currentEdges.resize(nbOfPoints - 1, vec3(0));
		currentVel.resize(nbOfPoints, vec3(0));
		iteraror = it;
		int i = 0;
		for (vec3 curr = begEdge; glm::distance(curr,endEdge)>0.1; curr+=it) {
			presetPoints[i] = curr;
			currentPoints[i] = curr;
			
			currentVel[i] = vec3(0, 0, 0);
			
			offsetBending[i] = vec3(0);
			i++;
		}
		//last point
		presetPoints[i] = endEdge;
		currentPoints[i] = endEdge;

		currentVel[i] = vec3(0, 0, 0);
		i++;
		if (i != nbOfPoints) {
			cout << "Error in rod construction";
		}
		else {
			cout << "Rod constructed with success";
		}
		setDefaultEdges();
		
		updateCurrentEdges();
		//currentPoints[5] = vec3(5, 0.5, 0);
	}
	void Rod::decrementOffset(int index, vec3 off)
	{
		offsetBending[index] = offsetBending[index] + off;
	}
	void Rod::addAndUpdate(bool start, float delta,char c)
	{
		switch (c) {
		case 'x':
			currentPoints[start?0:nbOfPoints-1].x += delta;
			break;
		case 'y':
			currentPoints[start ? 0 : nbOfPoints - 1].y += delta;
			break;
		case 'z':
			currentPoints[start ? 0 : nbOfPoints - 1].z += delta;
			break;
		}
		
		updateCurrentEdges();
	}
	void Rod::setOffSet() {
		for (int i = 1; i < nbOfPoints - 1; i++) {
			offsetBending[i] = forceAtPoint(i);
		}
	}
	void Rod::setDefaultEdges() {

		for (int i = 0; i < (nbOfPoints - 1); i++) {

			presetEdges[i] = (presetPoints[i + 1] - presetPoints[i]);

		}

	}

	float Rod::ebsolum() {
		return (currentLength - baseLength) / baseLength;
	}
	void Rod::setCurrentPoint(int i, vec3 newPos)
	{
		currentPoints[i] = newPos;
		updateCurrentEdges();
	}
	float Rod::stretchingEnergy() {

		float eb = ebsolum();
		return 0.5 * (YOUNGMOD * pow(ROD_RADIUS, 2) * PI) * pow(eb, 2);

		//distance entre deux points au carre times a constant
	}

	vec3 Rod::GetPresetPoint(int i)
	{
		return presetPoints[i];
	}

	vec3 Rod::stretchForce(int i) {

		if (i < 1 || i >= nbOfPoints - 1) return vec3(0.0);
		float ebso1 = (glm::length(currentEdges[i]) - glm::length(presetEdges[i])) / (glm::length(presetEdges[i]));
		float ebso2 =  (glm::length(currentEdges[i-1]) - glm::length(presetEdges[i-1])) / (glm::length(presetEdges[i-1]));
		
		
		ebso1 = max(ebso1, 0.f);
		ebso2 = max(ebso2, 0.f);
		vec3 rightForce = -K*ebso1 * glm::normalize(currentEdges[i]);

		vec3 leftForce = -K*ebso2 * glm::normalize(currentEdges[i-1]);

		return  leftForce - rightForce;

	}
	vec3 Rod::StretchForceAt0() {
		float ebso1 = (glm::length(currentEdges[0]) - glm::length(presetEdges[0])) / (glm::length(presetEdges[0]));
		ebso1 = max(ebso1, 0.f);
		//cout << ebso1;
		return  -K * ebso1 * glm::normalize(currentEdges[0]);
	}
	vec3 Rod::StretchForceAtEnd() {

		float ebso1 = (glm::length(currentEdges[nbOfPoints-2]) - glm::length(presetEdges[nbOfPoints-2])) / (glm::length(presetEdges[nbOfPoints-2]));
		//cout << ebso1;
		ebso1 = max(ebso1, 0.f);
		return  -K * ebso1 * glm::normalize(currentEdges[nbOfPoints-2]);
	}

	void Rod::updateEnds(int index,vec3 pos, vec3 velocity)
	{
		currentPoints[index] = pos;
		currentVel[index] = velocity;
		updateCurrentEdges();
	}

	vec3 Rod::GetCurrentEdge(int i) {
		return currentEdges[i];
	}
	vec3 Rod::GetPresetEdge(int i) {
		return presetEdges[i];
	}

	vec3 Rod::k(int i) {
		vec3 k = vec3();
		if (i > 0) {
			vec3 previousCurrentEdge = currentEdges[i - 1];
			vec3 currentEdge = currentEdges[i];
			vec3 presetEdge = presetEdges[i];
			vec3 presetPreviousEdge = presetEdges[i - 1];


			vec3 topPart = glm::cross((previousCurrentEdge * 2.f), currentEdge);

			float botPart = glm::length(presetPreviousEdge) * glm::length(presetEdge) + glm::dot(previousCurrentEdge, currentEdge);

			k = topPart / botPart;


		}
		return k;
	}

	

	//float Rod::bendingEnergy(int i) {


	//	vec3 ki = k(i);

	//	return 0.5 * (YOUNGMOD * pow(ROD_RADIUS, 4) * (PI / 4)); //pow(ki, 2));

	//}
	// i is between 1 and nbOfPoints-1
	float Rod::bendingEnergy(int i) {

		if (i < 1 || i >= nbOfPoints-1) return 0;
			float add = (glm::dot(k(i),k(i)) * BENDING_MODULUS) / glm::length((presetEdges[i - 1]) + glm::length(presetEdges[i]));
			
			return add; 
	}
	//currently using stretchenergy but can use bending for other purposes
	vec3 Rod::forceAtPoint(int i) {
		if (i < 1 || i >= nbOfPoints - 1) return vec3(0.0);
		//vec3 F1 = -(bendingEnergy(i) - bendingEnergy(i - 1) / max(MIN_LENGTH,(powf((glm::length(currentPoints[i]) - glm::length(currentPoints[i - 1])),2)))) * (currentPoints[i] - currentPoints[i - 1]);
		//vec3 F2 = -(bendingEnergy(i+1) - bendingEnergy(i) / max(MIN_LENGTH,(powf((glm::length(currentPoints[i+1]) - glm::length(currentPoints[i])),2)))) * (currentPoints[i+1] - currentPoints[i]);

		//vec3 final = F1 + F2;
		////cout << "\nForce at point " << i << ":" << final.x<< "," << final.y<<","<<final.z;
		//return final;
		float normalBendingEnergy = bendingEnergy(i);
		
		//x force
		
		currentPoints[i].x += DELTA;
		updateCurrentEdges();
		float DeltaBendingEnergy = bendingEnergy(i);
		

		float bendingForce = (normalBendingEnergy - DeltaBendingEnergy) / DELTA;
		
		float xForce = bendingForce;
		currentPoints[i].x -= DELTA;
		updateCurrentEdges();
		//y force
		currentPoints[i].y += DELTA;
		updateCurrentEdges();
		DeltaBendingEnergy = bendingEnergy(i);
		
		bendingForce = (normalBendingEnergy - DeltaBendingEnergy) / DELTA;
		
		float yForce = bendingForce;
		currentPoints[i].y -= DELTA;
		updateCurrentEdges();
		//z force
		currentPoints[i].z += DELTA;
		updateCurrentEdges();
		DeltaBendingEnergy = bendingEnergy(i);
		
		bendingForce = (normalBendingEnergy - DeltaBendingEnergy) / DELTA;
		
		float zForce = bendingForce;
		currentPoints[i].z -= DELTA;
		updateCurrentEdges();
		vec3 forceBend =  vec3(xForce, yForce, zForce);
		


		return forceBend+stretchForce(i)-offsetBending[i];
		

	}


	void Rod::EulerExplicit(int i) {
		vec3 newVel = vec3(0.0);
		vec3 newPos = vec3(0.0);

		newVel = currentVel[i] + (1/mass * forceAtPoint(i)-currentVel[i]*damping) * timeStep;

		newPos = currentPoints[i] + (newVel * timeStep);

		currentPoints[i] = newPos;
		currentVel[i] = newVel;
	}


	

	void Rod::updateCurrentEdges() {
		
		for (int i = 0; i < (nbOfPoints - 1);i++) {
		
			currentEdges[i] = (currentPoints[i + 1] - currentPoints[i]);
			

		}

	}


	void Rod::displayCurrentPoints() {

		for(int i =0;i<nbOfPoints;i++)
		{
			cout << "\nPoint " << i << " x: " << currentPoints[i].x << " y: " << currentPoints[i].y << " z: " << currentPoints[i].z;
		}
	}

	void Rod::displayEnergy() {

		for (int i = 0; i < nbOfPoints; i++)
		{
			cout << "\nPoint " << i << ":  " << bendingEnergy(i);
		}
	}
	void Rod::update() {

		updateCurrentEdges();
		for (int i = 1; i < nbOfPoints-1; i++) {
			EulerExplicit(i);
		}
	}

	vec3 Rod::getCurrentPoint(int i)
	{
		return currentPoints[i];
	}
	
	Not::Not(float ms) 
	{
		mass = ms;
		position = vec3(0);
		rightRod = -1;
		upRod = -1;
		leftRod = -1;
		downRod = -1;
		velocity = vec3(0);
		clamped = false;
		rods = nullptr;
		OffSet = vec3(0);
	}

	Not::Not(vec3 pos, bool stuck,float ms){
		mass = ms;
		position = pos;
		rightRod = -1;
		upRod = -1;
		leftRod = -1;
		downRod = -1;
		velocity = vec3(0);
		clamped = stuck;
		rods = nullptr;
		OffSet = vec3(0);
	}
	void Not::setVectorRef(vector<Rod>* rod)
	{
		rods = rod;
	}
	vec3 Not::getVelocity()
	{
		return velocity;
	}
	void Not::setMass(float ms)
	{
		mass = ms;
	}
	void Not::setDown(int index) { 
	
		downRod = index;
	}

	vec3 Not::KHorizontal() {
		

		vec3 previousCurrentEdge = rods->at(leftRod).GetCurrentEdge((rods->at(leftRod).nbOfPoints)-2);
		vec3 currentEdge = rods->at(rightRod).GetCurrentEdge(0);
		vec3 presetEdge = rods->at(rightRod).GetPresetEdge(0);
		vec3 presetPreviousEdge = rods->at(leftRod).GetPresetEdge((rods->at(leftRod).nbOfPoints) - 2);


		vec3 topPart = glm::cross((previousCurrentEdge * 2.f), currentEdge);

		float botPart = glm::length(presetPreviousEdge) * glm::length(presetEdge) + glm::dot(previousCurrentEdge, currentEdge);

		vec3 k = topPart / botPart;
		return k;
	}
	vec3 Not::getPosition()
	{
		return position;
	}
	void Not::setPosition(vec3 pos)
	{
		position = pos;
	}
	void Not::setClamp(bool isClamped)
	{
		clamped = isClamped;
	}
	vec3 Not::KVertical() {

		vec3 previousCurrentEdge = rods->at(downRod).GetCurrentEdge((rods->at(downRod).nbOfPoints) - 2);
		vec3 currentEdge = rods->at(upRod).GetCurrentEdge(0);
		vec3 presetEdge = rods->at(upRod).GetPresetEdge(0);
		vec3 presetPreviousEdge = rods->at(downRod).GetPresetEdge((rods->at(downRod).nbOfPoints) - 2);


		vec3 topPart = glm::cross((previousCurrentEdge * 2.f), currentEdge);

		float botPart = glm::length(presetPreviousEdge) * glm::length(presetEdge) + glm::dot(previousCurrentEdge, currentEdge);

		vec3 k = topPart / botPart;
		return k;
	}
	float Not::getBendingEnergy()
	{
		float verticalEnergy = 0;
		float horizontalEnergy = 0;
		if(downRod>=0&&upRod>=0) verticalEnergy = (glm::dot(KVertical(), KVertical()) * BENDING_MODULUS) / glm::length((rods->at(downRod).GetPresetEdge(rods->at(downRod).nbOfPoints - 2)) + glm::length(rods->at(upRod).GetPresetEdge(0)));
		horizontalEnergy = (glm::dot(KHorizontal(), KHorizontal()) * BENDING_MODULUS) / glm::length((rods->at(leftRod).GetPresetEdge(rods->at(leftRod).nbOfPoints - 2)) + glm::length(rods->at(rightRod).GetPresetEdge(0)));

		return verticalEnergy + horizontalEnergy;
	}
	vec3 Not::getStretchForce() {
		vec3 stretch = rods->at(leftRod).StretchForceAtEnd();
		stretch -= rods->at(rightRod).StretchForceAt0();
		if(upRod>=0)stretch -= rods->at(upRod).StretchForceAt0();
		
		if (downRod >= 0)stretch += rods->at(downRod).StretchForceAtEnd();
		
		return stretch;

		
		
	}
	bool Not::getClamped()
	{
		return clamped;
	}
	void Not::setLeft(int index) { 
		leftRod = index;
	}
	void Not::setRight(int index) { 
		rightRod = index;
	}
	void Not::setUp(int index) { 
		upRod = index;
	}

	void Not::updateX(bool add) {
		//x force
		if (!clamped) {
			if (leftRod >= 0)rods->at(leftRod).addAndUpdate(false, add ? DELTA : -DELTA, 'x');
			if (rightRod >= 0)rods->at(rightRod).addAndUpdate(true, add ? DELTA : -DELTA, 'x');
			if (downRod >= 0)rods->at(downRod).addAndUpdate(false, add ? DELTA : -DELTA, 'x');
			if (upRod >= 0)rods->at(upRod).addAndUpdate(true, add ? DELTA : -DELTA, 'x');
		}
	}
	void Not::updateY(bool add) {
		if (!clamped) {
			//x force
			if (leftRod >= 0)rods->at(leftRod).addAndUpdate(false, add ? DELTA : -DELTA, 'y');
			if (rightRod >= 0)rods->at(rightRod).addAndUpdate(true, add ? DELTA : -DELTA, 'y');
			if (downRod >= 0)rods->at(downRod).addAndUpdate(false, add ? DELTA : -DELTA, 'y');
			if (upRod >= 0)rods->at(upRod).addAndUpdate(true, add ? DELTA : -DELTA, 'y');
		}
	}
	void Not::updateZ(bool add) {
		//x force
		if (!clamped) {
			if (leftRod >= 0)rods->at(leftRod).addAndUpdate(false, add ? DELTA : -DELTA, 'z');
			if (rightRod >= 0)rods->at(rightRod).addAndUpdate(true, add ? DELTA : -DELTA, 'z');
			if (downRod >= 0)rods->at(downRod).addAndUpdate(false, add ? DELTA : -DELTA, 'z');
			if (upRod >= 0)rods->at(upRod).addAndUpdate(true, add ? DELTA : -DELTA, 'z');
		}
	}
	
	vec3 Not::GetTotalForce() {
		if (clamped) {
			return vec3(0);
		}
		float normalBendingEnergy = getBendingEnergy();
		
		//x force
		updateX(true);
		float DeltaBendingEnergy = getBendingEnergy();

		float bendingForce = (normalBendingEnergy - DeltaBendingEnergy) / DELTA;

		float xForce = bendingForce;
		updateX(false);
		//y force
		updateY(true);
		DeltaBendingEnergy = getBendingEnergy();

		bendingForce = (normalBendingEnergy - DeltaBendingEnergy) / DELTA;

		float yForce = bendingForce;
		updateY(false);
		//z force
		updateZ(true);
		DeltaBendingEnergy = getBendingEnergy();

		bendingForce = (normalBendingEnergy - DeltaBendingEnergy) / DELTA;

		float zForce = bendingForce;
		updateZ(false);
		vec3 forceBend =vec3(xForce, yForce, zForce);

		return forceBend+getStretchForce()-OffSet;
	}

	void Not::EulerExplicit() {
		
		vec3 newVel = vec3(0.0);
		vec3 newPos = vec3(0.0);

		newVel = velocity + (1/mass * GetTotalForce()-velocity*damping) * timeStep;

		newPos = position + (newVel * timeStep);

		position = newPos;
		velocity = newVel;
	}
	void Not::update() {

		EulerExplicit();
		BroadCastInfo();
	}
	void Not::BroadCastInfo()
	{
		if (!clamped) {
			if(leftRod>=0)rods->at(leftRod).updateEnds(rods->at(leftRod).nbOfPoints-1, position, velocity);
			if (rightRod >= 0)rods->at(rightRod).updateEnds(0, position, velocity);
			if (downRod >= 0)rods->at(downRod).updateEnds(rods->at(downRod).nbOfPoints-1, position, velocity);
			if (upRod >= 0)rods->at(upRod).updateEnds(0, position, velocity);
		}
	}
	void Not::setOffSet() {
		if(!clamped) OffSet = GetTotalForce();
	}
	