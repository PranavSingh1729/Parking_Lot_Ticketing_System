#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/parking.h"


// get next id by checking file
int getID() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return 1;

    Data d;
    int mx = 0;
    while (fread(&d, sizeof(d), 1, fp) == 1) {
        if (d.id > mx) mx = d.id;
    }
    fclose(fp);
    return mx + 1;
}

// calculate fee from in-time, out-time and type
float calc(time_t a, time_t b, int t) {
    double mins = difftime(b, a) / 60.0;
    if (mins <= 30) return 0;      // first 30 min free

    double h = (mins - 30) / 60.0;
    int x = (int)(h + 1);          // round up to next hour

    int rate = (t == 1 ? 10 : 20); // 2W = 10, 4W = 20
    float total = x * rate;
    if (total < 10) total = 10;    // minimum 10
    return total;
}

// entry of vehicle
void in() {
    FILE *fp = fopen(DATA_FILE, "ab");
    if (!fp) {
        printf("file err\n");
        return;
    }

    Data d;
    d.id = getID();
    printf("\nNumber Plate: ");
    scanf("%19s", d.num);
    printf("Type 1=2W 2=4W: ");
    scanf("%d", &d.type);

    d.in = time(NULL);
    d.out = 0;
    d.fee = 0;
    d.status = 1;     // parked

    fwrite(&d, sizeof(d), 1, fp);
    fclose(fp);

    printf("\nTicket %d added!\n", d.id);
}

// exit of vehicle
void outVehicle() {
    FILE *fp = fopen(DATA_FILE, "rb+");
    if (!fp) {
        printf("No data.\n");
        return;
    }

    char p[20];
    printf("\nEnter Plate: ");
    scanf("%19s", p);

    Data d;
    long pos;
    int f = 0;

    while ((pos = ftell(fp)), fread(&d, sizeof(d), 1, fp) == 1) {
        if (strcmp(d.num, p) == 0 && d.status == 1) {
            f = 1;
            break;
        }
    }
    if (!f) {
        printf("Not inside.\n");
        fclose(fp);
        return;
    }

    d.out = time(NULL);
    d.fee = calc(d.in, d.out, d.type);
    d.status = 2;

    fseek(fp, pos, SEEK_SET);
    fwrite(&d, sizeof(d), 1, fp);
    fclose(fp);

    double tm = difftime(d.out, d.in) / 60;
    printf("\nExit Done.\nTicket:%d Fee:Rs%.2f Time:%.1f mins\n",
           d.id, d.fee, tm);
}

// show all parked vehicles
void show() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        printf("No records.\n");
        return;
    }

    Data d;
    int c = 0;
    printf("\nInside Vehicles:\n");
    while (fread(&d, sizeof(d), 1, fp) == 1) {
        if (d.status == 1) {
            printf("%d  %s\n", d.id, d.num);
            c = 1;
        }
    }
    if (!c) printf("Empty.\n");
    fclose(fp);
}

// daily report for a date
void report() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) {
        printf("No file.\n");
        return;
    }

    int dd, mm, yy;
    printf("\nDate(dd mm yyyy): ");
    scanf("%d%d%d", &dd, &mm, &yy);

    Data d;
    int c2 = 0, c4 = 0;
    float sum = 0;

    while (fread(&d, sizeof(d), 1, fp) == 1) {
        if (d.status == 2) {
            struct tm *t = localtime(&d.out);
            if (t->tm_mday == dd && t->tm_mon + 1 == mm &&
                t->tm_year + 1900 == yy) {
                sum += d.fee;
                if (d.type == 1) c2++;
                else c4++;
            }
        }
    }
    fclose(fp);
    printf("\n2W:%d   4W:%d   Total:Rs%.2f\n", c2, c4, sum);
}

// main menu
int main() {
    int ch;
    while (1) {
        printf("\n=== Parking System ===\n1 Entry\n2 Exit\n3 List\n4 Report\n5 Quit\n> ");
        if (scanf("%d", &ch) != 1) break;

        if (ch == 1) in();
        else if (ch == 2) outVehicle();
        else if (ch == 3) show();
        else if (ch == 4) report();
        else if (ch == 5) break;
        else printf("Invalid.\n");
    }
    return 0;
}
