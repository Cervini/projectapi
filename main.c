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

typedef struct path{
	struct stop* stop;
	struct path* before;
	struct path* afters;
	struct path* other_way;
}path;

//= GLOBAL VARIABLES ====================================================================

struct stop* stops = NULL;
struct path* tree = NULL;

//= METHODS =============================================================================

//returns the stop with the defined distance
struct stop* getStop(int distance){
	struct stop* t = stops; //get the beginning of the stops list
	while(t){
		if(t->distance==distance)
			//if the distance is the wanted one return the stop
			return t;
		t = t->next; //go to the next stop
	}
	//if the wanted stop doesn't exist return null
	return NULL;
}

//adds a new stop with the defined distance
void addStop(int distance){
	//check if stop already exist
	if(getStop(distance)!=NULL){
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

//removes stop from stops list
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

//removes vehicle from vehicles list
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

//returns the gratest 'value' between the vehicles, or -1 if the station is empty
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

void printTreeContent(struct path* root) {
    if (root == NULL) {
        return;
    }

    // Print the distance of the stop in the current node
    printf("Stop distance: %d\n", root->stop->distance);

    // Recursively print content of 'before', 'afters', and 'other_way' branches
    printTreeContent(root->before);
    printTreeContent(root->afters);
    printTreeContent(root->other_way);
}

//adds to the node all the reachable stops as children
void addReachable(struct path* node, int finish){
	//get the distance of the stop in the node
	int from  = node->stop->distance;
	//start to travel from node's stop
	struct stop* traveller = node->stop;
	int autonomy = bestVehicle(traveller->distance);
	do{
		//check if stop is reachable
		if(traveller->distance-from<=autonomy){
			//stop is reachable
			struct path* newNode = (struct path*)malloc(sizeof(struct path));
			//set the stop as the node one
			newNode->stop = traveller;
			//set the node as the father of the new one
			newNode->before = node;
			//add the new node to the children of node
			newNode->other_way = node->afters;
			node->afters = newNode;
			addReachable(newNode, finish);
		}
		traveller = traveller->next;
	}while(traveller&&(traveller->distance<=finish));
	return;
}

void route(int start, int finish){
	//simple cases
	if(start==finish){
		printf(" %d",start);
		return;
	}
	struct stop* starting = getStop(start);
	if(!starting->vehicles){
		printf("nessun percorso\n");
		return;
	}
	struct path* root = (struct path*)malloc(sizeof(struct path));
	root->stop = getStop(start);
	tree = root;
	addReachable(tree, finish);
	printTreeContent(tree);
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
	
	//TODO: fix here [segmentation fault (core dumped)]
	char input[21];
	while(scanf("%s", input)){
		if(strcmp(input,"aggiungi-stazione")==0){
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
		} else if(strcmp(input,"demolisci-stazione")==0) {
			int distanza;
			scanf("%d", &distanza);
			deleteStop(distanza);
			break;
		} else if(strcmp(input,"aggiungi-auto")==0){
			int distanza, autonomia;
			scanf("%d %d", &distanza, &autonomia);
			addVehicle(distanza, autonomia);
			break;
		} else if(strcmp(input,"rottama-auto")==0){
			int distanza, autonomia;
			scanf("%d %d", &distanza, &autonomia);
			deleteVehicle(distanza, autonomia);
			break;
		} else if(strcmp(input,"pianifica-percorso")==0){
			int partenza, arrivo;
			scanf("%d %d", &partenza, &arrivo);
			route(partenza, arrivo);
			break;
		} else {
			printf("Should not end here");
		}
	}
	return 0;
}
