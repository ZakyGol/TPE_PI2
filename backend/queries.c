#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "queries.h"
typedef struct boroughTypes{
    char *type;
    unsigned int count;
    struct boroughTypes *next;
}*listBType;
struct q1Node {
    char *borough;
    listBType firstType;
    struct q1Node *next;
};
struct q2Node {
    char *borough;
    unsigned int count[HOURS];
    struct q2Node *next;
};
typedef struct latLongs{
    int quadLong;
    char **codes;
    unsigned int dim;
    unsigned int size;
    struct latLongs *next;
}*listLatLongs;
struct q3Node {
    int quadLat;
    listLatLongs firstLong;
    struct q3Node *next;
};
typedef struct agencyTypes{
    char *type;
    int recDay;
    int oldDay;
    struct agencyTypes *next;
}*listAgTy;
typedef struct boroughAg{
    char * agency;
    listAgTy firstType;
    struct boroughAg *next;
}*listBorAg;
struct q4Node {
    char *borough;
    listBorAg firstAgency;
    struct q4Node *next;
};
typedef struct quadYearMonth{
    int month;
    int recYTD;
    struct quadYearMonth *next;
}*listQuadYearMonth;
typedef struct quadYear{
    int year;
    listQuadYearMonth firstMonth;
    struct quadYear *next;
}*listQuadYear;
typedef struct latLongs5{
    int quadLong;
    listQuadYear firstYear;
    struct latLongs5 *next;
}*listLatLongs5;
struct q5Node {
    int quadLat;
    listLatLongs5 firstLong;
    struct q5Node *next;
};
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
static listBType addType(listBType lista, const char *type){
    int c;
    if(lista==NULL || (c=strcmp(lista->type,type))>0){
        listBType new=malloc(sizeof(struct boroughTypes));
        if(new==NULL) return lista;
        new->type=strdup(type);
        if(new->type==NULL){
            free(new);
            return lista;
        }
        new->count=1;
        new->next=lista;
        return new;
    }
    if(c==0){
        (lista->count)++;
        return lista;
    }
    lista->next=addType(lista->next,type);
    return lista;
}
static q1List addToQ1(q1List q1, const char * borough, const char *type){ 
    int c; 
    if(q1==NULL || (c=strcmp(q1->borough,borough))>0){
        q1List new=calloc(1,sizeof(struct q1Node));
        if(new==NULL){
            return q1;
        }
        new->borough=strdup(borough);
        if(new->borough==NULL){
            free(new);
            return q1;
        }
        new->firstType=addType(new->firstType,type);
        if(new->firstType==NULL){
            free(new->borough);
            free(new);
            return q1;
        }
        new->next=q1;
        return new;
    }
    if(c==0){
        q1->firstType=addType(q1->firstType,type);
        return q1;
    }
    q1->next=addToQ1(q1->next,borough, type);
    return q1;
}
static q2List addToQ2(q2List q2, const char * borough, int hour){ 
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
static listLatLongs addLong(listLatLongs lista, int quadLong, const char *code){
    int c;
    if(lista==NULL || (c=lista->quadLong-quadLong)>0){
        listLatLongs new=malloc(sizeof(struct latLongs));
        if(new==NULL) return lista;
        new->codes=malloc(BLOQUE*sizeof(char*));
        if(new->codes==NULL){
            free(new);
            return lista;
        }
        new->codes[0]=strdup(code);
        if(new->codes[0]==NULL){
            free(new->codes);
            free(new);
            return lista;
        }
        new->quadLong=quadLong;
        new->next=lista;
        new->size=BLOQUE;
        new->dim=1;
        return new;
    }
    if(c==0){
        for(unsigned int i=0;i<lista->dim;i++){
            if(strcmp(lista->codes[i],code)==0)
                return lista;
        }
        if(lista->dim==lista->size){
            char **tmp=realloc(lista->codes,(lista->size+BLOQUE)*sizeof(char*));
            if(tmp==NULL)
                return lista;
            lista->codes=tmp;
            lista->size+=BLOQUE;
        }
        lista->codes[lista->dim]=strdup(code);
        if(lista->codes[lista->dim]==NULL){
            return lista;
        }
        lista->dim++;
        return lista;
    }
    lista->next=addLong(lista->next,quadLong,code);
    return lista;
}
static q3List addToQ3(q3List q3, int quadLat, int quadLong, const char *code){ 
    int c; 
    if(q3==NULL || (c=q3->quadLat-quadLat)>0){
        q3List new=calloc(1,sizeof(struct q3Node));
        if(new==NULL){
            return q3;
        }
        new->firstLong=addLong(new->firstLong,quadLong,code);
        if(new->firstLong==NULL) {
            free(new);
            return q3;
        }
        new->quadLat=quadLat;
        new->next=q3;
        return new;
    }
    if(c==0){
        q3->firstLong=addLong(q3->firstLong,quadLong,code);
        return q3;
    }
    q3->next=addToQ3(q3->next,quadLat,quadLong,code);
    return q3;
}
static listAgTy addTypeToAg(listAgTy lista, const char *type, int date){
    int c;
    if(lista==NULL || (c=strcmp(lista->type,type))>0){
        listAgTy new=malloc(sizeof(struct agencyTypes));
        if(new==NULL) return lista;
        new->type=strdup(type);
        if(new->type==NULL){
            free(new);
            return lista;
        }
        new->oldDay=new->recDay=date;
        new->next=lista;
        return new;
    }
    if(c==0){
        if(date>lista->recDay)
            lista->recDay=date;
        if(date<lista->oldDay)
            lista->oldDay=date;
        return lista;
    }
    lista->next=addTypeToAg(lista->next,type,date);
    return lista;
}
static listBorAg addAgToB(listBorAg lista, const char *agency, const char *type, int date){
    int c;
    if(lista==NULL || (c=strcmp(lista->agency,agency))>0){
        listBorAg new=calloc(1,sizeof(struct boroughAg));
        if(new==NULL) return lista;
        new->firstType=addTypeToAg(new->firstType,type,date);
        if(new->firstType==NULL){
            free(new);
            return lista;
        }
        new->agency=strdup(agency);
        if(new->agency==NULL){
            free(new->firstType);
            free(new);
            return lista;
        }
        new->next=lista;
        return new;
    }
    if(c==0){
        lista->firstType=addTypeToAg(lista->firstType,type,date);
        return lista;
    }
    lista->next=addAgToB(lista->next,agency,type,date);
    return lista;
}
static q4List addToQ4(q4List q4, const char *borough, const char *agency, const char *type, int date){ 
    int c; 
    if(q4==NULL || (c=strcmp(q4->borough,borough))>0){
        q4List new=calloc(1,sizeof(struct q4Node));
        if(new==NULL){
            return q4;
        }
        new->firstAgency=addAgToB(new->firstAgency,agency,type,date);
        if(new->firstAgency==NULL){
            free(new);
            return q4;
        }
        new->borough=strdup(borough);
        if(new->borough==NULL){
            free(new->firstAgency);
            free(new);
            return q4;
        }
        new->next=q4;
        return new;
    }
    if(c==0){
        q4->firstAgency=addAgToB(q4->firstAgency,agency,type,date);
        return q4;
    }
    q4->next=addToQ4(q4->next,borough,agency,type,date);
    return q4;
}

static listQuadYearMonth addMonth(listQuadYearMonth lista, int month){
    int c;
    if(lista==NULL||(c=lista->month-month)>0){
        listQuadYearMonth new=malloc(sizeof(struct quadYearMonth));
        if(new==NULL) return lista;
        new->month=month;
        new->recYTD=1;
        new->next=lista;
        return new;
    }
    if(c==0){
        lista->recYTD++;
        return lista;
    }
    lista->next=addMonth(lista->next,month);
    return lista;
}
static listQuadYear addYear(listQuadYear lista, int year, int month){
    int c;
    if(lista==NULL || (c=lista->year-year)>0){
        listQuadYear new=calloc(1,sizeof(struct quadYear));
        if(new==NULL) return lista;
        new->firstMonth=addMonth(new->firstMonth,month);
        new->year=year;
        new->next=lista;
        return new;
    }
    if(c==0){
        lista->firstMonth=addMonth(lista->firstMonth,month);
        return lista;
    }
    lista->next=addYear(lista->next,year,month);
    return lista;
}
static listLatLongs5 addLong5(listLatLongs5 lista, int quadLong, int year, int month){
    int c;
    if(lista==NULL || (c=lista->quadLong-quadLong)>0){
        listLatLongs5 new=calloc(1,sizeof(struct latLongs5));
        if(new==NULL) return lista;
        new->firstYear=addYear(new->firstYear,year,month);
        if(new->firstYear==NULL){
            free(new);
            return lista;
        }
        new->quadLong=quadLong;
        new->next=lista;
        return new;
    }
    if(c==0){
        lista->firstYear=addYear(lista->firstYear,year,month);
        return lista;
    }
    lista->next=addLong5(lista->next,quadLong,year,month);
    return lista;
}
static q5List addToQ5(q5List q5, int quadLat, int quadLong, int year, int month, int YTD){ 
    int c; 
    if(q5==NULL || (c=q5->quadLat-quadLat)>0){
        q5List new=calloc(1,sizeof(struct q5Node));
        if(new==NULL){
            return q5;
        }
        new->firstLong=addLong5(new->firstLong,quadLong,year,month);
        if(new->firstLong==NULL){
            free(new);
            return q5;
        }
        new->quadLat=quadLat;
        new->next=q5;
        return new;
    }
    if(c==0){
        q5->firstLong=addLong5(q5->firstLong,quadLong,year,month);
        return q5;
    }
    q5->next=addToQ5(q5->next,quadLat,quadLong,year,month,YTD);
    return q5;
}

void addToQueries(queryADT q,const char * agency, const char * code, const char * type, const char * status, const char * borough, int year, int month, int day, int hour, double lat, double lon, int yMax, int yMin){
    if((lat>90 || lat<-90)|| (lon>180 || lon<-180)|| (hour>23 || hour<0) || (month<1 || month>12))
        return; 
    int quadLat,quadLong;
    quadLat=floor(lat*10);
    quadLong=floor(lon*10);
    int isClosed=strcmp(status,CERRADO)==0;
    q->q1=addToQ1(q->q1,borough,type);
    if(isClosed){
        q->q2=addToQ2(q->q2,borough,hour);
    }
    if(strcmp(status,"Open")==0)
        q->q3=addToQ3(q->q3,quadLat,quadLong,code);
    if(year>=yMin && year<=yMax){
        q->q4=addToQ4(q->q4,borough,agency,type,days_from_civil(year,month,day));
        if(isClosed){
            q->q5=addToQ5(q->q5,quadLat,quadLong,year,month,0);
        }
    }
}

void q1_foreach(queryADT q, Q1Visitor v, void *userData){
    for(q1List b = q->q1; b != NULL; b = b->next){
        for(listBType t = b->firstType; t != NULL; t = t->next){
            v(b->borough, t->type, t->count, userData);
        }
    }
}

void q2_foreach(queryADT q, Q2Visitor v, void *userData){
    for(q2List b=q->q2; b!=NULL; b=b->next){
        for(int i=0;i<HOURS;i++){
            v(b->borough,i,b->count[i],userData);
        }
    }
}

void q3_foreach(queryADT q, Q3Visitor v, void *userData){
    for(q3List lat=q->q3; lat!=NULL; lat=lat->next){
        for(listLatLongs lon=lat->firstLong; lon!=NULL; lon=lon->next){
            v(lat->quadLat,lon->quadLong,lon->dim,userData);
        }
    }
}

void q4_foreach(queryADT q, Q4Visitor v, void *userData){
    for(q4List b=q->q4; b!=NULL; b=b->next){
        for(listBorAg a=b->firstAgency; a!=NULL; a=a->next){
            for(listAgTy t=a->firstType;t!=NULL;t=t->next){
                v(b->borough,a->agency,t->type,(t->recDay-t->oldDay),userData);
            }
        }
    }
}

void q5_foreach(queryADT q, Q5Visitor v, void *userData){
    for(q5List lat=q->q5;lat!=NULL;lat=lat->next){
        for(listLatLongs5 lon=lat->firstLong;lon!=NULL;lon=lon->next){
            for(listQuadYear y=lon->firstYear;y!=NULL;y=y->next){
                int YTD=0;
                for(listQuadYearMonth m=y->firstMonth;m!=NULL;m=m->next){
                    YTD+=m->recYTD;                                                 //con la variable YTD vas acumulando los reclamos del año para cada mes
                    v(lat->quadLat,lon->quadLong,y->year,m->month,YTD,userData);
                }
            }
        }
    }
}
static void freeBTypes(listBType t){
    while(t!=NULL){
        listBType aux = t->next;
        free(t->type);
        free(t);
        t = aux;
    }
}
static void freeQ1(q1List q1){
    while(q1!=NULL){
        q1List aux = q1->next;
        free(q1->borough);
        freeBTypes(q1->firstType);
        free(q1);
        q1 = aux;
    }
}
static void freeQ2(q2List q2){
    while(q2!=NULL){
        q2List aux = q2->next;
        free(q2->borough);
        free(q2);
        q2 = aux;
    }
}
static void freeLatLongs(listLatLongs l){
    while(l!=NULL){
        listLatLongs aux = l->next;
        for(unsigned int i = 0; i < l->dim; i++)
            free(l->codes[i]);
        free(l->codes);
        free(l);
        l = aux;
    }
}
static void freeQ3(q3List q3){
    while(q3!=NULL){
        q3List aux = q3->next;
        freeLatLongs(q3->firstLong);
        free(q3);
        q3 = aux;
    }
}
static void freeAgTypes(listAgTy t){
    while(t!=NULL){
        listAgTy aux = t->next;
        free(t->type);
        free(t);
        t = aux;
    }
}
static void freeBorAg(listBorAg a){
    while(a!=NULL){
        listBorAg aux = a->next;
        free(a->agency);
        freeAgTypes(a->firstType);
        free(a);
        a = aux;
    }
}
static void freeQ4(q4List q4){
    while(q4!=NULL){
        q4List aux = q4->next;
        free(q4->borough);
        freeBorAg(q4->firstAgency);
        free(q4);
        q4 = aux;
    }
}
static void freeMonths(listQuadYearMonth m){
    while(m!=NULL){
        listQuadYearMonth aux=m->next;
        free(m);
        m=aux;
    }
}
static void freeYears(listQuadYear y){
    while(y!=NULL){
        listQuadYear aux = y->next;
        freeMonths(y->firstMonth);
        free(y);
        y = aux;
    }
}
static void freeLongs5(listLatLongs5 l){
    while(l!=NULL){
        listLatLongs5 aux = l->next;
        freeYears(l->firstYear);
        free(l);
        l = aux;
    }
}
static void freeQ5(q5List q5){
    while(q5!=NULL){
        q5List aux = q5->next;
        freeLongs5(q5->firstLong);
        free(q5);
        q5 = aux;
    }
}
void freeQueries(queryADT q){
    if(q == NULL)
        return;
    freeQ1(q->q1);
    freeQ2(q->q2);
    freeQ3(q->q3);
    freeQ4(q->q4);
    freeQ5(q->q5);
    free(q);
}
