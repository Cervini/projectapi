#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//= STRUCTURES =================================================================

typedef struct vehicle{
	int value;
	struct vehicle* next;
	struct vehicle* previous;
}vehicle;

typedef struct stop{
  int distance;
  struct vehicle* vehicles;
  struct stop* next;
  struct stop* previous;
}stop;

typedef struct node{
	int level;
	struct stop* stop;
	struct node* father;
	struct node* children;
	struct node* sibling;
}node;

//= CLEANING METHODS ===========================================================

void freeVehicles(struct vehicle* start){
	struct vehicle* cleaner = NULL;
	struct vehicle* garbage = NULL;
	cleaner = start;
	while(cleaner){
		garbage = cleaner;
		cleaner = cleaner->next;
		free(garbage);
	}
}

void freeStop(struct stop* start){
	struct stop* cleaner = NULL;
	struct stop* garbage = NULL;
	cleaner = start;
	while(cleaner){
		garbage = cleaner;
		freeVehicles(cleaner->vehicles);
		cleaner = cleaner->next;
		free(garbage);
	}
}

void freeTree(struct node* branch){
	if(branch != NULL){
		if(branch->children == NULL){
			free(branch);
		} else {
			struct node* child = branch->children;
			struct node* garbage = NULL;
			while(child){
				garbage = child;
				child = child->sibling;
				freeTree(garbage);
			}
			free(branch);
		}
	}
}

//= METHODS ====================================================================

struct stop* getStation(struct stop* stops, int distance){
	struct stop* pointer = stops;
	while(pointer){
		if(pointer->distance){
			if(pointer->distance == distance)
				return pointer;
		}
		pointer = pointer->next;
	}
	return NULL;
}

int addStation(struct stop* stops, int distance){
	//if stops is empty
	if(stops == NULL){
		struct stop* new_stop = (struct stop*)malloc(sizeof(struct stop));
		new_stop->distance = distance;
		new_stop->vehicles = NULL;
		new_stop->previous = NULL;
		stops = new_stop;
		return 1;
	} else {
		//if the station doesn't already exists
		if(getStation(stops, distance) == NULL){
			struct stop* backup = NULL;
			//allocate the new stop
			struct stop* new_stop = (struct stop*)malloc(sizeof(struct stop));
			if(new_stop == NULL){
				printf("Memory error");
				exit(0);
			}
			new_stop->distance = distance;
			new_stop->vehicles = NULL;
			//if the station is in the first place
			if(stops->distance > distance){
				stops->previous = new_stop;
				new_stop->next = stops;
				new_stop->previous = NULL;
				stops = new_stop;
				return 1;
			}
			//start travelling
			struct stop* pointer = stops;
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

int addVehicle(struct stop* stops, int distance, int autonomy){
	struct stop* station = getStation(stops, distance);
	if(station != NULL){
		struct vehicle* backup;
		struct vehicle* new_vehicle = (struct vehicle*)malloc(sizeof(struct vehicle));
		if(new_vehicle == NULL){
			printf("Memory error");
			exit(0);
		}
		new_vehicle->value = autonomy;
		//empty list
		if(station->vehicles == NULL){
			new_vehicle->previous = NULL;
			new_vehicle->next = NULL;
			station->vehicles = new_vehicle;
			new_vehicle = NULL;
			return 1;
		} else {
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
					new_vehicle = NULL;
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
			new_vehicle = NULL;
			return 1;
		}
	}
	return 0;
}

void deleteStation(struct stop* stops, int distance){
	struct stop* station = getStation(stops, distance);
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
	free(station);
	printf("demolita\n");
}

void deleteVehicle(struct stop* stops, int distance, int value){
	struct stop* station = getStation(stops, distance);
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
			free(traveller);
			printf("rottamata\n");
			return;
		}
		traveller = traveller->next;
	}
	printf("non rottamata\n");
}

struct node* createNodeFromDistance(struct stop* stops, int distance, int level){
	struct node* root = (struct node*)malloc(sizeof(struct node));
	if(root == NULL){
		printf("Memory error");
		exit(0);
	}
	root->stop = getStation(stops, distance);
	root->level = level;
	root->children = NULL;
	root->sibling = NULL;
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

void scanReachable(struct node* node, int finish, int level, struct stop* stops, struct node* best){
	level++;
	if(best != NULL){
		if(best->level <= level)
			return;
	}
	int starting_distance, reachable_distance;
	if((node!=NULL)&&(node->stop!=NULL)){
		starting_distance = node->stop->distance;
		if(node->stop->vehicles!=NULL){
			reachable_distance = node->stop->vehicles->value;
		} else {
			return;
		}
	} else {
		return;
	}
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
				struct node* child = createNodeFromDistance(stops, traveller->distance, level);
				node->children = NULL;
				//if it is, it's the last node that needs to be added
				addChild(node, child);
				//add the node to the possibilities
				best = child;
				return;
			} else {
				addChild(node, createNodeFromDistance(stops, traveller->distance, level));
			}
		}
		traveller = traveller->next;
	}
	//after all children have been added if the destination has not been reached
	struct node* to_scan = node->children;
	//scan from all the children of the current node
	while(to_scan){
		if(to_scan->stop->distance != finish){
			scanReachable(to_scan, finish, level, stops, best);
			to_scan = to_scan->sibling;
		} else {
			return;
		}
	}
}

void scanReachableReverse(struct node* node, int finish, int level, struct stop* stops, struct node* best){
	level++;
	if(best != NULL){
		if(best->level < level)
			return;
	}
	int starting_distance, reachable_distance;
	if((node!=NULL)&&(node->stop!=NULL)){
		starting_distance = node->stop->distance;
		if(node->stop->vehicles!=NULL){
			reachable_distance = node->stop->vehicles->value;
		} else {
			return;
		}
	} else {
		return;
	}
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
				struct node* child = createNodeFromDistance(stops, traveller->distance, level);
				//if it is, it's the last node that needs to be added
				node->children = NULL;
				addChild(node, child);
				free(best);
				best = child;
				return;
			} else {
				addChild(node, createNodeFromDistance(stops, traveller->distance, level));
			}
		}
		traveller = traveller->previous;
	}
	//after all children have been added if the destination has not been reached
	struct node* to_scan = node->children;
	//scan from all the children of the current node
	while(to_scan){
		if(to_scan->stop->distance != finish){
			scanReachableReverse(to_scan, finish, level, stops, best);
			to_scan = to_scan->sibling;
		} else {
			return;
		}
	}
}

struct node* buildTree(struct node* root, struct stop* stops, int start, int finish){
	struct node* best = NULL;
	if(start < finish){
		scanReachable(root, finish, 1, stops, best);
	} else {
		scanReachableReverse(root, finish, 1, stops, best);
	}
	return best;
}

void printPath(struct node* best){
	if(best == NULL){
		printf("nessun percorso\n");
	} else {
		int l = best->level,path[l], i;
		struct node* traveller = best;
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

void planPath(struct stop* stops, int start, int finish){
	if(start==finish){
		printf("%d\n", start);
	} else {
		struct node* root = createNodeFromDistance(stops, start, 1);
		struct node* best = buildTree(root, stops, start, finish);
		printPath(best);
		freeTree(root);
		free(best);
	}
}

//= MAIN =======================================================================

int main(int argc, char *argv[]){
	char input[21];
	struct stop* stops = NULL;
	while(scanf("%s", input)==1){
		fflush(stdin);
		if(strcmp(input,"aggiungi-stazione")==0){
			int distanza, numero, i;
			if(scanf("%d %d", &distanza, &numero)<1)
				break;
			//printf(" -> aggiungi-stazione %d\n", distanza);
			if(addStation(stops, distanza) == 1){
				printf("aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
					//printf(" -> aggiungendo veicolo %d\n", autonomia);
					addVehicle(stops, distanza, autonomia);
				}
			} else {
				printf("non aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
				}
			}

		} else if(strcmp(input,"demolisci-stazione")==0) {
			int distanza;
			if(scanf("%d", &distanza)!=1)
				break;
			deleteStation(stops, distanza);
		} else if(strcmp(input,"aggiungi-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			if(addVehicle(stops, distanza, autonomia) == 1)
				printf("aggiunta\n");
			else
				printf("non aggiunta\n");
		} else if(strcmp(input,"rottama-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			deleteVehicle(stops, distanza, autonomia);
		} else if(strcmp(input,"pianifica-percorso")==0){
			int partenza, arrivo;
			if(scanf("%d %d", &partenza, &arrivo)<1)
				break;
			planPath(stops, partenza, arrivo);
		} else {
			printf("Not a command\n");
		}
	}
	if(stops != NULL){
		freeStop(stops);
		stops = NULL;
	}
  return 0;
}
