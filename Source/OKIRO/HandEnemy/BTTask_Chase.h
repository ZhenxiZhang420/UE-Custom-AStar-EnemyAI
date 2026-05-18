// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarNode.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Chase.generated.h"

USTRUCT()
struct FChaseTaskMemory
{
    GENERATED_BODY()

    TArray<UAStarNode*> PathPoints;
    UAStarNode* AIStarNode;
    UAStarNode* NodeBeforeAIMoveToCatchPlayer;
    int32 CurrentPathIndex;
    float CatchPlayerTimeHandle;
    float LostPlayerTime;
    FVector PlayerPointWhenCanCatch;
};

/**
 * 
 */
UCLASS()
class OKIRO_API UBTTask_Chase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTask_Chase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
        float ChaseSpeed = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
        float RotationSpeed = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
        float LookAtPlayerRotationSpeed = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
        float AbandonChaseAfterSeconds = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
        bool ShowChasePath = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
        float ZOffsetCatchPoint = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
        float XYOffsetCatchPoint = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
        bool CatchPlayerEnable = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
        bool ShowDisBetweenPlayerAndEnemy = false;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual uint16 GetInstanceMemorySize() const override;

private:
    UAStarNode* FindAINearestNode(const FVector& AILocation, const FVector& PlayerLocation, const TArray<UAStarNode*>& Nodes) const;
    bool IsValidNode(const UAStarNode* Node, const FVector& AILocation, const FVector& PlayerLocation, float AIDistanceToPlayer, bool CheckDistance) const;
    UAStarNode* FindPlayerNearestNode(const FVector& PlayerLocation, const FVector& AILocation, const TArray<UAStarNode*>& Nodes) const;
    bool CanReachPlayerWithoutObstacle(AActor* PlayerActor, APawn* ControlledPawn, FChaseTaskMemory* TaskMemory, AEnemyAIController* EnemyAIController);
    void ShowDistanceBetweenPlayerAndEnemy(const FVector& PlayerLocation, const FVector& AILocation, bool CanCatchPlayer, bool PlayerInSight);
    void UpdatePath(UBehaviorTreeComponent& OwnerComp, FChaseTaskMemory* TaskMemory);
    bool CanReachStartNode(const FVector& AILocation, const UAStarNode* StartNode) const;
};
