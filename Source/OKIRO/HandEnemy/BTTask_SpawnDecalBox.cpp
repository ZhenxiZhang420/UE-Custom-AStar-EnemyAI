// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SpawnDecalBox.h"
#include <Kismet/GameplayStatics.h>

UBTTask_SpawnDecalBox::UBTTask_SpawnDecalBox()
{
    NodeName = "Spawn Decal Box";
    static ConstructorHelpers::FClassFinder<AActor> ActorBPClass(TEXT("/Game/OKIRO/Blueprints/character/Enemy/AI/BP_AttackDecalBox"));
    if (ActorBPClass.Class != NULL)
    {
        BlueprintClass = ActorBPClass.Class;
    }
}

EBTNodeResult::Type UBTTask_SpawnDecalBox::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    SpawnDecalBox(PlayerActor->GetActorLocation());
	return EBTNodeResult::Succeeded;
}

void UBTTask_SpawnDecalBox::SpawnDecalBox(const FVector& PlayerLocation)
{
    UWorld* World = GetWorld();
    if (World != nullptr)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner;
        SpawnParams.Instigator;

        // 生成Actor
        AActor* SpawnedActor = World->SpawnActor<AActor>(BlueprintClass, PlayerLocation, FRotator::ZeroRotator, SpawnParams);
    }
}
