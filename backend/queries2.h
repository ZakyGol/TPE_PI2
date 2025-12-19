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
void addToQueries(queryADT q,const char * agency, const char * code, const char *name, const char * status, const char * borough, int year, int month, int day, int hour, double lat, double lon, int yMax, int yMin);
void freeQueries(queryADT q);
#endif
