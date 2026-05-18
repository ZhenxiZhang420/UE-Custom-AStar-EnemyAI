// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WaitToChase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MoveFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "BTTask_Patrol.h"

UBTTask_WaitToChase::UBTTask_WaitToChase()
{
	NodeName = "Wait beforen Chase";
	bNotifyTick = true;
}


uint16 UBTTask_WaitToChase::GetInstanceMemorySize() const
{
	return sizeof(FWaitTaskMemory);
}

EBTNodeResult::Type UBTTask_WaitToChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    UE_LOG(LogTemp, Log, TEXT("Start check ControlledPawn"));
    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn) {
        UE_LOG(LogTemp, Warning, TEXT("ControlledPawn is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("ControlledPawn OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start check SkeletalMeshComp"));
    USkeletalMeshComponent* SkeletalMeshComp = ControlledPawn->FindComponentByClass<USkeletalMeshComponent>();
    if (!SkeletalMeshComp) {
        UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshComponent not found or is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("SkeletalMeshComp OK"));
    }

    UE_LOG(LogTemp, Log, TEXT("Start check AnimInstance"));
    UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();
    if (!AnimInstance) {
        UE_LOG(LogTemp, Warning, TEXT("AnimInstance is null"));
        return EBTNodeResult::Failed;
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("AnimInstance OK"));
    }

    FName IdlePropName = FName(TEXT("IsIlde?"));
    FBoolProperty* IdleBoolProp = FindFProperty<FBoolProperty>(AnimInstance->GetClass(), IdlePropName);
    if (!IdleBoolProp) {
        UE_LOG(LogTemp, Warning, TEXT("IdleBoolProp not found"));
    }
    else {
        IdleBoolProp->SetPropertyValue_InContainer(AnimInstance, false);
        UE_LOG(LogTemp, Log, TEXT("Idle state set to false"));
    }

    FName AlertPropName = FName(TEXT("IsAlert?"));
    FBoolProperty* AlertBoolProp = FindFProperty<FBoolProperty>(AnimInstance->GetClass(), AlertPropName);
    if (!AlertBoolProp) {
        UE_LOG(LogTemp, Warning, TEXT("AlertBoolProp not found"));
    }
    else {
        AlertBoolProp->SetPropertyValue_InContainer(AnimInstance, true);
        UE_LOG(LogTemp, Log, TEXT("Alert state set to true"));
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_WaitToChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

    float WaitToChaseDuration = BlackboardComp->GetValueAsFloat("WaitToChaseDuration");
    WaitToChaseDuration += DeltaSeconds * 2.0f; 
    BlackboardComp->SetValueAsFloat("WaitToChaseDuration", WaitToChaseDuration);

    UMoveFunctionLibrary::LookAtPlayer(GetWorld(), ControlledPawn, PlayerActor, DeltaSeconds, LookAtPlayerRotationSpeed);

    if (WaitToChaseDuration >= 1.2f)
    {
        BlackboardComp->SetValueAsEnum("EnemyState", static_cast<uint8>(EEnemyState_Cpp::Chase));

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

