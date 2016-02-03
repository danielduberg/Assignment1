// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Settings.h"
#include "AIController.h"
#include "RRT.generated.h"

UCLASS()
class ASSIGNMENT1_API ARRT : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "RRT")
	static TArray<FVector> generate_path();

private:
	struct Input {
		// TODO
		float velocity;
		float acceleration;
		float angle;
		float time;
	};

	struct State {
		// TODO
		float velocity;
		float direction;	// Orientation?
		float acceleration;	// is it needed?
	};

	struct Node {
		FVector2D pos;
		Node * cameFrom = nullptr;
		TArray<TTuple<Node, Input>> children;
		Input input;
		State state;
		float cost;
		// TODO: Input
		// TODO: State
	};

	static TArray<Node *> getNear(TArray<Node *> rrt, Node * node, float radius);

	static bool collisionFree(Node * node1, Node * node2, TArray<FVector2D> freeSpace);

	static TArray<FVector> makePath2(TArray<FVector> path);

	static TArray<FVector> makePath(TArray<Node> nodes);

	static TArray<Node> findPath(FVector2D start, FVector2D goal, TArray<FVector2D> freeSpace);

	static bool goTowards(Node * node, FVector2D from, FVector2D to, TArray<FVector2D> freeSpace);

	static Node * getClosest(TArray<Node *> rrt, FVector2D point);

	static FVector2D getRandomPoint(TArray<FVector2D> points);
	
	static TArray<FVector2D> getFreeSpace();
};
