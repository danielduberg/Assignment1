// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "MapCreator.generated.h"

/**
*
*/
UCLASS()
class ASSIGNMENT1_API AMapCreator : public ALevelScriptActor {
	GENERATED_BODY()

	const float gridSize = 100;							// Size of each grid in the game
	const float meshSide = 100;							// Size of a side of a cube mesh
	const float characterHeight = 90;					// Height of the character mesh

	TArray<TArray<int>> map;							// Obstacle map F
	FVector2D start;									// Starting position
	TSubclassOf<class UObject> blockBP;					// For spawning blocks

	public:
	UFUNCTION(BlueprintCallable, Category = "Map Creation")
		AActor * createMap(AStaticMeshActor * floor, ACameraActor * camera);

	private:
	TArray<TArray<int>> readData(FString fileName);

	FVector binaryMap(UWorld* const world);

	//FVector polygonalMap();
};
