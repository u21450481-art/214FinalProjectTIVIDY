# TIVIDY: Workflow Management System
![CI](https://github.com/u21450481-art/214FinalProjectTIVIDY/actions/workflows/ci.yml/badge.svg)

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

## Project layout

        app/       TUI front end (FTXUI)
        src/       engine sources
        include/   headers
        tests/     integration and unit tests
        docs/      diagrams and CI notes
        .github/   CI pipeline

    ## Building and testing

    Run these from the repo root in Linux or WSL:

        make            # build the TUI (tividy)
        make unit       # unit tests for the mediator and the strategies
        make test       # integration tests
        make check11    # engine compiles as plain C++11

## Bonus: Docker and CI

### Docker

The TUI is containerised with a two-stage Dockerfile. The first stage builds FTXUI (pinned to v6.1.9) and compiles the TUI on Ubuntu 24.04, and the second stage copies only the finished program into a clean image.

To run the demo:

```
docker build -t tividy .
docker run -t --rm tividy
```

The first build is slow because it compiles FTXUI from scratch. Use a large terminal window.

### CI pipeline

Every push to `dev` or a `feature/**` branch, and every pull request into `dev`, runs a GitHub Actions pipeline (`.github/workflows/ci.yml`):

| Job | What it does |
|---|---|
| `test` | Checks the engine compiles as C++11, then runs the unit and integration tests with AddressSanitizer and UBSan |
| `docker` | Builds the Docker image and checks the `tividy` program is inside it (runs only if `test` passes) |
| `static-analysis` | Runs cppcheck and reports advice, without failing the run |

See the **Actions** tab for the latest results. More detail is in `docs/CI.md`.