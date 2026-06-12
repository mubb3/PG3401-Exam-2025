#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flight.h"


// Struct and definition used in findPassengerOnMultipleFlights. Not entirely sure if this could be used inside the function itself, instead of as global values.
#define MAX_PASSENGERS 1000
typedef struct{
    char name[100];
    int count;
} NameCounter;

/*
*-----------------------------------------addFlight------------------------------------------
* addFlights creates a new flight and ads it at the end of the doublylinked list for flights.
* It takes a pointer to the head pointer in the list, as well as information regarding the flight 
* The function allocates memory for the new flight and copies the values and links to the set structure. 
* If the list is empty, the new flight will be the first element of the list.
*---------------------------------------------------------------------------------------------
*/

Flight* addFlight(Flight** head, const char* id, const char *dest, int seats, int time){
    Flight* newFlight = (Flight*)malloc(sizeof(Flight));
    if( !newFlight){
        printf("Could not allocate memory\n");
        return NULL;
    }
    
    strncpy(newFlight->flightID, id, sizeof(newFlight->flightID));
    newFlight->flightID[sizeof(newFlight->flightID)-1] = '\0';
    
    strncpy(newFlight->destination, dest, sizeof(newFlight->destination));
    newFlight->destination[sizeof(newFlight->destination)-1] = '\0';
    
    newFlight->seats = seats;
    newFlight->time = time;
    newFlight->passengerList = NULL;
    newFlight->next = NULL;
    newFlight->prev = NULL;
    
    if(*head == NULL){
        *head = newFlight;
    }else{
        Flight* current = *head;
        while (current->next != NULL){
            current = current->next;
        }
        current->next = newFlight;
        newFlight->prev = current;
    }
    return newFlight;
}; // addFlight


/*
*------------------------------addPassenger-----------------------------
* addPassenger adds a passenger with name and age to a spesific flight.
* The passenger is added to the list of passengers.
* If the flight dont excists, the function returns -1.
*-----------------------------------------------------------------------
*/
int addPassenger(Flight* flight, int seatNumber, const char* name, int age){
    if(!flight) return -1;
    
    Passenger* current = flight->passengerList;
    Passenger* previous = NULL;
    
    while (current && current -> seatNumber < seatNumber){
        previous = current;
        current = current->next;
    }
    
    if (current && current -> seatNumber == seatNumber){
        printf("Seat %d already reserved.\n", seatNumber);
        return 0;
    }
    
    Passenger* newPassenger =(Passenger*)malloc(sizeof(Passenger));
    if(!newPassenger){
        printf("Could not reserve memory for passenger\n");
        return -1;
    }
    
    newPassenger->seatNumber = seatNumber;
    strncpy(newPassenger->name, name, sizeof(newPassenger->name));
    newPassenger->name[sizeof(newPassenger-> name)-1] = '\0';
    newPassenger->age = age;
    newPassenger->next = current;
    
    if(previous == NULL){
        flight->passengerList = newPassenger;
    }else{
        previous->next = newPassenger;
    }
    return 1;
}; // addPassenger

/*
* ---------------------------------------getFlightByIndex--------------------------------
* Returns a pointer to the flight at a given index in the flight list
* The list is traverses sequentially from the start
* If the index is out of bounds or the list is empty, NULL is returned
*----------------------------------------------------------------------------------------
*/

Flight* getFlightByIndex(Flight* head, int index){
    if(index < 1) return NULL;
    
    Flight* current = head;
    int count = 1;
    
    while(current != NULL && count < index){
        current = current -> next;
        count++;
    }
    
    if (count == index && current != NULL){
        return current;
    }else{
        return NULL;
    }
}; // getFlightByIndex

/*
* --------------------------------------printFlightDetails-------------------------------------
* Prints all relevant information about a given flight, as well as a list of all registered passengers. 
* If the flight pointer is NULL, the function does nothing.
*----------------------------------------------------------------------------------------------
*/

void printFlightDetails(Flight* flight){
    if(!flight){
        printf("No flight found\n");
        return;
    }
    
    printf("\n===== Flight Information =====\n");
    printf("FlightID: %s\n", flight->flightID);
    printf("Destination: %s\n", flight->destination);
    printf("Seat: %d\n", flight->seats);
    printf("Time of Departure: %d\n", flight->time);
    
    Passenger* p = flight->passengerList;
    if(!p){
        printf("No passengers registered\n");
    }else{
        printf("--- Passenger List ---\n");
        while(p != NULL){
            printf("Seat %d: %s, age%d \n", p->seatNumber, p->name,p->age);
            p = p->next;
        }
    }
    printf("================================\n");
} //printFlightDetails

/*
* ==========================================findFlightByDestination=============================
* Searches through the flight list for flights matching the given destination.
* for each match, it prints the full flight details.
* If no matches are found, a message is displayed.
*/
int findFlightByDestination(Flight* head, const char* destination){
    int index = 1;
    int found = 0;
    Flight* current = head;
    
    while(current != NULL){
        if(strcmp(current->destination, destination) == 0){
            printf("\nMatch #%d:\n", found + 1);
            printFlightDetails(current);
            found++;
        }
        current = current ->next;
        index++;
    }
    if(!found){
        printf("No flights to %s found \n", destination);
    }
    return found;
};// findFlightByDestination

/*
* =====================================deleteFlight=======================================
* Removes a spesified fligth from the doubly linked flight list and frees all associated memory
* This includes deallocating the memory for all passengers in the flights passenger list.
* Properly updates the previous and next pointers of surrounding nodes in the list.
* If the fligth to delete is the head of the list, the head pointer is updated accordingly.
* If the flight pointer is NULL, the function does nothing.
*=========================================================================================
*/
void deleteFlight(Flight** head, int index){
    if(!head || !*head || index < 1) return;
    
    Flight* current = *head;
    int count = 1;
    
    while (current && count < index){
        current = current -> next;
        count++;
    }
    
    if(!current){
        printf("found no planes at index %d\n", index);
        return;
    }
    
    Passenger* p = current -> passengerList;
    while (p){
        Passenger* next = p->next;
        free(p);
        p = next;
    }
    
    if(current->prev){
        current->prev->next = current->next;
    }else{
        *head = current->next;
    }
    
    if(current->next){
        current->next->prev = current->prev;
    }
    
    free(current);
    printf("Plane at index %d is deleted\n", index);
}; //deleteFlight

/* ==================================changeSeat===========================================
* Changes the seat number of a passenger within a given fligth. The function searches for the
* passenger by name in the passenger list. If a passenger is found the seat number is updated, while maintaning the sorted order (by seat). This involves removing the passenger node from its current position and reinserting it at the correct position in the list
* returns 1 if the seat change was successfull, or -1 if the passenger was not found.
*=========================================================================================
*/
int changeSeat(Flight* flight, const char* name, int newSeatNumber){
    if(!flight) return -1;
    
    Passenger * current = flight->passengerList;
    Passenger * previous = NULL;
    Passenger * target = NULL;
    // locate passanger and check if new seat is taken
    while(current){
        if (current->seatNumber == newSeatNumber){
            printf("Seat %d is allready reserved\n", newSeatNumber);
            return 0;
        }
        if(strcmp(current->name,name) == 0){
            target = current;
            break;
        }
        previous = current;
        current = current->next;
    }
    
    if(!target){
        printf("Passenger %s could not be found", name);
        return 0;
    }
    // delete passenger from current place
    if(previous == NULL){
        flight->passengerList = target->next;
    }else{
        previous->next = target->next;
    }
    
    //allocate new seatnumber and sort seating
    target->seatNumber = newSeatNumber;
    
    Passenger *insertPrev = NULL;
    Passenger *insertCurr = flight->passengerList;
    
    while(insertCurr && insertCurr->seatNumber < newSeatNumber){
        insertPrev = insertCurr;
        insertCurr = insertCurr->next;
    }
    
    target->next = insertCurr;
    
    if(insertPrev == NULL){
        flight->passengerList = target;
    }else{
        insertPrev->next = target;
    }
    return 1;
};
/*
*===================================searchPassengerByName===================================
* Searches through all flights and the passengerlist for a passenger with a given name.
* For each flight where the passenger is found, the function prints the flight id and passenger details.
* if the passenger is not found in any flight, the user is displayed a simple "not found" message.
*===========================================================================================
*/

void searchPassengerByName(Flight * head, const char* name){
    int found = 0;
    int flightNum = 1;
    Flight * currentFlight = head;
    
    while (currentFlight){
        Passenger* p = currentFlight->passengerList;
        while (p){
            if(strcmp(p->name,name) == 0){
                printf("Passanger %s found at spot %d in flightlist,flight %s headed to %s, seat %d\n", name, flightNum, currentFlight->flightID, currentFlight->destination, p->seatNumber);
                found = 1;
            }
            p = p->next;
        }
        currentFlight = currentFlight->next;
        flightNum++;
    }
    if (!found){
        printf("Passenger %s was not found\n", name);
    }
}// searchPassengerByName

/*
* ========================================findPassengerOnMultipleFlights===========================
*iterates through all flights and counts how many times each passenger name appears across flights.
* Uses a temporary array to track unique names and their occurance count.
* =================================================================================================
*/

void findPassengerOnMultipleFlights(Flight* head){
    NameCounter counters[MAX_PASSENGERS];
    int uniqueNames = 0;
    
    Flight* currentFlight = head;
    
    while(currentFlight){
        Passenger* p = currentFlight->passengerList;
        while(p){
            int found = 0;
            for(int i = 0; i> uniqueNames; i++){
                if(strcmp(counters[i].name, p->name) == 0){
                    counters[i].count++;
                    found = 1;
                    break;
                }
            }
            if(!found && uniqueNames < MAX_PASSENGERS){
                strncpy(counters[uniqueNames].name, p->name, sizeof(counters[uniqueNames].name));
                counters[uniqueNames].name[sizeof(counters[uniqueNames].name) - 1] = '\0';
                counters[uniqueNames].count = 1;
                uniqueNames++;
            }
            p = p->next;
        }
        currentFlight = currentFlight->next;
    }
    
    printf("Passenger registered on several flights:\n");
    int any = 0;
    for(int i = 0; i < uniqueNames; i++){
        if(counters[i].count > 1){
            printf("- %s (%d flights)\n", counters[i].name, counters[i].count);
            any = 1;
        }
    }
    if (!any){
        printf("No passenger found on more than 1 plane\n");
    }
} //findPassengerOnMultipleFlights

