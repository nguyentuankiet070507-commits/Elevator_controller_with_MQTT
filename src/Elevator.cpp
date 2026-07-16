#include "../include/Elevator.hpp"

class Elevator {
    int currentFloor, targetFloor;
    ElevatorState state;
    Direction direction;
    public:

    Elevator();

    void CallFloor(int floor) {
        targetFloor = floor;
    }

    void Update() {
        if (currentFloor < targetFloor) {
            state = ElevatorState::MovingUp;
        } else if (currentFloor > targetFloor) {
            state = ElevatorState::MovingDown;
        }
    };

    void OpenDoor() {
        state = ElevatorState::DoorOpening;
    };

    void CloseDoor() {
        state = ElevatorState::DoorClosing;
    };

    void Move() {
        if (state == ElevatorState::MovingUp) {
            currentFloor++;
        } else if (state == ElevatorState::MovingDown) {
            currentFloor--;
        }
    };

};