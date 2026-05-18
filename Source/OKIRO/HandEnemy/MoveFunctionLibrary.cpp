// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>

bool UMoveFunctionLibrary::MoveToLocation(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float MovementSpeed, float RotationSpeed, float Tolerance)
{
    if (!Pawn)
    {
        return false;
    }

    FVector CurrentLocation = Pawn->GetActorLocation();

    FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);

    Pawn->SetActorLocation(NewLocation);

    if (FVector::Dist(NewLocation, TargetLocation) < Tolerance)
    {
        return true; // 到达目标位置
    }

    RotateToFaceTarget(Pawn, TargetLocation, DeltaTime, RotationSpeed);


    return false; // 尚未到达目标位置
}

bool UMoveFunctionLibrary::MoveToLocationWithoutTurn(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float MovementSpeed, float RotationSpeed, float Tolerance)
{
    if (!Pawn)
    {
        return false;
    }
    //USkeletalMeshComponent* EnemyMesh = Pawn->FindComponentByClass<USkeletalMeshComponent>();
    //FVector CurrentLocation = EnemyMesh->GetComponentLocation();
    //FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);
    //
    //EnemyMesh->SetWorldLocation(NewLocation);

    //if (FVector::Dist(NewLocation, TargetLocation) < Tolerance)
    //{
    //    return true; // 已经到达或接近目标位置
    //}

    FVector CurrentLocation = Pawn->GetActorLocation();
    FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, MovementSpeed);


    Pawn->SetActorLocation(NewLocation);

    if (FVector::Dist(NewLocation, TargetLocation) < Tolerance)
    {
        return true; // 到达目标位置
    }

    return false; // 尚未到达目标位置
}

bool UMoveFunctionLibrary::LookAtPlayer(UWorld* World, APawn* ControlledPawn, AActor* PlayerActor, float DeltaTime, float RotationSpeed)
{
    // 计算从AI到玩家的方向向量
    FVector DirectionToPlayer = (PlayerActor->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = DirectionToPlayer.Rotation();
    TargetRotation.Pitch = 0;

    // 使用RInterpTo平滑地旋转AI
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

    // 设置AI的旋转，使其朝向玩家
    ControlledPawn->SetActorRotation(NewRotation);

    UChildActorComponent* ChildActorComp = ControlledPawn->FindComponentByClass<UChildActorComponent>();
    if (ChildActorComp)
    {
        FVector ChildActorCompWorldLocation = ChildActorComp->GetComponentLocation();
        FVector DirectionToPlayer1 = (PlayerActor->GetActorLocation() - ChildActorCompWorldLocation).GetSafeNormal();
        FRotator TargetRotation1 = DirectionToPlayer1.Rotation();

        FRotator CurrentRotation1 = ChildActorComp->GetComponentRotation();
        FRotator NewRotation1 = FMath::RInterpTo(CurrentRotation1, TargetRotation1, DeltaTime, RotationSpeed);

        ChildActorComp->SetWorldRotation(NewRotation1);
    }
    return true;
}

bool UMoveFunctionLibrary::LookAtPlayer(UWorld* World, APawn* ControlledPawn, FVector PlayerLocation, float DeltaTime, float RotationSpeed)
{
    // 计算从AI到玩家位置的方向向量
    FVector DirectionToPlayer = (PlayerLocation - ControlledPawn->GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = DirectionToPlayer.Rotation();
    TargetRotation.Pitch = 0;

    // 使用RInterpTo平滑地旋转AI
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

    // 设置AI的旋转，使其朝向玩家位置
    //ControlledPawn->SetActorRotation(NewRotation);

    UChildActorComponent* ChildActorComp = ControlledPawn->FindComponentByClass<UChildActorComponent>();
    if (ChildActorComp)
    {
        FVector ChildActorCompWorldLocation = ChildActorComp->GetComponentLocation();
        FVector DirectionToPlayer1 = (PlayerLocation - ChildActorCompWorldLocation).GetSafeNormal();
        FRotator TargetRotation1 = DirectionToPlayer1.Rotation();

        FRotator CurrentRotation1 = ChildActorComp->GetComponentRotation();
        FRotator NewRotation1 = FMath::RInterpTo(CurrentRotation1, TargetRotation1, DeltaTime, RotationSpeed);

        ChildActorComp->SetWorldRotation(NewRotation1);
    }
    return true;
}

void UMoveFunctionLibrary::RotateToFaceTarget(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed)
{
    if (!Pawn)
    {
        return;
    }

    // 计算从AI到目标的方向向量，并进行标准化
    FVector Direction = (TargetLocation - Pawn->GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = Direction.Rotation();

    // 获取当前Pawn的旋转，并保留Z轴（俯仰）的值不变
    FRotator CurrentRotation = Pawn->GetActorRotation();
    TargetRotation.Pitch = CurrentRotation.Pitch;  // 可以选择保持俯仰角不变
    TargetRotation.Roll = CurrentRotation.Roll;    // 保持侧倾角不变，通常不需要，除非特殊情况

    // 通过插值计算新的旋转值
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

    // 设置新的旋转值
    Pawn->SetActorRotation(NewRotation);
}

bool UMoveFunctionLibrary::IsVectorADirectlyAboveVectorB(FVector VectorA, FVector VectorB)
{

    //if (FMath::IsNearlyEqual(VectorA.X, VectorB.X) && FMath::IsNearlyEqual(VectorA.Y, VectorB.Y)) 
    //{
    //    UE_LOG(LogTemp, Log, TEXT("VectorA: %s, VectorB: %s"), *VectorA.ToString(), *VectorB.ToString());
    //}
    return FMath::IsNearlyEqual(VectorA.X, VectorB.X) && FMath::IsNearlyEqual(VectorA.Y, VectorB.Y);
}

void UMoveFunctionLibrary::DrawMovePath(UWorld* World, FVector AIPosition, FVector TargetPosition)
{
    UKismetSystemLibrary::DrawDebugArrow(World, AIPosition, TargetPosition, 10.f, FLinearColor::Green, 2.f, 3.f);
}

void UMoveFunctionLibrary::SearchPlayer(UWorld* World, APawn* ControlledPawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed, float SearchFrequency, float SearchDegree)
{
    FVector AI_Location = ControlledPawn->GetActorLocation();
    FVector MoveDirection = (TargetLocation - AI_Location).GetSafeNormal();
    MoveDirection.Z = 0;
    // 获取基本旋转和时间变量
    FRotator BaseRotation = MoveDirection.Rotation();
    float Time = World->GetTimeSeconds();

    // 使用正弦波计算当前角度偏移
    float AngleOffset = FMath::Sin(Time * SearchFrequency * 2.0f * PI) * SearchDegree; // 正负30度摇摆

    // 计算当前需要看向的角度
    FRotator TargetRotation = BaseRotation + FRotator(0, AngleOffset, 0);
    FVector LookAtLocation = AI_Location + TargetRotation.Vector() * 250.0f; // 半径300米
    LookAtLocation.Z -= 100.0f;
    // 调用LookAtPlayer函数，使AI朝向计算出的位置
    UMoveFunctionLibrary::LookAtPlayer(World, ControlledPawn, LookAtLocation, DeltaTime, RotationSpeed);
}

bool UMoveFunctionLibrary::RotateCertainAngle(APawn* ControlledPawn, FRotator TargetRotation, float DeltaTime, float RotationSpeed)
{
    // 计算目标旋转
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();

    // 平滑旋转到目标
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
    ControlledPawn->SetActorRotation(NewRotation);

    // 检查是否已近似达到目标旋转
    if (FMath::IsNearlyEqual(NewRotation.Yaw, TargetRotation.Yaw, 5.0f))
    {
        return true;
    }
    else { return false; }
}
void UMoveFunctionLibrary::LookAtInitialPosition(UWorld* World, APawn* ControlledPawn, float DeltaTime, float RotationSpeed)
{
    FVector AI_Location = ControlledPawn->GetActorLocation();
    FRotator TargetRotation = ControlledPawn->GetActorRotation();
    FVector LookAtLocation = AI_Location + TargetRotation.Vector() * 250.0f; // 半径300米
    LookAtLocation.Z -= 100.0f;
    UMoveFunctionLibrary::LookAtPlayer(World, ControlledPawn, LookAtLocation, DeltaTime, RotationSpeed);
}
void UMoveFunctionLibrary::test(UWorld* World, APawn* ControlledPawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed, float SearchFrequency, float SearchDegree)
{
    FVector AI_Location = ControlledPawn->GetActorLocation();
    FVector MoveDirection = (TargetLocation - AI_Location).GetSafeNormal();
    MoveDirection.Z = 0;
    // 获取基本旋转和时间变量
    FRotator BaseRotation = MoveDirection.Rotation();
    float Time = World->GetTimeSeconds();

    // 使用正弦波计算当前角度偏移
    float AngleOffset = FMath::Sin(Time * SearchFrequency * 2.0f * PI) * SearchDegree / 4; // 正负30度摇摆

    // 计算当前需要看向的角度
    FRotator TargetRotation = BaseRotation + FRotator(0, AngleOffset, 0);
    ControlledPawn->SetActorRotation(TargetRotation);

    SearchPlayer(World, ControlledPawn, TargetLocation, DeltaTime, RotationSpeed, SearchFrequency, SearchDegree / 2);
}

//弃用方案
bool UMoveFunctionLibrary::TracePlayerWhenChase(UWorld* World, APawn* ControlledPawn, AActor* PlayerActor, float DeltaTime, float RotationSpeed)
{
    // 计算从AI到玩家的方向向量
    FVector DirectionToPlayer = (PlayerActor->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();
    FRotator TargetRotation = DirectionToPlayer.Rotation();
    TargetRotation.Pitch = 0;

    // 使用RInterpTo平滑地旋转AI
    FRotator CurrentRotation = ControlledPawn->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

    // 设置AI的旋转，使其朝向玩家
    ControlledPawn->SetActorRotation(NewRotation);

    UChildActorComponent* ChildActorComp = ControlledPawn->FindComponentByClass<UChildActorComponent>();
    if (ChildActorComp)
    {
        AActor* ChildActor = ChildActorComp->GetChildActor();
        if (ChildActor)
        {
            // 假设BP_EnemyfogVL是Child Actor的根组件
            USceneComponent* FogComponent = ChildActor->GetRootComponent();
            if (FogComponent)
            {
                // 获取灯光组件的世界坐标
                FVector FogComponentWorldLocation = FogComponent->GetComponentLocation();
                // 计算从灯光组件到玩家的方向向量
                FVector DirectionToPlayer1 = (PlayerActor->GetActorLocation() - FogComponentWorldLocation).GetSafeNormal();
                FRotator TargetRotation1 = DirectionToPlayer1.Rotation();

                // 使用RInterpTo平滑地旋转灯光组件
                FRotator CurrentRotation1 = FogComponent->GetComponentRotation();
                FRotator NewRotation1 = FMath::RInterpTo(CurrentRotation1, TargetRotation1, DeltaTime, RotationSpeed);

                // 设置组件的旋转，使其朝向玩家
                FogComponent->SetWorldRotation(NewRotation1);

                // 打印FogComponent的旋转值
                //UE_LOG(LogTemp, Log, TEXT("FogComponent Rotation: %s"), *NewRotation1.ToString());

                // 光线追踪
                FVector EndLocation = FogComponentWorldLocation + (FogComponent->GetForwardVector() * 1000.0f); // 假设光的最大范围为1000单位
                TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
                ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 只检测Pawn类型

                FHitResult HitResult;
                bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
                    World,
                    FogComponentWorldLocation,
                    EndLocation,
                    ObjectTypes,
                    false, // 不复杂
                    TArray<AActor*>(), // 忽略的Actor列表，这里为空
                    EDrawDebugTrace::None, // 可选，调试时显示光线
                    HitResult,
                    true // 忽略自身
                );

                if (bHit && HitResult.GetActor() == PlayerActor)
                {
                    // 玩家在光照范围内
                    //UE_LOG(LogTemp, Log, TEXT("Player detected in light beam"));

                    // 在碰撞点绘制一个半径为10的圆
                    FVector CollisionPoint = HitResult.Location;
                    FVector UpVector = FVector::UpVector;
                    FVector RightVector = FVector::CrossProduct(DirectionToPlayer1, UpVector).GetSafeNormal();
                    FVector ForwardVector = FVector::CrossProduct(RightVector, UpVector).GetSafeNormal();

                    float Radius = 30.0f;
                    int32 NumPoints = 12;
                    float AngleStep = 360.0f / NumPoints;

                    TArray<FVector> CirclePoints;

                    for (int32 i = 0; i < NumPoints; ++i)
                    {
                        float AngleRad = FMath::DegreesToRadians(i * AngleStep);
                        FVector PointOnCircle = CollisionPoint + (RightVector * FMath::Cos(AngleRad) + ForwardVector * FMath::Sin(AngleRad)) * Radius;
                        CirclePoints.Add(PointOnCircle);

                        // 从灯光位置到圆上的点发射光线
                        FVector ExtendedPoint = PointOnCircle + (PointOnCircle - FogComponentWorldLocation).GetSafeNormal() * 500.0f; // 延长射线长度
                        FHitResult CircleHitResult;
                        bool bCircleHit = UKismetSystemLibrary::LineTraceSingleForObjects(
                            World,
                            FogComponentWorldLocation,
                            ExtendedPoint,
                            ObjectTypes,
                            false, // 不复杂
                            TArray<AActor*>(), // 忽略的Actor列表，这里为空
                            EDrawDebugTrace::None, // 可选，调试时显示光线
                            CircleHitResult,
                            true // 忽略自身
                        );

                        if (bCircleHit && CircleHitResult.GetActor() == PlayerActor)
                        {
                            // 处理每条射线的命中结果
                            //UE_LOG(LogTemp, Log, TEXT("Hit location: %s"), *CircleHitResult.Location.ToString());

                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }

        }
    }
    return true;
}
bool UMoveFunctionLibrary::MovePawnToLocation(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float MovementSpeed, float RotationSpeed, float Tolerance)
{
    if (!Pawn)
    {
        return false;
    }

    // 获取Pawn的世界位置和Mesh的相对位置
    FVector CurrentPawnLocation = Pawn->GetActorLocation();
    USkeletalMeshComponent* EnemyMesh = Pawn->FindComponentByClass<USkeletalMeshComponent>();
    if (!EnemyMesh) // 确保Mesh组件存在
    {
        return false;
    }

    FVector CurrentMeshLocation = CurrentPawnLocation + EnemyMesh->GetRelativeLocation();

    // 计算Pawn应该移动到的位置，使得Mesh位置与目标位置对齐
    FVector CorrectedTargetLocation = TargetLocation - EnemyMesh->GetRelativeLocation();

    // 计算新位置，使Pawn移动向修正后的目标位置
    FVector NewLocation = FMath::VInterpConstantTo(CurrentPawnLocation, CorrectedTargetLocation, DeltaTime, MovementSpeed);

    // 设置Pawn的新位置
    Pawn->SetActorLocation(NewLocation);

    // 如果Mesh位置与目标位置的距离小于容忍度，则认为到达目标位置
    if (FVector::Dist(CurrentMeshLocation, TargetLocation) < Tolerance)
    {
        return true; // 到达目标位置
    }

    // 旋转Pawn以面向目标位置
    RotateToFaceTarget(Pawn, TargetLocation, DeltaTime, RotationSpeed);

    return false; // 尚未到达目标位置
}
void UMoveFunctionLibrary::RotatePawnToFaceTarget(APawn* Pawn, const FVector& TargetLocation, float DeltaTime, float RotationSpeed)
{
    // 获取Pawn的SkeletalMeshComponent
    USkeletalMeshComponent* MeshComponent = Pawn->FindComponentByClass<USkeletalMeshComponent>();

    // 计算Mesh实际世界位置
    FVector MeshWorldLocation = Pawn->GetActorLocation() + MeshComponent->GetRelativeLocation();

    // 计算从Mesh位置到目标的方向向量，并进行标准化
    FVector Direction = (TargetLocation - MeshWorldLocation).GetSafeNormal();
    FRotator TargetRotation = Direction.Rotation();

    // 获取当前Pawn的旋转，并保留Z轴（俯仰）的值不变
    FRotator CurrentRotation = Pawn->GetActorRotation();
    TargetRotation.Pitch = CurrentRotation.Pitch;  // 可以选择保持俯仰角不变
    TargetRotation.Roll = CurrentRotation.Roll;    // 保持侧倾角不变

    // 通过插值计算新的旋转值
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

    // 设置新的旋转值
    Pawn->SetActorRotation(NewRotation);
}



