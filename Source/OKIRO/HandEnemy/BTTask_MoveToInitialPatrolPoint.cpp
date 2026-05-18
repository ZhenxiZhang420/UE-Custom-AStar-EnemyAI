// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToInitialPatrolPoint.h"
#include "AIController.h"
#include "AStarNode.h"
#include "AStarAlgorithm.h"
#include <Kismet/KismetSystemLibrary.h>
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/GameplayStatics.h>
#include "MoveFunctionLibrary.h"
#include "BTTask_Patrol.h"


UBTTask_MoveToInitialPatrolPoint::UBTTask_MoveToInitialPatrolPoint()
{
    NodeName = "Move To Initial Patrol Point";
    bNotifyTick = true;
}

uint16 UBTTask_MoveToInitialPatrolPoint::GetInstanceMemorySize() const
{
    return sizeof(FMoveToInitLocTaskMemory);
}

EBTNodeResult::Type UBTTask_MoveToInitialPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController->GetPawn();
    FMoveToInitLocTaskMemory* TaskMemory = (FMoveToInitLocTaskMemory*)NodeMemory;
    UBlackboardComponent* BlackboardComponent = AIController ? AIController->GetBlackboardComponent() : nullptr;

    if (!ControlledPawn->GetName().Contains("BP_HandEnemy8"))
    {
        UpdatePath(OwnerComp, TaskMemory);

        UChildActorComponent* ChildActorComp = ControlledPawn->FindComponentByClass<UChildActorComponent>();
        if (ChildActorComp)
        {
            FRotator InitFogRotation = BlackboardComponent->GetValueAsRotator("InitFogRotation");
            ChildActorComp->SetRelativeRotation(InitFogRotation);
        }

        return EBTNodeResult::InProgress;
    }

    return EBTNodeResult::Failed;
}

void UBTTask_MoveToInitialPatrolPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    FMoveToInitLocTaskMemory* TaskMemory = (FMoveToInitLocTaskMemory*)NodeMemory;
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;

    if (BlackboardComp->GetValueAsBool("PlayerInSight"))
    {
        UE_LOG(LogTemp, Log, TEXT("Change EnemyState_Cpp to WaitToChase before in move to initial position"));
        //BlackboardComp->SetValueAsEnum("EnemyState", (uint8)EEnemyState_Cpp::WaitToChase);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }


    if (TaskMemory->CurrentPathIndex < TaskMemory->PathPoints.Num())
    {
        FVector TargetLocation = TaskMemory->PathPoints[TaskMemory->CurrentPathIndex]->Position;
        bool bReached = UMoveFunctionLibrary::MoveToLocation(ControlledPawn, TargetLocation, DeltaSeconds, MoveSpeed, RotationSpeed);

        if (bReached)
        {
            TaskMemory->CurrentPathIndex++;
        }
    }
    else
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

void UBTTask_MoveToInitialPatrolPoint::UpdatePath(UBehaviorTreeComponent& OwnerComp, FMoveToInitLocTaskMemory* TaskMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;

    if (ControlledPawn && PlayerActor && EnemyAIController)
    {
        UAStarNode* StartNode = FindAINearestNode(ControlledPawn->GetActorLocation(),EnemyAIController->ChasePoints);

        UAStarNode* EndNode = FindEndNode(BlackboardComp->GetValueAsVector("initLocation"), EnemyAIController->ChasePoints);
        //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());

        if (StartNode && EndNode)
        {
            TaskMemory->PathPoints = UAStarAlgorithm::FindPathReturnAStarNode(StartNode, EndNode);
            TaskMemory->CurrentPathIndex = 0;

            if (ShowPath) 
            {
                //Draw Debug Line
                for (int32 i = 0; i < TaskMemory->PathPoints.Num() - 1; ++i)
                {
                    FVector Start = TaskMemory->PathPoints[i]->Position;
                    FVector End = TaskMemory->PathPoints[i + 1]->Position;
                    if (FMath::IsNearlyEqual(Start.X, End.X) && FMath::IsNearlyEqual(Start.Y, End.Y))
                    {
                        UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 10.0f, FLinearColor::Blue, 3.0f, 5.0f);
                    }
                    else
                    {
                        UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 10.0f, FLinearColor::Green, 3.0f, 5.0f);
                    }
                }
            }
        }
    }
}

UAStarNode* UBTTask_MoveToInitialPatrolPoint::FindEndNode(const FVector& InitLocation, const TArray<UAStarNode*>& Nodes) const
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    UAStarNode* EndNode = nullptr;

    // 遍历所有节点以找到位置匹配的节点
    for (UAStarNode* Node : Nodes)
    {
        if (Node && Node->Position.Equals(InitLocation, 1.0f))  // 使用一定的容忍度来比较位置
        {
            EndNode = Node;
            break;
        }
    }

    return EndNode;
}

UAStarNode* UBTTask_MoveToInitialPatrolPoint::FindAINearestNode(const FVector& AILocation,const TArray<UAStarNode*>& Nodes) const
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    UAStarNode* NearestNode = nullptr;
    float NearestDistance = TNumericLimits<float>::Max();
    for (UAStarNode* Node : Nodes)
    {

        if (Node->Neighbors.Num() > 0) 
        {
            float Distance = FVector::Dist(AILocation, Node->Position);
            if (Distance < NearestDistance)
            {
                NearestNode = Node;
                NearestDistance = Distance;
            }
        }
    }

    return NearestNode;
}

