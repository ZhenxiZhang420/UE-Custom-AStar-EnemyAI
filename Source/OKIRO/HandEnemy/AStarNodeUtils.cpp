// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNodeUtils.h"
#include "AStarNode.h"
#include "Math/UnrealMathUtility.h"
#include <Serialization/BufferArchive.h>
#include <Kismet/KismetSystemLibrary.h>

FString UAStarNodeUtils::FileNameDebug = TEXT("Data.bin");
FString UAStarNodeUtils::PathDebug = TEXT("/GridData/BP_HandEnemy1/");
TMap<FString, TArray<UAStarNode*>> UAStarNodeUtils::GridCache;

FArchive& operator<<(FArchive& Ar, TPair<int32, int32>& Pair)
{
    Ar << Pair.Key;
    Ar << Pair.Value;
    return Ar;
}

void UAStarNodeUtils::SaveGridToBinaryFile(
    const TArray<UAStarNode*>& GridNodes,
    const FString& FileName,
    const FString& Path)
{
    FString SavePath =
        FPaths::ProjectContentDir()
        + Path
        + FileName;

    IPlatformFile& PlatformFile =
        FPlatformFileManager::Get()
        .GetPlatformFile();

    FString Directory =
        FPaths::GetPath(SavePath);

    if (!PlatformFile.DirectoryExists(*Directory))
    {
        PlatformFile.CreateDirectoryTree(
            *Directory
        );
    }

    TArray<uint8> BinaryData;

    FMemoryWriter ToBinary(
        BinaryData,
        true
    );

    for (UAStarNode* Node : GridNodes)
    {
        if (!Node)
        {
            continue;
        }

        // 为兼容旧数据统一存 float
        FVector3f Position(
            Node->Position
        );

        ToBinary << Position;

        int32 Index =
            Node->Index;

        ToBinary << Index;

        int32 NeighborCount =
            Node->Neighbors.Num();

        ToBinary << NeighborCount;

        for (UAStarNode* Neighbor :
            Node->Neighbors)
        {
            FVector3f NeighborPos(
                Neighbor->Position
            );

            ToBinary
                << NeighborPos;
        }
    }

    ToBinary.Flush();

    bool bSuccess =
        FFileHelper::SaveArrayToFile(
            BinaryData,
            *SavePath
        );

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Saved Grid: %s Success=%d Nodes=%d"),
        *SavePath,
        bSuccess,
        GridNodes.Num()
    );

}

void UAStarNodeUtils::SaveBoundaryConnectionsToFile(const TMap<int32, int32>& BoundaryConnections, const FString& FileName, const FString& Path)
{
    FString SavePath = FPaths::ProjectSavedDir() + Path + FileName;
    FBufferArchive BoundaryData;

    // 保存边界节点的连接信息
    int32 BoundaryCount = BoundaryConnections.Num();
    BoundaryData << BoundaryCount;

    for (TPair<int32, int32> Connection : BoundaryConnections)
    {
        BoundaryData << Connection;  // 使用我们定义的 << 运算符重载
    }

    if (FFileHelper::SaveArrayToFile(BoundaryData, *SavePath))
    {
        UE_LOG(LogTemp, Log, TEXT("Boundary connections saved successfully to %s"), *SavePath);
    }

    BoundaryData.FlushCache();
    BoundaryData.Empty();
}

TArray<UAStarNode*> UAStarNodeUtils::GenerateNodeGrid(UWorld* World, FVector Center, int32 CellsLeft, int32 CellsRight, int32 CellsForward, int32 CellsBackward, float CellSize, float RotationAngle)
{
    // 计算总网格宽度和长度
    int32 TotalWidth = CellsLeft + CellsRight;
    int32 TotalLength = CellsForward + CellsBackward;

    TArray<UAStarNode*> GridNodes;
    GridNodes.SetNum((TotalWidth + 1) * (TotalLength + 1));
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
    TArray<AActor*> ActorsToIgnore;

    // 角度转换为弧度并计算旋转四元数
    float Radians = FMath::DegreesToRadians(RotationAngle);
    FQuat RotationQuat = FQuat(FVector::UpVector, Radians); // 假设旋转是围绕Z轴（向上的轴）

    // 创建节点
    for (int32 x = -CellsLeft; x <= CellsRight; x++)
    {
        for (int32 y = -CellsBackward; y <= CellsForward; y++)
        {
            FVector GridPosition = FVector(x * CellSize, y * CellSize, 0); // 初始未旋转的位置
            FVector RotatedPosition = RotationQuat.RotateVector(GridPosition); // 旋转位置

            UAStarNode* Node = NewObject<UAStarNode>(World, UAStarNode::StaticClass());
            Node->Position = Center + RotatedPosition; // 将旋转后的位置调整到中心点
            GridNodes[(x + CellsLeft) * (TotalLength + 1) + (y + CellsBackward)] = Node;
        }
    }

    // 连接节点
    for (int32 x = -CellsLeft; x <= CellsRight; x++)
    {
        for (int32 y = -CellsBackward; y <= CellsForward; y++)
        {
            UAStarNode* Node = GridNodes[(x + CellsLeft) * (TotalLength + 1) + (y + CellsBackward)];

            // 连接邻居
            for (int32 nx = -1; nx <= 1; nx++)
            {
                for (int32 ny = -1; ny <= 1; ny++)
                {
                    if (nx == 0 && ny == 0) continue; // 排除自己

                    int32 neighborX = x + nx;
                    int32 neighborY = y + ny;

                    // 确保邻居在网格内
                    if (neighborX >= -CellsLeft && neighborX <= CellsRight && neighborY >= -CellsBackward && neighborY <= CellsForward)
                    {
                        UAStarNode* NeighborNode = GridNodes[(neighborX + CellsLeft) * (TotalLength + 1) + (neighborY + CellsBackward)];

                        FHitResult HitResult;
                        if (!UKismetSystemLibrary::LineTraceSingleForObjects(World, Node->Position, NeighborNode->Position, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
                        {
                            Node->AddUnidirectionalNeighbor(NeighborNode);
                        }
                    }
                }
            }
        }
    }

    return GridNodes;
}
TArray<UAStarNode*> UAStarNodeUtils::LoadGridFromBinaryFile(
    const FString& FileName,
    const FString& Path)
{
    TArray<UAStarNode*> GridNodes;

    FString LoadPath;

#if WITH_EDITOR
    LoadPath = FPaths::ProjectContentDir() + Path + FileName;
#else
    LoadPath = FPaths::Combine(
        FPaths::LaunchDir(),
        FString("/OKIRO/Content"),
        Path,
        FileName
    );
#endif

    if (!FPaths::FileExists(LoadPath))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Grid file not found: %s"),
            *LoadPath
        );

        return GridNodes;
    }

    TArray<uint8> BinaryData;

    if (!FFileHelper::LoadFileToArray(
        BinaryData,
        *LoadPath))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Failed to load: %s"),
            *LoadPath
        );

        return GridNodes;
    }

    FMemoryReader FromBinary(BinaryData, true);
    FromBinary.Seek(0);

    TArray<TArray<FVector>> TempNeighborPositions;

    while (FromBinary.Tell() < BinaryData.Num())
    {
        if (BinaryData.Num() - FromBinary.Tell() < 20)
        {
            break;
        }

        UAStarNode* Node =
            NewObject<UAStarNode>(
                GetTransientPackage(),
                UAStarNode::StaticClass());

        FVector3f NodePositionFloat;
        FromBinary << NodePositionFloat;

        Node->Position =
            FVector(NodePositionFloat);

        int32 SavedIndex;
        FromBinary << SavedIndex;

        Node->Index =
            GridNodes.Num();

        int32 NeighborsCount;
        FromBinary << NeighborsCount;

        if (NeighborsCount < 0 || NeighborsCount>64)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT(
                    "Invalid Neighbor Count:%d"),
                NeighborsCount
            );

            break;
        }

        TArray<FVector> NeighborPositions;

        for (
            int32 j = 0;
            j < NeighborsCount;
            ++j)
        {
            FVector3f NeighborFloat;

            FromBinary
                << NeighborFloat;

            NeighborPositions.Add(
                FVector(
                    NeighborFloat
                )
            );
        }

        GridNodes.Add(Node);

        TempNeighborPositions.Add(
            NeighborPositions
        );
    }

    for (int32 i = 0; i < GridNodes.Num(); i++)
    {
        UAStarNode* Node =
            GridNodes[i];

        for (const FVector&
            NeighborPosition :
            TempNeighborPositions[i])
        {
            UAStarNode**
                ActualNeighborPtr =
                GridNodes.FindByPredicate(
                    [&](UAStarNode* SearchNode)
                    {
                        return SearchNode
                            &&
                            SearchNode
                            ->Position.Equals(
                                NeighborPosition,
                                1.0f);
                    });

            if (
                ActualNeighborPtr
                &&
                *ActualNeighborPtr)
            {
                Node->Neighbors
                    .AddUnique(
                        *ActualNeighborPtr);
            }
        }
    }

    UE_LOG(
        LogTemp,
        Log,
        TEXT(
            "Loaded Grid: %s Nodes=%d"),
        *FileName,
        GridNodes.Num()
    );

    return GridNodes;
}
TMap<int32, int32> UAStarNodeUtils::LoadBoundaryConnectionsFromFile(const FString& FileName, const FString& Path)
{
    TMap<int32, int32> BoundaryConnections;
    FString LoadPath = FPaths::ProjectSavedDir() + Path + FileName;
    TArray<uint8> BinaryData;

    if (!FFileHelper::LoadFileToArray(BinaryData, *LoadPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *LoadPath);
        return BoundaryConnections;
    }

    FMemoryReader FromBinary = FMemoryReader(BinaryData, true);
    FromBinary.Seek(0);

    // 读取边界节点的连接信息
    int32 BoundaryCount;
    FromBinary << BoundaryCount;

    for (int32 i = 0; i < BoundaryCount; ++i)
    {
        int32 BoundaryIndex;
        int32 ConnectionIndex;
        FromBinary << BoundaryIndex;
        FromBinary << ConnectionIndex;
        BoundaryConnections.Add(BoundaryIndex, ConnectionIndex);
    }

    FromBinary.FlushCache();
    BinaryData.Empty();
    FromBinary.Close();

    return BoundaryConnections;
}
void UAStarNodeUtils::DrawGridNodesFromBin(UWorld* World, const TArray<FVector>& NodePositions, const TArray<TArray<FVector>>& NeighborPositions, const TArray<int32>& NodeIndexes, float Duration, float Thickness)
{
    if (!World)
    {
        return;
    }

    // 设置点和线的颜色
    const FColor NodeColor = FColor::Red;
    const FColor LineColor = FColor::Green;

    // 确保每个节点都有相应的邻居位置数组
    if (NodePositions.Num() != NeighborPositions.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Node positions and neighbor positions array sizes do not match."));
        return;
    }

    // 遍历所有节点位置
    for (int32 i = 0; i < NodePositions.Num(); ++i)
    {
        const FVector& NodePosition = NodePositions[i];
        int32 Index = NodeIndexes[i];
        // 在每个节点的位置画点
        UKismetSystemLibrary::DrawDebugPoint(
            World,
            NodePosition,
            10.0f,  // 点的大小
            NodeColor,
            Duration
        );
        FString Coordinates = FString::Printf(TEXT("Index: %d\n(%.1f, %.1f, %.1f)"), Index, NodePosition.X, NodePosition.Y, NodePosition.Z);
        UKismetSystemLibrary::DrawDebugString(World, NodePosition + FVector(0, 0, 20.0f), Coordinates, nullptr, FLinearColor::White, Duration);
        // 遍历当前节点的每个邻居并画线连接
        for (const FVector& NeighborPosition : NeighborPositions[i])
        {
            UKismetSystemLibrary::DrawDebugLine(
                World,
                NodePosition,
                NeighborPosition,
                LineColor,
                Duration,
                Thickness
            );
        }
    }
}
void UAStarNodeUtils::DrawGridNodesFromBin(UWorld* World, const TArray<FVector>& NodePositions, const TArray<TArray<FVector>>& NeighborPositions, const TArray<int32>& NodeIndexes, float Duration, float Thickness, const FColor& LineColor)
{
    if (!World)
    {
        return;
    }
    const FColor NodeColor = FColor::Red;
    // 确保每个节点都有相应的邻居位置数组
    if (NodePositions.Num() != NeighborPositions.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Node positions and neighbor positions array sizes do not match."));
        return;
    }

    // 遍历所有节点位置
    for (int32 i = 0; i < NodePositions.Num(); ++i)
    {
        const FVector& NodePosition = NodePositions[i];
        int32 Index = NodeIndexes[i];
        // 在每个节点的位置画点
        UKismetSystemLibrary::DrawDebugPoint(
            World,
            NodePosition + FVector(0, 0, 20.0f),
            10.0f,  // 点的大小
            NodeColor,
            Duration
        );
        FString Coordinates = FString::Printf(TEXT("Index: %d\n(%.1f, %.1f, %.1f)"), Index, NodePosition.X, NodePosition.Y, NodePosition.Z);
        UKismetSystemLibrary::DrawDebugString(World, NodePosition + FVector(0, 0, 40.0f), Coordinates, nullptr, FLinearColor::White, Duration);
        // 遍历当前节点的每个邻居并画线连接
        for (const FVector& NeighborPosition : NeighborPositions[i])
        {
            UKismetSystemLibrary::DrawDebugLine(
                World,
                NodePosition + FVector(0, 0, 20.0f),
                NeighborPosition + FVector(0, 0, 20.0f),
                LineColor,
                Duration,
                Thickness
            );
        }
    }
}
//show patrol points
void UAStarNodeUtils::VisualizeLoadedGrid(UWorld* World, const FString& FileName, const FString& Path, const FColor& LineColor)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileName, Path);

    // 提取节点位置和邻居位置用于绘制
    TArray<FVector> NodePositions;
    TArray<TArray<FVector>> NeighborPositions;
    TArray<int32> NodeIndexes;
    for (const UAStarNode* Node : GridNodes)
    {
        NodePositions.Add(Node->Position);
        NodeIndexes.Add(Node->Index);
        TArray<FVector> CurrentNeighborPositions;

        for (const UAStarNode* Neighbor : Node->Neighbors)
        {
            CurrentNeighborPositions.Add(Neighbor->Position);
        }

        NeighborPositions.Add(CurrentNeighborPositions);
    }

    // 调用绘制函数
    DrawGridNodesFromBin(World, NodePositions, NeighborPositions, NodeIndexes, 5.0f, 3.0f, LineColor);
}
void UAStarNodeUtils::VisualizeLoadedGrid(UWorld* World, const FString& FileName, const FString& Path)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileName, Path);

    // 提取节点位置和邻居位置用于绘制
    TArray<FVector> NodePositions;
    TArray<TArray<FVector>> NeighborPositions;
    TArray<int32> NodeIndexes;
    for (const UAStarNode* Node : GridNodes)
    {
        NodePositions.Add(Node->Position);
        NodeIndexes.Add(Node->Index);
        TArray<FVector> CurrentNeighborPositions;

        for (const UAStarNode* Neighbor : Node->Neighbors)
        {
            CurrentNeighborPositions.Add(Neighbor->Position);
        }

        NeighborPositions.Add(CurrentNeighborPositions);
    }

    // 调用绘制函数
    DrawGridNodesFromBin(World, NodePositions, NeighborPositions, NodeIndexes, 5.0f, 3.0f);
}

void UAStarNodeUtils::VisualizeLoadedGrid(UWorld* World, const TArray<UAStarNode*>& GridNodes)
{
    // 提取节点位置和邻居位置用于绘制
    TArray<FVector> NodePositions;
    TArray<TArray<FVector>> NeighborPositions;
    TArray<int32> NodeIndexes;  // 存储节点索引
    for (const UAStarNode* Node : GridNodes)
    {
        NodePositions.Add(Node->Position);
        NodeIndexes.Add(Node->Index);
        TArray<FVector> CurrentNeighborPositions;

        for (const UAStarNode* Neighbor : Node->Neighbors)
        {
            CurrentNeighborPositions.Add(Neighbor->Position);
        }

        NeighborPositions.Add(CurrentNeighborPositions);
    }

    // 调用绘制函数
    DrawGridNodesFromBin(World, NodePositions, NeighborPositions, NodeIndexes, 60.0f, 3.0f);
}

void UAStarNodeUtils::VisualizeLoadedGrid(UWorld* World, const FString& FileName, const FString& Path, const FVector& PlayerPosition)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileName, Path);

    // 提取节点位置、邻居位置及索引用于绘制
    TArray<FVector> NodePositions;
    TArray<TArray<FVector>> NeighborPositions;
    TArray<int32> NodeIndexes;  // 存储节点索引

    for (const UAStarNode* Node : GridNodes)
    {
        float Distance = FVector::Dist(Node->Position, PlayerPosition);

        if (Distance <= 800.0f)
        {
            NodePositions.Add(Node->Position);
            NodeIndexes.Add(Node->Index);  // 假设每个节点有一个 'Index' 属性
            TArray<FVector> CurrentNeighborPositions;

            for (const UAStarNode* Neighbor : Node->Neighbors)
            {
                if (FVector::Dist(Neighbor->Position, PlayerPosition) <= 800.0f)
                {
                    CurrentNeighborPositions.Add(Neighbor->Position);
                }
            }

            NeighborPositions.Add(CurrentNeighborPositions);
        }
    }

    DrawGridNodesFromBin(World, NodePositions, NeighborPositions, NodeIndexes, 5.0f, 3.0f);  // 传递索引
}

void UAStarNodeUtils::SetNodePositionByIndex(int32 NodeIndex, const FVector& NewPosition)
{
    TArray<UAStarNode*> GridNodes = UAStarNodeUtils::LoadGridFromBinaryFile(FileNameDebug, PathDebug);
    // 确保索引有效
    if (GridNodes.IsValidIndex(NodeIndex))
    {
        UAStarNode* Node = GridNodes[NodeIndex];
        if (Node)
        {
            // 设置新的位置
            Node->Position = NewPosition;
            SaveGridToBinaryFile(GridNodes, FileNameDebug, PathDebug);
            UE_LOG(LogTemp, Log, TEXT("Node at index %d position updated to %s."), NodeIndex, *NewPosition.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Node at index %d is null."), NodeIndex);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid node index: %d"), NodeIndex);
    }
}

void UAStarNodeUtils::CreateNode(UWorld* World, FVector Position, const TArray<int32>& NeighborNodeIDs)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileNameDebug, PathDebug);

    // 创建新的节点
    UAStarNode* Node = NewObject<UAStarNode>(World, UAStarNode::StaticClass());

    // 获取新的节点的索引
    int32 NewNodeIndex = GridNodes.Num() - 1;

    Node->Position = Position;
    Node->Index = NewNodeIndex;
    // 将新的节点添加到网格节点数组中
    GridNodes.Add(Node);

    // 遍历邻居节点ID数组，并添加双向邻居关系
    for (int32 NeighborID : NeighborNodeIDs)
    {
        if (GridNodes.IsValidIndex(NeighborID))
        {
            Node->AddBidirectionalNeighbor(GridNodes[NeighborID]);
        }
    }

    // 将网格节点保存到二进制文件
    SaveGridToBinaryFile(GridNodes, FileNameDebug, PathDebug);
}

void UAStarNodeUtils::CreateMultipleNodes(UWorld* World, FString& FileName, FString& Path, const TArray<FVector>& Positions)
{
    // 从二进制文件加载网格节点
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileName, Path);

    // 创建新节点并添加到网格节点数组中
    TArray<UAStarNode*> NewNodes;
    for (const FVector& Position : Positions)
    {
        UAStarNode* Node = NewObject<UAStarNode>(World, UAStarNode::StaticClass());
        Node->Position = Position;
        NewNodes.Add(Node);
        GridNodes.Add(Node);
    }

    // 将网格节点保存到二进制文件
    SaveGridToBinaryFile(GridNodes, FileName, Path);
}

void UAStarNodeUtils::RemoveNeighborFromNode(int32 NodeAIndex, int32 NodeBIndex)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileNameDebug, PathDebug);
    GridNodes[NodeAIndex]->RemoveNeighbor(GridNodes[NodeBIndex]);
    SaveGridToBinaryFile(GridNodes, FileNameDebug, PathDebug);
}

void UAStarNodeUtils::RemoveAllNeighbor(int32 NodeIndex)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileNameDebug, PathDebug);
    if (NodeIndex >= 0 && NodeIndex < GridNodes.Num())
    {
        GridNodes[NodeIndex]->RemoveAllNeighbors();
        SaveGridToBinaryFile(GridNodes, FileNameDebug, PathDebug);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Invalid node index: %d"), NodeIndex);
    }

}

void UAStarNodeUtils::ModifyNodePositionByIndex(int32 NodeIndex, FString& axis, float offset)
{
    TArray<UAStarNode*> GridNodes = UAStarNodeUtils::LoadGridFromBinaryFile(FileNameDebug, PathDebug);
    if (axis.Contains("x"))
    {
        GridNodes[NodeIndex]->Position.X += offset;
    }
    else if (axis.Contains("y"))
    {
        GridNodes[NodeIndex]->Position.Y += offset;
    }
    else if (axis.Contains("z"))
    {
        GridNodes[NodeIndex]->Position.Z += offset;
    }
    SaveGridToBinaryFile(GridNodes, FileNameDebug, PathDebug);
}

void UAStarNodeUtils::ShowNode(int Index)
{
    // 从二进制文件加载网格节点
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileNameDebug, PathDebug);

    if (GridNodes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("GridNodes array is empty."));
        return;
    }

    UAStarNode* NodeToShow = nullptr;
    int32 NodeIndex = 0;

    if (Index == -1)
    {
        // 显示最后一个节点
        NodeToShow = GridNodes.Last();
        NodeIndex = GridNodes.Num() - 1;
    }
    else if (GridNodes.IsValidIndex(Index))
    {
        // 显示指定索引的节点
        NodeToShow = GridNodes[Index];
        NodeIndex = Index;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Index: %d"), Index);
        return;
    }

    FVector NodePosition = NodeToShow->Position;
    UE_LOG(LogTemp, Log, TEXT("Current FileName1: %s"), *FileNameDebug);
    UE_LOG(LogTemp, Log, TEXT("Current Path1: %s"), *PathDebug);
    UE_LOG(LogTemp, Log, TEXT("Node Index: %d"), NodeIndex);
    UE_LOG(LogTemp, Log, TEXT("Node Position: (%f, %f, %f)"), NodePosition.X, NodePosition.Y, NodePosition.Z);


    if (NodeToShow->Neighbors.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Listing Neighbors:"));
        for (UAStarNode* Neighbor : NodeToShow->Neighbors)
        {
            if (Neighbor) // 确保邻居不是空指针
            {
                FVector NeighborPosition = Neighbor->Position;
                UE_LOG(LogTemp, Log, TEXT("Node Index: %d"), Neighbor->Index);
                UE_LOG(LogTemp, Log, TEXT("Neighbor Position: (%f, %f, %f)"), NeighborPosition.X, NeighborPosition.Y, NeighborPosition.Z);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Null neighbor found in neighbor list."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No neighbors found for this node."));
    }
}

void UAStarNodeUtils::DeleteNode(int32 NodeIndex)
{
    TArray<UAStarNode*> GridNodes =
        LoadGridFromBinaryFile(
            FileNameDebug,
            PathDebug
        );

    if (!GridNodes.IsValidIndex(NodeIndex))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("DeleteNode: Invalid Node Index %d"),
            NodeIndex
        );
        return;
    }

    UAStarNode* NodeToDelete =
        GridNodes[NodeIndex];

    if (!NodeToDelete)
    {
        return;
    }

    // 删除所有节点对它的引用
    for (UAStarNode* Node : GridNodes)
    {
        if (!Node)
        {
            continue;
        }

        Node->Neighbors.Remove(NodeToDelete);
    }

    // 删除节点
    GridNodes.RemoveAt(NodeIndex);

    // 重排索引
    for (int32 i = 0; i < GridNodes.Num(); i++)
    {
        if (GridNodes[i])
        {
            GridNodes[i]->Index = i;
        }
    }

    // 清缓存（如果用了）
    GridCache.Empty();

    UAStarNodeUtils::SaveGridToBinaryFile(
        GridNodes,
        FileNameDebug,
        PathDebug
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Deleted Node %d"),
        NodeIndex
    );
}

void UAStarNodeUtils::AddBidirectionalNeighbor(int32 NodeAIndex, int32 NodeBIndex)
{
    TArray<UAStarNode*> GridNodes = LoadGridFromBinaryFile(FileNameDebug, PathDebug);
    GridNodes[NodeAIndex]->AddBidirectionalNeighbor(GridNodes[NodeBIndex]);
    SaveGridToBinaryFile(GridNodes, FileNameDebug, PathDebug);
}

void UAStarNodeUtils::AddPatrolPoint(UWorld* World, int32 NodeIndex, const TArray<int32>& NeighborNodeIDs)
{
    TArray<UAStarNode*> PGridNodes = LoadGridFromBinaryFile(FileNameDebug, PathDebug);
    TArray<UAStarNode*> CGridNodes = LoadGridFromBinaryFile("Data.bin", PathDebug);
    CreateNode(World, CGridNodes[NodeIndex]->Position, NeighborNodeIDs);
}

void UAStarNodeUtils::ClearGridCache()
{
    GridCache.Empty();
}

