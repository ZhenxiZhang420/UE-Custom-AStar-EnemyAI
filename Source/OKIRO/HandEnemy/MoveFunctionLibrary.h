// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MoveFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class OKIRO_API UMoveFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool MoveToLocation(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float MovementSpeed, float RotationSpeed, float Tolerance = 10.0f);
	static bool MoveToLocationWithoutTurn(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float MovementSpeed, float RotationSpeed, float Tolerance = 10.0f);
	static bool LookAtPlayer(UWorld* World, APawn* ControlledPawn, AActor* PlayerActor, float DeltaTime, float RotationSpeed);
	static bool LookAtPlayer(UWorld* World, APawn* ControlledPawn, FVector PlayerLocation, float DeltaTime, float RotationSpeed);
	static void RotateToFaceTarget(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed);
	static bool IsVectorADirectlyAboveVectorB(FVector VectorA, FVector VectorB);
	static void DrawMovePath(UWorld* World, FVector AIPosition, FVector TargetPosition);
	static void test(UWorld* World, APawn* ControlledPawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed, float SearchFrequency, float SearchDegree);
	static bool TracePlayerWhenChase(UWorld* World, APawn* ControlledPawn, AActor* PlayerActor, float DeltaTime, float RotationSpeed);
	static void SearchPlayer(UWorld* World, APawn* ControlledPawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed, float SearchFrequency, float SearchDegree);
	static bool RotateCertainAngle(APawn* ControlledPawn, FRotator TargetRotation, float DeltaTime, float RotationSpeed);
	static void LookAtInitialPosition(UWorld* World, APawn* ControlledPawn, float DeltaTime, float RotationSpeed);
	static bool MovePawnToLocation(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float MovementSpeed, float RotationSpeed, float Tolerance = 10.0f);
	static void RotatePawnToFaceTarget(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed);
};
