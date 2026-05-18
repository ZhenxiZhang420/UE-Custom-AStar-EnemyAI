// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Patrol.generated.h"

UENUM(BlueprintType)
enum class EEnemyState_Cpp : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Patrol      UMETA(DisplayName = "Patrol"),
	WaitToChase UMETA(DisplayName = "WaitToChase"),
	Chase       UMETA(DisplayName = "Chase"),
	Attack      UMETA(DisplayName = "Attack"),
	SearchPlayer      UMETA(DisplayName = "SearchPlayer")
};



USTRUCT()
struct FPatrolTaskMemory
{
	GENERATED_BODY()

		TArray<UAStarNode*> PathPoints;
	int32 CurrentPathIndex;
	int32 LastSelectedIndex;
	int32 LastTargetIndex;
};


/**
 *
 */
UCLASS()
class OKIRO_API UBTTask_Patrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Patrol();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float PatrolSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float RotationSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float SearchFrequency = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float SearchDegree = 15.0f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	int32 GetNextPatrolPointIndex(int32 LastIndex, int32 TotalPoints) const;
	void UpdatePath(UBehaviorTreeComponent& OwnerComp, FPatrolTaskMemory* TaskMemory);
	UAStarNode* FindStartNode(const FVector& InitLocation, const TArray<UAStarNode*>& Nodes);
	UAStarNode* FindEndNode(const TArray<UAStarNode*>& Nodes);
};
