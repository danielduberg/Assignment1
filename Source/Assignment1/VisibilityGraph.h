// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

static TMultiMap<FVector2D, FVector2D> visibilityGraph;


static TMultiMap<FVector2D, FVector2D> getVisibilityGraph(TArray<TArray<FVector2D>> edges, TArray<FVector2D> vertices);

static bool intersect(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4);

static bool canSee(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> & edges);

static void add(FVector2D start, FVector2D end);

TMultiMap<FVector2D, FVector2D> getVisibilityGraph(TArray<TArray<FVector2D>> edges, TArray<FVector2D> vertices)
{
	for (int32 c = 0; c < vertices.Num(); c++) {
		for (int32 g = 0; g < vertices.Num(); g++) {
			if (c == g) continue;
			if (canSee(vertices[c], vertices[g], edges)) {
				add(vertices[c], vertices[g]);
				//add(vertices[g], vertices[c]);
			}
		}
	}

	TArray<FVector2D> keys;
	visibilityGraph.GetKeys(keys);

	UE_LOG(LogTemp, Warning, TEXT("Visibility graph:"));
	for (int32 c = 0; c < keys.Num(); c++) {
		TArray<FVector2D> connected;
		 visibilityGraph.MultiFind(keys[c], connected);

		 FString str;
		for (int32 g = 0; g < connected.Num(); g++) {
			str.Append("(");
			str.Append(FString::SanitizeFloat(connected[g].X));
			str.Append(", ");
			str.Append(FString::SanitizeFloat(connected[g].Y));
			str.Append("), ");
		}

		UE_LOG(LogTemp, Warning, TEXT("Key #%d (%f, %f): %s\r\n"), c+1, keys[c].X, keys[c].Y, *str);
	}

	return visibilityGraph;
}

bool intersect(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4)
{
	FVector2D direction1 = point2 - point1;
	FVector2D direction2 = point4 - point3;

	// For sanity
	if (-direction2.X * direction1.Y + direction1.X * direction2.Y == 0) {
		return false;
	}

	float s = (-direction1.Y * (point1.X - point3.X) + direction1.X * (point1.Y - point3.Y)) / (-direction2.X * direction1.Y + direction1.X * direction2.Y);
	float t = (direction2.X * (point1.Y - point3.Y) - direction2.Y * (point1.X - point3.X)) / (-direction2.X * direction1.Y + direction1.X * direction2.Y);

	// For John
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		return true;
	}

	return false;
}

bool canSee(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> & edges)
{
	for (int32 c = 0; c < edges.Num(); c++) {
		if ((edges[c].Contains(start) && edges[c].Contains(end))) {
			return true;
		}
	}

	for (int32 c = 0; c < edges.Num(); c++) {

		if (start == edges[c][0] || start == edges[c][1] || end == edges[c][0] || end == edges[c][1]) continue;

		if (intersect(start, end, edges[c][0], edges[c][1])) {
			return false;
		}
	}

	return true;
}

void add(FVector2D start, FVector2D end)
{
	visibilityGraph.Add(start, end);
}
