// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include <Kismet/GameplayStatics.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "MoveFunctionLibrary.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = "Move And Try Catch";
	bNotifyTick = true;
}

uint16 UBTTask_Attack::GetInstanceMemorySize() const
{
    return sizeof(FAttackTaskMemory);
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
    USkeletalMeshComponent* SkeletalMeshComp = ControlledPawn->FindComponentByClass<USkeletalMeshComponent>();
    UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    
    // 获取IsAlert的值
    FName AlertPropName = FName(TEXT("IsAlert?"));
    FBoolProperty* AlertBoolProp = FindFProperty<FBoolProperty>(AnimInstance->GetClass(), AlertPropName);
    if (!AlertBoolProp) {
        UE_LOG(LogTemp, Warning, TEXT("Property %s not found in AnimInstance"), *AlertPropName.ToString());
        return EBTNodeResult::Failed;
    }

    bool IsAlert = AlertBoolProp->GetPropertyValue_InContainer(AnimInstance);

    if (IsAlert)
    {
        // 如果IsAlert是True，则随机选择一个攻击类型
        int32 RandomAttackIndex = FMath::RandRange(0, 2); // 随机生成0到2的索引
        EAttackType SelectedAttackType = static_cast<EAttackType>(RandomAttackIndex);

        BlackboardComp->SetValueAsEnum("RandomAttackType", static_cast<uint8>(SelectedAttackType));

        // 根据选择的攻击类型设置相应的动画属性
        FName PropertyName;
        switch (SelectedAttackType)
        {
        case EAttackType::Attack:
            PropertyName = FName(TEXT("IsAttack?"));
            break;
        case EAttackType::Pinch:
            PropertyName = FName(TEXT("Is_pinch?"));
            break;
        case EAttackType::Pat:
            PropertyName = FName(TEXT("Is_pat?"));
            break;
        default:
            PropertyName = FName(TEXT("IsAttack?"));
            break;
        }

        FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(AnimInstance->GetClass(), PropertyName);
        if (BoolProp) {
            BoolProp->SetPropertyValue_InContainer(AnimInstance, true);
        }
        else {
            // 如果没有找到相应的属性
            UE_LOG(LogTemp, Warning, TEXT("Property %s not found in AnimInstance"), *PropertyName.ToString());
        }

        AlertBoolProp->SetPropertyValue_InContainer(AnimInstance, false);
        return EBTNodeResult::InProgress;
    }
    else
    {
        AlertBoolProp->SetPropertyValue_InContainer(AnimInstance, false);
        return EBTNodeResult::Succeeded;
    }

}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    FAttackTaskMemory* TaskMemory = (FAttackTaskMemory*)NodeMemory;

    TaskMemory->TimeSinceAlert += DeltaSeconds;
    if (TaskMemory->TimeSinceAlert >= CatchCheckTime)
    {
        // 检测玩家是否在捕捉区域
        UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
        bool PlayerInCatchArea = BlackboardComp->GetValueAsBool("PlayerInCatchArea");

        if (PlayerInCatchArea)
        {
            APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
            if (PlayerPawn)
            {
                // 获得玩家Pawn的类，并检查是否存在名为"HandleEnemyAttack"的函数
                UFunction* HandleEnemyAttackFunc = PlayerPawn->FindFunction(FName(TEXT("HandleEnemyAttack")));
                if (HandleEnemyAttackFunc)
                {
                    uint8 AttackTypeValue = BlackboardComp->GetValueAsEnum("RandomAttackType");
                    EAttackType AttackType = static_cast<EAttackType>(AttackTypeValue);
                    // 创建参数结构
                    FHandleEnemyAttackParams Params;
                    Params.AttackType = AttackType;

                    // 调用蓝图中的自定义事件
                    PlayerPawn->ProcessEvent(HandleEnemyAttackFunc, &Params);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("HandleEnemyAttack function not found in BP_Player."));
                }
            }
        }
        TaskMemory->TimeSinceAlert = 0;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
