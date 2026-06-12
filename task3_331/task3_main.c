#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "flight.h"

/*
* =====================================clearInputBuffer=======================================
* simply clears the input buffer to remove leftover characters, it's used after scanf.
*/
void clearInputBuffer(){
    int c;
    c = getchar();
    while (c != '\n' && c != EOF){
        c = getchar();
    }
}

/*
* =======================================toLowerCase===========================================
* Converts all characters in a string to lower case. 
* Used so that user input is not case sensitive.
*/
void toLowerCase(char* str){
    while (*str){
        *str = tolower((unsigned char)*str);
        str++;
    }
}

/*
* Main contains a simple menu-driven interface, where the user can chose to add, retrieve, modify and delete flight and passenger data.
*Values entered by the user are used as parameters to functions implemented in fligth.c
*
*/

int main(int argc, char *argv[]){
    Flight * flightList = NULL;
    int choice;
    do{
        printf("==== FLIGHT PROGRAM ====\n");
        printf("1. Add flight\n");
        printf("2. Add passenger\n");
        printf("3. Fetch flight\n");
        printf("4. Find flight by destination\n");
        printf("5. Delete flight\n");
        printf("6. Change seat for passenger\n");
        printf("7. Search for passenger by name\n");
        printf("8. Find passengers on several flights\n");
        printf("0. Quit\n");
        printf("Enter number:");
        
        scanf("%d", &choice);
        clearInputBuffer();
        if(choice == 1){
            //Add Flight
            char id[10], dest[100];
            int seats, time;
            printf("FlightID: ");
            fgets(id, sizeof(id),stdin);
            toLowerCase(id);
            printf("Destination: "); 
            fgets(dest, sizeof(dest), stdin);
            toLowerCase(dest);
            printf("Number of seats: "); 
            scanf("%d", &seats);
            clearInputBuffer();
            printf("Time of departure(e.g 1600): "); 
            scanf("%d", &time);
            clearInputBuffer();
            id[strcspn(id, "\n")] = '\0';
            dest[strcspn(dest, "\n")] = '\0';
            addFlight(&flightList, id, dest, seats, time);
            printf("Added flight: %s to %s \n", id, dest);
        }
        else if(choice == 2){
            //Add passenger
            int n, seat, age;
            char name[100];
            printf("Enter index of flight: "); 
            scanf("%d", &n);
            clearInputBuffer();
            Flight* f = getFlightByIndex(flightList, n);
            if(!f){
                printf("No Flights at index %s\n", n);
                continue;
            }
            printf("Seatnumber: ");
            scanf("%d", &seat);
            clearInputBuffer();
            printf("Name: "); 
            fgets(name, sizeof(name), stdin);
            toLowerCase(name);
            printf("Age: "); scanf("%d", &age);
            name[strcspn(name, "\n")] = '\0';
            addPassenger(f, seat, name, age);
        }
        else if(choice == 3){
            //Find flight by index
            int n;
            printf("Flight num: "); 
            scanf("%d", &n);
            clearInputBuffer();
            Flight* f = getFlightByIndex(flightList, n);
            if(f){
                printFlightDetails(f);
            }else{
                printf("Could not find flight\n");
            }
        }
        
        else if(choice == 4){
            //Find flight by destination
            char dest[100];
            printf("Destination: "); 
            fgets(dest, sizeof(dest), stdin);
            toLowerCase(dest);
            dest[strcspn(dest,"\n")] = '\0';
            findFlightByDestination(flightList, dest);
        }
        else if(choice == 5){
            //Delete flight 
            int n;
            printf("Enter index of flight to delete: "); 
            scanf("%d", &n);
            clearInputBuffer();
            deleteFlight(&flightList, n);
        }
        else if(choice == 6){
            //Change seat
            int n, newSeat;
            char name[100];
            printf("Enter index number of flight to change seats: "); 
            scanf("%d", &n);
            clearInputBuffer();
            Flight* f = getFlightByIndex(flightList, n);
            if(!f){
                printf("Illegal flight\n");
                continue;
            }
            printf("Name: ");
            fgets(name, sizeof(name), stdin);
            toLowerCase(name);
            name[strcspn(name, "\n")] = '\0';
            printf("new seat: "); 
            scanf("%d", &newSeat);
            clearInputBuffer();
            changeSeat(f, name, newSeat);
        }
        else if(choice == 7){
            //Search for passenger
            char name[100];
            printf("Name: "); 
            fgets(name, sizeof(name), stdin);
            toLowerCase(name);
            name[strcspn(name, "\n")] = '\0';
            searchPassengerByName(flightList, name);
        }
        else if(choice == 8){
            //find passengers on multiple flights
            findPassengerOnMultipleFlights(flightList);
        }
        else{
            printf("Illegal choice\n");
        }
        
       } while (choice != 0);
  
    return 0;
}
