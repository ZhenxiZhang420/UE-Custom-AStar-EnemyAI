# UE Custom A* Enemy AI System

Custom node-based A* enemy AI system developed in Unreal Engine using C++.

Instead of relying on Unreal Engine NavMesh, this project implements a complete custom navigation solution integrated with Behavior Trees and enemy AI behaviors.

Features include custom pathfinding, runtime node editing tools, patrol/chase/search behaviors, and attack positioning systems.

---

# Demo

## Node Graph Visualization

[Insert Image / GIF Here]

_Custom A* node graph and neighbor connections_

---

## Enemy Patrol

[Insert GIF Here]

_Enemy patrol behavior using custom pathfinding_

---

## Detect → Chase → Attack

[Insert GIF Here]

_Enemy detects player and enters combat behavior_

---

## Search Last Known Position

[Insert GIF Here]

_Enemy searches player's last known position after losing sight_

---

## Attack Warning Visualization

[Insert GIF Here]

_Attack prediction using decal indicators_

---

# Features

### Custom A* Pathfinding

Implemented a custom node-based A* algorithm instead of using Unreal Engine NavMesh.

Features:

- Open / Closed list search
- G / H / F cost calculation
- Path reconstruction
- Nearest node selection
- Dynamic path updates

---

### AI Behaviors

Enemy AI supports:

- Patrol
- Detect Player
- Wait Before Chase
- Chase
- Search Last Known Position
- Move To Attack Position
- Attack

Implemented using:

- Behavior Tree
- Blackboard
- AI Controller

---

### Runtime Debugging Tools

Built custom CheatManager tools for navigation graph editing and debugging.

Supported functionality:

- Create nodes
- Delete nodes
- Modify node positions
- Add/remove neighbor connections
- Generate grid nodes
- Visualize node graphs

Purpose:

- Rapid navigation graph creation
- Runtime debugging
- Faster level setup workflow

---

### Binary Save / Load System

Implemented binary serialization for navigation data.

Files:

```cpp
Data.bin
PatrolPoints.bin
```

Allows:

- Save generated node graphs
- Runtime loading
- Reduced setup time

---

# System Architecture

```text
EnemyAIController
        ↓

Behavior Tree
        ↓

BTTask Nodes
        ↓

AStar Algorithm
        ↓

Node Utilities
        ↓

Movement Function Library
```

Main classes:

```cpp
AStarAlgorithm

AStarNode

AStarNodeUtils

EnemyAIController

MoveFunctionLibrary

BTTask_Chase

BTTask_SearchPlayer

BTTask_Attack

BTTask_Patrol

BTTask_MoveToAttackPosition
```

---

# Technical Challenges

### Avoiding NavMesh limitations

Instead of relying on Unreal Engine NavMesh, a custom node-based navigation system was developed to support more flexible movement behavior.

---

### Runtime graph editing

Large node networks became difficult to maintain manually.

Solution:

Developed CheatManager debugging tools to create and modify nodes during runtime.

---

### Stable AI movement

Frequent path updates caused unstable movement behavior.

Solution:

Added waiting logic and state transitions to improve movement stability.

---

# Technologies

- Unreal Engine
- C++
- A* Algorithm
- Behavior Tree
- Blackboard
- AI Controller
- Binary Serialization
- Cheat Manager

---

# Future Improvements

Planned upgrades:

- Dynamic obstacle avoidance
- Path smoothing
- Multithreaded A*
- Hierarchical pathfinding
- Cooperative enemy AI

---

# Author

ZhenXi Zhang
