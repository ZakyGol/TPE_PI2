#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htmlTable.h"
#include "queries.h"
#include "fromQueryToFile.h"

static void q1ToFile(const TList q1, const char *csvFile, const char *htmlFile){
    if (q1 == NULL){
        return;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return;
    }
    htmlTable html = newTable(htmlFile, 3, "borough", "type", "requests"); 
    if(html==NULL){
      fclose(csv);
      return;
    }
    // inserto encabezado
    fprintf(csv, "borough;type;requests\n");
    while (q1 != NULL){
        char respToChar[10];  
        sprintf(respToChar, "%d", q1->resp);
        fprintf(csv, "%s;%s;%d\n", q1->reclamo->borough, q1->reclamo->name, q1->resp);
        addHTMLRow(html, q1->reclamo->borough, q1->reclamo->name, respToChar);
        q1=q1->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return;
}
static void q2ToFile(const TList q2, const char *csvFile, const char *htmlFile){
    if (q2 == NULL){
        return;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return;
    }
    htmlTable html = newTable(htmlFile, 3, "borough", "dayHour", "requests"); 
    if(html==NULL){
      fclose(csv);
      return;
    }
    // inserto encabezado
    fprintf(csv, "borough;dayHour;requests\n");
    while (q2 != NULL){
        char respToChar[10];
        char dayHour[2];  
        sprintf(respToChar, "%d", q2->resp);
        sprintf(dayHour, "%d", q2->reclamo->hour);
        fprintf(csv, "%s;%d;%d\n", q2->reclamo->borough, q2->reclamo->hour, q2->resp);
        addHTMLRow(html, q2->reclamo->borough, dayHour, respToChar);
        q2=q2->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return;
}
static void q3ToFile(const TList q3, const char *csvFile, const char *htmlFile){
    if (q3 == NULL){
        return;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return;
    
    }
    htmlTable html = newTable(htmlFile, 3, "quadLat", "quadLong", "distinticOpenTypes"); 
    if(html==NULL){
      fclose(csv);
      return;
    }
    // inserto encabezado
    fprintf(csv, "quadLat;quadLong;distinticOpenTypes\n");
    while (q3 != NULL){
        char respToChar[10];
        char quadLong[5];
        char quadLat[5]; 
        sprintf(respToChar, "%d", q3->resp);
        sprintf(quadLong, "%d", q3->reclamo->quadLong);
        sprintf(quadLat, "%d", q3->reclamo->quadLat);
        fprintf(csv, "%d;%d;%d\n", q3->reclamo->quadLat, q3->reclamo->quadLong, q3->resp);
        addHTMLRow(html, quadLat, quadLong, respToChar);
        q3=q3->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return;
}
static void q4ToFile(const TList q4, const char *csvFile, const char *htmlFile){
    if (q4 == NULL){
        return;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return;
    
    }
    htmlTable html = newTable(htmlFile, 4, "neighbourhood", "agency", "type","daysBetween" ); 
    if(html==NULL){
      fclose(csv);
      return;
    }
    // inserto encabezado
    fprintf(csv, "quadLat;quadLong;distinticOpenTypes;daysBetween\n");
    while (q4 != NULL){
        char respToChar[10];
        sprintf(respToChar, "%d", q4->resp);
        fprintf(csv, "%s;%s;%s;%d\n", q4->reclamo->borough, q4->reclamo->agency, q4->reclamo->name, q4->resp);
        addHTMLRow(html, q4->reclamo->borough, q4->reclamo->agency, q4->reclamo->name, respToChar);
        q4=q4->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return;
}
static void q5ToFile(const TList q5, const char *csvFile, const char *htmlFile){
    if (q5 == NULL){
        return;
    }   
    FILE *csv = fopen(csvFile, "w");
    if (csv == NULL){
        return;
    
    }
    htmlTable html = newTable(htmlFile, 5, "quadLat", "quadLong", "year","month", "closedRequestsYTD" ); 
    if(html==NULL){
      fclose(csv);
      return;
    }
    // inserto encabezado
    fprintf(csv, "quadLat;quadLong;year;month;closedRequestsYTD\n");
    while (q5 != NULL){
        char respToChar[10];
        char month[2];
        char year[4];
        char quadLong[5];
        char quadLat[5];
        sprintf(respToChar, "%d", q5->resp);
        sprintf(month, "%d", q5->reclamo->month);
        sprintf(year, "%d", q5->reclamo->year);
        sprintf(quadLong, "%d", q5->reclamo->quadLong);
        sprintf(quadLat, "%d", q5->reclamo->quadLat);
        fprintf(csv, "%d;%d;%d;%d;%d\n", q5->reclamo->quadLat, q5->reclamo->quadLong, q5->reclamo->year, q5->reclamo->month,q5->resp);
        addHTMLRow(html, quadLat, quadLong, year, month, respToChar);
        q5=q5->next;
    }
    fclose(csv);
    closeHTMLTable(html);
    return;
}
void queriesToFile(const queryADT q, const char *csvFile1, const char *htmlFile1, const char *csvFile2, const char *htmlFile2, const char *csvFile3, const char *htmlFile3, const char *csvFile4, const char *htmlFile4,const char *csvFile5, const char *htmlFile5){
    q1ToFile(q->queries[1],csvFile1,htmlFile1);
    q2ToFile(q->queries[2],csvFile2,htmlFile2);
    q3ToFile(q->queries[3],csvFile3,htmlFile3);
    q4ToFile(q->queries[4],csvFile4,htmlFile4);
    q5ToFile(q->queries[5],csvFile5,htmlFile5);
}