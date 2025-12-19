#ifndef __QUERIES_H__
#define __QUERIES_H__
#if defined(CITY_NYC)
#define CERRADO "Closed"
#elif defined(CITY_CHICAGO)
#define CERRADO "Completed"
#else 
#error "Debe definirse CITY_NYC O CITY_CHICAGO"
#endif
#define HOURS 24
#define BLOQUE 50
typedef struct q1Node *q1List;
typedef struct q2Node *q2List;
typedef struct q3Node *q3List;
typedef struct q4Node *q4List;
typedef struct q5Node *q5List;
typedef struct queryCDT * queryADT;
typedef void (*Q1Visitor)(const char *borough,const char *type, unsigned int count,void *userData);
typedef void (*Q2Visitor)(const char *borough, int hour, unsigned int count, void *userData);
typedef void (*Q3Visitor)(int quadLat, int quadLong, unsigned int rec, void *userData);
typedef void (*Q4Visitor)(const char *borough, const char *agency, const char *type, int daysBetween, void *userData);
typedef void (*Q5Visitor)(int quadLat, int quadLong, int year, int month, int recYTD, void *userData);
queryADT newQueries();
void addToQueries(queryADT q,const char * agency, const char * code, const char *name, const char * status, const char * borough, int year, int month, int day, int hour, double lat, double lon, int yMax, int yMin);
void q1_foreach(queryADT q, Q1Visitor v, void *userData);
void q2_foreach(queryADT q, Q2Visitor v, void *userData);
void q3_foreach(queryADT q, Q3Visitor v, void *userData);
void q4_foreach(queryADT q, Q4Visitor v, void *userData);
void q5_foreach(queryADT q, Q5Visitor v, void *userData);
void freeQueries(queryADT q);
#endif
