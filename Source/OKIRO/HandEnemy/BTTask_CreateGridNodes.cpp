// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CreateGridNodes.h"
#include "AIController.h"
#include "AStarNodeUtils.h"
#include "CustomNode.h"
#include <Serialization/BufferArchive.h>
#include <Kismet/GameplayStatics.h>
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_CreateGridNodes::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UWorld* World = OwnerComp.GetWorld();
    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController->GetPawn();
    AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    FVector Center = AIPawn->GetActorLocation();

    FString FileName = TEXT("Data.bin");
    if (DebugScenario1 && AIPawn->GetName().Contains("BP_HandEnemy1"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy1/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        if (ShowFullMap) { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path); }
        else { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, PlayerActor->GetActorLocation()); }
    }
    if (DebugScenario2 && AIPawn->GetName().Contains("BP_HandEnemy2"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy2/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        if (ShowFullMap) { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path); }
        else { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, PlayerActor->GetActorLocation()); }
    }
    if (DebugScenario3 && AIPawn->GetName().Contains("BP_HandEnemy4"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy4/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        if (ShowFullMap) { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path); }
        else { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, PlayerActor->GetActorLocation()); }
    }
    if (DebugScenario4 && AIPawn->GetName().Contains("BP_HandEnemy7"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy7/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        if (ShowFullMap) { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path); }
        else { UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, PlayerActor->GetActorLocation()); }
    }

    FileName = TEXT("PatrolPoints.bin");
    if (DebugEnemy1 && AIPawn->GetName().Contains("BP_HandEnemy1"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy1/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }
    if (DebugEnemy2 && AIPawn->GetName().Contains("BP_HandEnemy2"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy2/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }
    if (DebugEnemy3 && AIPawn->GetName().Contains("BP_HandEnemy3"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy3/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        //UAStarNodeUtils::VisualizeLoadedGrid(World, "Data.bin", Path);
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }
    if (DebugEnemy4 && AIPawn->GetName().Contains("BP_HandEnemy4"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy4/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }
    if (DebugEnemy5 && AIPawn->GetName().Contains("BP_HandEnemy5"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy5/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }
    if (DebugEnemy6 && AIPawn->GetName().Contains("BP_HandEnemy6"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy6/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }
    if (DebugEnemy7 && AIPawn->GetName().Contains("BP_HandEnemy7"))
    {
        FString Path = TEXT("/GridData/BP_HandEnemy7/");
        UAStarNodeUtils::FileNameDebug = FileName;
        UAStarNodeUtils::PathDebug = Path;
        UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path, FColor::Blue);
    }

    //TArray<UAStarNode*> GridNodes = UAStarNodeUtils::LoadGridFromBinaryFile("Data.bin", "/GridData/BP_HandEnemy4/");
    //UAStarNodeUtils::SaveGridToBinaryFile(GridNodes, "Data.bin", "/GridData/BP_HandEnemy4/");

    //if (AIPawn->GetName().Contains("BP_HandEnemy7"))
    //{
    //    //Center = FVector(6103.0, 11876.0, -2211.0);
    //    //TArray<UAStarNode*> GridNodes = UAStarNodeUtils::GenerateNodeGrid(GetWorld(), Center, 1, 1, 10, 0, 200.f,-10.0f);
    //    //FileName = TEXT("Data3.bin");
    //    //FString Path = TEXT("/GridData/BP_HandEnemy7/");
    //    TArray<UAStarNode*> GridNode1 = UAStarNodeUtils::LoadGridFromBinaryFile("Data1.bin", "/GridData/BP_HandEnemy7/");
    //    TArray<UAStarNode*> GridNode2 = UAStarNodeUtils::LoadGridFromBinaryFile("Data2.bin", "/GridData/BP_HandEnemy7/");
    //    TArray<UAStarNode*> GridNode3 = UAStarNodeUtils::LoadGridFromBinaryFile("Data3.bin", "/GridData/BP_HandEnemy7/");
    //    GridNode1.Append(GridNode2);
    //    GridNode1.Append(GridNode3);
    //    //UAStarNodeUtils::SaveGridToBinaryFile(GridNode1, "Data.bin", "/GridData/BP_HandEnemy2/");
    //    //for (UAStarNode* Node : GridNodes)
    //    //{
    //    //    if (Node && FMath::IsNearlyEqual(Node->Position.Z, -1720.0f, KINDA_SMALL_NUMBER))
    //    //    {
    //    //        Node->Position.Z = -1730.0f; // 修改 Z 值
    //    //    }
    //    //}
    //    UAStarNodeUtils::SaveGridToBinaryFile(GridNode1, "Data.bin", "/GridData/BP_HandEnemy7/");
    //    //UAStarNodeUtils::VisualizeLoadedGrid(World, FileName, Path);
    //}

    return EBTNodeResult::Succeeded;
}

TArray<UCustomNode*> UBTTask_CreateGridNodes::LoadGridFromBinaryFile(const FString& FileName)
{
    TArray<UCustomNode*> GridNodes;

    // 构建文件路径
    FString LoadPath = FPaths::ProjectSavedDir() + TEXT("/GridData/BP_HandEnemy1/") + FileName;

    // 读取文件到数组
    TArray<uint8> BinaryData;
    if (FFileHelper::LoadFileToArray(BinaryData, *LoadPath))
    {
        // 使用内存阅读器从数组中解析数据
        FMemoryReader FromBinary = FMemoryReader(BinaryData, true); // true for persistent (won't destroy data)
        FromBinary.Seek(0);

        // 反序列化数据
        while (FromBinary.Tell() < BinaryData.Num())
        {
            UCustomNode* Node = NewObject<UCustomNode>(); // 创建新节点
            FVector NodePosition;
            FromBinary << NodePosition; // 读取节点位置
            Node->Position = NodePosition;

            int32 NeighborsCount;
            FromBinary << NeighborsCount; // 读取邻居数量
            for (int32 j = 0; j < NeighborsCount; ++j)
            {
                FVector NeighborPosition;
                FromBinary << NeighborPosition;

                // 创建邻居节点，这里只记录位置，后面处理引用
                UCustomNode* NeighborNode = NewObject<UCustomNode>();
                NeighborNode->Position = NeighborPosition;
                Node->Neighbors.Add(NeighborNode); // 添加到临时列表
            }

            GridNodes.Add(Node); // 添加节点到主列表
        }

        FromBinary.Close();

        // 处理邻居节点的实际引用（因为当前只是独立创建的临时节点）
        for (UCustomNode* Node : GridNodes)
        {
            for (UCustomNode*& TempNeighbor : Node->Neighbors)
            {
                // 在GridNodes中查找真正的节点引用
                UCustomNode** ActualNeighborPtr = GridNodes.FindByPredicate([&](UCustomNode* SearchedNode) {
                    return SearchedNode->Position == TempNeighbor->Position;
                    });

                if (ActualNeighborPtr) // 确保找到了匹配项
                {
                    UCustomNode* ActualNeighbor = *ActualNeighborPtr; // 解引用得到 UCustomNode*
                    TempNeighbor = ActualNeighbor; // 更新为真实引用
                }
                else
                {
                    // 处理未找到匹配项的情况
                    TempNeighbor = nullptr;
                }
            }
        }
    }

    return GridNodes;
}

TArray<UAStarNode*> UBTTask_CreateGridNodes::ConvertAndSaveGridToAStarNodes(const TArray<UCustomNode*>& CustomNodes)
{
    TArray<UAStarNode*> AStarNodes;

    for (UCustomNode* CustomNode : CustomNodes)
    {
        UAStarNode* NewNode = NewObject<UAStarNode>();  // 创建新的AStarNode
        NewNode->Position = CustomNode->Position;  // 位置复制
        NewNode->G = 0;  // 初始化G, H, F值
        NewNode->H = 0;
        NewNode->F = 0;

        // 复制邻居信息
        for (UCustomNode* Neighbor : CustomNode->Neighbors)
        {
            UAStarNode* NeighborNode = NewObject<UAStarNode>();
            NeighborNode->Position = Neighbor->Position;
            NewNode->Neighbors.Add(NeighborNode);
        }

        AStarNodes.Add(NewNode);
    }

    return AStarNodes;
}