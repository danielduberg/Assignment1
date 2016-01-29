// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "RRT.generated.h"

UCLASS()
class ASSIGNMENT1_API ARRT : public AAIController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "RRT")
		static TArray<FVector> generate_path(int32 n);

private:
	static TArray<TArray<int32>> readData(FString fileName);

	static void writePathToFile(TArray<FVector> path, FString fileName);

	static TArray<TArray<float>> infMap(int32 rows, int32 columns);

	static float dist_between(FVector2D start, FVector2D goal);

	static float heuristic_cost_estimate(FVector2D start, FVector2D goal);

	static TArray<FVector> reconstruct_path(TMap<FVector2D, FVector2D> cameFrom, FVector2D current);

	static void getNeighbours(TQueue<FVector2D> & neighbours, TArray<TArray<int32>> map, FVector2D node, int32 numNeighbours);
};
