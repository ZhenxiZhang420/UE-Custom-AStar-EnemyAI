// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveToAttackPosition.generated.h"

/**
 * 
 */
UCLASS()
class OKIRO_API UMoveToAttackPosition : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UMoveToAttackPosition();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
