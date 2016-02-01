// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Settings.h"
#include "Engine/LevelScriptActor.h"
#include "MapCreator.generated.h"

/**
*
*/
UCLASS()
class ASSIGNMENT1_API AMapCreator : public ALevelScriptActor {
	GENERATED_BODY()

	TArray<TArray<int>> map;							// Obstacle map F
	FVector2D start;									// Starting position
	TSubclassOf<class UObject> blockBP;					// For spawning blocks

	public:
	UFUNCTION(BlueprintCallable, Category = "Map Creation")
		AActor * createMap(bool binary, ACameraActor * camera, AStaticMeshActor * floor);

	private:
	FVector binaryMap(UWorld* const world);

	//FVector polygonalMap();
};
