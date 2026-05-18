// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AStarNode.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include <Engine/StaticMeshActor.h>
#include "EnemyAIController.generated.h"


/**
 *
 */
UCLASS()
class OKIRO_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()


public:
	AEnemyAIController();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		TArray<UAStarNode*> ChasePoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		TArray<UAStarNode*> PatrolPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy1")
		TArray<TSoftObjectPtr<AActor>> StaticMeshActors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy1")
		TMap<float, float> HeightPairs_Enemy1;

	// 指向行为树的指针
	UPROPERTY(EditAnywhere, Category = "AI")
		UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI")
		UBlackboardComponent* BlackboardComponent;

		TArray<AActor*> ActorsNeedToIgnore;

	UFUNCTION(BlueprintCallable, Category = "BluePrint function")
	void addActorToIgnoreList(AActor* ActorToAdd);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Perception")
		float SightRadius = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Perception")
		float LoseSightRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Perception")
		float PeripheralVisionAngleDegrees = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sight Perception")
		float MaxAge = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
		bool bShowEnemyStateDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
		float DebugRefreshRate = 0.1f;

	FTimerHandle EnemyStateDebugTimerHandle;

		void StartEnemyStateDebug();
		void StopEnemyStateDebug();
		void DrawEnemyStateDebug();



protected:
	// 感知组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
		UAIPerceptionComponent* AIPerceptionComponent;

	// 视觉感知配置
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
		UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Perception")
		UAISenseConfig_Hearing* HearingConfig;

	UFUNCTION()
		void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	void LoadAllPathPoints(APawn* ControlledPawn);

};
