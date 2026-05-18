// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNode.h"

UAStarNode::UAStarNode()
{
    Position = FVector::ZeroVector;
    Parent = nullptr;
    G = 0.0f;
    H = 0.0f;
    F = 0.0f;
}

void UAStarNode::PrintNodeProperties() const
{
    UE_LOG(LogTemp, Log, TEXT("Position: %s, G: %f, H: %f, F: %f"),
        *Position.ToString(), G, H, F);
}

void UAStarNode::AddUnidirectionalNeighbor(UAStarNode* Neighbor)
{
    if (Neighbors.Num() < 10 && Neighbor != nullptr && !Neighbors.Contains(Neighbor))
    {
        Neighbors.Add(Neighbor);
    }
}

void UAStarNode::AddBidirectionalNeighbor(UAStarNode* Neighbor)
{
    if (Neighbor && Neighbor != this)
    {
        // 添加 Neighbor 为当前节点的邻居
        if (!Neighbors.Contains(Neighbor))
        {
            Neighbors.Add(Neighbor);
            UE_LOG(LogTemp, Log, TEXT("Neighbor added to current node."));
        }

        // 同时确保当前节点也被添加到 Neighbor 的邻居列表中
        if (!Neighbor->Neighbors.Contains(this))
        {
            Neighbor->Neighbors.Add(this);
            UE_LOG(LogTemp, Log, TEXT("Current node added to neighbor's neighbors."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid neighbor or self-referencing attempt."));
    }
}

void UAStarNode::RemoveAllNeighbors()
{
    // Remove this node from all its neighbors' neighbor lists
    for (int32 i = Neighbors.Num() - 1; i >= 0; i--)
    {
        if (UAStarNode* Neighbor = Neighbors[i])
        {
            RemoveNeighbor(Neighbor);  // Remove this node from each neighbor's list
        }
    }

}

void UAStarNode::RemoveNeighbor(UAStarNode* NeighborToRemove)
{
    if (!NeighborToRemove)
    {
        UE_LOG(LogTemp, Warning, TEXT("Passed neighbor is null."));
        return;
    }

    // 检查并移除当前节点的邻居
    if (Neighbors.Contains(NeighborToRemove))
    {
        Neighbors.RemoveSingle(NeighborToRemove);
        UE_LOG(LogTemp, Log, TEXT("Neighbor removed from current node."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Neighbor not found in current node's neighbors."));
    }

    // 检查并从传入的邻居的邻居列表中移除当前节点
    if (NeighborToRemove->Neighbors.Contains(this))
    {
        NeighborToRemove->Neighbors.RemoveSingle(this);
        UE_LOG(LogTemp, Log, TEXT("Current node removed from neighbor's neighbors."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Current node not found in neighbor's neighbors."));
    }
}

UAStarNode* UAStarNode::GetNeighborAt(int32 NeighborIndex) const
{
    if (Neighbors.IsValidIndex(NeighborIndex))
    {
        return Neighbors[NeighborIndex];
    }
    return nullptr;
}