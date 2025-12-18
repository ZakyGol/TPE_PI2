#ifndef __QUERIES_H__
#define __QUERIES_H__

#define HOURS 24
#define BLOQUE 50
typedef struct queryCDT * queryADT;
queryADT newQueries(void);
void addToQueries(queryADT q,const char * agency, const char * code, const char *name, const char * status, const char * borough, int year, int month, int day, int hour, int quadLat, int quadLong, int yMax, int yMin);
void freeQueries(queryADT q);

#endif
