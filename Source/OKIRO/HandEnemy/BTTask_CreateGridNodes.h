// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNodeUtils.h"
#include "CustomNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CreateGridNodes.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UBTTask_CreateGridNodes : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy2 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy3 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy4 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy5 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy6 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugPatrolPoints")
		bool DebugEnemy7 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugChasePoints")
		bool ShowFullMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugChasePoints")
		bool DebugScenario1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugChasePoints")
		bool DebugScenario2 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugChasePoints")
		bool DebugScenario3 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugChasePoints")
		bool DebugScenario4 = false;


protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	TArray<UCustomNode*> LoadGridFromBinaryFile(const FString& FileName);
	TArray<UAStarNode*> ConvertAndSaveGridToAStarNodes(const TArray<UCustomNode*>& CustomNodes);
};
