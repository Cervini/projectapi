#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//= STRUCTURES =========================================================================

typedef struct vehicle{
	struct vehicle* next;
	struct vehicle* previous;
	int value;
}vehicle;

typedef struct stop{
  int distance;
  struct vehicle* vehicles;
  struct stop* next;
  struct stop* previous;
}stop;

//= GLOBAL VARIABLES ====================================================================

struct stop* stops = NULL;

//= METHODS =============================================================================

//adds a new stop with the defined distance
void addStop(int distance){
	//check if the stops list is empty
	struct stop* temp = (struct stop*)malloc(sizeof(struct stop));//allocate in memory the space for a stop struct
	temp->distance = distance; //distance from the start (from input)
	temp->vehicles = NULL;
	temp->next = NULL; //when added the station is the last
	if(!stops){
		temp->previous = NULL;
		stops = temp;
		return;
	}
	struct stop* t = stops;
	while(t->next != NULL){
	//go to the end of the list
	t = t->next;
	}
	temp->previous = t; //set the last stop as the previous of the newly added item
	t->next = temp; //set the newly added item as the last
}

//returns the stop with the defined distance
struct stop* getStop(int distance){
	struct stop* t = stops; //get the beginning of the stops list
	do{
		if(t->distance==distance)
			//if the distance is the wanted one return the stop
			return t;
		t = t->next; //go to the next stop
	}while(t);
	//if the wanted stop doesn't exist return null
	return NULL;
}

//adds a new vehicle to the stop with the determined distance
void addVehicle(int distance, int value){
	struct stop* stop = getStop(distance);
	struct vehicle* temp = (struct vehicle*)malloc(sizeof(struct vehicle));//allocate in memory the space for a vehicle struct
	temp->value = value;
	temp->next = stop->vehicles; //set the vehicles list following the temp variable
	temp->previous = NULL;
	if(stop->vehicles)
		temp->next->previous = temp;
	stop->vehicles = temp; //set the temp variable as the vehicles list starting point
}

void deleteStop(int distance){
	struct stop* stop = getStop(distance);
	if(stop == NULL){
		printf("non demolita\n");
	} else {
		if(stop->previous){
			stop->previous->next = stop->next;
		} else {
			stops = stop->next;
		}
		printf("demolita\n");
	}
}

void deleteVehicle(int distance, int number){
	struct stop* stop = getStop(distance);
	if(stop != NULL){
		struct vehicle* t = stop->vehicles;
		do{
			if(t->value == number){
				if(t->previous){
					t->previous->next = t->next;
				} else {
					stop->vehicles = t->next;
				}
				break;
			} else {
				t = t->next;
			}
		}while(t);
		printf("rottamata\n");
	} else {
		printf("non rottamata\n");
	}
}

//this method prints the values of a list of vehicles
void printVehicles(int distance){
	struct stop* stop = getStop(distance);
	if(!stop->vehicles){
		printf("\n");
		return;
	}
	struct vehicle* t = stop->vehicles;
	do{
		printf(" %d", t->value);
		t= t->next;
	}while(t);
	printf("\n");
}

//this method prints the distances of a list of stops
void printDistances(){
	if(!stops){
		printf("empty list\n");
		return;
	}
	printf("Stop distances:\n");
	struct stop* t = stops;
	do{
		printf("%d", t->distance);
		printVehicles(t->distance);
		t = t->next;
	}while(t != NULL);
}

//= MAIN ==============================================================================

int main(int argc, char *argv[]){
	struct stop* stops = NULL; //create empty list of stations
	char input[20];
	scanf("%s", input);
	while(input){
		if(strcmp(input,"aggiungi-stazione")){
			int distanza, numero;
			scanf("%d %d", &distanza, &numero);
			addStop(distanza);
			int i=0;
			for(i=0; i<numero; i++){
				int autonomia;
				scanf("%d", &autonomia);
				addVehicle(distanza, autonomia);
			}
			break;
		} else if(strcmp(input,"demolisci-stazione")) {
			int distanza;
			scanf("%d", &distanza);
			deleteStop(distanza);
			break;
		} else if(strcmp(input,"aggiungi-auto")){
			int distanza, autonomia;
			scanf("%d %d", &distanza, &autonomia);
			addVehicle(distanza, autonomia);
			break;
		} else if(strcmp(input,"rottama-auto")){
			int distanza, autonomia;
			scanf("%d %d", &distanza, &autonomia);
			deleteVehicle(distanza, autonomia);
			break;
		} else if(strcmp(input,"pianifica-percorso")){
			int partenza, arrivo;
			scanf("%d %d", &partenza, &arrivo);
			break;
		} else {
			printf("Should not end here");
		}
	}
	printDistances();
	return 0;
}
