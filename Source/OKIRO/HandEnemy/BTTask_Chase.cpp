// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Chase.h"
#include <Kismet/GameplayStatics.h>
#include "AStarAlgorithm.h"
#include "AIController.h"
#include "AStarNode.h"
#include "MoveFunctionLibrary.h"
#include "AStarNodeUtils.h"
#include "EnemyAIController.h"
#include <Kismet/KismetSystemLibrary.h>
#include "BTTask_Patrol.h"
#include "BTTask_SearchPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Camera/CameraComponent.h>
#include "Components/CapsuleComponent.h"

UBTTask_Chase::UBTTask_Chase()
{
    NodeName = "Update Path And Move";
    bNotifyTick = true;
}

uint16 UBTTask_Chase::GetInstanceMemorySize() const
{
    return sizeof(FChaseTaskMemory);
}

EBTNodeResult::Type UBTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UE_LOG(LogTemp, Log, TEXT("Start check NodeMemory"));
    if (!NodeMemory) {
        UE_LOG(LogTemp, Warning, TEXT("NodeMemory is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("NodeMemory OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start check AIController"));
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) {
        UE_LOG(LogTemp, Warning, TEXT("AIController is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("AIController OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start check AIPawn"));
    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) {
        UE_LOG(LogTemp, Warning, TEXT("AIPawn is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("AIPawn OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start check EnemyAIController casting"));
    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    if (!EnemyAIController) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast AIController to AEnemyAIController"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("Casting to EnemyAIController OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start reinterpret TaskMemory from NodeMemory"));
    FChaseTaskMemory* TaskMemory = reinterpret_cast<FChaseTaskMemory*>(NodeMemory);
    if (!TaskMemory) {
        UE_LOG(LogTemp, Warning, TEXT("TaskMemory is null after reinterpret_cast"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("TaskMemory reinterpretation OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start check BlackboardComponent"));
    UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();
    if (!BlackboardComponent) {
        UE_LOG(LogTemp, Warning, TEXT("BlackboardComponent is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("BlackboardComponent OK"));
    }

    BlackboardComponent->SetValueAsVector("PlayerPointWhenCanCatch", FVector::ZeroVector);
    BlackboardComponent->SetValueAsVector("PlayerPointWhenLost", FVector::ZeroVector);

    if (!AIPawn->GetName().Contains("BP_HandEnemy8")) {
        UE_LOG(LogTemp, Log, TEXT("Start update path"));
        UpdatePath(OwnerComp, TaskMemory);
        TaskMemory->CatchPlayerTimeHandle = 0;
        UE_LOG(LogTemp, Log, TEXT("Path updated successfully and CatchPlayerTimeHandle reset."));
        return EBTNodeResult::InProgress;
    }

    UE_LOG(LogTemp, Log, TEXT("Execution conditions not met, returning Failed."));
    return EBTNodeResult::Failed;
}


void UBTTask_Chase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    FChaseTaskMemory* TaskMemory = (FChaseTaskMemory*)NodeMemory;
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;

    const FName EnemyStateKeyName = "EnemyState";
    EEnemyState_Cpp EnemyState = static_cast<EEnemyState_Cpp>(BlackboardComp->GetValueAsEnum(EnemyStateKeyName));

    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);

    if (!BlackboardComp->GetValueAsBool("PlayerInSight")) 
    {
        TaskMemory->LostPlayerTime += DeltaSeconds;
        if (TaskMemory->LostPlayerTime >= 2.0f) 
        {
            TaskMemory->LostPlayerTime = 0;
            TaskMemory->AIStarNode = nullptr;
            BlackboardComp->SetValueAsEnum("SearchPlayerState", static_cast<uint8>(ESearchPlayerState_Cpp::SearchPlayerAfterLost));
            BlackboardComp->SetValueAsEnum(EnemyStateKeyName, (uint8)EEnemyState_Cpp::SearchPlayer);
            FVector PlayerPointWhenLost = PlayerActor->GetActorLocation();
            BlackboardComp->SetValueAsVector("PlayerPointWhenLost", PlayerPointWhenLost);
            //UKismetSystemLibrary::DrawDebugSphere(
            //    GetWorld(),                  // 世界上下文
            //    PlayerPointWhenLost,         // 球心位置
            //    50.0f,                       // 球体半径
            //    32,                          // 球面的细分层数
            //    FColor::Red,                 // 球体的颜色
            //    10.0f,                       // 持续时间
            //    1.0f);
            FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
            return;
        }
    }

    UMoveFunctionLibrary::LookAtPlayer(GetWorld(), ControlledPawn, PlayerActor, DeltaSeconds, LookAtPlayerRotationSpeed);

    if (ShowDisBetweenPlayerAndEnemy)
    {
        bool canCatchPlayer = CanReachPlayerWithoutObstacle(PlayerActor, ControlledPawn, TaskMemory, EnemyAIController);
        ShowDistanceBetweenPlayerAndEnemy(PlayerActor->GetActorLocation(), ControlledPawn->GetActorLocation(), canCatchPlayer, BlackboardComp->GetValueAsBool("PlayerInSight"));
    }

    if (CatchPlayerEnable)
    {
        if (BlackboardComp->GetValueAsBool("PlayerInCatchArea") && BlackboardComp->GetValueAsBool("PlayerInSight"))
        {
            BlackboardComp->SetValueAsVector("PlayerPointWhenCanCatch", ControlledPawn->GetActorLocation());
            BlackboardComp->SetValueAsEnum(EnemyStateKeyName, (uint8)EEnemyState_Cpp::Attack);
            UE_LOG(LogTemp, Log, TEXT("Can catch player because in the catch area"));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }


        bool canCatchPlayer = CanReachPlayerWithoutObstacle(PlayerActor, ControlledPawn, TaskMemory, EnemyAIController);
        float AIDistanceToPlayer = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation());
        //UE_LOG(LogTemp, Log, TEXT("Distance: %f"), AIDistanceToPlayer);
        //UE_LOG(LogTemp, Log, TEXT("canCatchPlayer: %s"), canCatchPlayer ? TEXT("true") : TEXT("false"));
        if (BlackboardComp->GetValueAsBool("PlayerInSight") && canCatchPlayer && AIDistanceToPlayer < 250)
        {
            BlackboardComp->SetValueAsVector("PlayerPointWhenCanCatch", (TaskMemory->PlayerPointWhenCanCatch + FVector(0, 0, -56)));
            TaskMemory->NodeBeforeAIMoveToCatchPlayer = TaskMemory->AIStarNode;
            TaskMemory->AIStarNode = nullptr;
            BlackboardComp->SetValueAsEnum(EnemyStateKeyName, (uint8)EEnemyState_Cpp::Attack);
            UE_LOG(LogTemp, Log, TEXT("Can catch player because pass can catch player"));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }
    }


    if (TaskMemory->CurrentPathIndex < TaskMemory->PathPoints.Num())
    {
        FVector TargetLocation = TaskMemory->PathPoints[TaskMemory->CurrentPathIndex]->Position;

        //550.f
        bool bReached = UMoveFunctionLibrary::MoveToLocationWithoutTurn(ControlledPawn, TargetLocation, DeltaSeconds, ChaseSpeed, RotationSpeed);

        if (bReached)
        {
            TaskMemory->AIStarNode = TaskMemory->PathPoints[TaskMemory->CurrentPathIndex];
            if (ControlledPawn->GetActorLocation().Equals(TaskMemory->PathPoints[0]->Position, KINDA_SMALL_NUMBER))
            {
                TaskMemory->CurrentPathIndex++;
            }
            else
            {
                //UE_LOG(LogTemp, Log, TEXT("Update Path because move to next point"));
                UpdatePath(OwnerComp, TaskMemory);
            }

        }
    }
    else
    {
        TaskMemory->CatchPlayerTimeHandle += DeltaSeconds;
        //UE_LOG(LogTemp, Log, TEXT("Update Path because finish all path"));
        UpdatePath(OwnerComp, TaskMemory);
    }

    if (TaskMemory->CatchPlayerTimeHandle >= AbandonChaseAfterSeconds)
    {
        TaskMemory->CatchPlayerTimeHandle = 0;
        BlackboardComp->SetValueAsEnum(EnemyStateKeyName, (uint8)EEnemyState_Cpp::Idle);
        TaskMemory->AIStarNode = nullptr;
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

}


void UBTTask_Chase::UpdatePath(UBehaviorTreeComponent& OwnerComp, FChaseTaskMemory* TaskMemory)
{
    if (!TaskMemory) {
        UE_LOG(LogTemp, Warning, TEXT("TaskMemory is null"));
        return;
    }
    else { UE_LOG(LogTemp, Warning, TEXT("TaskMemory ok")); }

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) {
        UE_LOG(LogTemp, Warning, TEXT("AIController is null"));
        return;
    }
    else { UE_LOG(LogTemp, Warning, TEXT("AIController ok")); }

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) {
        UE_LOG(LogTemp, Warning, TEXT("ControlledPawn is null"));
        return;
    }
    else { UE_LOG(LogTemp, Warning, TEXT("ControlledPawn ok")); }

    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerActor) {
        UE_LOG(LogTemp, Warning, TEXT("PlayerActor is null"));
        return;
    }
    else { UE_LOG(LogTemp, Warning, TEXT("PlayerActor ok")); }

    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(AIController);
    if (!EnemyAIController) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cast AIController to AEnemyAIController"));
        return;
    }
    else { UE_LOG(LogTemp, Warning, TEXT("EnemyAIController ok")); }

    if (EnemyAIController->ChasePoints.Num() == 0) {
        UE_LOG(LogTemp, Warning, TEXT("ChasePoints is empty"));
        return;
    }


    if (ControlledPawn && PlayerActor && EnemyAIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Access EnemyAIController->ChasePoints"));
        // 在继续之前检查ChasePoints是否为空
        if (EnemyAIController->ChasePoints.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("ChasePoints is null"));
            return; // 如果ChasePoints为空，则退出函数
        }
        UE_LOG(LogTemp, Warning, TEXT("finish Access EnemyAIController->ChasePoints"));
        UAStarNode* StartNode = nullptr;

        if (TaskMemory->AIStarNode == nullptr)
        {
            // 第一次调用，找到最近的AStarNode并存储索引
            StartNode = FindAINearestNode(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation(), EnemyAIController->ChasePoints);
            if (StartNode == nullptr)
            {
                StartNode = TaskMemory->NodeBeforeAIMoveToCatchPlayer;
            }
        }
        else
        {
            // 后续调用，直接使用存储的索引
            StartNode = TaskMemory->AIStarNode;
        }

        UAStarNode* EndNode = FindPlayerNearestNode(PlayerActor->GetActorLocation(), ControlledPawn->GetActorLocation(), EnemyAIController->ChasePoints);
        if (EndNode == nullptr)
        {
            EndNode = StartNode;
        }
        else
        {
            //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());
            float EndNodeDistanceToPlayer = FVector::Dist(EndNode->Position, PlayerActor->GetActorLocation());
            if (EndNodeDistanceToPlayer > 300)
            {
                EndNode = StartNode;
            }
        }

        if (StartNode && EndNode)
        {
            TaskMemory->PathPoints = UAStarAlgorithm::FindPathReturnAStarNode(StartNode, EndNode);
            TaskMemory->CurrentPathIndex = 0;

            if (ShowChasePath)
            {
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
            //Draw Debug Line
        }
    }
}

bool UBTTask_Chase::CanReachStartNode(const FVector& AILocation, const UAStarNode* StartNode) const
{
    FHitResult HitResult;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));  // 举例：检测静态世界物体
    //ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic)); // 举例：检测动态世界物体
    TArray<AActor*> ActorsToIgnore;

    bool bHasObstacle = UKismetSystemLibrary::LineTraceSingleForObjects(
        GetWorld(),
        AILocation + FVector(0, 0, 56.0f),
        StartNode->Position,
        ObjectTypes,
        false,  // 不需要复杂碰撞
        ActorsToIgnore,
        EDrawDebugTrace::None,  // 或者选择 ForOneFrame 或 ForDuration 来进行调试
        HitResult,
        true,
        FLinearColor::Green,
        FLinearColor::Red,
        5.0f
    );

    if (bHasObstacle)
    {
        return false;
        //StartNode = TaskMemory->NodeBeforeAIMoveToCatchPlayer;
        //UE_LOG(LogTemp, Log, TEXT("Use NodeBeforeAIMoveToCatchPlayer as Start node"));
    }

    //TaskMemory->AIStarNode = StartNode;
    return true;
}




UAStarNode* UBTTask_Chase::FindAINearestNode(const FVector& AILocation, const FVector& PlayerLocation, const TArray<UAStarNode*>& Nodes) const
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }

    UAStarNode* NearestNode = nullptr;
    float NearestAIDistance = FLT_MAX;
    float AIDistanceToPlayer = FVector::Dist(AILocation, PlayerLocation);

    // 第一轮尝试：符合所有条件
    for (UAStarNode* Node : Nodes)
    {
        if (Node->Neighbors.Num() > 0)
        {
            float NodeDistanceToPlayer = FVector::Dist(Node->Position, PlayerLocation);
            float NodeDistanceToAI = FVector::Dist(Node->Position, AILocation);

            if (NodeDistanceToPlayer < AIDistanceToPlayer && Node->Position.Z > PlayerLocation.Z && CanReachStartNode(AILocation, Node))
            {
                if (NodeDistanceToAI < NearestAIDistance)
                {
                    NearestNode = Node;
                    NearestAIDistance = NodeDistanceToAI;
                }
            }
        }
    }

    // 如果找到合适的节点，则直接返回
    if (NearestNode)
    {
        return NearestNode;
    }

    // 第二轮尝试：放宽 NodeDistanceToPlayer < AIDistanceToPlayer 条件
    for (UAStarNode* Node : Nodes)
    {
        if (Node->Neighbors.Num() > 0 && Node->Position.Z > PlayerLocation.Z)
        {
            float NodeDistanceToAI = FVector::Dist(Node->Position, AILocation);
            if (NodeDistanceToAI < NearestAIDistance && CanReachStartNode(AILocation, Node))
            {
                NearestNode = Node;
                NearestAIDistance = NodeDistanceToAI;
            }
        }
    }

    // 最终返回最适合的节点，或者如果都不合适，返回nullptr
    return NearestNode;
}



UAStarNode* UBTTask_Chase::FindPlayerNearestNode(const FVector& PlayerLocation, const FVector& AILocation, const TArray<UAStarNode*>& Nodes) const
{
    if (Nodes.Num() == 0)
    {
        return nullptr;
    }
    UAStarNode* NearestNode = nullptr;
    float NearestDistance = TNumericLimits<float>::Max();
    for (UAStarNode* Node : Nodes)
    {
        float zOffset = Node->Position.Z - PlayerLocation.Z;
        if (Node->Neighbors.Num() > 0 && zOffset >= 0) // 确保节点有邻居且 Z 轴值高于玩家位置
        {
            float Distance = FVector::Dist(PlayerLocation, Node->Position);
            if (Distance < NearestDistance)
            {
                NearestNode = Node;
                NearestDistance = Distance;
            }
        }
    }

    return NearestNode;
}

bool UBTTask_Chase::CanReachPlayerWithoutObstacle(AActor* PlayerActor, APawn* ControlledPawn, FChaseTaskMemory* TaskMemory, AEnemyAIController* EnemyAIController)
{
    if (!PlayerActor || !ControlledPawn)
        return false;

    // 获取AI的Pawn和Capsule组件
    UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(ControlledPawn->GetComponentByClass(UCapsuleComponent::StaticClass()));
    if (!CapsuleComp)
        return false;

    FVector StartLocation = CapsuleComp->GetComponentLocation();
    FVector EndLocation = PlayerActor->GetActorLocation();

    FVector DirectionToPlayer = EndLocation - StartLocation;
    DirectionToPlayer.Z = 0;
    DirectionToPlayer = DirectionToPlayer.GetSafeNormal();
    float DotProduct = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), DirectionToPlayer);
    const float CosineOfRequiredAngle = FMath::Cos(FMath::DegreesToRadians(10.0f));
    bool isFaceToPlayer = DotProduct > CosineOfRequiredAngle;

    FVector C = FVector(StartLocation.X, StartLocation.Y, EndLocation.Z);

    FVector Direction = C - PlayerActor->GetActorLocation();
    Direction.Normalize();

    EndLocation = PlayerActor->GetActorLocation() + Direction * XYOffsetCatchPoint;

    EndLocation.Z += ZOffsetCatchPoint;

    FHitResult OutHit;
    UE_LOG(LogTemp, Warning, TEXT("Access EnemyAIController->ActorsNeedToIgnore"));
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
    //执行sweep检测
    bool bHasHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
        GetWorld(),
        StartLocation,
        EndLocation,
        FVector(48, 48, 48),
        ControlledPawn->GetActorRotation(),
        ObjectTypes,
        false,
        EnemyAIController->ActorsNeedToIgnore,  // 忽略的Actor列表
        EDrawDebugTrace::None,
        OutHit,
        true,
        FLinearColor::Green,
        FLinearColor::Red,
        5.0f
    );
    UE_LOG(LogTemp, Warning, TEXT("Finish Access EnemyAIController->ActorsNeedToIgnore"));
    //Debug show hit object
    
    //if (bHasHit)
    //{
    //    // 如果有碰撞发生，打印碰撞的Actor的名字
    //    if (OutHit.GetActor() != nullptr)
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *OutHit.GetActor()->GetName());
    //    }
    //    return false;
    //}

    if (!bHasHit&&isFaceToPlayer)
    {
        TaskMemory->PlayerPointWhenCanCatch = EndLocation;
        return true;
    }

    // 如果有碰撞发生，返回false
    return false;
}

void UBTTask_Chase::ShowDistanceBetweenPlayerAndEnemy(const FVector& PlayerLocation, const FVector& AILocation, bool CanCatchPlayer, bool PlayerInSight)
{
    float Distance = FVector::Dist(PlayerLocation, AILocation);
    FVector MidPoint = PlayerLocation;
    MidPoint.Z += 40.f;  // 调整显示文本的高度

    FString DistanceText = FString::Printf(TEXT("Distance: %.2f"), Distance);
    FString CatchText = CanCatchPlayer ? TEXT("Can Catch Player: Yes") : TEXT("Can Catch Player: No");
    FString SightText = PlayerInSight ? TEXT("Player In Sight: Yes") : TEXT("Player In Sight: No");

    // 画出从玩家到AI的连线
    UKismetSystemLibrary::DrawDebugLine(
        GetWorld(),
        PlayerLocation,
        AILocation,
        FColor::White,
        0.0f,
        3.0f
    );

    // 在屏幕上显示距离
    UKismetSystemLibrary::DrawDebugString(
        GetWorld(),
        MidPoint,
        DistanceText,
        nullptr,
        FColor::White,
        0.0f
    );

    // 将文本向上移动以避免重叠
    FVector CatchPoint = MidPoint;
    CatchPoint.Z += 15.f;
    FVector SightPoint = CatchPoint;
    SightPoint.Z += 15.f;

    // 在屏幕上显示是否可以抓住玩家和玩家是否在视线中的信息
    UKismetSystemLibrary::DrawDebugString(
        GetWorld(),
        CatchPoint,
        CatchText,
        nullptr,
        FColor::Green,
        0.0f
    );

    UKismetSystemLibrary::DrawDebugString(
        GetWorld(),
        SightPoint,
        SightText,
        nullptr,
        FColor::Red,
        0.0f
    );
}

