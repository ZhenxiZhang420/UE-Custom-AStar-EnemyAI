// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
    // 获取角色当前的旋转值
    Rotation = GetActorRotation();

    // 获取基于当前旋转的前向向量
    FVector ForwardVector = GetActorForwardVector();

    // 计算新的位置：在角色的前方80.0f单位，并在Z轴方向上固定减少60.0f单位
    Location = GetActorLocation() + ForwardVector * 80.0f;

    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerActor)
    {
        // 如果有效，使用玩家的Z坐标
        Location.Z = PlayerActor->GetActorLocation().Z;
    }

    USceneComponent* ChildActorComp = GetMesh()->GetChildComponent(0);
    if (ChildActorComp)
    {
        // 获取ChildActor的世界位置的Z值
        float ChildActorZ = ChildActorComp->GetComponentLocation().Z;

        // 设置Location的Z值不超过ChildActor的Z值
        Location.Z = FMath::Min(Location.Z, ChildActorZ);
    }

    // 确保Location的Z值不小于GetActorLocation().Z - 60.0f
    float MinZ = GetActorLocation().Z - 60.0f;
    Location.Z = FMath::Max(Location.Z, MinZ);

}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

