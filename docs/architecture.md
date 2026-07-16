# MQTTLift Architecture

## 1. Purpose

MQTTLift is a small but complete distributed elevator control simulator used to learn and demonstrate:

- C++17 project structure
- MQTT publish/subscribe design
- state-machine-based control logic
- clean separation between core logic and platform code
- a realistic end-to-end IoT workflow

The MVP intentionally focuses on the controller and MQTT. Dashboard, 3D visualization, logger, and ESP32 porting are future milestones, not part of the first release.

## 2. MVP Scope

### In scope
- 1 elevator
- 5 floors
- C++17 controller running on PC
- MQTT broker using Mosquitto
- FIFO request queue
- command topics and one JSON status snapshot topic
- door behavior with timed open/close cycle
- basic motor behavior with tick-based movement

### Out of scope for MVP
- Node-RED dashboard
- Three.js 3D viewer
- SQLite logging
- multi-elevator scheduling
- smart directional scheduling
- ESP32 deployment
- obstacle sensor simulation
- advanced safety logic

## 3. High-Level Architecture

The system is event-driven through MQTT.

```text
User / test client
        |
        v
   MQTT Broker (Mosquitto)
        |
        v
 C++ ElevatorController
        |
        +--> internal RequestQueue
        +--> internal Door state
        +--> internal Motor state
        +--> publish status snapshot
```

The controller is the only part that decides the elevator behavior. External clients publish commands and subscribe to status.

## 4. Main Design Principles

### 4.1 Keep the MVP small
The project should be finishable in a realistic amount of time. Every addition must help learning, completion, or clarity.

### 4.2 Separate core logic from MQTT plumbing
The control logic should stay readable and testable. MQTT should be treated as an input/output layer, not mixed into every part of the logic.

### 4.3 Prefer a tick-based update loop
The controller advances through a fixed update loop instead of blocking long operations. This matches embedded-style reasoning and keeps the behavior easy to simulate.

### 4.4 Prefer simple, stable decisions
If a feature adds complexity without helping the MVP, it should be postponed to future work.

## 5. Domain Model

### 5.1 ElevatorController
The main object that owns the whole elevator state.

Responsibilities:
- receive requests
- hold the queue
- update motion
- update door state
- publish status
- process emergency and reset commands

### 5.2 Request
A request is a domain object, not just a floor number.

```cpp
struct Request
{
    int floor;
    Direction direction;
    RequestSource source;
};
```

### 5.3 Direction
```cpp
enum class Direction
{
    Up,
    Down,
    None
};
```

### 5.4 RequestSource
```cpp
enum class RequestSource
{
    Hall,
    Cabin
};
```

### 5.5 ElevatorState
```cpp
enum class ElevatorState
{
    Idle,
    MovingUp,
    MovingDown,
    DoorOpening,
    DoorOpen,
    DoorClosing,
    Alarm
};
```

### 5.6 DoorState
```cpp
enum class DoorState
{
    Closed,
    Opening,
    Open,
    Closing
};
```

### 5.7 MotorState
```cpp
enum class MotorState
{
    Stopped,
    Running
};
```

## 6. MQTT Topic Design

### 6.1 Topic root
The topic root is:

```text
elevator/
```

This keeps the MVP simple and easy to read.

### 6.2 Command topics
```text
elevator/command/call
elevator/command/emergency
elevator/command/reset
```

### 6.3 Status topic
```text
elevator/status
```

The status topic publishes a full JSON snapshot of the current system state.

### 6.4 Log topic
```text
elevator/log
```

This topic exists in the architecture so that a logger can be added later without changing the controller design.

## 7. MQTT Payload Strategy

### 7.1 Commands
Commands are published as JSON objects. Example:

```json
{
  "floor": 5,
  "direction": "UP",
  "source": "HALL"
}
```

### 7.2 Status
Status is published as one JSON snapshot instead of many tiny status topics.

Example:

```json
{
  "currentFloor": 3,
  "targetFloor": 5,
  "direction": "UP",
  "state": "MOVING_UP",
  "door": "CLOSED",
  "motor": "RUNNING",
  "queueLength": 2
}
```

### 7.3 Why a snapshot topic?
A single status snapshot is easy to debug in MQTT Explorer, simple to consume later by Node-RED or Three.js, and compact enough for an MVP.

## 8. Update Loop Architecture

The controller runs in a tick-based loop:

```cpp
while (true)
{
    mqtt.Process();
    elevator.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

### Why this approach?
- easy to reason about
- close to embedded firmware style
- avoids unnecessary threads in the MVP
- makes timing behavior predictable

## 9. Folder Structure

```text
MQTTLift/
├── README.md
├── docs/
│   ├── architecture.md
│   ├── state-machines.md
│   └── design-decisions.md
├── src/
│   ├── main.cpp
│   ├── elevator_controller.cpp
│   ├── elevator_controller.hpp
│   ├── mqtt_client.cpp
│   ├── mqtt_client.hpp
│   ├── request_queue.cpp
│   └── request_queue.hpp
├── include/
├── tests/
└── CMakeLists.txt
```

The exact folder names may evolve, but the key principle remains: core logic should be easy to find and separate from platform code.

## 10. Runtime Behavior

### 10.1 Command flow
1. A client publishes a command to `elevator/command/call`.
2. The MQTT client receives it.
3. The controller converts it into a `Request`.
4. The request is added to the FIFO queue.
5. The update loop advances the elevator toward the target floor.
6. The controller publishes a status snapshot on `elevator/status`.

### 10.2 Door flow
1. Elevator reaches target floor.
2. State changes to `DoorOpening`.
3. Door becomes `Open`.
4. A timeout keeps the door open.
5. State changes to `DoorClosing`.
6. Door returns to `Closed`.
7. Controller goes back to `Idle` or processes the next queued request.

## 11. Future Work

Future milestones may add:
- Node-RED dashboard
- Three.js viewer
- SQLite logger
- smarter scheduling
- multiple elevators
- ESP32 port
- safety sensors
- obstacle re-open behavior
- retained status and LWT refinements

These are intentionally left outside the MVP.

## 12. Design Outcome

This architecture is intentionally modest. It is meant to be:
- understandable
- finishable
- easy to extend later
- useful as a GitHub portfolio project
