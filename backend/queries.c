#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queries.h"

#if defined(CITY_NYC)
#define CERRADO "Closed"
#elif defined(CITY_CHICAGO)
#define CERRADO "Completed"
#else
#error "Debe definirse CITY_NYC O CITY_CHICAGO"
#endif

struct q1Node {
    char *borough;
    char *name;
    unsigned int count;
    struct q1Node *next;
};
struct q2Node {
    char *borough;
    unsigned int count[HOURS];
    struct q2Node *next;
};
struct q3Node {
    int quadLat;
    int quadLong;
    char **codes;
    size_t dim;
    size_t esp;    
    struct q3Node *next;
};
struct q4Node {
    char *borough;
    char *agency;
    char *name;
    int oldDay;
    int recDay;
    struct q4Node *next;
};
struct q5Node {
    int quadLat;
    int quadLong;
    int year;
    int month;
    unsigned int acc;
    struct q5Node *next;
};
typedef struct q1Node *q1List; 
typedef struct q2Node *q2List; 
typedef struct q3Node *q3List; 
typedef struct q4Node *q4List; 
typedef struct q5Node *q5List;

struct queryCDT {
    q1List q1;
    q2List q2;
    q3List q3;
    q4List q4;
    q5List q5;
};
queryADT newQueries(void){
    return calloc(1,sizeof(struct queryCDT));
}
static int days_from_civil(int y, int m, int d) {
   y -= m <= 2;
   int era = (y >= 0 ? y : y - 399) / 400;
   int yoe = y - era * 400;
   int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
   int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
   return era * 146097 + doe - 719468;
}
static int cmpQ1(const char *bor1, const char *name1, const char *bor2, const char *name2 ){
    int c = strcmp(bor1,bor2);
    if (c != 0)
        return c;
    return strcmp(name1, name2);
}
static int cmpQ3(int lat1, int lon1, int lat2, int lon2){
    int c = lat1 - lat2;
    if (c != 0)
        return c;
    return lon1 - lon2;
}
static int cmpQ4(const char *bor1, const char *ag1, const char *name1, const char *bor2, const char *ag2,const char *name2){
    int c= strcmp(bor1,bor2);
    if (c!=0)
        return c;
    c=strcmp(ag1,ag2);
    if (c!=0)
        return c;
    return strcmp(name1,name2);
}
static int cmpQ5(int lat1, int lon1, int y1, int m1, int lat2, int lon2, int y2, int m2){
    int c=lat1-lat2;
    if(c!=0)
        return c;
    c=lon1-lon2;
    if(c!=0)
        return c;
    c= y1-y2;
    if(c!=0)
        return c;
    return m1-m2;
}
static q1List addToQ1(q1List q1, char * borough, char *name){ 
    int c; 
    if(q1==NULL || (c=cmpQ1(q1->borough,q1->name,borough,name))>0){
        q1List new=malloc(sizeof(struct q1Node));
        if(new==NULL){
            return q1;
        }
        new->borough=strdup(borough);
        if(new->borough==NULL){
            free(new);
            return q1;
        }
        new->name=strdup(name);
        if(new->name==NULL){
            free(new->borough);
            free(new);
            return q1;
        }
        new->count=1;
        new->next=q1;
        return new;
    }
    if(c==0){
        (q1->count)++;
        return q1;
    }
    q1->next=addToQ1(q1->next,borough, name);
    return q1;
}
static q2List addToQ2(q2List q2, char * borough, int hour){ 
    int c; 
    if(q2==NULL || (c=strcmp(q2->borough,borough))>0){
        q2List new=calloc(1,sizeof(struct q2Node));
        if(new==NULL){
            return q2;
        }
        new->borough=strdup(borough);
        if(new->borough==NULL){
            free(new);
            return q2;
        }
        new->count[hour]=1;
        new->next=q2;
        return new;
    }
    if(c==0){
        q2->count[hour]++;
        return q2;
    }
    q2->next=addToQ2(q2->next,borough, hour);
    return q2;
}
static q3List addToQ3(q3List q3, int quadLat, int quadLong, char *code){ 
    int c; 
    if(q3==NULL || (c=cmpQ3(q3->quadLat,q3->quadLong,quadLat,quadLong))>0){
        q3List new=malloc(sizeof(struct q3Node));
        if(new==NULL){
            return q3;
        }
        new->codes=malloc(BLOQUE*sizeof(char*));
        if(new->codes==NULL){
            free(new);
            return q3;
        }
        new->codes[0]=strdup(code);
        if(new->codes[0]==NULL){
            free(new->codes);
            free(new);
            return q3;
        }
        new->quadLat=quadLat;
        new->quadLong=quadLong;
        new->esp=BLOQUE;
        new->dim=1;
        new->next=q3;
        return new;
    }
    if(c==0){
        for(int i=0;i<q3->dim;i++){
            if(strcmp(q3->codes[i],code)==0)
                return q3;
        }
        if(q3->dim==q3->esp){
            char **tmp=realloc(q3->codes,(q3->esp+BLOQUE)*sizeof(char*));
            if(tmp==NULL)
                return q3;
            q3->codes=tmp;
            q3->esp+=BLOQUE;
        }
        q3->codes[q3->dim]=strdup(code);
        if(q3->codes[q3->dim]==NULL){
            return q3;
        }
        q3->dim++;
        return q3;
    }
    q3->next=addToQ3(q3->next,quadLat,quadLong,code);
    return q3;
}
static q4List addToQ4(q4List q4, char *borough, char *agency, char *name, int date){ 
    int c; 
    if(q4==NULL || (c=cmpQ4(q4->borough,q4->agency,q4->name,borough,agency,name))>0){
        q4List new=malloc(sizeof(struct q4Node));
        if(new==NULL){
            return q4;
        }
        new->borough=strdup(borough);
        new->agency=strdup(agency);
        new->name=strdup(name);
        if(new->borough==NULL || new->agency==NULL || new->name==NULL){
            free(new->borough);
            free(new->agency);
            free(new->name);
            free(new);
            return q4;
        }
        new->oldDay=new->recDay=date;
        new->next=q4;
        return new;
    }
    if(c==0){
        if(date>q4->recDay){
            q4->recDay=date;
        }
        if(date<q4->oldDay){
            q4->oldDay=date;
        }
        return q4;
    }
    q4->next=addToQ4(q4->next,borough,agency,name,date);
    return q4;
}
static q5List addToQ5(q5List q5, int quadLat, int quadLong, int year, int month, int YTD){ 
    int c; 
    if(q5==NULL || (c=cmpQ5(q5->quadLat,q5->quadLong,q5->year,q5->month,quadLat,quadLong,year,month))>0){
        q5List new=malloc(sizeof(struct q5Node));
        if(new==NULL){
            return q5;
        }
        new->quadLat=quadLat;
        new->quadLong=quadLong;
        new->year=year;
        new->month=month;
        new->acc=YTD+1;
        new->next=q5;
        return new;
    }
    if(c==0){
        (q5->acc)++;
        return q5;
    }
    if(q5->quadLat==quadLat && q5->quadLong==quadLong && q5->year==year){
        YTD=q5->acc;
    }
    q5->next=addToQ5(q5->next,quadLat,quadLong,year,month,YTD);
    return q5;
}
static void accQ5(q5List q5, int quadLat, int quadLong, int year, int month){ //actualiza los acc de los meses posteriores de ese mismo año(les suma 1)
    while(q5!=NULL && (q5->quadLat!=quadLat || q5->quadLong!=quadLong || q5->year!=year || q5->month!=month))
        q5=q5->next;
    if(q5==NULL) return;
    q5=q5->next;
    while(q5!=NULL && q5->quadLat==quadLat && q5->quadLong==quadLong && q5->year==year){
        (q5->acc)++;
        q5=q5->next;
    }
}
void addToQueries(queryADT q,const char * agency, const char * code, const char * name, const char * status, const char * borough, 
                    int year, int month, int day, int hour, 
                    int quadLat, int quadLong, int yMax, int yMin){
    int isClosed=strcmp(status,CERRADO)==0;
    q->q1=addToQ1(q->q1,borough,name);
    if(isClosed){
        q->q2=addToQ2(q->q2,borough,hour);
    }
    if(strcmp(status,"Open")==0)
        q->q3=addToQ3(q->q3,quadLat,quadLong,code);
    if(year>yMin && year<yMax){
        q->q4=addToQ4(q->q4,borough,agency,name,days_from_civil(year,month,day));
        if(isClosed){
            q->q5=addToQ5(q->q5,quadLat,quadLong,year,month,0);
            accQ5(q->q5,quadLat,quadLong,year,month);
        }
    }
}
void freeQ1(q1List q1){
    while(q1!=NULL){
        q1List aux=q1;
        q1=q1->next;
        free(aux->borough);
        free(aux->name);
        free(aux);
    }
}
void freeQ2(q2List q2){
    while(q2!=NULL){
        q2List aux=q2;
        q2=q2->next;
        free(aux->borough);
        free(aux);
    }
}
void freeQ3(q3List q3){
    while(q3!=NULL){
        q3List aux=q3;
        q3=q3->next;
        for(int i=0;i<aux->dim;i++){
            free(aux->codes[i]);
        }
        free(aux->codes);
        free(aux);
    }
}
void freeQ4(q4List q4){
    while(q4!=NULL){
        q4List aux=q4;
        q4=q4->next;
        free(aux->borough);
        free(aux->agency);
        free(aux->name);
        free(aux);
    }
}
void freeQ5(q5List q5){
    while(q5!=NULL){
        q5List aux=q5;
        q5=q5->next;
        free(aux);
    }
}
void freeQueries(queryADT q){
    freeQ1(q->q1);
    freeQ2(q->q2);
    freeQ3(q->q3);
    freeQ4(q->q4);
    freeQ5(q->q5);
    free(q);
}
