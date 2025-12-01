#ifndef PARKING_H
#define PARKING_H

#include <time.h>

#define DATA_FILE "park.dat"

typedef struct {
    int id;
    char num[20];
    int type;      // 1=2W, 2=4W
    time_t in, out;
    float fee;
    int status;    // 1=in, 2=out
} Data;

int getID();
float calc(time_t a, time_t b, int t);
void in();            // vehicle entry
void outVehicle();    // vehicle exit
void show();          // show parked list
void report();        // daily report

#endif
