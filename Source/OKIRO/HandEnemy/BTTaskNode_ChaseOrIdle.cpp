// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_ChaseOrIdle.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_Patrol.h"
#include "EnemyAIController.h"
#include "AStarAlgorithm.h"
#include "MoveFunctionLibrary.h"
#include "AIController.h"
#include <Kismet/GameplayStatics.h>
#include "BTTask_Attack.h"
#include "BTTask_SearchPlayer.h"

UBTTaskNode_ChaseOrIdle::UBTTaskNode_ChaseOrIdle()
{
    NodeName = "Chase Player or Idle after attack Player";
    bNotifyTick = true;
}

uint16 UBTTaskNode_ChaseOrIdle::GetInstanceMemorySize() const
{
    return sizeof(FPredictionTaskMemory);
}

EBTNodeResult::Type UBTTaskNode_ChaseOrIdle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    BlackboardComp->SetValueAsVector("PlayerPointWhenLost", FVector::ZeroVector);

    if (PlayerActor)
    {
        FProperty* Prop = PlayerActor->GetClass()->FindPropertyByName(FName("CanMove"));
        if (Prop && Prop->IsA<FBoolProperty>())
        {
            FBoolProperty* CanMoveProp = CastField<FBoolProperty>(Prop);
            bool bCanMove = CanMoveProp->GetPropertyValue_InContainer(PlayerActor);
            if (bCanMove)
            {
                BlackboardComp->SetValueAsEnum("SearchPlayerState", static_cast<uint8>(ESearchPlayerState_Cpp::SearchPlayerAfterAttack));
                BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::SearchPlayer));

                FVector PlayerPointWhenLost = PlayerActor->GetActorLocation();
                BlackboardComp->SetValueAsVector("PlayerPointWhenLost", PlayerPointWhenLost);

                EAttackType ActivationAttackType = static_cast<EAttackType>(BlackboardComp->GetValueAsEnum("RandomAttackType"));
                USkeletalMeshComponent* SkeletalMeshComp = ControlledPawn->FindComponentByClass<USkeletalMeshComponent>();
                UAnimInstance* AnimInstance = SkeletalMeshComp ? SkeletalMeshComp->GetAnimInstance() : nullptr;
                if (AnimInstance)
                {
                    FName PropertyName = NAME_None;
                    switch (ActivationAttackType)
                    {
                    case EAttackType::Attack:
                        PropertyName = FName(TEXT("IsAttack?"));
                        break;
                    case EAttackType::Pinch:
                        PropertyName = FName(TEXT("Is_pinch?"));
                        break;
                    case EAttackType::Pat:
                        PropertyName = FName(TEXT("Is_pat?"));
                        break;
                    default:
                        PropertyName = FName(TEXT("IsAttack?"));
                        break;
                    }

                    FProperty* AttackProp = AnimInstance->GetClass()->FindPropertyByName(PropertyName);
                    if (AttackProp && AttackProp->IsA<FBoolProperty>())
                    {
                        FBoolProperty* BoolProp = CastField<FBoolProperty>(AttackProp);
                        BoolProp->SetPropertyValue_InContainer(AnimInstance, false);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Property %s not found in AnimInstance"), *PropertyName.ToString());
                    }

                    FName AlertPropName = FName(TEXT("IsAlert?"));
                    FProperty* AlertProp = AnimInstance->GetClass()->FindPropertyByName(AlertPropName);
                    if (AlertProp && AlertProp->IsA<FBoolProperty>())
                    {
                        FBoolProperty* AlertBoolProp = CastField<FBoolProperty>(AlertProp);
                        AlertBoolProp->SetPropertyValue_InContainer(AnimInstance, true);
                    }
                }
            }
            else
            {
                BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
            }
        }
    }

    UChildActorComponent* ChildActorComp = ControlledPawn ? ControlledPawn->FindComponentByClass<UChildActorComponent>() : nullptr;
    if (ChildActorComp)
    {
        ChildActorComp->SetRelativeRotation(BlackboardComp->GetValueAsRotator("InitFogRotation"));
    }
    return EBTNodeResult::Succeeded;
}

void UBTTaskNode_ChaseOrIdle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    FVector PlayerPointWhenDisappear = BlackboardComp->GetValueAsVector("PlayerPointWhenDisappear");
    FPredictionTaskMemory* TaskMemory = (FPredictionTaskMemory*)NodeMemory;

    const FName EnemyStateKeyName = "EnemyState";
    EEnemyState_Cpp EnemyState = static_cast<EEnemyState_Cpp>(BlackboardComp->GetValueAsEnum(EnemyStateKeyName));

    if (EnemyState == EEnemyState_Cpp::Idle)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    bool LostPlayer = BlackboardComp->GetValueAsBool("LostPlayer"); 
    if (!LostPlayer) 
    {
        BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Chase));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    //bool PlayerInSight = BlackboardComp->GetValueAsBool("PlayerInSight");
    //if (PlayerInSight) 
    //{
    //    BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Chase));
    //    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    //    return;
    //}

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
        //USkeletalMeshComponent* SkeletalMeshComp = ControlledPawn->FindComponentByClass<USkeletalMeshComponent>();
        //UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();

        //FName AlertPropName = FName(TEXT("IsAlert?"));
        //UBoolProperty* AlertBoolProp = FindField<UBoolProperty>(AnimInstance->GetClass(), AlertPropName);
        //AlertBoolProp->SetPropertyValue_InContainer(AnimInstance, false);

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
                        BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
                        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                        return;
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
            BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Idle));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }

}

void UBTTaskNode_ChaseOrIdle::UpdatePath(UBehaviorTreeComponent& OwnerComp, FPredictionTaskMemory* TaskMemory)
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

        UAStarNode* EndNode = FindEndNode(ControlledPawn->GetActorLocation(), BlackboardComp->GetValueAsVector("PlayerPointWhenLost"),EnemyAIController->ChasePoints);
        //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());

        if (StartNode && EndNode)
        {
            TaskMemory->PathPoints = UAStarAlgorithm::FindPathReturnAStarNode(StartNode, EndNode);
            TaskMemory->CurrentPathIndex = 0;

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

UAStarNode* UBTTaskNode_ChaseOrIdle::FindStartNode(const FVector& AILocation, const FVector& PlayerLocation, const TArray<UAStarNode*>& Nodes)
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

UAStarNode* UBTTaskNode_ChaseOrIdle::FindEndNode(const FVector& AILocation,const FVector& PlayerPointWhenDisappear, const TArray<UAStarNode*>& Nodes)
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    // 计算玩家的运动方向
    FVector Direction = (PlayerPointWhenDisappear - AILocation).GetSafeNormal();
    // 预测玩家可能的未来位置，假设玩家持续以相同的速度和方向移动
    //FVector PredictedLocation = PlayerPointWhenDisappear + Direction * 1000;
    FVector PredictedLocation = PlayerPointWhenDisappear;

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

