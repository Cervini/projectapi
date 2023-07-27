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
	int level;
	struct node* father;
	struct node* children;
	struct node* sibling;
}node;

//= GLOBAL VARIABLES ===========================================================

struct stop* stops = NULL;
struct node* tree = NULL;
struct node* possible_best = NULL;
struct stop* path = NULL;

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
	if(stops == NULL){
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
				stops->previous = new_stop;
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
					pointer->previous = new_stop;
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
	if(station->previous != NULL){
		if(station->next != NULL){
			station->next->previous = station->previous;
			station->previous->next = station->next;
		} else {
			station->previous->next = NULL;
		}
	} else {
		stops = station->next;
	}
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

struct node* createNodeFromDistance(int distance, int level){
	struct node* root = (struct node*)malloc(sizeof(struct node));
	root->stop = getStation(distance);
	root->level = level;
	return root;
}

void addChild(struct node* father, struct node* child){
	child->father = father;
	if(father->children != NULL){
		struct node* traveller = father->children;
		while(traveller->sibling != NULL){
			traveller = traveller->sibling;
		}
		traveller->sibling = child;
	} else {
		father->children = child;
	}
	child->sibling = NULL;
}

void scanReachable(struct node* node, int finish, int level){
	level++;
	if(possible_best != NULL){
		if(possible_best->level <= level)
			return;
	}
	//stop if scanning too far
	if(node->stop->distance>=finish)
		return;
	int starting_distance = node->stop->distance, reachable_distance = node->stop->vehicles->value;
	struct stop* traveller = NULL;
	if(node->stop->next)
		traveller = node->stop->next;
	else
		return;
	//add all the children to the node
	while((traveller!=NULL) && (traveller->distance <= finish)){
		//check if the stop is the destination
		if(traveller->distance-starting_distance <= reachable_distance){
			if(traveller->distance == finish){
				struct node* child = createNodeFromDistance(traveller->distance, level);
				{
					struct node* cleaner = node->children;
					struct node* garbage;
					while(cleaner){
						garbage = cleaner;
						cleaner = cleaner->sibling;
						free(garbage);
					}
				}
				node->children = NULL;
				//if it is, it's the last node that needs to be added
				addChild(node, child);
				//add the node to the possibilities
				free(possible_best);
				possible_best = child;
				return;
			} else {
				addChild(node, createNodeFromDistance(traveller->distance, level));
			}
		}
		traveller = traveller->next;
	}
	//after all children have been added if the destination has not been reached
	struct node* to_scan = node->children;
	//scan from all the children of the current node
	while(to_scan){
		if(to_scan->stop->distance != finish){
			scanReachable(to_scan, finish, level);
			to_scan = to_scan->sibling;
		} else {
			return;
		}
	}
}

void scanReachableReverse(struct node* node, int finish, int level){
	level++;
	if(possible_best != NULL){
		if(possible_best->level < level)
			return;
	}
	//stop if scanning too far
	if(node->stop->distance<=finish)
		return;
	int starting_distance = node->stop->distance, reachable_distance = node->stop->vehicles->value;
	struct stop* traveller = NULL;
	if(node->stop->previous)
		traveller = node->stop->previous;
	else
		return;
	//add all the children to the node
	while((traveller!=NULL) && (traveller->distance >= finish)){
		//check if the stop is the destination
		if(starting_distance-traveller->distance <= reachable_distance){
			if(traveller->distance == finish){
				struct node* child = createNodeFromDistance(traveller->distance, level);
				//if it is, it's the last node that needs to be added
				{
					struct node* cleaner = node->children;
					struct node* garbage;
					while(cleaner){
						garbage = cleaner;
						cleaner = cleaner->sibling;
						free(garbage);
					}
				}
				node->children = NULL;
				addChild(node, child);
				free(possible_best);
				possible_best = child;
				return;
			} else {
				addChild(node, createNodeFromDistance(traveller->distance, level));
			}
		}
		traveller = traveller->previous;
	}
	//after all children have been added if the destination has not been reached
	struct node* to_scan = node->children;
	//scan from all the children of the current node
	while(to_scan){
		if(to_scan->stop->distance != finish){
			scanReachableReverse(to_scan, finish, level);
			to_scan = to_scan->sibling;
		} else {
			return;
		}
	}
}

void buildTree(int start, int finish){
	struct node* root = createNodeFromDistance(start, 1);
	if(start < finish){
		scanReachable(root, finish, 1);
	} else {
		scanReachableReverse(root, finish, 1);
	}
}

void printPath(){
	if(possible_best == NULL){
		printf("nessun percorso\n");
	} else {
		int l = possible_best->level,path[l], i;
		struct node* traveller = possible_best;
		for(i=l-1; i>-1; i--){
			path[i] = traveller->stop->distance;
			traveller = traveller->father;
		}
		printf("%d", path[0]);
		for(i=1; i<l; i++){
			printf(" %d", path[i]);

		}
		printf("\n");

	}
}

void planPath(int start, int finish){
	if(start==finish){
		printf("%d\n", start);
	} else {
		buildTree(start,finish);
		printPath();
	}
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

void printDistancesReverse(){
	if(!stops){
		printf("empty list\n");
		return;
	}
	printf("Stop distances:\n");
	struct stop* t = stops;
	while(t->next!=NULL){
		t = t->next;
	}
	while(t){
		printf("%d ", t->distance);
		printVehicles(t->distance);
		t = t->previous;
	}
	printf("\n");

}

//= MAIN =======================================================================

int main(int argc, char *argv[]){
	char input[21];
	while(scanf("%s", input)==1){
		fflush(stdin);
		if(strcmp(input,"aggiungi-stazione")==0){
			int distanza, numero, i;
			if(scanf("%d %d", &distanza, &numero)<1)
				break;
			fflush(stdin);
			if(addStation(distanza) == 1){
				printf("aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
					addVehicle(distanza, autonomia);
				}
			} else {
				printf("non aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
				}
			}

		} else if(strcmp(input,"stampa-stazioni")==0){
			printDistancesReverse();
		} else if(strcmp(input,"demolisci-stazione")==0) {
			int distanza;
			if(scanf("%d", &distanza)!=1)
				break;
			deleteStation(distanza);
		} else if(strcmp(input,"aggiungi-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			if(addVehicle(distanza, autonomia) == 1)
				printf("aggiunta\n");
			else
				printf("non aggiunta\n");
		} else if(strcmp(input,"rottama-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			deleteVehicle(distanza, autonomia);
		} else if(strcmp(input,"pianifica-percorso")==0){
			int partenza, arrivo;
			if(scanf("%d %d", &partenza, &arrivo)<1)
				break;
			planPath(partenza, arrivo);
			possible_best = NULL;
		} else {
			printf("Not a command\n");
		}
	}
  return 0;
}
