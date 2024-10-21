/*
Author: Tereza Lapčíková, xlapci03
*/

#include <vector>
#include <iostream>
#include <fstream>
#include "cell.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <limits.h>
#include <csignal>

#define LOOKAHEAD_DISTANCE 7
#define MAX_SPEED 10
#define OVERBREAKING 1.2
#define ACCELERATE 1
#define SLOW 2
#define VEHICLE_SPACING 7
#define MATRIX_WIDTH 150
#define OBSTACLE 0 // whether matrix contains obstacle
#define OBSTACLE_POSITION 75 // position of obstacle in matrix
#define OBSTACLE_TIME 40 // time duration for which the obstacle is to be removed
#define PROBABILITY 25 // probability, that vehicle suddenly slows down

int cell_counter = 0;
int time_tick = 0;

// function generating vehicles
Cell generate_car() {
    Cell cell = Cell(rand()%MAX_SPEED+1);
    cell.type = vehicle;
    cell.ID = cell_counter;
    return cell;
}

// initialization of matrix
void init_matrix(std::vector<Cell> *matrix) {
    int init_speed = 1;
    for (int i = 0; i < MATRIX_WIDTH; i++) {
        Cell cell = Cell(init_speed);
        if (i%VEHICLE_SPACING == 0) {
            cell = generate_car();
            cell_counter++;
        }
        matrix->push_back(cell);
    }
    if (OBSTACLE) {
        (*matrix)[OBSTACLE_POSITION].type = obstacle;
    }
}

// function updating matrix with each time tick
void update_matrix(std::vector<Cell> *matrix, std::vector<Cell> *oldmatrix) {
    *oldmatrix = *matrix;
    if (time_tick == OBSTACLE_TIME && OBSTACLE) {
        (*matrix)[OBSTACLE_POSITION].type = empty;
    }
    for (int j = MATRIX_WIDTH-1; j >= 0; j--) {
        bool is_object_infront = false;
        Cell *cell = &(*matrix)[j];
        int old_speed = cell->vehicle_speed;
        if (cell->type == vehicle) {
            // looking ahead whether there is an object (obstacle or another vehicle) infront in specified distance
            for (int next_car = j+1; next_car <= (j+cell->vehicle_speed+LOOKAHEAD_DISTANCE); next_car++) {
                int distance = next_car-j;
                Cell *object_infront = &(*matrix)[next_car%MATRIX_WIDTH];
                if (object_infront->type == vehicle || object_infront->type == obstacle) {
                    is_object_infront = true;
                    // vehicle deccelerates in dependce on the speed of car infront
                    int overbreaking = int(object_infront->decceleration * OVERBREAKING);
                    // vehicle slows down to avoid crash
                    if (distance <= cell->vehicle_speed) {
                        cell->vehicle_speed = distance-1-overbreaking;
                    }
                    // vehicle slows down as a reaction to car in lookahead distance, even though crash is not eminent
                    else {
                        cell->vehicle_speed = cell->vehicle_speed-overbreaking;
                    }
                    // vehicle speed can't be negative
                    if (cell->vehicle_speed < 0) {
                        cell->vehicle_speed = 0;
                    }
                    break;
                }
            }
            // vehicle can accelerate if there is no object infront and has not reached the maximum speed
            if (is_object_infront == false && cell->vehicle_speed < MAX_SPEED) {
                cell->vehicle_speed += ACCELERATE;
                if (cell->vehicle_speed > MAX_SPEED) {
                    cell->vehicle_speed = MAX_SPEED;
                }
            }
            // there is a certain probability, that vehicle slows down randomly
            int value = rand()%100+1;
            if (value > 100-PROBABILITY && (cell->vehicle_speed-SLOW > 0)) {
                cell->vehicle_speed -= SLOW;
            }
            cell->decceleration = old_speed - cell->vehicle_speed;
            if (cell->decceleration < 0) {
                cell->decceleration = 0;
            }
            // actual movement of the vehicle
            if(cell->vehicle_speed == 0 ) continue; // No need to change places
            Cell empty_cell = (*matrix)[(j+cell->vehicle_speed)%MATRIX_WIDTH];
            (*matrix)[(j+cell->vehicle_speed)%MATRIX_WIDTH] = (*matrix)[j];
            (*matrix)[j] = empty_cell;
        }
    }
}

// printing the matrix using ncurses
void print_matrix(std::vector<Cell> matrix) {
    clear();
    init_pair(66, COLOR_BLACK, COLOR_BLACK);
    init_pair(31, COLOR_WHITE, COLOR_BLACK);
        for (int j = 0; j < MATRIX_WIDTH; j++) {
            Cell current_cell = matrix[j];
            init_pair(current_cell.color, COLOR_BLACK, current_cell.color);
            if (current_cell.type == empty) {
                attron(COLOR_PAIR(66));
                printw("   ");
                attroff(COLOR_PAIR(66));
            }
            else if (current_cell.type == obstacle) {
                attron(COLOR_PAIR(31));
                printw(" X ");
                attroff(COLOR_PAIR(31));
            }
            else{
                attron(COLOR_PAIR(current_cell.color));
                printw("%3d", current_cell.ID);
                attroff(COLOR_PAIR(current_cell.color));
            }
        }
    printw("\n");
    printw("Time tick: %d\n", time_tick);
    printw("Overbreaking: %.1f\n", OVERBREAKING);
    printw("Lookahead distance: %d\n", LOOKAHEAD_DISTANCE);
    usleep(50000);
    refresh();
}

// handling signal interrupts
void signal_handler(int signum) {
    endwin();
    exit(0);
}

int main(int argc, char **argv) {
    std::vector<Cell> matrix;
    std::vector<Cell> oldmatrix;

    signal(SIGINT, signal_handler);

    srand(time(0));
    initscr();
    start_color();

    init_matrix(&matrix);
    print_matrix(matrix);

    while(time_tick < INT_MAX) {
        time_tick++;
        update_matrix(&matrix, &oldmatrix);
        print_matrix(matrix);
    }

    endwin();
    return 0;
}