// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment1.h"
#include "MapCreator.h"

#include <iostream>

static FString fileMap = "discObst.txt";
static FString filePositions = "positions.txt";

AActor * AMapCreator::createMap(AStaticMeshActor * floor, ACameraActor * camera)
{
	// Read map data
	map = readData(fileMap);

	// Read Character start position
	TArray<TArray<int32>> positions = readData(filePositions);
	start = FVector2D(positions[0][0] - 1, positions[0][1] - 1);

	// TODO: KOLLA ATT DETTA FORTFARANDE STÄMMER FÖR POLYGONAL!
	// Set floor size depending on map dimensions
	float ratio = gridSize / meshSide;
	float width = map[0].Num();
	float height = map.Num();
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

TArray<TArray<int>> AMapCreator::readData(FString fileName)
{
	TArray<FString> strArray;
	FString projectDir = FPaths::GameDir();
	projectDir += "Input Data/" + fileName;
	FFileHelper::LoadANSITextFileToStrings(*projectDir, NULL, strArray);

	TArray<TArray<int32>> data;
	for (FString line : strArray) {
		if (line.Trim().IsEmpty()) continue;

		TArray<int32> row;

		FString solid;

		while (line.Split(FString("\t"), &solid, &line)) {
			row.Add(FCString::Atoi(*solid));
		}

		row.Add(FCString::Atoi(*line));

		data.Add(row);
	}

	return data;
}

FVector AMapCreator::binaryMap(UWorld* const world)
{
	// Spawn maze blocks
	for (int c = 0; c < map.Num(); c++) {
		for (int g = 0; g < map[c].Num(); g++) {
			if (map[c][g] == 1) {
				FVector location(-c * gridSize, g * gridSize, -(characterHeight / 2));
				world->SpawnActor<UObject>(blockBP, location, { 0,0,0 });
			}
		}
	}

	// Spawn character
	return FVector(-start[0] * gridSize + (gridSize / 2),
				   start[1] * gridSize + (gridSize / 2), 0);
}