// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarAlgorithm.h"
#include "AStarNode.h"

void UAStarAlgorithm::ResetNodes(TArray<UAStarNode*>& Nodes)
{
    for (UAStarNode* Node : Nodes)
    {
        Node->Parent = nullptr;
        Node->G = 0;
        Node->H = 0;
        Node->F = 0;
    }
}

TArray<FVector> UAStarAlgorithm::FindPath(UAStarNode* StartNode, UAStarNode* EndNode)
{
    //UE_LOG(LogTemp, Log, TEXT("StartNode Position: %s"), *StartNode->Position.ToString());
    //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());

    TArray<UAStarNode*> OpenList;
    TArray<UAStarNode*> ClosedList;
    TArray<FVector> Path;

    if (!StartNode || !EndNode)
    {
        UE_LOG(LogTemp, Error, TEXT("Start or End node is null"));
        return Path;
    }

    OpenList.Add(StartNode);
    int count = 0;
    //OpenList.Num() > 0
    while (OpenList.Num() > 0 && count < 1000)
    {
        count++;
        OpenList.Sort([](const UAStarNode& A, const UAStarNode& B) {
            return A.F < B.F;
            });

        UAStarNode* CurrentNode = OpenList[0];
        OpenList.RemoveAt(0);
        ClosedList.Add(CurrentNode);

        if (CurrentNode == EndNode)
        {
            while (CurrentNode != nullptr) // 添加计数器限制
            {
                Path.Add(CurrentNode->Position);
                CurrentNode = CurrentNode->Parent;
            }

            // 重置所有节点的状态
            ResetNodes(ClosedList);
            ResetNodes(OpenList);

            Algo::Reverse(Path);
            return Path;
        }

        if (CurrentNode->Neighbors.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("CurrentNode has no neighbors"));
            continue;
        }


        for (UAStarNode* Neighbor : CurrentNode->Neighbors)
        {
            if (ClosedList.Contains(Neighbor))
            {
                continue;
            }
            //check value invalid
            
            if (!Neighbor)
            {
                UE_LOG(LogTemp, Error, TEXT("Neighbor is null"));
                continue;
            }

            // 检查 CurrentNode 和 Neighbor 是否有效
            if (!CurrentNode)
            {
                UE_LOG(LogTemp, Error, TEXT("CurrentNode is null"));
                continue;
            }

            if (CurrentNode->Position.IsNearlyZero() || Neighbor->Position.IsNearlyZero())
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid node position: CurrentNode: %s, Neighbor: %s"), *CurrentNode->Position.ToString(), *Neighbor->Position.ToString());
                continue;
            }

            //check value invalid

            float TentativeG = CurrentNode->G + FVector::Dist(CurrentNode->Position, Neighbor->Position);

            if (!OpenList.Contains(Neighbor))
            {
                OpenList.Add(Neighbor);
            }
            else if (TentativeG >= Neighbor->G)
            {
                continue;
            }

            Neighbor->Parent = CurrentNode;
            Neighbor->G = TentativeG;
            Neighbor->H = FVector::Dist(Neighbor->Position, EndNode->Position); // Using straight-line distance as heuristic
            Neighbor->F = Neighbor->G + Neighbor->H;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Can not find path"));
    ResetNodes(ClosedList);
    ResetNodes(OpenList);

    return Path;
}

TArray<UAStarNode*> UAStarAlgorithm::FindPathReturnAStarNode(UAStarNode* StartNode, UAStarNode* EndNode)
{
    //UE_LOG(LogTemp, Log, TEXT("StartNode Position: %s"), *StartNode->Position.ToString());
    //UE_LOG(LogTemp, Log, TEXT("EndNode Position: %s"), *EndNode->Position.ToString());

    TArray<UAStarNode*> OpenList;
    TArray<UAStarNode*> ClosedList;
    TArray<UAStarNode*> Path;

    if (!StartNode || !EndNode)
    {
        UE_LOG(LogTemp, Error, TEXT("Start or End node is null"));
        return Path;
    }

    OpenList.Add(StartNode);
    int count = 0;
    while (OpenList.Num() > 0 && count < 1000)
    {
        count++;
        OpenList.Sort([](const UAStarNode& A, const UAStarNode& B) {
            return A.F < B.F;
            });

        UAStarNode* CurrentNode = OpenList[0];
        OpenList.RemoveAt(0);
        ClosedList.Add(CurrentNode);

        if (CurrentNode == EndNode)
        {
            while (CurrentNode != nullptr) // 添加计数器限制
            {
                Path.Add(CurrentNode);
                CurrentNode = CurrentNode->Parent;
            }

            // 重置所有节点的状态
            ResetNodes(ClosedList);
            ResetNodes(OpenList);

            Algo::Reverse(Path);
            return Path;
        }

        if (CurrentNode->Neighbors.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("CurrentNode has no neighbors"));
            continue;
        }

        for (UAStarNode* Neighbor : CurrentNode->Neighbors)
        {
            if (ClosedList.Contains(Neighbor))
            {
                continue;
            }

            if (!Neighbor)
            {
                UE_LOG(LogTemp, Error, TEXT("Neighbor is null"));
                continue;
            }

            if (!CurrentNode)
            {
                UE_LOG(LogTemp, Error, TEXT("CurrentNode is null"));
                continue;
            }

            if (CurrentNode->Position.IsNearlyZero() || Neighbor->Position.IsNearlyZero())
            {
                UE_LOG(LogTemp, Error, TEXT("Invalid node position: CurrentNode: %s, Neighbor: %s"), *CurrentNode->Position.ToString(), *Neighbor->Position.ToString());
                continue;
            }

            float TentativeG = CurrentNode->G + FVector::Dist(CurrentNode->Position, Neighbor->Position);

            if (!OpenList.Contains(Neighbor))
            {
                OpenList.Add(Neighbor);
            }
            else if (TentativeG >= Neighbor->G)
            {
                continue;
            }

            Neighbor->Parent = CurrentNode;
            Neighbor->G = TentativeG;
            Neighbor->H = FVector::Dist(Neighbor->Position, EndNode->Position); // Using straight-line distance as heuristic
            Neighbor->F = Neighbor->G + Neighbor->H;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Can not find path"));
    ResetNodes(ClosedList);
    ResetNodes(OpenList);

    return Path;
}