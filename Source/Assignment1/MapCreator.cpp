// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment1.h"
#include "MapCreator.h"

AActor * AMapCreator::createMap(bool binary, ACameraActor * camera, AStaticMeshActor * floor)
{
	binMap = binary;

	// Read Character start position
	getPositions();
	start = FVector2D(positions[0][0] - 1, positions[0][1] - 1);

	// TODO: KOLLA ATT DETTA FORTFARANDE STÄMMER FÖR POLYGONAL!
	// Set floor size depending on map dimensions
	float ratio = gridSize / meshSide;
	float width = getMap()[0].Num();
	float height = getMap().Num();
	floor->SetActorScale3D(FVector((width * ratio), (height * ratio), 1));
	floor->SetActorLocation(FVector(-((width * gridSize) / 2) + gridSize,
									((height * gridSize) / 2),
									-((meshSide / 2) + (characterHeight / 2))));

	// TODO: FIXA SÅ DENNA FUNGERAR FÖR BÅDE BINÄR OCH POLYGONAL!
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
	FVector location;
	UWorld* const world = GWorld->GetWorld();
	if (world) {
		// TODO: FIXA SÅ MAN VET OM DET ÄR BINARY ELLER POLYGONAL MAN SKA KÖRA!
		location = binaryMap(world);
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("No world!"));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	//								 FString::Printf(TEXT("{%f.2, %f.2, %f.2}"),
	//												 location.X, location.Y, location.Z));
	return world->SpawnActor<AActor>(characterBP, location, { 0,0,0 });
}

FVector AMapCreator::binaryMap(UWorld* const world)
{
	// Spawn maze blocks
	for (int c = 0; c < getMap().Num(); c++) {
		for (int g = 0; g < getMap()[c].Num(); g++) {
			if (getMap()[c][g] == 1) {
				FVector location(-c * gridSize, g * gridSize, -(characterHeight / 2));
				world->SpawnActor<UObject>(blockBP, location, { 0,0,0 });
			}
		}
	}

	// Spawn character
	if (binMap) {
		return FVector(-start[0] * gridSize + (gridSize / 2),
			start[1] * gridSize + (gridSize / 2), 0);
	} else {
		return FVector(-start[0] * 20,
			start[1] * 20, 0);
	}
}