// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SearchPlayer.h"
#include "EnemyAIController.h"
#include <Kismet/GameplayStatics.h>
#include "AStarAlgorithm.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_Patrol.h"
#include "MoveFunctionLibrary.h"

UBTTask_SearchPlayer::UBTTask_SearchPlayer()
{
	NodeName = "Search Player after Lost Player";
	bNotifyTick = true;
}

uint16 UBTTask_SearchPlayer::GetInstanceMemorySize() const
{
    return sizeof(FSearchPlayerTaskMemory);
}

EBTNodeResult::Type UBTTask_SearchPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;
    FVector PlayerPointWhenLost = BlackboardComp->GetValueAsVector("PlayerPointWhenLost");
    FSearchPlayerTaskMemory* TaskMemory = (FSearchPlayerTaskMemory*)NodeMemory;
    if (PlayerPointWhenLost == FVector::ZeroVector)
    {
        BlackboardComp->SetValueAsEnum("EnemyState", (uint8)EEnemyState_Cpp::Chase);
        //FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return EBTNodeResult::Failed;
    }
    else
    {
        UpdatePath(OwnerComp, TaskMemory);
        return EBTNodeResult::InProgress;
    }
}

void UBTTask_SearchPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    FSearchPlayerTaskMemory* TaskMemory = (FSearchPlayerTaskMemory*)NodeMemory;

    const FName EnemyStateKeyName = "EnemyState";
    EEnemyState_Cpp EnemyState = static_cast<EEnemyState_Cpp>(BlackboardComp->GetValueAsEnum(EnemyStateKeyName));

    if (EnemyState == EEnemyState_Cpp::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    bool PlayerInSight = BlackboardComp->GetValueAsBool("PlayerInSight");
    if (PlayerInSight) 
    {
        RestAllVariable(TaskMemory);
        BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Chase));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    uint8 Value = BlackboardComp->GetValueAsEnum("SearchPlayerState");
    ESearchPlayerState_Cpp State = static_cast<ESearchPlayerState_Cpp>(Value);
    switch (State)
    {
        case ESearchPlayerState_Cpp::SearchPlayerAfterAttack:
            if (!TaskMemory->finishSearchPlayerByRotation && !TaskMemory->finishSearchPlayerByMove)
            {
                bool finishSearchRotation = SearchPlayerByRotation(ControlledPawn, TaskMemory, DeltaSeconds);
                if (finishSearchRotation)
                {
                    TaskMemory->finishSearchPlayerByRotation = true;
                }
            }

            if (TaskMemory->finishSearchPlayerByRotation) 
            {
                bool finishSearchMove = SearchPlayerByMove(ControlledPawn, TaskMemory, DeltaSeconds);
                if (finishSearchMove)
                {
                    UE_LOG(LogTemp, Log, TEXT("Finish SearchPlayerByMove"));
                    TaskMemory->finishSearchPlayerByRotation = false;
                    TaskMemory->finishSearchPlayerByMove = true;

                }
            }

            if (!TaskMemory->finishSearchPlayerByRotation && TaskMemory->finishSearchPlayerByMove) 
            {
                bool finishSearchRotation = SearchPlayerByRotation(ControlledPawn, TaskMemory, DeltaSeconds);
                if (finishSearchRotation)
                {
                    TaskMemory->finishSearchPlayerByRotation = false;
                    TaskMemory->finishSearchPlayerByMove = false;
                    BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
                    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                    return;
                }
            }
        break;

        case ESearchPlayerState_Cpp::SearchPlayerAfterLost:
            if (!TaskMemory->finishSearchPlayerByMove) 
            {
                bool finishSearchMove = SearchPlayerByMove(ControlledPawn, TaskMemory, DeltaSeconds);
                if (finishSearchMove) 
                {
                    TaskMemory->finishSearchPlayerByMove = true;
                }
            }

            if (TaskMemory->finishSearchPlayerByMove) 
            {
                bool finishSearchRotation = SearchPlayerByRotation(ControlledPawn, TaskMemory, DeltaSeconds);
                if (finishSearchRotation) 
                {
                    UE_LOG(LogTemp, Log, TEXT("Finish SearchPlayerByRotation"));
                    TaskMemory->finishSearchPlayerByMove = false;
                    BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
                    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                    return;
                }
            }
        break;

        default:
            UE_LOG(LogTemp, Warning, TEXT("Unhandled search state: %d"), State);
            if (SearchPlayerByRotation(ControlledPawn, TaskMemory, DeltaSeconds)) {
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return;
            }
        break;
    }

    //if (TaskMemory->CurrentPathIndex < TaskMemory->PathPoints.Num())
    //{

    //    FVector TargetLocation = TaskMemory->PathPoints[TaskMemory->CurrentPathIndex]->Position;
    //    bool bMoveReached = UMoveFunctionLibrary::MoveToLocation(ControlledPawn, TargetLocation, DeltaSeconds, MoveSpeed, RotationSpeed);

    //    //UMoveFunctionLibrary::SearchPlayer(GetWorld(), ControlledPawn, TargetLocation, DeltaSeconds, RotationSpeed, SearchFrequency, SearchDegree);

    //    if (bMoveReached)
    //    {
    //        TaskMemory->CurrentPathIndex++;
    //    }
    //}
    //else
    //{
    //    //USkeletalMeshComponent* SkeletalMeshComp = ControlledPawn->FindComponentByClass<USkeletalMeshComponent>();
    //    //UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();

    //    //FName AlertPropName = FName(TEXT("IsAlert?"));
    //    //UBoolProperty* AlertBoolProp = FindField<UBoolProperty>(AnimInstance->GetClass(), AlertPropName);
    //    //AlertBoolProp->SetPropertyValue_InContainer(AnimInstance, false);

    //    SearchPlayerByRotation(ControlledPawn, TaskMemory,DeltaSeconds);
    //}
}


void UBTTask_SearchPlayer::UpdatePath(UBehaviorTreeComponent& OwnerComp, FSearchPlayerTaskMemory* TaskMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;

    if (ControlledPawn && EnemyAIController)
    {
        UAStarNode* StartNode = FindStartNode(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation(), EnemyAIController->ChasePoints);
        //if (StartNode == nullptr)
        //{
        //    StartNode = FindStartNode(BlackboardComp->GetValueAsVector("InitLocation"), EnemyAIController->ChasePoints);
        //}

        UAStarNode* EndNode = FindEndNode(ControlledPawn->GetActorLocation(), BlackboardComp->GetValueAsVector("PlayerPointWhenLost"), EnemyAIController->ChasePoints);
        //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());

        if (StartNode && EndNode)
        {
            
            TaskMemory->PathPoints = UAStarAlgorithm::FindPathReturnAStarNode(StartNode, EndNode);
            TaskMemory->CurrentPathIndex = 0;

            //Draw Debug Line
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

UAStarNode* UBTTask_SearchPlayer::FindStartNode(const FVector& AILocation, const FVector& PlayerLocation, const TArray<UAStarNode*>& Nodes)
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    UAStarNode* NearestNode = nullptr;
    float NearestAIDistance = FLT_MAX;
    float AIDistanceToPlayer = FVector::Dist(AILocation, PlayerLocation);

    for (UAStarNode* Node : Nodes)
    {
        if (Node->Neighbors.Num() > 0)  // 确保节点有邻居
        {
            float NodeDistanceToPlayer = FVector::Dist(Node->Position, PlayerLocation);
            float NodeDistanceToAI = FVector::Dist(Node->Position, AILocation);

            // 选出的点到玩家的位置的距离比 AI 当前位置到玩家的位置的距离更短，且点的高度高于玩家位置
            if (NodeDistanceToPlayer < AIDistanceToPlayer)
            {
                // 找到离 AI 最近的节点
                if (NodeDistanceToAI < NearestAIDistance)
                {
                    NearestNode = Node;
                    NearestAIDistance = NodeDistanceToAI;
                }
            }
        }
    }
    return NearestNode;
}

UAStarNode* UBTTask_SearchPlayer::FindEndNode(const FVector& AILocation, const FVector& PlayerPointWhenDisappear, const TArray<UAStarNode*>& Nodes)
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    // 计算玩家的运动方向
    FVector Direction = (PlayerPointWhenDisappear - AILocation).GetSafeNormal();
    // 预测玩家可能的未来位置，假设玩家持续以相同的速度和方向移动
    FVector PredictedLocation = PlayerPointWhenDisappear + Direction * 1000;


    UAStarNode* EndNode = nullptr;
    float MinDistance = FLT_MAX; // 初始化为最大浮点数，确保任何实际距离都会更小

    // 遍历所有节点，找到与 PlayerPointWhenDisappear 距离最近的节点
    for (UAStarNode* Node : Nodes)
    {
        if (Node)
        {
            float Distance = FVector::Dist(PlayerPointWhenDisappear, Node->Position);
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                EndNode = Node;
            }
        }
    }

    return EndNode;
}

bool UBTTask_SearchPlayer::SearchPlayerByRotation(APawn* ControlledPawn,FSearchPlayerTaskMemory* TaskMemory, float DeltaSeconds)
{
    if (TaskMemory->CurrentRotateIndex < RotationAngles.Num())
    {
        if (!TaskMemory->alreadyCalculateCurrentRotator)
        {
            TaskMemory->TargetRotation = ControlledPawn->GetActorRotation() - FRotator(0, RotationAngles[TaskMemory->CurrentRotateIndex], 0);
            TaskMemory->alreadyCalculateCurrentRotator = true;
        }
        bool bReached = UMoveFunctionLibrary::RotateCertainAngle(ControlledPawn, TaskMemory->TargetRotation, DeltaSeconds, RotationSpeed);
        if (bReached)
        {
            TaskMemory->CurrentRotateIndex++;
            TaskMemory->alreadyCalculateCurrentRotator = false;
            TaskMemory->RotationIdleDuration = 0;
            UE_LOG(LogTemp, Log, TEXT("Reached the target rotation angle."));
        }
        else
        {
            TaskMemory->RotationIdleDuration += DeltaSeconds;
            if (TaskMemory->RotationIdleDuration >= 1.5f)
            {
                UE_LOG(LogTemp, Log, TEXT("no rotation over 1.5f."));
                if (TaskMemory->CurrentRotateIndex < RotationAngles.Num())
                {
                    TaskMemory->CurrentRotateIndex++;
                    TaskMemory->alreadyCalculateCurrentRotator = false;
                    TaskMemory->RotationIdleDuration = 0;
                }
                else
                {
                    TaskMemory->RotationIdleDuration = 0;
                    TaskMemory->CurrentPathIndex = 0;
                    TaskMemory->CurrentRotateIndex = 0;
                    TaskMemory->alreadyCalculateCurrentRotator = false;
                    //BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
                    //FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                    return true;
                }
            }

            UE_LOG(LogTemp, Log, TEXT("Not yet reached the target rotation angle."));
        }
    }
    else
    {
        TaskMemory->RotationIdleDuration = 0;
        TaskMemory->CurrentPathIndex = 0;
        TaskMemory->CurrentRotateIndex = 0;
        TaskMemory->alreadyCalculateCurrentRotator = false;
        //BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
        //FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return true;
    }
    return false;
}

bool UBTTask_SearchPlayer::SearchPlayerByMove(APawn* ControlledPawn, FSearchPlayerTaskMemory* TaskMemory, float DeltaSeconds)
{
    if (TaskMemory->CurrentPathIndex < TaskMemory->PathPoints.Num())
    {

        FVector TargetLocation = TaskMemory->PathPoints[TaskMemory->CurrentPathIndex]->Position;
        bool bMoveReached = UMoveFunctionLibrary::MoveToLocation(ControlledPawn, TargetLocation, DeltaSeconds, MoveSpeed, RotationSpeed);

        //UMoveFunctionLibrary::SearchPlayer(GetWorld(), ControlledPawn, TargetLocation, DeltaSeconds, RotationSpeed, SearchFrequency, SearchDegree);

        if (bMoveReached)
        {
            TaskMemory->CurrentPathIndex++;
        }
    }
    else
    {
        return true;
    }
    return false;
}

void UBTTask_SearchPlayer::RestAllVariable(FSearchPlayerTaskMemory* TaskMemory)
{
    TaskMemory->finishSearchPlayerByRotation = false;
    TaskMemory->finishSearchPlayerByMove = false;
    TaskMemory->RotationIdleDuration = 0;
    TaskMemory->CurrentPathIndex = 0;
    TaskMemory->CurrentRotateIndex = 0;
    TaskMemory->alreadyCalculateCurrentRotator = false;
}




