// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment1.h"
#include "RRT.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include "CoreMisc.h"
#include <limits>
#include <stdlib.h>
#include <time.h>

TArray<FVector> ARRT::generate_path()
{
	srand(time(NULL));

	getMap();
	getPositions();

	FVector2D start = FVector2D(positions[0][0] - 1, positions[0][1] - 1);

	FVector2D goal = FVector2D(positions[1][0] - 1, positions[1][1] - 1);

	TArray<FVector2D> freeSpace = getFreeSpace();

	TArray<Node> pathNode = findPath(start, goal, freeSpace);

	TArray<FVector> path = makePath(pathNode);

	path.Add(FVector(goal, 0));

	writePathToFile(path, "path.txt");

	path = makePath2(path);

	return path;
}

TArray<FVector> ARRT::makePath2(TArray<FVector> path)
{
	for (int32 c = 0; c < path.Num(); c++) {
		path[c] = 100 * path[c];
		path[c] = FVector(path[c].Y, path[c].X, 0);
	}
	
	return path;
}

TArray<FVector> ARRT::makePath(TArray<Node> nodes)
{
	TArray<FVector> path;

	for (int c = 0; c < nodes.Num(); c++) {
		path.Add(FVector(nodes[c].pos, 0));
	}

	return path;
}

TArray<ARRT::Node> ARRT::findPath(FVector2D start, FVector2D goal, TArray<FVector2D> freeSpace)
{
	TArray<Node *> rrt;

	Node * current = new Node;
	current->pos = start;
	current->cameFrom = nullptr;
	current->cost = 0;

	rrt.Add(current);

	FVector2D randomPoint;

	int32 iter = 1000;
	for (int32 i = 0; i < iter; i++) {
		randomPoint = getRandomPoint(freeSpace);

		Node * closest = getClosest(rrt, randomPoint);

		Node * newCurrent = new Node;
		if (goTowards(newCurrent, closest->pos, randomPoint, freeSpace)) {
			current = newCurrent;
			current->cameFrom = closest;
			current->cost = closest->cost + FVector2D::Distance(closest->pos, current->pos);

			TArray<Node *> nearNodes = getNear(rrt, current, FVector2D::Distance(closest->pos, current->pos));
			
			Node * nodeMin = closest;
			for (int32 c = 0; c < nearNodes.Num(); c++) {
				if (collisionFree(nearNodes[c], current, freeSpace) && nearNodes[c]->cost < nodeMin->cost) {
					nodeMin = nearNodes[c];
				}
			}

			current->cameFrom = nodeMin;
			current->cost = nodeMin->cost + FVector2D::Distance(nodeMin->pos, current->pos);

			// Adopt children
			for (int32 c = 0; c < nearNodes.Num(); c++) {
				if (collisionFree(current, nearNodes[c], freeSpace) && current->cost + FVector2D::Distance(current->pos, nearNodes[c]->pos) < nearNodes[c]->cost) {
					nearNodes[c]->cameFrom = current;
					nearNodes[c]->cost = current->cost + FVector2D::Distance(current->pos, nearNodes[c]->pos);
				}
			}

			rrt.Add(current);
		}
		else {
			delete newCurrent;
		}
	}

	// Check if we can reach the goal
	bool reachGoal = false;
	for (int32 c = 0; c < rrt.Num(); c++) {
		if (rrt[c]->pos.Equals(goal, 3)) {
			current = rrt[c];
			reachGoal = true;
			break;
		}
	}

	// TODO: If not reach goal, continue...

	/*
	while (!current->pos.Equals(goal, 3) && freeSpace.Num() > 0) {
		randomPoint = getRandomPoint(freeSpace);

		Node * closest = getClosest(rrt, randomPoint);

		UE_LOG(LogTemp, Warning, TEXT("Free space: %d, current: %f %f, random: %f %f, closest: %f %f"), freeSpace.Num(), current->pos[0], current->pos[1], randomPoint[0], randomPoint[1], closest->pos[0], closest->pos[1]);

		// TODO: get input?!

		Node * newCurrent = new Node;
		UE_LOG(LogTemp, Warning, TEXT("Test"));
		if (goTowards(newCurrent, closest->pos, randomPoint, freeSpace)) {
			UE_LOG(LogTemp, Warning, TEXT("Hej"));
			current = newCurrent;
			current->cameFrom = closest;
			//freeSpace.RemoveSingle(current->pos);
			rrt.Add(current);
		} else {
			delete newCurrent;
		}
		UE_LOG(LogTemp, Warning, TEXT("!Hej då"));
	}
	*/

	UE_LOG(LogTemp, Warning, TEXT("DONE! Current: %f %f"), current->pos[0], current->pos[1]);

	TArray<Node> path;
	path.Add(*current);

	while (current->cameFrom != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("DONE! Current: %f %f"), current->pos[0], current->pos[1]);
		current = current->cameFrom;
		path.Insert(*current, 0);
	}
	
	return path;
}

TArray<ARRT::Node *> ARRT::getNear(TArray<Node *> rrt, Node * node, float radius)
{
	TArray<Node *> nearNodes;
	for (int32 c = 0; c < rrt.Num(); c++) {
		if (FVector2D::Distance(rrt[c]->pos, node->pos) < radius) {
			nearNodes.Add(rrt[c]);
		}
	}

	return nearNodes;
}

bool ARRT::collisionFree(Node * node1, Node * node2, TArray<FVector2D> freeSpace)
{
	FVector2D from = node1->pos;
	FVector2D to = node2->pos;

	FVector2D line = to - from;

	if (FVector2D::Distance(from, to) > 10) {
		line.Normalize();
		line *= 10;
	}

	float phi = 0.001;
	float n = 0;
	FVector2D test;
	while (n <= 1) {
		test = scaleToIndex * (from + n * line);
		if (freeSpace.Contains(FVector2D(int32(test.X), int32(test.Y)))) {
			n += phi;
		}
		else {
			return false;
		}
	}

	return true;
}

bool ARRT::goTowards(Node * node, FVector2D from, FVector2D to, TArray<FVector2D> freeSpace)
{
	FVector2D line = to - from;

	if (FVector2D::Distance(from, to) > 10) {
		line.Normalize();
		line *= 10;
	}

	float phi = 0.001;
	float n = 0;
	FVector2D test;
	while (n <= 1) {
		test = scaleToIndex * (from + n * line);
		if (freeSpace.Contains(FVector2D(int32 (test.X), int32 (test.Y)))) {
			n += phi;
		} else {
			break;
		}
	}

	if (n == 0) {
		return false;
	}

	if (n > 1) {
		n -= phi;
	}

	node->pos = test;

	return true;

	/*
	float distance = std::numeric_limits<float>::infinity();
	FVector2D best;

	FVector2D test = FVector2D(from[0] - 1, from[1]);
	if (freeSpace.Contains(test)) {
		if (FVector2D::Distance(test, to) < distance) {
			distance = FVector2D::Distance(test, to);
			best = test;
			success = true;
		}
	}
	test = FVector2D(from[0] + 1, from[1]);
	if (freeSpace.Contains(test)) {
		if (FVector2D::Distance(test, to) < distance) {
			distance = FVector2D::Distance(test, to);
			best = test;
			success = true;
		}
	}
	test = FVector2D(from[0], from[1] - 1);
	if (freeSpace.Contains(test)) {
		if (FVector2D::Distance(test, to) < distance) {
			distance = FVector2D::Distance(test, to);
			best = test;
			success = true;
		}
	}
	test = FVector2D(from[0], from[1] + 1);
	if (freeSpace.Contains(test)) {
		if (FVector2D::Distance(test, to) < distance) {
			distance = FVector2D::Distance(test, to);
			best = test;
			success = true;
		}
	}

	node->pos = best;

	return success;
	*/
}

ARRT::Node * ARRT::getClosest(TArray<Node *> rrt, FVector2D point)
{
	int32 bestIndex = 0;
	float distance = std::numeric_limits<float>::infinity();
	
	float tempDistance;
	for (int32 c = 0; c < rrt.Num(); c++) {
		tempDistance = FVector2D::Distance(rrt[c]->pos, point);
		if (tempDistance < distance) {
			distance = tempDistance;
			bestIndex = c;
		}
	}


	return rrt[bestIndex];
}

FVector2D ARRT::getRandomPoint(TArray<FVector2D> points)
{
	int32 index = rand() % points.Num();
	FVector2D point = points[index];
	return point;
}

TArray<FVector2D> ARRT::getFreeSpace()
{
	TArray<FVector2D> freeSpace;

	for (int c = 0; c < map.Num(); c++) {
		for (int g = 0; g < map[c].Num(); g++) {
			if (map[c][g] == 0) {
				freeSpace.Add(FVector2D(g, c));
			}
		}
	}

	return freeSpace;
}