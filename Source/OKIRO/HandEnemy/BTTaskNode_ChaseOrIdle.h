// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ChaseOrIdle.generated.h"


USTRUCT()
struct FPredictionTaskMemory
{
	GENERATED_BODY()

	TArray<UAStarNode*> PathPoints;
	int32 CurrentPathIndex;
	int32 CurrentRotateIndex;
	FRotator TargetRotation;
	FRotator PreviousRotation;
	bool alreadyCalculateCurrentRotator;
	float RotationIdleDuration;
};

/**
 * 
 */
UCLASS()
class OKIRO_API UBTTaskNode_ChaseOrIdle : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_ChaseOrIdle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float MoveSpeed = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float RotationSpeed = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float SearchFrequency = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float SearchDegree = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		TArray<float> RotationAngles;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

private:
	void UpdatePath(UBehaviorTreeComponent& OwnerComp, FPredictionTaskMemory* TaskMemory);
	UAStarNode* FindStartNode(const FVector& AILocation, const FVector& PlayerLocation, const TArray<UAStarNode*>& Nodes);
	UAStarNode* FindEndNode(const FVector& AILocation,const FVector& PlayerPointWhenDisappear, const TArray<UAStarNode*>& Nodes);
};
