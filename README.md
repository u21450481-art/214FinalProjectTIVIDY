# TIVIDY: Workflow Management System

COS 214 Project (2026), Practical 6: research and initial design phase.

## Group Members
* **Bowdene Pillay** – 24582094
* **Carla Erasmus** – u21450481
* **Luthando Shilhwaio** – u25504747
* **Tshegofatso Mautjane** – u25646006
* **Vimbai Chivunga** - u25136608

TIVIDY is a **generic workflow management system** written in C++11. It stores workflow definitions, creates independent running instances from them, and coordinates the work, people and external systems involved until each instance reaches a final outcome. The engine knows nothing about any particular business process.

To demonstrate the engine, we use one scenario: **employee onboarding**. The scenario is one application of TIVIDY. It does not define the engine's architecture.

## Design at a glance

- **Definition vs instance:** a workflow definition is an immutable template. Each running instance has its own data, progress and history.
- **Work structure:** stages, work items and sub-processes form a tree that the engine treats uniformly.
- **Variable parts kept separate:** assignment rules, approval and escalation, events and traversal.

## Design patterns (GoF)

| Pattern | What it does in TIVIDY |
|---------|------------------------|
| Facade | `WorkflowEngine` gives clients a single entry point to start workflows |
| Builder | Assembles a workflow definition step by step |
| Composite | Treats single tasks and nested sub-processes uniformly |
| Adapter | Connects external systems (email, Home Affairs) to the engine |
| Mediator | Coordinates which task starts next, so tasks never trigger each other directly |
| Strategy | Swappable rules for assigning work to participants |
| Command | Records actions as objects for the audit trail and undo |
| State | Each work item enforces its own lifecycle rules |
| Chain of Responsibility | Escalates approvals up the management hierarchy |
| Iterator | Walks the nested work tree without exposing its structure |
|Prototype | Clones workflow definitions to spawn concurrent, memory-isolated runtime instances |

## Scenario: Insurance Claim & Payout Workflow Example

An insurance client submits a high-value claim. The system validates policy details, routes the claim through an approval hierarchy (Manager -> Director -> CEO) using the Chain of Responsibility pattern, runs parallel fraud verification checks via external adapters, and triggers payout or denial tasks. Edge cases and failures include invalid documentation, fraudulent flags, and unresponsive approvers who trigger automated escalation fallbacks.