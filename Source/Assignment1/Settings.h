// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>

#define OUTPUT
#define MATLAB

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
static TArray<TArray<FVector2D>> obstacles;
static TArray<TArray<FVector2D>> edges;
static TArray<FVector2D> vertices;
static TMap<FVector2D, FVector2D> verticesToOriginal;

static bool positionsRead = false;
static TArray<TArray<float>> positions;

static const float scaleFactor = 1;
static const float phi = 0.01;
static float scaleToIndex = 1;
static const float meshScale = meshSide * scaleFactor;

static const float bufferSize = 2;


/*
 * Functions
 */

static TArray<TArray<float>> readData(const FString fileName);

static TArray<TArray<float>> & getMap();

static TArray<TArray<FVector2D>> getObstacles(TArray<TArray<float>> map);

static TArray<TArray<FVector2D>> getEdges(TArray<TArray<float>> map);

static TArray<FVector2D> getVertices(TArray<TArray<float>> map);

static FVector2D makeBufferPoint(int32 obstacle, FVector2D p0, FVector2D p1, FVector2D p2);

static bool insidePoly(TArray<FVector2D> poly, FVector2D point);

static TArray<TArray<float>> & getPositions();

static void writePathToFile(TArray<FVector> path, const FString fileName);

static TArray<TArray<float>> makeDiscreteMap();

/*
 * Implementations
 */

TArray<TArray<float>> & getMap()
{
	if (!mapRead) {
		map = readData(fileMap);

		if (!binMap) {
			obstacles = getObstacles(map);

			edges = getEdges(map);

			vertices = getVertices(map);

			//map = makeDiscreteMap();
		}

		mapRead = true;
	}

	return map;
}

TArray<TArray<FVector2D>> getObstacles(TArray<TArray<float>> map)
{
	TArray<TArray<FVector2D>> obstacles;
	
	FVector2D vertice;
	bool obstDone = false;
	TArray<FVector2D> obstacle;
	for (int32 c = 0; c < map.Num(); c++) {
		if (obstDone) {
			obstDone = false;
			obstacles.Add(obstacle);
			obstacle.Empty();
		}

		vertice = FVector2D(map[c][0], map[c][1]);

		obstacle.Add(vertice);

		if (map[c][2] == 3) {
			obstDone = true;
		}
	}

	if (obstDone) {
		obstacles.Add(obstacle);
	}

#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("Obstacles:"));
	for (int32 c = 0; c < obstacles.Num(); c++) {
		UE_LOG(LogTemp, Warning, TEXT("\tObstacle #%d:"), c+1);
		FString str;
		for (int32 g = 0; g < obstacles[c].Num(); g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(obstacles[c][g].X));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(obstacles[c][g].Y));
			str.Append("), ");
		}
		UE_LOG(LogTemp, Warning, TEXT("\t\t%s\r\n"), *str);
	}
#endif
	

	return obstacles;
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

#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("Edges:"));
	for (int32 c = 0; c < edges.Num(); c++) {
		UE_LOG(LogTemp, Warning, TEXT("\tEdge #%d: (%f, %f) - (%f, %f)\r\n"), c + 1, edges[c][0].X, edges[c][0].Y, edges[c][1].X, edges[c][1].Y);
	}
#endif

	return edges;
}

TArray<FVector2D> getVertices(TArray<TArray<float>> map)
{
	TArray<FVector2D> vertices;

	FVector2D currPoint, lastPoint, nextPoint;
	for (int32 c = 0; c < obstacles.Num(); c++) {
		for (int32 g = 0; g < obstacles[c].Num(); g++) {
			if (g == 0) {
				lastPoint = obstacles[c][obstacles[c].Num() - 1];
			} else {
				lastPoint = obstacles[c][g - 1];
			}

			currPoint = obstacles[c][g];

			if (g + 1 == obstacles[c].Num()) {
				nextPoint = obstacles[c][0];
			} else {
				nextPoint = obstacles[c][g + 1];
			}

			FVector2D vertice = makeBufferPoint(c, lastPoint, currPoint, nextPoint);
			vertices.Add(vertice);
			verticesToOriginal.Add(vertice, currPoint);
		}
	}

#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("Vertices:"));
	for (int32 c = 0; c < vertices.Num(); c++) {
		UE_LOG(LogTemp, Warning, TEXT("\tVertice #%d: (%f, %f)\r\n"), c + 1, vertices[c].X, vertices[c].Y);
	}
#endif

#ifdef MATLAB
	UE_LOG(LogTemp, Warning, TEXT("Vertices:"));
	for (int32 c = 0; c < vertices.Num(); c++) {
		UE_LOG(LogTemp, Warning, TEXT("%f %f"), vertices[c].X, vertices[c].Y);
	}
#endif


	return vertices;

	/*
	TArray<FVector2D> vertices;

	for (int32 c = 0; c < map.Num(); c++) {
		vertices.Add(FVector2D(map[c][0], map[c][1]));
	}

#ifdef OUTPUT
	UE_LOG(LogTemp, Warning, TEXT("Vertices:"));
	for (int32 c = 0; c < vertices.Num(); c++) {
		UE_LOG(LogTemp, Warning, TEXT("\tVertice #%d: (%f, %f)\r\n"), c+1, vertices[c].X, vertices[c].Y);
	}
#endif

	return vertices;*/
}

FVector2D makeBufferPoint(int32 obstacle, FVector2D p0, FVector2D p1, FVector2D p2)
{
	FVector2D line1 = p1 - p0;
	FVector2D line2 = p1 - p2;

	line1.Normalize();
	line2.Normalize();

	FVector2D result = line1 + line2;
	result.Normalize();

	FVector2D bufferPoint = p1 + bufferSize * result;

	if (insidePoly(obstacles[obstacle], bufferPoint)) {
		return p1 - (bufferSize * result);
	} else {
		return bufferPoint;
	}
}

bool insidePoly(TArray<FVector2D> poly, FVector2D point)
{
	bool insidePoly = false;
	for (int32 c = 0, g = poly.Num() - 1; c < poly.Num(); g = c++) {
		if (((poly[c].Y > point.Y) != (poly[g].Y > point.Y)) &&
			(point.X < (poly[g].X - poly[c].X) * (point.Y - poly[c].Y) / (poly[g].Y - poly[c].Y) + poly[c].X)) {
			insidePoly = !insidePoly;
		}
	}

	return insidePoly;
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