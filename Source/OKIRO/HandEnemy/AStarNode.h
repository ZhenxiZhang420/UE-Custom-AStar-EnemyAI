// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AStarNode.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UAStarNode : public UObject
{
	GENERATED_BODY()
	
public:
    FVector Position;
    int32 Index;
    UAStarNode* Parent;
    float G;  // 从起点到该节点的代价
    float H;  // 启发式估计（从该节点到终点的估计代价）
    float F;  // 总代价

    TArray<UAStarNode*> Neighbors;

    UAStarNode();
    void PrintNodeProperties() const;

    void AddUnidirectionalNeighbor(UAStarNode* Neighbor);
    void AddBidirectionalNeighbor(UAStarNode* Neighbor);
    void RemoveAllNeighbors();
    void RemoveNeighbor(UAStarNode* NeighborToRemove);
    UAStarNode* GetNeighborAt(int32 Index) const;
};
