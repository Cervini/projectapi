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
  int visited;
  struct vehicle* vehicles;
  struct stop* next;
  struct stop* previous;
}stop;

typedef struct path{
	struct stop* stop;
	struct path* before;
	struct path* afters;
	struct path* other_way;
}path;

//= GLOBAL VARIABLES ====================================================================

struct stop* stops = NULL;

//= METHODS =============================================================================

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

//adds a new stop with the defined distance
void addStop(int distance){
	if(getStop(distance)){
		printf("non aggiunta\n");
		return;
	}
	//check if the stops list is empty
	struct stop* temp = (struct stop*)malloc(sizeof(struct stop));//allocate in memory the space for a stop struct
	temp->distance = distance; //distance from the start (from input)
	temp->vehicles = NULL;
	temp->next = NULL; //when added the station is the last
	if(!stops){
		temp->previous = NULL;
		stops = temp;
		printf("aggiunta\n");
		return;
	}
	struct stop* t = stops;
	while(t->next != NULL){
	//go to the end of the list
	t = t->next;
	}
	temp->previous = t; //set the last stop as the previous of the newly added item
	t->next = temp; //set the newly added item as the last
	printf("aggiunta\n");
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
	printf("aggiunta\n");
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

void resetVisited(){
	struct stop* stop = stops;
	do{
		stop->visited = 0;
		stop = stop->next;
	}while(stop);
}

//returns the value of the autonomy of the vehicle with the greater one, or -1 if the station is empty
int bestVehicle(int distance){
	struct vehicle* vehicle = getStop(distance)->vehicles;
	if(vehicle){
		int max = vehicle->value;
		vehicle = vehicle->next;
		while(vehicle){
			if(vehicle->value > max){
				max = vehicle->value;
			}
			vehicle = vehicle->next;
		}
		return max;
	} else {
		return -1;
	}
}

void fastestPath(int start, int finish){
	//simple cases
	if(start==finish){
		printf(" %d", &start);
		return;
	}
	struct stop* starting = getStop(start);
	if(!starting->vehicles){
		printf("nessun percorso\n");
		return;
	}
	
	
	//if distance is increasing
	if(start<=finish){
		//look for vehicle with the longest autonomy
		int current = bestVehicle(start);
		//create a tree with all the possible paths
		struct path* tree = (struct path*)malloc(sizeof(struct path));
		//set as root the starting stop
		tree->stop = getStop(start);
		
		//use t to travel along the stops and n to bulid the tree
		struct path* node = tree;
		struct stop* temp = getStop(finish);
		do{
			//add all the reachable stops to the node
			do{
				//set 'road' ad the distance between the stop currently observed and t
				int road = temp->distance - start;
				if(road<=current){
					//if the stop 't' is reachable create a new node on the tree
					struct path* way = (struct path*)malloc(sizeof(struct path));
					way->stop = temp;
					way->before = node;
					if(!node->afters){
						//if the node has no children set the new node as one
						node->afters = way;
					} else {
						//if the node already has children add the new one to the list
						way->other_way = node->afters;
						node->afters = way;
					}
				} else {
					t = t->previous;
				}
			}while(t&&t->distance>=start);
			
			//go to the next node
			if(node->other_way){
				node = node->other_way;
			} else if(node->afters){
				node = node->afters;
			} else {
				break;
			}
		}while(true);
		
		
		
		
	} else { //TODO
		
		//look for vehicle with the longest autonomy
		int current = bestVehicle(start);
		//create a tree with all the possible paths
		struct path* tree = (struct path*)malloc(sizeof(struct path));
		//set as root the starting stop
		tree->stop = getStop(start);
		
		//use t to travel along the stops and n to bulid the tree
		struct path* node = tree;
		struct stop* temp = getStop(finish);
		do{
			//add all the reachable stops to the node
			do{
				//set 'road' ad the distance between the stop currently observed and t
				int road = temp->distance - start;
				if(road<=current){
					//if the stop 't' is reachable create a new node on the tree
					struct path* way = (struct path*)malloc(sizeof(struct path));
					way->stop = temp;
					way->before = node;
					if(!node->afters){
						//if the node has no children set the new node as one
						node->afters = way;
					} else {
						//if the node already has children add the new one to the list
						way->other_way = node->afters;
						node->afters = way;
					}
				} else {
					t = t->previous;
				}
			}while(t&&t->distance>=start);
			
			//go to the next node
			if(node->other_way){
				node = node->other_way;
			} else if(node->afters){
				node = node->afters;
			} else {
				break;
			}
		}while(true);
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
			fastestPath(partenza, arrivo);
			break;
		} else {
			printf("Should not end here");
		}
	}
	return 0;
}
