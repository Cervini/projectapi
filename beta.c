#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//= STRUCTURES =================================================================

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

typedef struct node{
	struct stop* stop;
	struct node* father;
	struct node* children;
	struct node* sibling;
}node;

//= GLOBAL VARIABLES ===========================================================

struct stop* stops = NULL;
struct path* tree = NULL;

//= METHODS ====================================================================

struct stop* getStation(int distance){
	struct stop* pointer = stops;
	while(pointer){
		if(pointer->distance == distance)
			return pointer;
		pointer = pointer->next;
	}
	return NULL;
}

int addStation(int distance){
	//if stops is empty
	if(!stops){
		struct stop* new_stop = (struct stop*)malloc(sizeof(struct stop));
		new_stop->distance = distance;
		new_stop->previous = NULL;
		stops = new_stop;
		return 1;
	}
	if(getStation(distance) == NULL){
		if(stops){
			struct stop* backup;
			struct stop* pointer = stops;
			struct stop* new_stop = (struct stop*)malloc(sizeof(struct stop));
			new_stop->distance = distance;
			if(pointer->distance > distance){
				new_stop->next = pointer;
				new_stop->previous = NULL;
				stops = new_stop;
				return 1;
			}
			while(pointer){
				if(pointer->distance > distance){
					pointer->previous->next = new_stop;
					new_stop->next = pointer;
					new_stop->previous = pointer->previous;
					return 1;
				}
				if(pointer->next == NULL){
					backup = pointer;
				}
				pointer = pointer->next;
			}
			pointer = NULL;
			new_stop->next = NULL;
			new_stop->previous = backup;
			backup->next = new_stop;
			return 1;
		}
	}
	return 0;
}

int addVehicle(int distance, int autonomy){
	struct stop* station = getStation(distance);
	if(station != NULL){
		struct vehicle* backup;
		struct vehicle* new_vehicle = (struct vehicle*)malloc(sizeof(struct vehicle));
		new_vehicle->value = autonomy;
		//empty list
		if(station->vehicles == NULL){
			new_vehicle->previous = NULL;
			new_vehicle->next = NULL;
			station->vehicles = new_vehicle;
			return 1;
		}
		//new one is the biggest
		if(station->vehicles->value <= autonomy){
			new_vehicle->next = station->vehicles;
			new_vehicle->previous = NULL;
			station->vehicles->previous = new_vehicle;
			station->vehicles = new_vehicle;
			return 1;
		}
		struct vehicle* pointer = station->vehicles;
		while(pointer){
			if(pointer->value < autonomy){
				pointer->previous->next = new_vehicle;
				new_vehicle->previous = pointer->previous;
				pointer->previous = new_vehicle;
				new_vehicle->next = pointer;
				return 1;
			}
			if(pointer->next == NULL){
				backup = pointer;
			}
			pointer = pointer->next;
		}
		pointer = NULL;
		new_vehicle->next = NULL;
		new_vehicle->previous = backup;
		backup->next = new_vehicle;
		return 1;
	}
	return 0;
}

void deleteStation(int distance){
	struct stop* station = getStation(distance);
	if(!station){
		printf("non demolita\n");
		return;
	}
	if(station->next != NULL)
		station->next->previous = station->previous;
	if(station->previous != NULL)
		station->previous->next = station->next;
	else
		stops = station->next;
	printf("demolita\n");
}

void deleteVehicle(int distance, int value){
	struct stop* station = getStation(distance);
	if(!station||!station->vehicles){
		printf("non rottamata\n");
		return;
	}
	struct vehicle* traveller = station->vehicles;
	while(traveller){
		if(traveller->value == value){
			if(traveller->next != NULL)
				traveller->next->previous = traveller->previous;
			if(traveller->previous != NULL)
				traveller->previous->next = traveller->next;
			else
				station->vehicles = traveller->next;
			printf("rottamata\n");
			return;
		}
		traveller = traveller->next;
	}
	printf("non rottamata\n");
}

struct node* createNode(struct stop* stop){
	struct node* root = (struct node*)malloc(sizeof(struct node));
	root->stop = stop;
}

void addChild(struct stop* father, struct stop* child){
	node->father = father;
	node->sibling = father->children;
	father->children = node;
}

struct node* buildTree(int start, int finish){
	struct node* root = createNode(getStation(start));

	int starting_distance = root->stop->distance, reachable_distance = root->stop->vehicles->value;
	struct stop* traveller = getStation(start);
	while(traveller){
		if(traveller->distance - starting_distance < reachable_distance){
			addChild(root, createNode(traveller));
		}
		traveller = traveller->next;
	}
}

void shortestBranch(struct node* root){

}

void planPath(int start, int finish){
	struct node* root = buildTree(start,finish);
	shortestBranch(root);
}

//= TESTING METHODS ============================================================

//this method prints the values of a list of vehicles
void printVehicles(int distance){
	struct stop* stop = getStation(distance);
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

void printVehiclesReverse(int distance){
	struct stop* stop = getStation(distance);
	if(!stop->vehicles){
		printf("\n");
		return;
	}
	struct vehicle* t = stop->vehicles;
	while(t->next!=NULL){
		t = t->next;
	}
	while(t){
		printf(" %d", t->value);
		t = t->previous;
	}
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

//= MAIN =======================================================================

int main(int argc, char *argv[]){
	struct stop* stops = NULL; //create empty list of stations

	char input[21];
	while(scanf("%s", input) == 1){
		fflush(stdin);
		if(strcmp(input,"aggiungi-stazione")==0){
			int distanza, numero, i;
			scanf("%d %d", &distanza, &numero);
			fflush(stdin);
			if(addStation(distanza) == 1){
				printf("aggiunta\n");
			} else {
				printf("non aggiunta\n");
			}
			for(i=0; i<numero; i++){
				int autonomia;
				scanf("%d", &autonomia);
				addVehicle(distanza, autonomia);
			}
		} else if(strcmp(input,"stampa-stazioni")==0){
			printDistances();
		} else if(strcmp(input,"demolisci-stazione")==0) {
			int distanza;
			scanf("%d", &distanza);
			deleteStation(distanza);
		} else if(strcmp(input,"aggiungi-auto")==0){
			int distanza, autonomia;
			scanf("%d %d", &distanza, &autonomia);
			if(addVehicle(distanza, autonomia) == 1)
				printf("aggiunta\n");
			else
				printf("non aggiunta\n");
		} else if(strcmp(input,"rottama-auto")==0){
			int distanza, autonomia;
			scanf("%d %d", &distanza, &autonomia);
			deleteVehicle(distanza, autonomia);
		} /*else if(strcmp(input,"pianifica-percorso")==0){
			int partenza, arrivo;
			scanf("%d %d", &partenza, &arrivo);
			route(partenza, arrivo);
		} */ else {
			printf("Not a command\n");
		}
	}
  return 0;
}
