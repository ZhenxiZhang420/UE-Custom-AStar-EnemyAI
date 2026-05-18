// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveToAttackPosition.h"
#include "AIController.h"
#include <Kismet/GameplayStatics.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "MoveFunctionLibrary.h"
#include "BTTask_Patrol.h"

UMoveToAttackPosition::UMoveToAttackPosition() 
{
	NodeName = "Move To Attack Position";
	bNotifyTick = true;
}


EBTNodeResult::Type UMoveToAttackPosition::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;
	FVector PlayerPointWhenCanCatch = BlackboardComp->GetValueAsVector("PlayerPointWhenCanCatch");
	if (PlayerPointWhenCanCatch == FVector::ZeroVector)
	{
		BlackboardComp->SetValueAsEnum("EnemyState", (uint8)EEnemyState_Cpp::Chase);
		//FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	else 
	{
		return EBTNodeResult::InProgress;
	}
}

void UMoveToAttackPosition::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	UBlackboardComponent* BlackboardComp = AIController ? AIController->GetBlackboardComponent() : nullptr;
	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	const FName EnemyStateKeyName = "EnemyState";
	EEnemyState_Cpp EnemyState = static_cast<EEnemyState_Cpp>(BlackboardComp->GetValueAsEnum(EnemyStateKeyName));

	FVector Direction = ControlledPawn->GetActorLocation() - PlayerActor->GetActorLocation();
	Direction.Normalize();

	FVector PlayerPointWhenCanCatch = BlackboardComp->GetValueAsVector("PlayerPointWhenCanCatch");

	bool bReached = UMoveFunctionLibrary::MoveToLocationWithoutTurn(ControlledPawn, PlayerPointWhenCanCatch, DeltaSeconds, 600.f, 5.f);
	if (bReached)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}