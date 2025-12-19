#include "fromQueryToFile.h"
#include <stdio.h>
#include <stdlib.h>
#include "../backend/queries.c"
#include "../htmlTable.c"
static int q1ToFile(q1List q1, const char *csvFile, const char *htmlFile){
    if (q1 == NULL){
        return 0;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return 0;
    }
    htmlTable html = newTable(htmlFile, 3, "borough", "type", "requests"); 
    if(html==NULL){
      fclose(csv);
      return 0;
    }
    // inserto encabezado
    fprintf(csv, "borough;type;requests\n");
    listBType aux;
    while (q1 != NULL){
        aux=q1->firstType;
        while(aux!=NULL){
            char respToChar[10];  
            sprintf(respToChar, "%d", aux->count);
            fprintf(csv, "%s;%s;%d\n", q1->borough, aux->type, aux->count);
            addHTMLRow(html, q1->borough, aux->type, respToChar);
            aux=aux->next;
        }
        q1=q1->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
static int q2ToFile(q2List q2, const char *csvFile, const char *htmlFile){
    if (q2 == NULL){
        return 0;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return 0;
    }
    htmlTable html = newTable(htmlFile, 3, "borough", "dayHour", "requests"); 
    if(html==NULL){
      fclose(csv);
      return 0;
    }
    // inserto encabezado
    fprintf(csv, "borough;dayHour;requests\n");
    while (q2 != NULL){
        char respToChar[10];
        char dayHour[4];
        for(int i=0;i<HOURS;i++){  
            sprintf(respToChar, "%d", q2->count[i]);
            sprintf(dayHour, "%d", i);
            fprintf(csv, "%s;%d;%d\n", q2->borough, i, q2->count[i]);
            addHTMLRow(html, q2->borough, dayHour, respToChar);
        }
        q2=q2->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
static int q3ToFile(q3List q3, const char *csvFile, const char *htmlFile){
    if (q3 == NULL){
        return 0;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return 0;
    
    }
    htmlTable html = newTable(htmlFile, 3, "quadLat", "quadLong", "distinticOpenTypes"); 
    if(html==NULL){
      fclose(csv);
      return 0;
    }
    // inserto encabezado
    fprintf(csv, "quadLat;quadLong;distinticOpenTypes\n");
    listLatLongs aux;
    while (q3 != NULL){
        aux=q3->firstLong;
        while(aux!=NULL){
            char respToChar[10];
            char quadLong[5];
            char quadLat[5]; 
            sprintf(respToChar, "%ld", aux->dim);
            sprintf(quadLong, "%d", aux->quadLong);
            sprintf(quadLat, "%d", q3->quadLat);
            fprintf(csv, "%d;%d;%ld\n", q3->quadLat, aux->quadLong, aux->dim);
            addHTMLRow(html, quadLat, quadLong, respToChar);
            aux=aux->next;
        }
        q3=q3->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
static int q4ToFile(q4List q4, const char *csvFile, const char *htmlFile){
    if (q4 == NULL){
        return 0;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return 0;
    
    }
    htmlTable html = newTable(htmlFile, 4, "neighbourhood", "agency", "type","daysBetween" ); 
    if(html==NULL){
      fclose(csv);
      return 0;
    }
    // inserto encabezado
    fprintf(csv, "neighbourhood;agency;type;daysBetween\n");
    listBorAg aux;
    listAgTy aux2;
    while (q4 != NULL){
        aux=q4->firstAgency;
        while(aux!=NULL){
            aux2=aux->firstType;
            while(aux2!=NULL){
                char respToChar[10];
                sprintf(respToChar, "%d", (aux2->recDay-aux2->oldDay));
                fprintf(csv, "%s;%s;%s;%d\n", q4->borough, aux->agency, aux2->type,(aux2->recDay-aux2->oldDay));
                addHTMLRow(html, q4->borough, aux->agency, aux2->type, respToChar);
                aux2=aux2->next;
            }
            aux=aux->next;
        }
        q4=q4->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}

static int q5ToFile(q5List q5, const char *csvFile, const char *htmlFile){
    if (q5 == NULL){
        return 0;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return 0;
    
    }
    htmlTable html = newTable(htmlFile, 5, "quadLat", "quadLong", "year","month", "closedRequestsYTD" ); 
    if(html==NULL){
      fclose(csv);
      return 0;
    }
    // inserto encabezado
    fprintf(csv, "quadLat;quadLong;year;month;closedRequestsYTD\n");
    listLatLongs5 aux;
    listQuadYear aux2;
    listQuadYearMonth aux3;
    while (q5 != NULL){
        aux=q5->firstLong;
        while(aux!=NULL){
            aux2=aux->firstYear;
            while(aux2!=NULL){
                aux3=aux2->firstMonth;
                int YTD=0;
                while(aux3!=NULL){
                    YTD+=aux3->recYTD;
                    char respToChar[10];
                    char month[3];
                    char year[5];
                    char quadLong[5];
                    char quadLat[5];
                    sprintf(respToChar, "%d", YTD);
                    sprintf(month, "%d", aux3->month);
                    sprintf(year, "%d", aux2->year);
                    sprintf(quadLong, "%d", aux->quadLong);
                    sprintf(quadLat, "%d", q5->quadLat);
                    fprintf(csv, "%d;%d;%d;%d;%d\n", q5->quadLat, aux->quadLong, aux2->year, aux3->month,YTD);
                    addHTMLRow(html, quadLat, quadLong, year, month, respToChar);
                    aux3=aux3->next;
                    }
                aux2=aux2->next;
            }
            aux=aux->next;
        }
        q5=q5->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
    
int queriesToFile(queryADT q, const char *csvFile1, const char *htmlFile1, const char *csvFile2, const char *htmlFile2, const char *csvFile3, const char *htmlFile3, const char *csvFile4, const char *htmlFile4,const char *csvFile5, const char *htmlFile5){
    return (q1ToFile(q->q1,csvFile1,htmlFile1) &&
    q2ToFile(q->q2,csvFile2,htmlFile2) &&
    q3ToFile(q->q3,csvFile3,htmlFile3) &&
    q4ToFile(q->q4,csvFile4,htmlFile4) &&
    q5ToFile(q->q5,csvFile5,htmlFile5));
}