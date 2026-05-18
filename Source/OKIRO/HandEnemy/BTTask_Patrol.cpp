// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"
#include "AIController.h"
#include "AStarNode.h"
#include "AStarNodeUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AStarAlgorithm.h"
#include "MoveFunctionLibrary.h"
#include "EnemyAIController.h"
#include <Kismet/KismetSystemLibrary.h>

UBTTask_Patrol::UBTTask_Patrol()
{
    NodeName = "BTTask_Patrol";
    bNotifyTick = true;  // 确保这一行被添加到构造函数中!!!!!!
}

uint16 UBTTask_Patrol::GetInstanceMemorySize() const
{
    return sizeof(FPatrolTaskMemory);
}


EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController->GetPawn();
    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;
    FPatrolTaskMemory* TaskMemory = (FPatrolTaskMemory*)NodeMemory;

    if (AIPawn->GetName().Contains("BP_HandEnemy7") || AIPawn->GetName().Contains("BP_HandEnemy2") || AIPawn->GetName().Contains("BP_HandEnemy3") || AIPawn->GetName().Contains("BP_HandEnemy1"))
    {
        TaskMemory->CurrentPathIndex = 0;
    }
    else
    {
        UpdatePath(OwnerComp, TaskMemory);
    }
    return EBTNodeResult::InProgress;
    //if (AIPawn->GetName().Equals("BP_HandEnemy_2"))
    //{
    //    FPatrolTaskMemory* TaskMemory = (FPatrolTaskMemory*)NodeMemory;

    //    UAStarNode* StartNode = nullptr;
    //    int32 StartIndex;
    //    // 检查是否有上一次的目标位置
    //    if (TaskMemory->LastTargetIndex == INDEX_NONE)
    //    {
    //        // 如果没有上一次的目标位置，使用初始点
    //        StartIndex = 12; 
    //        StartNode = EnemyAIController->PatrolPoints[StartIndex];
    //        TaskMemory->LastTargetIndex = StartIndex;
    //        
    //    }
    //    else
    //    {
    //        // 否则，使用上一次的目标位置作为起点
    //        StartIndex = TaskMemory->LastTargetIndex;
    //        StartNode = EnemyAIController->PatrolPoints[StartIndex];
    //    }

    //    int32 TotalPoints = EnemyAIController->PatrolPoints.Num();
    //    //int32 TargetIndex = GetNextPatrolPointIndex(TaskMemory->LastSelectedIndex, TotalPoints);
    //    int32 TargetIndex = FMath::RandRange(0, TotalPoints - 1);
    //    TaskMemory->LastSelectedIndex = TargetIndex;
    //    UAStarNode* TargetNode = EnemyAIController->PatrolPoints[TargetIndex];

    //    TaskMemory->PathPoints = UAStarAlgorithm::FindPathReturnAStarNode(StartNode, TargetNode);
    //    TaskMemory->CurrentPathIndex = 0;

    //    //UAStarNodeUtils::VisualizeLoadedGrid(GetWorld(), "PatrolPoints.bin", "/GridData/BP_HandEnemy1/");
    //    //UE_LOG(LogTemp, Log, TEXT("PatrolPoints Length: %d"), TaskMemory->PatrolPoints.Num());
    //    return EBTNodeResult::InProgress;
    //}

    //return EBTNodeResult::Failed;
}

void UBTTask_Patrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    FPatrolTaskMemory* TaskMemory = (FPatrolTaskMemory*)NodeMemory;
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;
    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    if (!ControlledPawn || !BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 检查 EnemyState 是否为 WaitToChase
    const FName EnemyStateKeyName = "EnemyState";
    EEnemyState_Cpp EnemyState = static_cast<EEnemyState_Cpp>(BlackboardComp->GetValueAsEnum(EnemyStateKeyName));

    // 输出 EnemyState 的值到日志
    //const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState_Cpp"), true);
    //FString EnemyStateString = EnumPtr ? EnumPtr->GetNameStringByValue(static_cast<int64>(EnemyState)) : TEXT("Invalid");
    //UE_LOG(LogTemp, Log, TEXT("Current EnemyState: %s"), *EnemyStateString);
    
    //UE_LOG(LogTemp, Log, TEXT("Patrol tick task before check PlayerInSight"));
    if (BlackboardComp->GetValueAsBool("PlayerInSight"))
    {
        BlackboardComp->SetValueAsEnum(EnemyStateKeyName, (uint8)EEnemyState_Cpp::WaitToChase);
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    if (ControlledPawn->GetName().Contains("BP_HandEnemy7") || ControlledPawn->GetName().Contains("BP_HandEnemy2") || ControlledPawn->GetName().Contains("BP_HandEnemy3") || ControlledPawn->GetName().Contains("BP_HandEnemy1"))
    {
        if (TaskMemory->CurrentPathIndex < EnemyAIController->PatrolPoints.Num())
        {
            FVector TargetLocation = EnemyAIController->PatrolPoints[TaskMemory->CurrentPathIndex]->Position;
            bool bReached = UMoveFunctionLibrary::MoveToLocation(ControlledPawn, TargetLocation, DeltaSeconds, PatrolSpeed, RotationSpeed);
            //UMoveFunctionLibrary::test(GetWorld(), ControlledPawn, TargetLocation,DeltaSeconds, RotationSpeed, SearchFrequency, SearchDegree);
            UMoveFunctionLibrary::SearchPlayer(GetWorld(), ControlledPawn, TargetLocation, DeltaSeconds, RotationSpeed, SearchFrequency, SearchDegree);
            //if (ControlledPawn->GetName().Contains("BP_HandEnemy7") || ControlledPawn->GetName().Contains("BP_HandEnemy3"))
            //{
            //    UMoveFunctionLibrary::SearchPlayer(GetWorld(), ControlledPawn, TargetLocation, DeltaSeconds, RotationSpeed, SearchFrequency, SearchDegree);
            //}

            if (bReached)
            {
                TaskMemory->CurrentPathIndex++;
            }
        }
        else
        {
            TaskMemory->CurrentPathIndex = 0;
            //UpdatePath(OwnerComp, TaskMemory);
        }
    }
    else
    {
        if (TaskMemory->CurrentPathIndex < TaskMemory->PathPoints.Num())
        {
            FVector TargetLocation = TaskMemory->PathPoints[TaskMemory->CurrentPathIndex]->Position;
            //bool bReached = UMoveFunctionLibrary::MoveToLocation(ControlledPawn, TargetLocation, DeltaSeconds, PatrolSpeed, RotationSpeed);
            bool bReached = UMoveFunctionLibrary::MovePawnToLocation(ControlledPawn, TargetLocation, DeltaSeconds, PatrolSpeed, RotationSpeed);
            if (bReached)
            {
                TaskMemory->CurrentPathIndex++;
            }
        }
        else
        {
            UpdatePath(OwnerComp, TaskMemory);
        }
    }
}

void UBTTask_Patrol::UpdatePath(UBehaviorTreeComponent& OwnerComp, FPatrolTaskMemory* TaskMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;

    if (ControlledPawn && EnemyAIController)
    {
        UAStarNode* StartNode = FindStartNode(ControlledPawn->GetActorLocation(), EnemyAIController->PatrolPoints);
        if (StartNode == nullptr)
        {
            StartNode = FindStartNode(BlackboardComp->GetValueAsVector("InitLocation"), EnemyAIController->PatrolPoints);
        }


        UAStarNode* EndNode = FindEndNode(EnemyAIController->PatrolPoints);
        //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());

        if (StartNode && EndNode)
        {
            TaskMemory->PathPoints = UAStarAlgorithm::FindPathReturnAStarNode(StartNode, EndNode);
            TaskMemory->CurrentPathIndex = 0;

            ////Draw Debug Line
            //for (int32 i = 0; i < TaskMemory->PathPoints.Num() - 1; ++i)
            //{
            //    FVector Start = TaskMemory->PathPoints[i]->Position;
            //    FVector End = TaskMemory->PathPoints[i + 1]->Position;
            //    if (FMath::IsNearlyEqual(Start.X, End.X) && FMath::IsNearlyEqual(Start.Y, End.Y))
            //    {
            //        UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 10.0f, FLinearColor::Blue, 3.0f, 5.0f);
            //    }
            //    else
            //    {
            //        UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Start, End, 10.0f, FLinearColor::Green, 3.0f, 5.0f);
            //    }
            //}
        }
    }
}

UAStarNode* UBTTask_Patrol::FindStartNode(const FVector& InitLocation, const TArray<UAStarNode*>& Nodes)
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

UAStarNode* UBTTask_Patrol::FindEndNode(const TArray<UAStarNode*>& Nodes)
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    UAStarNode* EndNode = nullptr;

    int32 TargetIndex = FMath::RandRange(0, Nodes.Num() - 1);
    EndNode = Nodes[TargetIndex];
    return EndNode;
}

int32 UBTTask_Patrol::GetNextPatrolPointIndex(int32 LastIndex, int32 TotalPoints) const
{
    TArray<int32> ValidIndices;

    if (LastIndex >= 0 && LastIndex <= 4)
    {
        // 上一个点在0-4范围内，选择20-24范围内的点
        for (int32 i = 20; i <= 24; i++)
        {
            if (i < TotalPoints)
            {
                ValidIndices.Add(i);
            }
        }
    }
    else if (LastIndex >= 20 && LastIndex <= 24)
    {
        // 上一个点在20-24范围内，选择0-4范围内的点
        for (int32 i = 0; i <= 4; i++)
        {
            if (i < TotalPoints)
            {
                ValidIndices.Add(i);
            }
        }
    }
    else
    {
        // 初始选择或者上一个点不在预期范围内，随机选择0-4范围内的点
        for (int32 i = 0; i <= 4; i++)
        {
            if (i < TotalPoints)
            {
                ValidIndices.Add(i);
            }
        }
    }

    if (ValidIndices.Num() > 0)
    {
        return ValidIndices[FMath::RandRange(0, ValidIndices.Num() - 1)];
    }

    // 如果没有找到有效点，返回一个随机点
    return FMath::RandRange(0, TotalPoints - 1);
}
