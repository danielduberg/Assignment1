// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Settings.h"
#include "AIController.h"
#include "AStar.generated.h"

UCLASS()
class ASSIGNMENT1_API AAStar : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AStar - Discrete")
	static TArray<FVector> generate_path(int32 n);

	UFUNCTION(BlueprintCallable, Category = "AStar - Polygonal")
	static TArray<FVector> generate_path2(float buffer);

private:
	static int32 findLowestFScore(TArray<FVector2D> openSet, TArray<TArray<float>> fScore);

	static int32 findLowestFScore(TArray<FVector2D> openSet, TMap<FVector2D, float> fScore);

	static TArray<TArray<float>> infMap(int32 rows, int32 columns);

	static TMap<FVector2D, float> infMap();

	static float dist_between(FVector2D start, FVector2D goal);

	static float heuristic_cost_estimate(FVector2D start, FVector2D goal);

	static TArray<FVector> reconstruct_path(TMap<FVector2D, FVector2D> cameFrom, FVector2D current);

	static void getNeighbours(TQueue<FVector2D> & neighbours, FVector2D node, int32 numNeighbours);

	static void getNeighbours(TQueue<FVector2D> & queue, FVector2D current);
};
