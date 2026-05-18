// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SearchPlayer.generated.h"


UENUM(BlueprintType)
enum class ESearchPlayerState_Cpp : uint8
{
	SearchPlayerAfterAttack        UMETA(DisplayName = "SearchPlayerAfterAttack"),
	SearchPlayerAfterLost      UMETA(DisplayName = "SearchPlayerAfterLost"),
};

USTRUCT()
struct FSearchPlayerTaskMemory
{
	GENERATED_BODY()
	TArray<UAStarNode*> PathPoints;
	int32 CurrentPathIndex;
	int32 CurrentRotateIndex;
	FRotator TargetRotation;
	FRotator PreviousRotation;
	bool alreadyCalculateCurrentRotator;
	float RotationIdleDuration;
	bool finishSearchPlayerByRotation;
	bool finishSearchPlayerByMove;
};

/**
 * 
 */
UCLASS()
class OKIRO_API UBTTask_SearchPlayer : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_SearchPlayer();

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
	void UpdatePath(UBehaviorTreeComponent& OwnerComp, FSearchPlayerTaskMemory* TaskMemory);
	UAStarNode* FindStartNode(const FVector& AILocation, const FVector& PlayerLocation, const TArray<UAStarNode*>& Nodes);
	UAStarNode* FindEndNode(const FVector& AILocation, const FVector& PlayerPointWhenDisappear, const TArray<UAStarNode*>& Nodes);
	bool SearchPlayerByRotation(APawn* ControlledPawn, FSearchPlayerTaskMemory* TaskMemory, float DeltaSeconds);
	bool SearchPlayerByMove(APawn* ControlledPawn, FSearchPlayerTaskMemory* TaskMemory, float DeltaSeconds);
	void RestAllVariable(FSearchPlayerTaskMemory* TaskMemory);
};
