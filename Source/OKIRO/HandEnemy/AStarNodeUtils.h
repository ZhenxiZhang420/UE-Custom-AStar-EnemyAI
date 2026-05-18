// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AStarNodeUtils.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UAStarNodeUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FString FileNameDebug;
	static FString PathDebug;

	static void SaveGridToBinaryFile(const TArray<UAStarNode*>& GridNodes, const FString& FileName, const FString& Path);
	static void SaveBoundaryConnectionsToFile(const TMap<int32, int32>& BoundaryConnections, const FString& FileName, const FString& Path);
	static TArray<UAStarNode*> GenerateNodeGrid(UWorld* World, FVector Center, int32 CellsLeft, int32 CellsRight, int32 CellsForward, int32 CellsBackward, float CellSize, float RotationAngle);
	static TArray<UAStarNode*> LoadGridFromBinaryFile(const FString& FileName, const FString& Path);
	static TMap<int32, int32> LoadBoundaryConnectionsFromFile(const FString& FileName, const FString& Path);
	static void VisualizeLoadedGrid(UWorld* World, const FString& FileName, const FString& Path);
	static void VisualizeLoadedGrid(UWorld* World, const FString& FileName, const FString& Path, const FColor& LineColor);
	static void VisualizeLoadedGrid(UWorld* World, const TArray<UAStarNode*>& GridNodes);
	static void VisualizeLoadedGrid(UWorld* World, const FString& FileName, const FString& Path, const FVector& PlayerPosition);
	static void SetNodePositionByIndex(int32 NodeIndex, const FVector& NewPosition);
	static void CreateNode(UWorld* World, FVector Position, const TArray<int32>& NeighborNodeIDs);
	static void CreateMultipleNodes(UWorld* World, FString& FileName, FString& Path, const TArray<FVector>& Positions);
	static void RemoveNeighborFromNode(int32 NodeAIndex, int32 NodeBIndex);
	static void RemoveAllNeighbor(int32 NodeIndex);
	static void ShowNode(int Index);
	static void AddBidirectionalNeighbor(int32 NodeAIndex, int32 NodeBIndex);
	static void AddPatrolPoint(UWorld* World, int32 NodeIndex, const TArray<int32>& NeighborNodeIDs);
	static void ModifyNodePositionByIndex(int32 NodeIndex, FString& axis, float offset);
	static void DeleteNode(int32 NodeIndex);
	static void ClearGridCache();
private:
	static void DrawGridNodesFromBin(UWorld* World, const TArray<FVector>& NodePositions, const TArray<TArray<FVector>>& NeighborPositions, const TArray<int32>& NodeIndexes, float Duration, float Thickness);
	static void DrawGridNodesFromBin(UWorld* World, const TArray<FVector>& NodePositions, const TArray<TArray<FVector>>& NeighborPositions, const TArray<int32>& NodeIndexes, float Duration, float Thickness, const FColor& LineColor);
	static TMap<FString, TArray<UAStarNode*>> GridCache;
};
