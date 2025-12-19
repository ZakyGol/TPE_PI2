#ifndef __QUERIES_H__
#define __QUERIES_H__
#if defined(CITY_NYC)
#define CERRADO "Closed"
#elif defined(CITY_CHICAGO)
#define CERRADO "Completed"
#else
#error "Debe definirse CITY_NYC O CITY_CHICAGO"
#endif
#include <stdlib.h>
#define HOURS 24
#define BLOQUE 50
#define MONTHS 12
typedef struct q1Node *q1List;
typedef struct q2Node *q2List;
typedef struct q3Node *q3List;
typedef struct q4Node *q4List;
typedef struct q5Node *q5List;
typedef struct queryCDT * queryADT;

q1List getQ1(queryADT q);
q2List getQ2(queryADT q);
q3List getQ3(queryADT q);
q4List getQ4(queryADT q);
q5List getQ5(queryADT q);

queryADT newQueries(void);
void addToQueries(queryADT q,
                  const char *agency,
                  const char *code,
                  const char *typeName,
                  const char *status,
                  const char *borough,
                  int year, int month, int day, int hour,
                  double lat, double lon,
                  int yMax, int yMin);
void freeQueries(queryADT q);

//funciones para separar back de front
// Q1: borough;type;requests
typedef void (*q1RowFn)(void *ctx, const char *borough, const char *typeName, unsigned int requests);
void q1ForEach(const queryADT q, q1RowFn fn, void *ctx);

// Q2: borough;dayHour;requests 
typedef void (*q2RowFn)(void *ctx, const char *borough, int dayHour, unsigned int requests);
void q2ForEach(const queryADT q, q2RowFn fn, void *ctx);

// Q3: quadLat;quadLong;distinctOpenTypes
typedef void (*q3RowFn)(void *ctx, int quadLat, int quadLong, size_t distinctOpenTypes);
void q3ForEach(const queryADT q, q3RowFn fn, void *ctx);

// Q4: neighbourhood;agency;type;daysBetween
typedef void (*q4RowFn)(void *ctx, const char *borough, const char *agency, const char *typeName, int daysBetween);
void q4ForEach(const queryADT q, q4RowFn fn, void *ctx);

// Q5: quadLat;quadLong;year;month;closedRequestsYTD
typedef void (*q5RowFn)(void *ctx, int quadLat, int quadLong, int year, int month, unsigned int closedRequestsYTD);
void q5ForEach(const queryADT q, q5RowFn fn, void *ctx);

#endif
