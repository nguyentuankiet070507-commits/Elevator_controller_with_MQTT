#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include "ElevatorState.hpp"
#include "Direction.hpp"
class Elevator {
    int currentFloor, targetFloor;
    ElevatorState state;
    Direction direction;
    public:

    Elevator();

    void CallFloor(int floor);

    void Update();

    void OpenDoor();

    void CloseDoor();

    void Move();

};

#endif // ELEVATOR_HPP