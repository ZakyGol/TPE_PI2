#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(CITY_NYC)
#define cerrado "Closed"
#elif defined(CITY_CHICAGO)
#define cerrado "Completed"
#endif
#define CANT 6
typedef struct reclameCDT * reclameADT;
typedef struct node * TList;
typedef struct queryCDT * queryADT;
#ifndef 
queryADT newQueries();
void addToQueries(queryADT q,const char * agency, const char * code, const char * status, const char * borough, int year, int month, int hour, int day, int quadLat, int quadLong);
void freeQueries(queryADT q);
#endif
