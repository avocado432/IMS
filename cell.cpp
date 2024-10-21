#include "cell.h"
#include <stdlib.h>
#include <curses.h>
#include <time.h>

short choose_color() {
    int num = rand()%6+1;
    switch(num){
        case 0:
            return COLOR_RED;
            break;
        case 1:
            return COLOR_WHITE;
            break;
        case 2:
            return COLOR_YELLOW;
            break;
        case 3:
            return COLOR_BLUE;
            break;
        case 4:
            return COLOR_MAGENTA;
            break;
        case 5:
            return COLOR_CYAN;
            break;
    }
    return COLOR_WHITE;
}

Cell::Cell(int vehicle_speed) {
    this->type = empty;
    this->vehicle_speed = vehicle_speed;
    this->decceleration = 0;
    this->color = choose_color();
    this->ID = 0;
}