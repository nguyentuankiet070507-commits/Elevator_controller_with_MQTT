# MQTTLift State Machines

## 1. Purpose

This document defines the runtime behavior of the MVP. It focuses on what states exist, how transitions happen, and what is treated as an event instead of a state.

## 2. Core Rules

- The controller uses a tick-based update loop.
- State transitions should be simple and deterministic.
- A short-lived condition is usually an event, not a state.
- The MVP uses FIFO scheduling.
- The door follows a timed open-close cycle.
- No smart scheduler is used in the MVP.

## 3. Elevator State Machine

### 3.1 States
- `Idle`
- `MovingUp`
- `MovingDown`
- `DoorOpening`
- `DoorOpen`
- `DoorClosing`
- `Alarm`

### 3.2 Main transitions

```text
Idle
  -> MovingUp      when a request floor is above current floor
  -> MovingDown    when a request floor is below current floor

MovingUp
  -> DoorOpening   when currentFloor == targetFloor
  -> Alarm         on unrecoverable error

MovingDown
  -> DoorOpening   when currentFloor == targetFloor
  -> Alarm         on unrecoverable error

DoorOpening
  -> DoorOpen      when opening is complete

DoorOpen
  -> DoorClosing   when open timeout expires

DoorClosing
  -> Idle          when door is fully closed and queue is empty
  -> MovingUp      when next request is above current floor
  -> MovingDown    when next request is below current floor

Alarm
  -> Idle          when reset command is received
```

### 3.3 Event vs state
`Arrived` is not a state. It is an event that occurs inside `Update()` when the current floor matches the target floor.

Example:

```cpp
if (currentFloor == targetFloor)
{
    // Arrived event
    state = ElevatorState::DoorOpening;
}
```

## 4. Door State Machine

### 4.1 States
- `Closed`
- `Opening`
- `Open`
- `Closing`

### 4.2 Main transitions

```text
Closed
  -> Opening     when the elevator reaches the target floor

Opening
  -> Open        when the opening action is complete

Open
  -> Closing     when the open timeout expires

Closing
  -> Closed      when closing is complete
```

### 4.3 MVP door timing
The door stays open for a fixed timeout.

Recommended default:
- open duration: 3 seconds

### 4.4 Why no obstacle handling in MVP?
Obstacle detection, hold-door behavior, and automatic reopen logic are realistic features, but they add extra complexity. They are better treated as future work.

## 5. Motor State Machine

The motor is intentionally simple in the MVP.

### 5.1 States
- `Stopped`
- `Running`

### 5.2 Behavior
- `Stopped` when the elevator is idle or the door is open
- `Running` while the elevator is moving between floors

### 5.3 Transition rules
```text
Stopped
  -> Running   when an active request is being served

Running
  -> Stopped   when the target floor is reached
```

## 6. Request Queue Behavior

### 6.1 Queue policy
The MVP uses FIFO.

### 6.2 Behavior
Requests are handled in the order they arrive.

Example:

```text
Queue:
5
2
4
```

The controller serves:
- 5
- 2
- 4

### 6.3 Why FIFO?
FIFO is simple, easy to explain, and good enough for the MVP. It keeps the project focused on architecture and MQTT rather than scheduling optimization.

## 7. Command Behavior

### 7.1 Call command
A `call` command creates a request.

Example payload:

```json
{
  "floor": 5,
  "direction": "UP",
  "source": "HALL"
}
```

### 7.2 Emergency command
Emergency immediately puts the elevator into `Alarm`.

### 7.3 Reset command
Reset clears alarm state and returns the system to `Idle` if the system is safe to recover.

## 8. Update Loop Behavior

Each tick performs a small step.

Example:

1. Process MQTT input
2. If there is no active request, stay idle
3. If there is an active request, move one step toward it
4. Update door state if needed
5. Publish status snapshot

This keeps the logic predictable and easy to debug.

## 9. Timing Rules

### 9.1 Floor movement
Movement between floors is simulated with a fixed tick progression.

### 9.2 Door open duration
The door remains open for a fixed timeout:
- 3 seconds by default

### 9.3 Tick interval
The controller tick interval is a small fixed delay such as 100 ms.

## 10. Alarm Behavior

### 10.1 Alarm entry
The system may enter `Alarm` on:
- emergency command
- unrecoverable controller failure
- future fault conditions

### 10.2 Alarm exit
The MVP leaves `Alarm` only after a reset command.

## 11. State Summary

### Elevator
```text
Idle -> MovingUp / MovingDown -> DoorOpening -> DoorOpen -> DoorClosing -> Idle
```

### Door
```text
Closed -> Opening -> Open -> Closing -> Closed
```

### Motor
```text
Stopped -> Running -> Stopped
```

## 12. Scope Reminder

This document reflects the MVP only. More advanced behaviors such as obstacle detection, directional scheduling, multi-elevator dispatch, or hardware porting are intentionally excluded for now.
