// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNodeCheatManager.h"
#include "Engine/Engine.h"
#include "AStarNodeUtils.h"
#include "AIController.h"
#include "EngineUtils.h"

static FAutoConsoleCommand SetNodePosition
(
    TEXT("c.SetNodePositionByIndex"),
    TEXT("Set node position by index. Usage: NodeIndex X Y Z"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() >= 4)
            {
                int32 NodeIndex = FCString::Atoi(*Args[0]);
                float X = FCString::Atof(*Args[1]);
                float Y = FCString::Atof(*Args[2]);
                float Z = FCString::Atof(*Args[3]);
                FVector NewPosition(X, Y, Z);
                UAStarNodeUtils::SetNodePositionByIndex(NodeIndex, NewPosition);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: NodeIndex X Y Z"));
            }
        })
);

static FAutoConsoleCommand ModifyNodePosition
(
    TEXT("c.ModifyNodePositionByIndex"),
    TEXT("Modify node position by index. Usage: NodeIndex Axis Offset"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() >= 3)
            {
                int32 NodeIndex = FCString::Atoi(*Args[0]);
                FString Axis = Args[1];
                float Offset = FCString::Atof(*Args[2]);

                // 调用修改节点位置的函数
                UAStarNodeUtils::ModifyNodePositionByIndex(NodeIndex, Axis, Offset);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: NodeIndex Axis Offset"));
            }
        })
);

static FAutoConsoleCommand RemoveNeighborFromNode
(
    TEXT("c.RemoveNeighborFromNode"),
    TEXT("Remove Neighbor of a Node. Usage: NodeIndexA, NodeIndexB"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() == 2)
            {
                int32 NodeIndexA = FCString::Atoi(*Args[0]);
                int32 NodeIndexB = FCString::Atoi(*Args[1]);

                UAStarNodeUtils::RemoveNeighborFromNode(NodeIndexA, NodeIndexB);
                UE_LOG(LogTemp, Log, TEXT("Remove Neighbor success"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: NodeIndexA, NodeIndexB"));
            }
        })
);

static FAutoConsoleCommand AddBidirectionalNeighbor
(
    TEXT("c.AddBidirectionalNeighbor"),
    TEXT("Add Bidirectional Neighbor of a Node. Usage: NodeIndexA, NodeIndexB"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() == 2)
            {
                int32 NodeIndexA = FCString::Atoi(*Args[0]);
                int32 NodeIndexB = FCString::Atoi(*Args[1]);

                UAStarNodeUtils::AddBidirectionalNeighbor(NodeIndexA, NodeIndexB);
                UE_LOG(LogTemp, Log, TEXT("Add Bidirectional Neighbor success"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: Usage: NodeIndexA, NodeIndexB"));
            }
        })
);

static FAutoConsoleCommand ShowNodeInfor
(
    TEXT("c.ShowNodeInfor"),
    TEXT("Show information of a Node. Usage: NodeIndexA. if want to show last node, use -1 index"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() == 1)
            {
                int32 NodeIndex = FCString::Atoi(*Args[0]);

                UAStarNodeUtils::ShowNode(NodeIndex);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: Usage: NodeIndex"));
            }
        })
);

static FAutoConsoleCommand AddPatrolPoint
(
    TEXT("c.AddPatrolPoint"),
    TEXT("Add a node from chase point to partrol point. Usage: NodeIndex [NeighborID1 NeighborID2 ...]."),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() >= 2)
            {
                int32 NodeIndex = FCString::Atoi(*Args[0]);
                TArray<int32> NeighborIDs;

                for (int32 i = 1; i < Args.Num(); ++i)
                {
                    int32 NeighborID = FCString::Atoi(*Args[i]);
                    NeighborIDs.Add(NeighborID);
                }
                UAStarNodeUtils::AddPatrolPoint(GWorld, NodeIndex, NeighborIDs);
            }
            else if (Args.Num() == 1)
            {
                int32 NodeIndex = FCString::Atoi(*Args[0]);
                TArray<int32> NeighborIDs;
                UAStarNodeUtils::AddPatrolPoint(GWorld, NodeIndex, NeighborIDs);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: Usage: NodeIndex"));
            }
        })
);

static FAutoConsoleCommand RemoveAllNeighbor
(
    TEXT("c.RemoveAllNeighbor"),
    TEXT("Remove all neighbor of a Node. Usage: NodeIndex"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() == 1)
            {
                int32 NodeIndex = FCString::Atoi(*Args[0]);

                UAStarNodeUtils::RemoveAllNeighbor(NodeIndex);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: Usage: NodeIndex"));
            }
        })
);

static FAutoConsoleCommand CreateNode
(
    TEXT("c.CreateNode"),
    TEXT("Create a Node. Usage: c.CreateNode X Y Z [NeighborID1 NeighborID2 ...]"),
    FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
        {
            if (Args.Num() >= 3)
            {
                float X = FCString::Atof(*Args[0]);
                float Y = FCString::Atof(*Args[1]);
                float Z = FCString::Atof(*Args[2]);
                FVector NewPosition(X, Y, Z);
                TArray<int32> NeighborIDs;

                // Add any additional arguments as neighbor IDs
                for (int32 i = 3; i < Args.Num(); ++i)
                {
                    int32 NeighborID = FCString::Atoi(*Args[i]);
                    NeighborIDs.Add(NeighborID);
                }

                UAStarNodeUtils::CreateNode(GWorld, NewPosition, NeighborIDs);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Insufficient arguments. Usage: c.CreateNode X Y Z [NeighborID1 NeighborID2 ...]"));
            }
        })
);

static FAutoConsoleCommand DeleteNodeCmd
(
    TEXT("c.DeleteNode"),
    TEXT("Delete node by index"),
    FConsoleCommandWithArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args)
        {
            if (Args.Num() < 1)
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Usage: c.DeleteNode NodeIndex")
                );
                return;
            }

            int32 NodeIndex =
                FCString::Atoi(*Args[0]);

            UAStarNodeUtils::DeleteNode(
                NodeIndex
            );
        }
    )
);

static FAutoConsoleCommand GenerateNodeGridCmd
(
    TEXT("c.GenerateNodeGrid"),
    TEXT("Generate grid around BP_HandEnemy1. Usage: c.GenerateNodeGrid Left Right Forward Backward"),
    FConsoleCommandWithArgsDelegate::CreateLambda(
        [](const TArray<FString>& Args)
        {
            if (Args.Num() != 4)
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Usage: c.GenerateNodeGrid Left Right Forward Backward")
                );
                return;
            }

            if (!GWorld)
            {
                UE_LOG(LogTemp, Error, TEXT("GenerateNodeGrid failed: GWorld is null."));
                return;
            }

            APawn* TargetEnemyPawn = nullptr;

            for (TActorIterator<APawn> It(GWorld); It; ++It)
            {
                APawn* Pawn = *It;
                if (!Pawn)
                {
                    continue;
                }

                if (Pawn->GetName().Contains(TEXT("BP_HandEnemy1")))
                {
                    TargetEnemyPawn = Pawn;
                    break;
                }
            }

            if (!TargetEnemyPawn)
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("GenerateNodeGrid failed: Cannot find pawn containing name BP_HandEnemy1.")
                );
                return;
            }

            int32 CellsLeft = FCString::Atoi(*Args[0]);
            int32 CellsRight = FCString::Atoi(*Args[1]);
            int32 CellsForward = FCString::Atoi(*Args[2]);
            int32 CellsBackward = FCString::Atoi(*Args[3]);

            if (
                CellsLeft < 0 ||
                CellsRight < 0 ||
                CellsForward < 0 ||
                CellsBackward < 0
                )
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("GenerateNodeGrid failed: Cell values must be >= 0.")
                );
                return;
            }

            const FVector Center = TargetEnemyPawn->GetActorLocation();

            TArray<UAStarNode*> GridNodes =
                UAStarNodeUtils::GenerateNodeGrid(
                    GWorld,
                    Center,
                    CellsLeft,
                    CellsRight,
                    CellsForward,
                    CellsBackward,
                    200.0f,
                    0.0f
                );

            if (GridNodes.Num() == 0)
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("GenerateNodeGrid finished but generated 0 nodes.")
                );
                return;
            }

            const FString FileName = TEXT("Data.bin");
            const FString Path = TEXT("/GridData/BP_HandEnemy1/");

            UAStarNodeUtils::SaveGridToBinaryFile(
                GridNodes,
                FileName,
                Path
            );

            UAStarNodeUtils::FileNameDebug = FileName;
            UAStarNodeUtils::PathDebug = Path;

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("GenerateNodeGrid success. Enemy=%s Center=%s Nodes=%d Saved=%s%s"),
                *TargetEnemyPawn->GetName(),
                *Center.ToString(),
                GridNodes.Num(),
                *Path,
                *FileName
            );
        }
    )
);


void UAStarNodeCheatManager::InitCheatManager()
{
	ReceiveInitCheatManager();

}
