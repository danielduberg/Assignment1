// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class ASSIGNMENT1_API VisibilityGraph
{
public:
	static TMap<FVector2D, TArray<FVector2D>> visibilityGraph;


	static TMap<FVector2D, TArray<FVector2D>> getVisibilityGraph(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> edges, TArray<FVector2D> vertices)
	{
		vertices.Add(start);
		vertices.Add(end);
		for (int32 c = 0; c < vertices.Num(); c++) {
			for (int32 g = c + 1; g < vertices.Num(); g++) {
				if (canSee(vertices[c], vertices[g], edges)) {
					add(vertices[c], vertices[g]);
					add(vertices[g], vertices[c]);
				}
			}
		}

		return visibilityGraph;
	}

	static bool intersect(FVector2D point1, FVector2D point2, FVector2D point3, FVector2D point4)
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

private:
	static bool canSee(FVector2D start, FVector2D end, TArray<TArray<FVector2D>> & edges)
	{
		for (int32 c = 0; c < edges.Num(); c++) {
			if (intersect(start, end, edges[c][0], edges[c][1]) && !(edges[c].Contains(start) && edges[c].Contains(end))) {
				return false;
			}
		}

		return true;
	}

	static void add(FVector2D start, FVector2D end)
	{
		TArray<FVector2D> temp;
		if (visibilityGraph.Contains(start)) {
			temp = visibilityGraph[start];
		}

		temp.Add(end);

		visibilityGraph.Add(start, temp);
	}
};
