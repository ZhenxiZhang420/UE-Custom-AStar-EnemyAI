// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToInitialPatrolPoint.generated.h"


USTRUCT()
struct FMoveToInitLocTaskMemory
{
    GENERATED_BODY()

    TArray<UAStarNode*> PathPoints;
    int32 CurrentPathIndex;
};
/**
 * 
 */
UCLASS()
class OKIRO_API UBTTask_MoveToInitialPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveToInitialPatrolPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
	float MoveSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
	float RotationSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
	bool ShowPath = false;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	void UpdatePath(UBehaviorTreeComponent& OwnerComp, FMoveToInitLocTaskMemory* TaskMemory);
	UAStarNode* FindEndNode(const FVector& InitLocation, const TArray<UAStarNode*>& Nodes) const;
	UAStarNode* FindAINearestNode(const FVector& AILocation, const TArray<UAStarNode*>& Nodes) const;
	virtual uint16 GetInstanceMemorySize() const override;
};
