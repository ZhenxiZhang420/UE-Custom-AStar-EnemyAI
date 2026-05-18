// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AStarAlgorithm.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UAStarAlgorithm : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		static TArray<FVector> FindPath(UAStarNode* StartNode, UAStarNode* EndNode);
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
		static TArray<UAStarNode*> FindPathReturnAStarNode(UAStarNode* StartNode, UAStarNode* EndNode);

private:
	static void ResetNodes(TArray<UAStarNode*>& Nodes);
	
};
