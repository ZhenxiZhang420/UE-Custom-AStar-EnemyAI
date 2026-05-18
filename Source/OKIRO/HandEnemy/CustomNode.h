// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomNode.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UCustomNode : public UObject
{
	GENERATED_BODY()

public:
    // Constructor
    UCustomNode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node Properties")
        FVector Position;

    // Property to hold the reference to the 10 neighbor nodes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nodes")
        TArray<UCustomNode*> Neighbors;

    // Function to add a neighbor to the node
    UFUNCTION(BlueprintCallable, Category = "Nodes")
        void AddNeighbor(UCustomNode* Neighbor);

    // Function to get a neighbor from the list
    UFUNCTION(BlueprintCallable, Category = "Nodes")
        UCustomNode* GetNeighborAt(int32 Index) const;

    UFUNCTION(BlueprintCallable, Category = "Nodes")
        void RemoveNeighbor(UCustomNode* NeighborToRemove);

    UFUNCTION(BlueprintCallable, Category = "Nodes")
        void AddBidirectionalNeighbor(UCustomNode* Neighbor);
    
    UFUNCTION(BlueprintCallable, Category = "Nodes")
        void RemoveAllNeighbors();
	
};
