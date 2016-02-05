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

	float height, width;
	TSubclassOf<class UObject> blockBP;					// For spawning blocks
	TSubclassOf<class UObject> blockBP2;					// For spawning small blocks

	public:
	UFUNCTION(BlueprintCallable, Category = "Map Creation")
	AActor * createMap(bool binary, ACameraActor * camera, AStaticMeshActor * floor);

	private:
	FVector spawnMap(UWorld* const world);
	FVector binaryMap(UWorld* const world);

	//FVector polygonalMap();
};
