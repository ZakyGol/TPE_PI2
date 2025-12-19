#include "fromQueryToFile.h"
#include <stdio.h>
#include <stdlib.h>
#include "../back/queries.h"
#include "../back/htmlTable.h"
typedef struct {
    FILE *csv;
    htmlTable html;
} qFiles;
static void q1Writer(const char *borough,const char *type,unsigned int count, void *userData){
    qFiles *arc = userData;
    char buf[16];
    sprintf(buf, "%u", count);
    fprintf(arc->csv, "%s;%s;%u\n", borough, type, count);
    addHTMLRow(arc->html, borough, type, buf);
}
static void q2Writer(const char *borough, int hour, unsigned int count, void *userData){
    qFiles *arc= userData;
    char hourString[5];
    char countString[16];
    sprintf(hourString,"%d",hour);
    sprintf(countString,"%u", count);
    fprintf(arc->csv,"%s;%d;%u\n",borough,hour,count);
    addHTMLRow(arc->html,borough,hourString,countString);
}
static void q3Writer(int quadLat, int quadLong, unsigned int dim, void *userData){
    qFiles *arc= userData;
    char quadLatString[5];
    char quadLongString[5];
    char dimString[10];
    sprintf(quadLatString,"%d",quadLat);
    sprintf(quadLongString,"%d",quadLong);
    sprintf(dimString,"%u",dim);
    fprintf(arc->csv,"%d;%d;%u\n",quadLat,quadLong,dim);
    addHTMLRow(arc->html,quadLatString,quadLongString,dimString);
}
static void q4Writer(const char *borough, const char *agency, const char *type, int daysBetween, void *userData){
    qFiles *arc=userData;
    char daysBetweenString[10];
    sprintf(daysBetweenString,"%d",daysBetween);
    fprintf(arc->csv,"%s;%s;%s;%d\n",borough,agency,type,daysBetween);
    addHTMLRow(arc->html,borough,agency,type,daysBetweenString);
}
static void q5Writer(int quadLat, int quadLong, int year, int month, int recYTD, void *userData){
    qFiles *arc=userData;
    char quadLatString[5];
    char quadLongString[5];
    char yearString[5];
    char monthString[3];
    char recYTDString[10];
    sprintf(quadLatString,"%d",quadLat);
    sprintf(quadLongString,"%d",quadLong);
    sprintf(yearString,"%d",year);
    sprintf(monthString,"%d",month);
    sprintf(recYTDString,"%d",recYTD);
    fprintf(arc->csv,"%d;%d;%d;%d;%d\n",quadLat,quadLong,year,month,recYTD);
    addHTMLRow(arc->html,quadLatString,quadLongString,yearString,monthString,recYTDString);
}
static int q1ToFile(queryADT q, const char *csvFile, const char *htmlFile){
    FILE *csv = fopen(csvFile, "w");
    if(!csv) return 0;

    htmlTable html = newTable(htmlFile, 3, "borough","type","requests");
    if(!html){
        fclose(csv);
        return 0;
    }

    fprintf(csv, "borough;type;requests\n");

    qFiles arc = { csv, html };
    q1_foreach(q, q1Writer, &arc);
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
static int q2ToFile(queryADT q, const char *csvFile, const char *htmlFile){
    FILE *csv = fopen(csvFile, "w");
    if(!csv) return 0;

    htmlTable html = newTable(htmlFile, 3, "borough","dayHour","requests");
    if(!html){
        fclose(csv);
        return 0;
    }

    fprintf(csv, "borough;dayHour;requests\n");
    qFiles arc = { csv, html };
    q2_foreach(q, q2Writer, &arc);
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
static int q3ToFile(queryADT q, const char *csvFile, const char *htmlFile){
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
    qFiles arc={ csv, html };
    q3_foreach(q,q3Writer,&arc);
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
static int q4ToFile(queryADT q, const char *csvFile, const char *htmlFile){
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
    qFiles arc={ csv, html };
    q4_foreach(q,q4Writer,&arc);
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}

static int q5ToFile(queryADT q, const char *csvFile, const char *htmlFile){
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
    qFiles arc= {csv, html};
    q5_foreach(q,q5Writer,&arc);
    fclose(csv);
    closeHTMLTable(html);
    return 1;
}
    
int queriesToFile(queryADT q, const char *csvFile1, const char *htmlFile1, const char *csvFile2, const char *htmlFile2, const char *csvFile3, const char *htmlFile3, const char *csvFile4, const char *htmlFile4,const char *csvFile5, const char *htmlFile5){
    return (q1ToFile(q,csvFile1,htmlFile1) &&
    q2ToFile(q,csvFile2,htmlFile2) &&
    q3ToFile(q,csvFile3,htmlFile3) &&
    q4ToFile(q,csvFile4,htmlFile4) &&
    q5ToFile(q,csvFile5,htmlFile5));
}
