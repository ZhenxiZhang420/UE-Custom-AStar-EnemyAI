// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WaitToChase.generated.h"


struct FWaitTaskMemory
{
	float TimeSinceFindPlayer = 0.0f;
};

/**
 * 
 */
UCLASS()
class OKIRO_API UBTTask_WaitToChase : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_WaitToChase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float LookAtPlayerRotationSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Properties")
		float WaitToChaseTime = 1.2f;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};
