// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "AStarNode.h"
#include "AStarNodeUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"
#include <Kismet/GameplayStatics.h>
#include "OKIRO/Framework/OKIROCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_Patrol.h"

AEnemyAIController::AEnemyAIController()
{
    // 创建感知组件
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    // 创建并配置视觉感知
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
    SightConfig->SetMaxAge(MaxAge);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // 注册视觉感知到感知组件
    AIPerceptionComponent->ConfigureSense(*SightConfig);

    // 创建并配置听觉感知
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1000.0f; // 假设听觉范围为600单位
    HearingConfig->bUseLoSHearing = false; // 启用视线外的声音侦测
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    AIPerceptionComponent->ConfigureSense(*HearingConfig);

    //AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}


void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    StartEnemyStateDebug();

    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
    }

    APawn* ControlledPawn = GetPawn();
    if (ControlledPawn)
    {
        FString PawnName = ControlledPawn->GetName();
        FString BasePath = "/GridData/";
        int32 MaxEnemies = 7;
        FString EnemyBaseName = "BP_HandEnemy";

        for (int32 i = 1; i <= MaxEnemies; i++)
        {
            FString EnemyName = EnemyBaseName + FString::FromInt(i);
            if (PawnName.Contains(EnemyName))
            {
                FString DataPath = BasePath + EnemyName + "/";
                ChasePoints = UAStarNodeUtils::LoadGridFromBinaryFile("Data.bin", DataPath);
                PatrolPoints = UAStarNodeUtils::LoadGridFromBinaryFile("PatrolPoints.bin", DataPath);
                break; // 找到匹配项后跳出循环
            }
        }
    }


    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    ActorsNeedToIgnore.Add(PlayerActor);
    for (const TSoftObjectPtr<AActor>& SoftMeshRef : StaticMeshActors)
    {
        // 尝试加载软引用指向的Static Mesh Actor
        AActor* Actor = SoftMeshRef.LoadSynchronous();
        ActorsNeedToIgnore.Add(Actor);
    }


    // 确保行为树已设置
    if (BehaviorTree)
    {
        if (UseBlackboard(BehaviorTree->BlackboardAsset, BlackboardComponent))
        {
            UChildActorComponent* ChildActorComp = ControlledPawn->FindComponentByClass<UChildActorComponent>();
            if (ChildActorComp)
            {
                FRotator InitFogRotation = ChildActorComp->GetRelativeRotation();
                BlackboardComponent->SetValueAsRotator("InitFogRotation", InitFogRotation);
            }

            BlackboardComponent->SetValueAsVector("PlayerPointWhenCanCatch",FVector::ZeroVector);
            BlackboardComponent->SetValueAsVector("PlayerPointWhenLost", FVector::ZeroVector);
            BlackboardComponent->SetValueAsVector("InitLocation", ControlledPawn->GetActorLocation());
            RunBehaviorTree(BehaviorTree);
        }
        // 运行行为树
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BehaviorTree is not set for %s"), *GetName());
    }
}

void AEnemyAIController::addActorToIgnoreList(AActor* ActorToAdd)
{
    // 检查传入的Actor是否为空
    if (ActorToAdd != nullptr)
    {
        // 将Actor添加到忽略列表中
        ActorsNeedToIgnore.AddUnique(ActorToAdd);  // 使用AddUnique确保列表中不会有重复的Actor
    }
    else
    {
        // 打印错误或处理空指针的情况
        UE_LOG(LogTemp, Warning, TEXT("Attempted to add a null actor to the ignore list."));
    }
}


void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Actor && Actor->IsA(AOKIROCharacterBase::StaticClass()))
    {
        UE_LOG(LogTemp, Log, TEXT("Actor->IsA(AOKIROCharacterBase::StaticClass()"));
        bool bPlayerInSight = Stimulus.WasSuccessfullySensed();
        //BlackboardComponent->SetValueAsBool(TEXT("PlayerInSight"), bPlayerInSight);

        if (bPlayerInSight)
        {
            APawn* ControlledPawn = GetPawn();
            // 检查ControlledPawn是否非空
            if (!ControlledPawn)
            {
                UE_LOG(LogTemp, Log, TEXT("ControlledPawn is nullptr"));
                return;
            }
            else { UE_LOG(LogTemp, Log, TEXT("ControlledPawn is OK")); }

            // 尝试获取USkeletalMeshComponent
            USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(ControlledPawn->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
            // 检查MeshComponent是否非空
            if (!MeshComponent)
            {
                UE_LOG(LogTemp, Log, TEXT("MeshComponent is nullptr"));
                return;
            }
            else { UE_LOG(LogTemp, Log, TEXT("MeshComponent is OK")); }
            
            USceneComponent* ChildActorComp = MeshComponent->GetChildComponent(0);
            if (!ChildActorComp)
            {
                UE_LOG(LogTemp, Log, TEXT("ChildActorComp is nullptr"));
                return;
            }
            else { UE_LOG(LogTemp, Log, TEXT("ChildActorComp is OK")); }
            //UChildActorComponent* ChildActorComp = Cast<UChildActorComponent>(MeshComponent->GetChildComponent(0));
            float ChildActorZ = ChildActorComp->GetComponentLocation().Z;
            float PlayerActorZ = Actor->GetActorLocation().Z;
            
            if (PlayerActorZ <= ChildActorZ)
            {
                UE_LOG(LogTemp, Log, TEXT("%s was seen at location: %s"), *Actor->GetName(), *Actor->GetActorLocation().ToString());
                BlackboardComponent->SetValueAsBool(TEXT("PlayerInSight"), true);
            }
        }
        else
        {
            BlackboardComponent->SetValueAsBool(TEXT("PlayerInSight"), false);
            BlackboardComponent->SetValueAsVector("PlayerPointWhenDisappear", Actor->GetActorLocation());
            //UE_LOG(LogTemp, Log, TEXT("%s was lost from sight"), *Actor->GetName());
        }
    }
    else 
    {
        UE_LOG(LogTemp, Log, TEXT("Actor->IsNotA(AOKIROCharacterBase::StaticClass()"));
    }
}

void AEnemyAIController::LoadAllPathPoints(APawn* ControlledPawn)
{
    if (ControlledPawn->GetName().Contains("BP_HandEnemy"))
    {
        TArray<UAStarNode*> Data2Points = UAStarNodeUtils::LoadGridFromBinaryFile("Data2.bin", "/GridData/BP_HandEnemy1/");
        TArray<UAStarNode*> Data3Points = UAStarNodeUtils::LoadGridFromBinaryFile("Data3.bin", "/GridData/BP_HandEnemy1/");

        TMap<int32, int32> Connections = UAStarNodeUtils::LoadBoundaryConnectionsFromFile("Data2_3.bin", "/GridData/BP_HandEnemy1/");

        // 连接边界节点
        for (const TPair<int32, int32>& Connection : Connections)
        {
            int32 Data2Index = Connection.Key;
            int32 Data3Index = Connection.Value;

            if (Data2Points.IsValidIndex(Data2Index) && Data3Points.IsValidIndex(Data3Index))
            {
                Data2Points[Data2Index]->Neighbors.Add(Data3Points[Data3Index]);
                Data3Points[Data3Index]->Neighbors.Add(Data2Points[Data2Index]);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid boundary connection indices: %d, %d"), Data2Index, Data3Index);
            }
        }


        // 合并数据到 ChasePoints
        ChasePoints = Data2Points;
        ChasePoints.Append(Data3Points);
        PatrolPoints = UAStarNodeUtils::LoadGridFromBinaryFile("PatrolPoints.bin", "/GridData/BP_HandEnemy1/");
        //UAStarNodeUtils::VisualizeLoadedGrid(GetWorld(), PatrolPoints);
    }
}

void AEnemyAIController::StartEnemyStateDebug()
{
    if (!bShowEnemyStateDebug)
    {
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(
        EnemyStateDebugTimerHandle,
        this,
        &AEnemyAIController::DrawEnemyStateDebug,
        DebugRefreshRate,
        true
    );
}

void AEnemyAIController::StopEnemyStateDebug()
{
    GetWorld()->GetTimerManager().ClearTimer(
        EnemyStateDebugTimerHandle
    );
}

void AEnemyAIController::DrawEnemyStateDebug()
{
    if (!bShowEnemyStateDebug)
    {
        return;
    }

    APawn* ControlledPawn = GetPawn();
    UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

    if (!ControlledPawn || !BlackboardComp)
    {
        return;
    }

    EEnemyState_Cpp EnemyState =
        static_cast<EEnemyState_Cpp>(
            BlackboardComp->GetValueAsEnum("EnemyState")
            );

    FString StateText = TEXT("UNKNOWN");

    switch (EnemyState)
    {
    case EEnemyState_Cpp::Idle:
        StateText = TEXT("IDLE");
        break;

    case EEnemyState_Cpp::Patrol:
        StateText = TEXT("PATROL");
        break;

    case EEnemyState_Cpp::WaitToChase:
        StateText = TEXT("WAIT TO CHASE");
        break;

    case EEnemyState_Cpp::Chase:
        StateText = TEXT("CHASE");
        break;

    case EEnemyState_Cpp::Attack:
        StateText = TEXT("ATTACK");
        break;

    case EEnemyState_Cpp::SearchPlayer:
        StateText = TEXT("SEARCH");
        break;
    }

    UKismetSystemLibrary::DrawDebugString(
        GetWorld(),
        ControlledPawn->GetActorLocation() + FVector(0, 0, 180),
        StateText,
        nullptr,
        FLinearColor::Yellow,
        DebugRefreshRate + 0.02f
    );
}