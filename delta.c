#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct vehicle{
	int value;
	struct vehicle* next;
	struct vehicle* previous;
}vehicle;

typedef struct edge{
	struct stop* pointing;
	struct edge* next;
	struct edge* previous;
}edge;

typedef struct stop{
  int distance;
	int updated;
  struct vehicle* vehicles;
  struct stop* next;
  struct stop* previous;
	struct edge* forward;
	struct edge* backward;
}stop;

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

void freeEdges(struct edge** start){
	struct edge* cleaner = NULL;
	struct edge* garbage = NULL;
	cleaner = *start;
	while(cleaner){
		garbage = cleaner;
		cleaner = cleaner->next;
		free(garbage);
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

void addForwardEdge(struct stop** from, struct stop* to){
	//create new edge and point it to the 'to' stop
	struct edge* new_edge = (struct edge*)malloc(sizeof(struct edge));
	new_edge->pointing = *to;
	if((*from)->forward == NULL){
		//'from' stop has no edges going forward
		new_edge->next = NULL;
		new_edge->previous = NULL;
		(*from)->forward = new_edge;
	} else {
		//'from' stop already has edges going forward, set new_edge as first
		new_edge->next = (*from)->forward;
		new_edge->previous = NULL;
		(*from)->forward->previous = new_edge;
		(*from)->forward = new_edge;
	}
}

void forwardEdges(struct stop** stop){
	if((*stop)->forward != NULL)
		freeEdges(&((*stop)->forward));
	if((*stop != NULL)&&((*stop)->vehicles != NULL)){
		int reach = (*stop)->vehicles->value;
		struct stop* traveller = *stop;
		while((traveller != NULL) && (traveller->distance-(*stop)->distance <= reach)){
			addForwardEdge(&(*stop), traveller);
		}
	}
}

void addBackwardEdge(struct stop** from, struct stop* to){
	//create new edge and point it to the 'to' stop
	struct edge* new_edge = (struct edge*)malloc(sizeof(struct edge));
	new_edge->pointing = *to;
	if((*from)->backward == NULL){
		//'from' stop has no edges going forward
		new_edge->next = NULL;
		new_edge->previous = NULL;
		(*from)->backward = new_edge;
	} else {
		//'from' stop already has edges going forward, set new_edge as first
		new_edge->next = (*from)->backward;
		new_edge->previous = NULL;
		(*from)->backward->previous = new_edge;
		(*from)->backward = new_edge;
	}
}

void backwardEdges(struct stop** stop){
	if((*stop)->backward != NULL)
		freeEdges(&((*stop)->backward));
	if((*stop != NULL)&&((*stop)->vehicles != NULL)){
		int reach = (*stop)->vehicles->value;
		struct stop* traveller = *stop;
		while((traveller != NULL) && ((*stop)->distance - traveller->distance <= reach)){
			addBackwardEdge(&(*stop), traveller);
		}
	}
}

void updateGraph(struct stop** stops){
	struct stop* traveller = *stops;
	while(traveller != NULL){
		if(traveller->vehicles != NULL)
			if((traveller->updated != traveller->vehicles->value)){
				forwardEdges(&traveller);
				backwardEdges(&traveller);
				traveller->updated = traveller->vehicles->value;
			}
		traveller = traveller->next;
	}
}

//= MAIN =======================================================================

int main(int argc, char *argv[]){
	char input[21];
	struct stop* stops = NULL;
	while(scanf("%s", input)==1){
		fflush(stdin);
		// aggiungi-stazione
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
				updateGraph(&stops);
			} else {
				printf("non aggiunta\n");
				for(i=0; i<numero; i++){
					int autonomia;
					if(scanf("%d", &autonomia)<1)
						break;
				}
			}
		}
		// demolisci-stazione
		else if(strcmp(input,"demolisci-stazione")==0) {
			int distanza;
			if(scanf("%d", &distanza)!=1)
				break;
			deleteStation(&stops, distanza);
		}
		// aggiungi-auto
		else if(strcmp(input,"aggiungi-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			if(addVehicle(&stops, distanza, autonomia) == 1){
				printf("aggiunta\n");
				struct stop* check = getStation(&stops, distanza);
				if(check->updated == check->vehicles->value){
					forwardEdges(&check);
					backwardEdges(&check);
					traveller->updated = traveller->vehicles->value;
				}
			}
			else
				printf("non aggiunta\n");
		}
		// rottama-auto
		else if(strcmp(input,"rottama-auto")==0){
			int distanza, autonomia;
			if(scanf("%d %d", &distanza, &autonomia)<1)
				break;
			deleteVehicle(&stops, distanza, autonomia);
			struct stop* check = getStation(&stops, distanza);
			if(check->updated == check->vehicles->value){
				forwardEdges(&check);
				backwardEdges(&check);
				traveller->updated = traveller->vehicles->value;
			}
		}
		// pianifica-percorso
		else if(strcmp(input,"pianifica-percorso")==0){
			int partenza, arrivo;
			if(scanf("%d %d", &partenza, &arrivo)<1)
				break;
			planPath(stops, partenza, arrivo);
		}
		// comando non esistente
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
