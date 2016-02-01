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

TArray<FVector> ARRT::generate_path(int32 n)
{
	srand(time(NULL));

	getMap();
	getPositions();

	FVector2D start = FVector2D(positions[0][0] - 1, positions[0][1] - 1);

	FVector2D goal = FVector2D(positions[1][0] - 1, positions[1][1] - 1);

	TArray<FVector2D> freeSpace = getFreeSpace();

	TArray<Node> pathNode = findPath(start, goal, freeSpace);

	TArray<FVector> path = makePath(pathNode);

	writePathToFile(path, "Test.txt");

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

	rrt.Add(current);

	freeSpace.RemoveSingle(start);

	FVector2D randomPoint;

	while (!current->pos.Equals(goal, 0.1) && freeSpace.Num() > 0) {
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
			freeSpace.RemoveSingle(current->pos);
			rrt.Add(current);
		} else {
			delete newCurrent;
		}
		UE_LOG(LogTemp, Warning, TEXT("!Hej då"));
	}

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

bool ARRT::goTowards(Node * node, FVector2D from, FVector2D to, TArray<FVector2D> freeSpace)
{

	float distance = std::numeric_limits<float>::infinity();
	FVector2D best;

	bool success = false;

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