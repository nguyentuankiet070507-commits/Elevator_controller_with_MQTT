# MQTTLift Design Decisions

This file records the decisions that have been intentionally locked for the MVP. These are the decisions the code and the documentation should follow unless there is a strong reason to revisit them.

## DD-001 — MVP scope

**Decision:** The MVP focuses on the controller and MQTT only.

**Why:** This keeps the first version finishable and helps the project stay centered on IoT and networking fundamentals.

**Deferred to future work:** dashboard, 3D viewer, SQLite logger, ESP32 port, multi-elevator scheduling.

---

## DD-002 — Controller platform

**Decision:** The controller is written in C++17 and runs on PC first.

**Why:** This keeps the project close to embedded-style logic while avoiding hardware friction early on.

**Deferred to future work:** porting to ESP32 or other hardware.

---

## DD-003 — Runtime model

**Decision:** The controller uses a tick-based `Update()` loop.

**Why:** This is simple, predictable, and close to how embedded firmware is often structured.

**Alternative considered:** blocking calls or thread-heavy design.

---

## DD-004 — Main domain object

**Decision:** `Request` is a domain object, not just a floor number.

```cpp
struct Request
{
    int floor;
    Direction direction;
    RequestSource source;
};
```

**Why:** This keeps the model realistic enough to be useful later without adding too much complexity.

---

## DD-005 — Request source model

**Decision:** Requests have a source: `Hall` or `Cabin`.

**Why:** This is more expressive than a plain floor value and matches the basic real-world model of elevator requests.

---

## DD-006 — Queue policy

**Decision:** The MVP uses FIFO scheduling.

**Why:** FIFO is easy to understand and keeps the project focused on architecture and MQTT rather than scheduling research.

**Alternative considered:** directional or smart scheduling.

---

## DD-007 — Door behavior

**Decision:** The door follows a timed cycle: `Opening -> Open -> Closing`.

**Why:** This is enough to demonstrate system behavior and state transitions without adding sensor complexity.

**Deferred to future work:** obstacle detection, hold door, auto reopen.

---

## DD-008 — Arrived is an event, not a state

**Decision:** `Arrived` is handled as an event inside `Update()`.

**Why:** It is a short-lived condition that does not need its own state.

---

## DD-009 — Topic root

**Decision:** The MQTT topic root is:

```text
elevator/
```

**Why:** It is simple, readable, and enough for the MVP.

**Alternative considered:** `building/elevator/1/`.

---

## DD-010 — Command topics

**Decision:** Commands are split into separate topics:

```text
elevator/command/call
elevator/command/emergency
elevator/command/reset
```

**Why:** Different actions are easier to understand when separated by topic.

---

## DD-011 — Status payload

**Decision:** Status is published as one JSON snapshot on `elevator/status`.

**Why:** One snapshot is easier to debug, easier to consume later, and fits the MVP well.

**Alternative considered:** one topic per status field.

---

## DD-012 — Logger topic reserved

**Decision:** `elevator/log` is reserved in the architecture.

**Why:** It lets a logger be added later without changing the controller design.

**Current status:** not implemented in MVP.

---

## DD-013 — External UI tools

**Decision:** Node-RED, Three.js, and SQLite are future milestones, not MVP requirements.

**Why:** They are useful, but they would distract from the core learning goal if added too early.

---

## DD-014 — Design philosophy

**Decision:** Keep the project small, clean, and finishable.

**Why:** The goal is to complete a working project and learn the full workflow, not to create a highly optimized industrial system.

---

## DD-015 — Change control

**Decision:** Once a design decision is locked, it should not be changed casually.

**Why:** Stable design reduces rework and keeps the documentation aligned with the code.

**Allowed reasons to revisit:** bug, implementation failure, or a real new requirement.

---

## DD-016 — Future work policy

**Decision:** Advanced ideas are recorded as future work instead of being added to the MVP.

**Why:** This keeps the repository focused and protects the completion path.

**Examples:** smart scheduler, multi-elevator dispatch, ESP32 port, obstacle sensors, retained status refinement, LWT tuning.
