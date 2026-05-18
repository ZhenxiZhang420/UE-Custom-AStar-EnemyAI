// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SpawnDecalBox.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UBTTask_SpawnDecalBox : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SpawnDecalBox();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	TSubclassOf<AActor> BlueprintClass;
	void SpawnDecalBox(const FVector& PlayerLocation);
};
