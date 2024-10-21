#pragma once

typedef enum { vehicle, empty, obstacle } TYPE;

class Cell {
public:
    Cell(int vehicle_speed);
    ~Cell(){};
    TYPE type;
    int vehicle_speed;
    int decceleration;
    short color;
    int ID;
};