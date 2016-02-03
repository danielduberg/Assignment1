// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment1.h"
#include "MapCreator.h"

AActor * AMapCreator::createMap(bool binary, ACameraActor * camera, AStaticMeshActor * floor)
{
	binMap = binary;

	// Read Character start position
	getPositions();

	// Adjust gridSize and meshSize different depending on if binary or not
	float ratio = gridSize / meshSide;

	// Set floor size depending on map and cell to view dimensions
	height = getMap().Num();
	width = getMap()[0].Num();

	floor->SetActorScale3D(FVector(height * ratio, width * ratio, 1));
	floor->SetActorLocation(FVector((width / 2) * gridSize, (height / 2) * gridSize, -((meshSide / 2) + (characterHeight / 2))));

	// Center camera above the center of floor, at a distance that fits the entire field
	float FOV = 90;
	float cameraHeight = width / tan(FOV / 2);
	camera->SetActorLocation(floor->GetActorLocation() + FVector(0, 0, height * gridSize));

	// Get MazeBlock blueprint
	auto cls = StaticLoadObject(UObject::StaticClass(), nullptr,
								TEXT("Blueprint'/Game/Blueprints/MazeBlock.MazeBlock'"));
	UBlueprint * bp = Cast<UBlueprint>(cls);
	blockBP = (UClass*)bp->GeneratedClass;

	// Get Character blueprint
	cls = StaticLoadObject(UObject::StaticClass(), nullptr,
						   TEXT("Blueprint'/Game/Blueprints/Character.Character'"));
	bp = Cast<UBlueprint>(cls);
	TSubclassOf<class UObject> characterBP = (UClass*)bp->GeneratedClass;

	// Spawn objects
	FVector location = spawnMap(GWorld->GetWorld());

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	//								 FString::Printf(TEXT("{%f.2, %f.2, %f.2}"),
	//												 location.X, location.Y, location.Z));
	return GWorld->GetWorld()->SpawnActor<AActor>(characterBP, location, { 0,0,0 });
}

FVector AMapCreator::spawnMap(UWorld* const world)
{
	// Spawn maze blocks
	for (int c = 0; c < height; c++) {
		for (int g = 0; g < width; g++) {
			if (getMap()[c][g] == 1) {
				float xLoc;
				if (binMap) xLoc = (height - c) * gridSize;
				else		xLoc = c * gridSize;

				FVector location(xLoc, g * gridSize, -(characterHeight / 2));
				AActor * block = world->SpawnActor<AActor>(blockBP, location, { 0,0,0 });
			}
		}
	}

	// Spawn character
	float xLoc = positions[0][1] * gridSize;
	float yLoc = positions[0][0] * gridSize;
	if (binMap) {
		xLoc += (gridSize / 2);
		yLoc += (gridSize / 2);
	}

	return scaleToIndex * FVector(xLoc, yLoc, 0);
}