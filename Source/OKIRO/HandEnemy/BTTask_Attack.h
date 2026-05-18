// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

enum class EAttackType : uint8
{
	Attack UMETA(DisplayName = "Attack"),
	Pinch UMETA(DisplayName = "Pinch"),
	Pat UMETA(DisplayName = "Pat")
};
struct FHandleEnemyAttackParams
{
	EAttackType AttackType;
};
struct FAttackTaskMemory
{
	float TimeSinceAlert = 0.0f;
};

/**
 *
 */
UCLASS()
class OKIRO_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public: 
	UBTTask_Attack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float CatchCheckTime = 0.25f;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};
