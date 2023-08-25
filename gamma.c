#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"

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

typedef struct data{
	int steps;
	int visited;
	struct stop* stop;
	struct data* previous_step;
	struct data* next;
}data;

//= CLEANING METHODS ===========================================================

void freeVehicles(struct vehicle** start){
	struct vehicle* cleaner = NULL;
	struct vehicle* garbage = NULL;
	cleaner = *start;
	while(cleaner){
		garbage = cleaner;
		cleaner = cleaner->next;
		free(garbage);
	}
}

void freeStops(struct stop** start){
	struct stop* cleaner = NULL;
	struct stop* garbage = NULL;
	cleaner = *start;
	while(cleaner){
		garbage = cleaner;
		freeVehicles(&(cleaner->vehicles));
		cleaner = cleaner->next;
		free(garbage);
	}
}

void freePaths(struct data** start){
	struct data* cleaner = NULL;
	struct data* garbage = NULL;
	cleaner = *start;
	while(cleaner){
		garbage = cleaner;
		cleaner = cleaner->next;
		free(garbage);
	}
}

//= METHODS ====================================================================

int positive(int a, int b){
	if(a>=b)
		return a-b;
	else
		return b-a;
}

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

int addStation(struct stop** stops, int distance){
	//if stops is empty
	if(*stops == NULL){
		struct stop* new_stop = (struct stop*)malloc(sizeof(struct stop));
		new_stop->distance = distance;
		new_stop->vehicles = NULL;
		new_stop->previous = NULL;
		new_stop->next = NULL;
		*stops = new_stop;
		return 1;
	} else {
		//if the station doesn't already exists
		if(getStation(*stops, distance) == NULL){
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
			if((*stops)->distance > distance){
				(*stops)->previous = new_stop;
				new_stop->next = *stops;
				new_stop->previous = NULL;
				*stops = new_stop;
				return 1;
			}
			//start travelling
			struct stop* pointer = *stops;
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

int addVehicle(struct stop** stops, int distance, int autonomy){
	//get the pointer to the station
	struct stop* station = getStation(*stops, distance);
	//check if the station exists
	if(station != NULL){
		//it exists
		struct vehicle* backup;
		struct vehicle* new_vehicle = (struct vehicle*)malloc(sizeof(struct vehicle));
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
				if(pointer->value <= autonomy){
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
	} else {
		return 0;
	}
}

void deleteStation(struct stop** stops, int distance){
	struct stop* station = getStation(*stops, distance);
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
		*stops = station->next;
	}
	freeVehicles(&(station->vehicles));
	free(station);
	printf("demolita\n");
}

void deleteVehicle(struct stop** stops, int distance, int value){
	struct stop* station = getStation(*stops, distance);
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

void dijkstraScan(struct data** node, int variant){
	//update distances from node being scanned (node)
	if(variant == 0){
		struct data* traveller = (*node);
		//see if the other stations are reachable
		while(traveller != NULL){
			//if reachable
			if(positive(traveller->stop->distance,(*node)->stop->distance) <= (*node)->stop->vehicles->value){
				if((traveller->steps == -1)||(traveller->steps > (*node)->steps+1)){
					traveller->steps = (*node)->steps + 1;
					traveller->previous_step = (*node);
				}
			}
			traveller = traveller->next;
		}
	} else if(variant == 1) {
		struct data* traveller = (*node);
		//see if the other stations are reachable
		while(traveller != NULL){
			//if reachable
			if(positive(traveller->stop->distance,(*node)->stop->distance) <= (*node)->stop->vehicles->value){
				if((traveller->steps == -1)||(traveller->steps >= (*node)->steps+1)){
					traveller->steps = (*node)->steps + 1;
					traveller->previous_step = (*node);
				}
			}
			traveller = traveller->next;
		}
	}
	//set scanned node as visited
	(*node)->visited = 1;
	//look for unvisited node with least steps
	struct data* next = (*node)->next;
	while(next != NULL){
		if(next->steps != -1){
			dijkstraScan(&next, variant);
			break;
		}
		next = next->next;
	}
}

void printPath(struct data** start){
	struct data* traveller = *start;
	if(traveller->previous_step == NULL){
		printf("nessun percorso\n");
		return;
	}
	int l = traveller->steps+1, i;
	int arr[l];
	for(i=l-1; i>=0; i--){
		arr[i] = traveller->stop->distance;
		traveller = traveller->previous_step;
	}
	printf("%d", arr[0]);
	for(i=1; i<l; i++){
		printf(" %d", arr[i]);
	}
	printf("\n");
}

/*

// DEBUGGING METHOD
void printPaths(struct data** start){
	struct data* traveller = (*start);
	while(traveller != NULL){
		printf(". %d. steps: %d. visited: %d. %p\n", traveller->stop->distance, traveller->steps, traveller->visited, traveller->previous_step);
		traveller = traveller->next;
	}
} */

void dijkstraForward(struct stop* stops, int partenza, int arrivo){
	//build steps list
	struct data* paths = (struct data*)malloc(sizeof(struct data));
	struct data* destination = paths;
	paths->stop = getStation(stops, arrivo);
	paths->steps = -1;
	paths->visited = 0;
	paths->previous_step = NULL;
	paths->next = NULL;
	struct stop* traveller = paths->stop;
	traveller = traveller->previous;
	while((traveller != NULL) && (traveller->distance >= partenza)){
		struct data* new_data = (struct data*)malloc(sizeof(struct data));
		new_data->stop = traveller;
		new_data->steps = -1;
		new_data->visited = 0;
		new_data->previous_step = NULL;
		new_data->next = paths;
		paths = new_data;
		traveller = traveller->previous;
	}
	paths->steps = 0;
	dijkstraScan(&paths, 0);
	printPath(&destination);
	freePaths(&paths);
}

void dijkstraBackward(struct stop* stops, int partenza, int arrivo){
	//build steps list
	struct data* paths = (struct data*)malloc(sizeof(struct data));
	struct data* destination = paths;
	paths->stop = getStation(stops, arrivo);
	paths->steps = -1;
	paths->visited = 0;
	paths->previous_step = NULL;
	paths->next = NULL;
	struct stop* traveller = paths->stop;
	traveller = traveller->next;
	while((traveller != NULL) && (traveller->distance <= partenza)){
		struct data* new_data = (struct data*)malloc(sizeof(struct data));
		new_data->stop = traveller;
		new_data->steps = -1;
		paths->visited = 0;
		new_data->previous_step = NULL;
		new_data->next = paths;
		paths = new_data;
		traveller = traveller->next;
	}
	paths->steps = 0;
	dijkstraScan(&paths, 1);
	/*
	printf("--------------------------------\n");
	printPaths(&paths);
	printf("--------------------------------\n");
	*/
	printPath(&destination);
	freePaths(&paths);
}

void planPath(struct stop* stops, int partenza, int arrivo){
	if(partenza < arrivo){
		dijkstraForward(stops, partenza, arrivo);
	} else if(partenza > arrivo){
		dijkstraBackward(stops, partenza, arrivo);
	} else {
		printf("%d\n", partenza);
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
			if(addStation(&stops, distanza) == 1){
				printf("aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
					addVehicle(&stops, distanza, autonomia);
				}
			} else {
				printf("non aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
				}
			}
		}
		else if(strcmp(input,"demolisci-stazione")==0) {
			int distanza;
			if(scanf("%d", &distanza)!=1)
				break;
			deleteStation(&stops, distanza);
		}
		else if(strcmp(input,"aggiungi-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			if(addVehicle(&stops, distanza, autonomia) == 1)
				printf("aggiunta\n");
			else
				printf("non aggiunta\n");
		}
		else if(strcmp(input,"rottama-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			deleteVehicle(&stops, distanza, autonomia);
		}
		else if(strcmp(input,"pianifica-percorso")==0){
			int partenza, arrivo;
			if(scanf("%d %d", &partenza, &arrivo)<1)
				break;
			planPath(stops, partenza, arrivo);

		}
		else {
			printf("Not a command\n");
		}
	}
	if(stops != NULL){
		freeStops(&stops);
		stops = NULL;
	}
  return 0;
}
