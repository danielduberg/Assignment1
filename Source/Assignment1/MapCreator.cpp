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
	floor->SetActorLocation(FVector((width / 2) * gridSize, (height / 2) * gridSize, -((meshSide / 2) + characterHeight)));

	// Center camera above the center of floor, at a distance that fits the entire field
	float FOV = 90;
	float cameraHeight = width / tan(FOV / 2);
	camera->SetActorLocation(floor->GetActorLocation() + FVector(0, 0, height * gridSize));

	// Get MazeBlock blueprint
	auto cls = StaticLoadObject(UObject::StaticClass(), nullptr,
								TEXT("Blueprint'/Game/Blueprints/MazeBlock.MazeBlock'"));
	UBlueprint * bp = Cast<UBlueprint>(cls);
	blockBP = (UClass*)bp->GeneratedClass;

	// Get MazeBlock2 blueprint
	cls = StaticLoadObject(UObject::StaticClass(), nullptr,
		TEXT("Blueprint'/Game/Blueprints/MazeBlock2.MazeBlock2'"));
	bp = Cast<UBlueprint>(cls);
	blockBP2 = (UClass*)bp->GeneratedClass;

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
	if (binMap) {
		for (int c = 0; c < height; c++) {
			for (int g = 0; g < width; g++) {
				if (getMap()[c][g] == 1) {
					float xLoc;
					if (binMap) xLoc = (height - c - 1) * gridSize;
					else		xLoc = c * gridSize;

					FVector location(xLoc, g * gridSize, -(characterHeight / 2));
					AActor * block = world->SpawnActor<AActor>(blockBP2, location, { 0,0,0 });
				}
			}
		}
	} else {
		for (int32 c = 0; c < edges.Num(); c++) {
			FVector2D edgeStart(edges[c][0].Y * gridSize, edges[c][0].X * gridSize);
			FVector2D edgeEnd(edges[c][1].Y * gridSize, edges[c][1].X * gridSize);

			FVector2D line = edgeEnd - edgeStart;

			FRotator rot = FVector(line.X, line.Y, 0).Rotation();

			float distance = FVector2D::Distance(edgeStart, edgeEnd);

			FVector2D location(edgeStart + 0.5 * line);
			AActor * block = world->SpawnActor<AActor>(blockBP, FVector(location, -(characterHeight / 2)), rot);
			FOutputDeviceNull ar;
			const FString command = FString::Printf(TEXT("changeScale %f"), distance / gridSize);
			block->CallFunctionByNameWithArguments(*command, ar, NULL, true);
		}
	}

	// Spawn character
	float xLoc, yLoc;

	if (binMap) {
		xLoc = (height - positions[0][1]) * gridSize + (gridSize / 2);
		yLoc = (positions[0][0] - 1) * gridSize + (gridSize / 2);
	} else {
		yLoc = positions[0][0] * gridSize;
		xLoc = positions[0][1] * gridSize;
	}

	return FVector(xLoc, yLoc, 0);
}