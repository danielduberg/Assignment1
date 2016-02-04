// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>

/*
 * Variables
 */

static const float gridSize = 100;							// Size of each grid in the game
static const float meshSide = 100;							// Size of a side of a cube mesh
static const float characterHeight = 90;					// Height of the character mesh

static const float expansion = 10;

static const FString fileMap = "obstacles.txt";
static const FString filePositions = "positions.txt";
static const FString fileOutName = "path.txt";

static bool binMap;

static bool mapRead = false;
static TArray<TArray<float>> map;
static TArray<FVector2D> points;
static TArray<TArray<FVector2D>> edges;
static TArray<FVector2D> vertices;

static bool positionsRead = false;
static TArray<TArray<float>> positions;

static const float scaleFactor = 1;
static const float phi = 0.01;
static float scaleToIndex = 1;
static const float meshScale = meshSide * scaleFactor;


/*
 * Functions
 */

static TArray<TArray<float>> readData(const FString fileName);

static TArray<TArray<float>> & getMap();

static TArray<TArray<float>> & getPositions();

static void writePathToFile(TArray<FVector> path, const FString fileName);

static TArray<TArray<float>> makeDiscreteMap();

static TArray<TArray<FVector2D>> getEdges(TArray<TArray<float>> map);

static TArray<FVector2D> getVertices(TArray<TArray<float>> map);

/*
 * Implementations
 */

TArray<TArray<float>> & getMap()
{
	if (!mapRead) {
		map = readData(fileMap);

		edges = getEdges(map);

		vertices = getVertices(map);

		map = makeDiscreteMap();

		mapRead = true;
	}

	return map;
}

TArray<TArray<FVector2D>> getEdges(TArray<TArray<float>> map)
{
	TArray<TArray<FVector2D>> edges;

	FVector2D init;
	FVector2D startLine;
	FVector2D endLine;
	bool newObst = true;
	for (int32 c = 0; c < map.Num(); c++) {
		if (newObst) {
			newObst = false;
			init = FVector2D(map[c][0], map[c][1]);
			startLine = FVector2D(map[c][0], map[c][1]);
			endLine = FVector2D(map[c + 1][0], map[c + 1][1]);
		}
		else {
			startLine = endLine;
			if (map[c][2] == 1) {
				endLine = FVector2D(map[c + 1][0], map[c + 1][1]);
			}
			else {
				newObst = true;
				endLine = init;
			}
		}

		TArray<FVector2D> temp;
		temp.Add(startLine);
		temp.Add(endLine);
		edges.Add(temp);
	}

	/*
	for (int32 c = 0; c < edges.Num(); c++) {
		UE_LOG(LogTemp, Warning, TEXT("Obstacle #%d:"), c+1);
		for (int32 g = 0; g < edges[c].Num(); g++) {
			UE_LOG(LogTemp, Warning, TEXT("\t(%f, %f)"), edges[c][g].X, edges[c][g].Y);
		}
	}
	*/

	return edges;
}

TArray<FVector2D> getVertices(TArray<TArray<float>> map)
{
	TArray<FVector2D> vertices;

	for (int32 c = 0; c < map.Num(); c++) {
		vertices.Add(FVector2D(map[c][0], map[c][1]));
	}

	return vertices;
}

TArray<TArray<float>> & getPositions()
{
	if (!positionsRead) {
		positions = readData(filePositions);
		positionsRead = true;
	}

	return positions;
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
		str += FString::SanitizeFloat(path[c][0] + 1) + "\t" + FString::SanitizeFloat(path[c][1] + 1) + "\r\n";
	}

	FString projectDir = FPaths::GameDir();
	projectDir += "Output Data/" + fileName;
	FFileHelper::SaveStringToFile(str, *projectDir);
}

static TArray<TArray<float>> makeDiscreteMap()
{
	scaleToIndex = 1 / scaleFactor;

	TArray<float> x;
	TArray<float> y;
	TArray<float> button;

	int32 xMax = 0;
	int32 yMax = 0;

	for (int32 c = 0; c < map.Num(); c++) {
		x.Add(map[c][0]);
		if (map[c][0] > xMax) {
			xMax = map[c][0];
		}
		y.Add(map[c][1]);
		if (map[c][1] > yMax) {
			yMax = map[c][1];
		}
		button.Add(map[c][2]);
	}

	xMax += expansion;
	yMax += expansion;

	TArray<TArray<float>> newMap;
	for (int32 c = 0; c < yMax * scaleToIndex; c++) {
		TArray<float> row;
		for (int32 g = 0; g < xMax * scaleToIndex; g++) {
			row.Add(0);
		}
		newMap.Add(row);
	}

	FVector2D init;
	FVector2D startLine;
	FVector2D endLine;
	bool newObst = true;
	for (int32 c = 0; c < x.Num(); c++) {
		if (newObst) {
			newObst = false;
			init = FVector2D(x[c], y[c]);
			startLine = FVector2D(x[c], y[c]);
			endLine = FVector2D(x[c + 1], y[c + 1]);
		}
		else {
			startLine = endLine;
			if (button[c] == 1) {
				endLine = FVector2D(x[c + 1], y[c + 1]);
			}
			else {
				newObst = true;
				endLine = init;
			}
		}

		float n = 0;

		while (n <= 1) {
			FVector2D index = startLine + n * (endLine - startLine);

			newMap[(index.Y - 1) * scaleToIndex][(index.X - 1) * scaleToIndex] = 1;

			n += phi;
		}
	}
	/*
	for (int32 c = 0; c < newMap.Num(); c++) {
		FString row;
		for (int32 g = 0; g < newMap[c].Num(); g++) {
			row.AppendInt(newMap[c][g]);
			row.Append(" ");
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *row);
	}
	*/

	return newMap;
}