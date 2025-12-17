#include "queries.h"
struct reclameCDT{
    char * agency;
    char * code;
    char * name;
    char * status;
    char * borough;
    int year;
    int month;
    int day;
    int hour;
    int quadLat;
    int quadLong;
};
struct node{
    reclameADT reclamo;
    unsigned int resp; //en resp se guarda la respuesta de cada query
    int old;           //en Q4 se guardan los dias desde civil del mas viejo
    int recent;        //en Q4 se guardan los dias desde civil del mas nuevo
    char ** codes;     //en Q3 se guardan todos los codigos de los tipos de reclamos
    struct node * next;
};
struct queryCDT{ 
    TList queries[CANT]; //en queries[0] hay una lista con todos los reclamos para el free
};                       //despues en queries[n] esta la lista de la n-esima query
static int days_from_civil(int y, int m, int d) {
   y -= m <= 2;
   int era = (y >= 0 ? y : y - 399) / 400;
   int yoe = y - era * 400;
   int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
   int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
   return era * 146097 + doe - 719468;
}
queryADT newQueries(){
    return calloc(1,sizeof(struct queryCDT));
}
static reclameADT addReclame (const char * agency, const char * code, const char * name, const char * status, const char * borough, int year, int month, int hour, int day, int quadLat, int quadLong){
    reclameADT new=malloc(sizeof(*new));
    if(new==NULL){
        return NULL;
    }
    new->agency=strdup(agency);
    new->code=strdup(code);
    new->name=strdup(name);
    new->status=strdup(status);
    new->borough=strdup(borough);
    if(new->agency==NULL || new->code==NULL|| new->name==NULL || new->status==NULL||new->borough==NULL){
        free(new->agency);
        free(new->code);
        free(new->status);
        free(new->borough);
        free(new);
        return NULL;
    }
    new->year=year;
    new->month=month;
    new->day=day;
    new->hour=hour;
    new->quadLat=quadLat;
    new->quadLong=quadLong;
    return new;
}
typedef int (*cmpReclamo)(const reclameADT, const reclameADT);
typedef int (*filReclamo)(const reclameADT);
static int cmpQ1(const reclameADT a, const reclameADT b){
    int c = strcmp(a->borough, b->borough);
    if (c != 0)
        return c;
    return strcmp(a->code, b->code);
}
static int cmpQ2(const reclameADT a, const reclameADT b){
    int c = strcmp(a->borough, b->borough);
    if (c != 0)
        return c;
    return a->hour - b->hour;
}
static int cmpQ3(const reclameADT a, const reclameADT b){
    int c = a->quadLat - b->quadLat;
    if (c != 0)
        return c;
    return a->quadLong - b->quadLong;
}
static int cmpQ4(const reclameADT a, const reclameADT b){
    int c= strcmp(a->borough,b->borough);
    if (c!=0)
        return c;
    c=strcmp(a->agency,b->agency);
    if (c!=0)
        return c;
    return strcmp(a->code,b->code);
}
static int cmpQ5(const reclameADT a, const reclameADT b){
    int c=a->quadLat-b->quadLat;
    if(c!=0)
        return c;
    c=a->quadLong-b->quadLong;
    if(c!=0)
        return c;
    c=a->year-b->year;
    if(c!=0)
        return c;
    return a->month-b->month;
}
static cmpReclamo cmpVec[CANT] = {
    NULL,
    cmpQ1,
    cmpQ2,
    cmpQ3,
    cmpQ4,
    cmpQ5
};
static TList allRec(TList all, const reclameADT reclamo){
    TList new=malloc(sizeof(struct node));
    if(new==NULL)
        return all;
    new->reclamo=reclamo;
    new->next=all;
    return new;
}
static TList addToQ(TList q, const reclameADT reclamo, cmpReclamo cmp){
    int c;
    if(q==NULL || ((c=cmp(q->reclamo,reclamo))>0)){
        TList new=malloc(sizeof(struct node));
        if(new==NULL){
            return q;
        }
        new->reclamo=reclamo;
        new->resp=1;
        new->next=q;
        return new;
    }
    if(c==0){
        (q->resp)++;
        return q;
    }
    q->next=addToQ(q->next,reclamo, cmp);
    return q;
}
static TList addToQ3(TList q3, const reclameADT reclamo, cmpReclamo cmp){
    int c;
    if(q3==NULL || ((c=cmp(q3->reclamo,reclamo))>0)){
        TList new=malloc(sizeof(struct node));
        if(new==NULL){
            return q3;
        }
        new->reclamo=reclamo;
        new->resp=1;
        new->codes=malloc(sizeof(char *));
        if(new->codes==NULL){
            free(new);
            return q3;
        }
        new->codes[0]=strdup(reclamo->code);
        if(new->codes[0]==NULL){
            free(new->codes);
            free(new);
            return q3;
        }
        new->next=q3;
        return new;
    }
    if(c==0){
        for(int i=0;i<q3->resp;i++){
            if(strcmp(q3->codes[i],reclamo->code)==0)
                return q3;
        }
        char ** tmp=realloc(q3->codes,(q3->resp+1)*sizeof(char *));
        if(tmp==NULL)
            return q3;
        q3->codes=tmp;
        q3->codes[q3->resp]=strdup(reclamo->code);
        if(q3->codes[q3->resp]==NULL)
            return q3;
        q3->resp++;
        return q3;
    }
    q3->next=addToQ3(q3->next,reclamo, cmp);
    return q3;
}
static TList addToQ4(TList q4, const reclameADT reclamo, cmpReclamo cmp){
    int c;
    if(q4==NULL || ((c=cmp(q4->reclamo,reclamo))>0)){
        TList new=malloc(sizeof(struct node));
        if(new==NULL){
            return q4;
        }
        new->reclamo=reclamo;
        new->resp=0;
        new->recent=new->old=days_from_civil(reclamo->year,reclamo->month,reclamo->day);
        new->next=q4;
        return new;
    }
    if(c==0){
        int n;
        if((n=days_from_civil(reclamo->year,reclamo->month,reclamo->day))>q4->recent)
            q4->recent=n;
        else if(n<q4->old)
            q4->old=n;
        q4->resp=q4->recent-q4->old;
        return q4;
    }
    q4->next=addToQ4(q4->next,reclamo, cmp);
    return q4;
}
static int sameQY(const reclameADT a, const reclameADT b){
    return a->quadLat == b->quadLat &&
           a->quadLong == b->quadLong &&
           a->year == b->year;
}
static TList addToQ5(TList q5, const reclameADT reclamo, cmpReclamo cmp, int YTD){
    int c;
    if(q5==NULL || ((c=cmp(q5->reclamo,reclamo))>0)){
        TList new=malloc(sizeof(struct node));
        if(new==NULL){
            return q5;
        }
        new->reclamo=reclamo;
        new->resp=YTD+1;
        new->next=q5;
        return new;
    }
    if(c==0){
        (q5->resp)++;
        return q5;
    }
    if(sameQY(q5->reclamo,reclamo)){
        YTD=q5->resp;
    }
    q5->next=addToQ5(q5->next,reclamo, cmp, YTD);
    return q5;
}
static void respQ5(TList q5, const reclameADT reclamo){ //actualiza los resp de los meses posteriores de ese mismo año(les suma 1)
    while(q5!=NULL && q5->reclamo!=reclamo)
        q5=q5->next;
    if(q5==NULL) return;
    q5=q5->next;
    while(q5!=NULL && sameQY(q5->reclamo,reclamo)){
        (q5->resp)++;
        q5=q5->next;
    }
}
void addToQueries(queryADT q,const char * agency, const char * code, const char * status, const char * borough, int year, int month, int hour, int day, int quadLat, int quadLong){
    if(q==NULL)
        return;
    reclameADT newReclame=addReclame(agency,code,status,borough,year,month,hour,day,quadLat,quadLong);
    if(newReclame==NULL)
        return;
    q->queries[0]=allRec(q->queries[0],newReclame);
    q->queries[1]=addToQ(q->queries[1],newReclame,cmpVec[1]);
    q->queries[4]=addToQ4(q->queries[4],newReclame,cmpVec[4]);
    if(strcmp(status,cerrado)==0){
        q->queries[2]=addToQ(q->queries[2],newReclame,cmpVec[2]);
        q->queries[5]=addToQ5(q->queries[5],newReclame,cmpVec[5],0);
        respQ5(q->queries[5],newReclame);
    }
    if(strcmp(status,"Open")==0){
        q->queries[3]=addToQ3(q->queries[3],newReclame,cmpVec[3]);
    }
}
static void freeRec(reclameADT r){
    if(r==NULL)
        return;
    free(r->agency);
    free(r->borough);
    free(r->code);
    free(r->status);
    free(r);
}
static void freeQ(TList q){   //freeQ se usa para todas las queries menos la 3
    if(q==NULL)
        return;
    freeQ(q->next);
    free(q);
}
static void freeQ3(TList q3){ //freeQ3 se usa para liberar q3 ya que usa el **char codes
    if(q3==NULL)
        return;
    freeQ3(q3->next);
    for(int i=0;i<q3->resp;i++)
        free(q3->codes[i]);
    free(q3->codes);
    free(q3);
}

static void freeAllRec(TList all){
    if(all==NULL)
        return;
    freeAllRec(all->next);
    freeRec(all->reclamo);
    free(all);
}
void freeQueries(queryADT q){
    freeQ(q->queries[1]);
    freeQ(q->queries[2]);
    freeQ(q->queries[4]);
    freeQ(q->queries[5]);
    freeQ3(q->queries[3]);
    freeAllRec(q->queries[0]);
    free(q);
}