
#include "CustomNode.h"

UCustomNode::UCustomNode()
{
    Neighbors.Reserve(10);
    Position = FVector(0.f, 0.f, 0.f);
}

void UCustomNode::AddNeighbor(UCustomNode* Neighbor)
{
    if (Neighbors.Num() < 10 && Neighbor != nullptr && !Neighbors.Contains(Neighbor))
    {
        Neighbors.Add(Neighbor);
    }
}

UCustomNode* UCustomNode::GetNeighborAt(int32 Index) const
{
    if (Neighbors.IsValidIndex(Index))
    {
        return Neighbors[Index];
    }
    return nullptr;
}

void UCustomNode::RemoveNeighbor(UCustomNode* NeighborToRemove)
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

void UCustomNode::AddBidirectionalNeighbor(UCustomNode* Neighbor)
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

void UCustomNode::RemoveAllNeighbors()
{
    // Inform about the node deletion
    UE_LOG(LogTemp, Log, TEXT("Deleting node at position: %s and disconnecting all neighbors."), *Position.ToString());

    // Remove this node from all its neighbors' neighbor lists
    for (UCustomNode* Neighbor : Neighbors)
    {
        if (Neighbor)
        {
            RemoveNeighbor(Neighbor);  // Remove this node from each neighbor's list
            UE_LOG(LogTemp, Log, TEXT("Removed this node neighbor at position: %s."), *Neighbor->Position.ToString());
        }
    }

}