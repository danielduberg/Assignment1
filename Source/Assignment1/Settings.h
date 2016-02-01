// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>

static const float gridSize = 100;							// Size of each grid in the game
static const float meshSide = 100;							// Size of a side of a cube mesh
static const float characterHeight = 90;					// Height of the character mesh

static const FString fileMap = "obstacles.txt";
static const FString filePositions = "positions.txt";
static const FString fileOutName = "path.txt";

static bool mapRead = false;
static TArray<TArray<float>> map;
static bool binMap;

TArray<TArray<float>> readData(const FString fileName);

TArray<TArray<float>> & getMap()
{
	if (!mapRead) {
		map = readData(fileMap);
		mapRead = true;
	}

	return map;
}

TArray<TArray<float>> readData(const FString fileName)
{
	TArray<FString> strArray;
	FString projectDir = FPaths::GameDir();
	projectDir += "Input Data/" + fileName;
	FFileHelper::LoadANSITextFileToStrings(*projectDir, NULL, strArray);

	TArray<TArray<float>> data;
	for (FString line : strArray) {
		if (line.Trim().IsEmpty()) continue;

		TArray<float> row;

		FString solid;

		while (line.Split(FString("\t"), &solid, &line)) {
			row.Add(FCString::Atof(*solid));
		}

		row.Add(FCString::Atof(*line));

		data.Add(row);
	}

	return data;
}

void writePathToFile(TArray<FVector> path, const FString fileName)
{
	FString str;

	for (int32 c = 0; c < path.Num(); c++) {
		str += FString::SanitizeFloat(path[c][0] + 1) + "\t" + FString::SanitizeFloat(path[c][1] + 1) + "\n";
	}

	FString projectDir = FPaths::GameDir();
	projectDir += "Output Data/" + fileName;
	FFileHelper::SaveStringToFile(str, *projectDir);
}