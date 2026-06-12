#ifndef FLIGHT_H
#define FLIGHT_H

#include <stdio.h>

typedef struct Passenger {
    int seatNumber;
    char name[100];
    int age;
    struct Passenger* next;
} Passenger;

typedef struct Flight{
    char flightID[10];
    char destination[100];
    int seats;
    int time;
    Passenger* passengerList;
    struct Flight* prev;
    struct Flight* next;
} Flight;

Flight* addFlight(Flight** head, const char* id, const char* dest, int seats, int time);
int addPassenger(Flight* flight, int seatNumber, const char* name, int age);

Flight* getFlightByIndex(Flight* head, int index);
void printFlightDetails(Flight* flight);
int findFlightByDestination(Flight* head, const char* destination);

void deleteFlight(Flight** head, int index);
int changeSeat(Flight* flight, const char* name, int newSeatNumber);

void searchPassengerByName(Flight* head, const char* name);
void findPassengerOnMultipleFlights(Flight* head);

void freeAll(Flight* head);

#endif // FLIGHT_H
