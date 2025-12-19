#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./back/queries.h"
#include "./front/lector.h"
#include "./front/fromQueryToFile.h"
#define YEAR_MIN_DEFAULT 1900
#define YEAR_MAX_DEFAULT 2100
int main(int argc, char *argv[]) {
    // Validar que se reciban exactamente 2 parámetros
    if (argc<3 || argc>5) {
        fprintf(stderr, "Uso: %s <archivo_reclamos> <archivo_tipos>\n", argv[0]);
        return 1;
    }
    int yMin, yMax;
    yMin=YEAR_MIN_DEFAULT;
    yMax=YEAR_MAX_DEFAULT;
    if(argc>3){
        yMin=atoi(argv[3]);
    }
    if(argc>4){
        yMax=atoi(argv[4]);
    }
    queryADT q=newQueries();
    typevector types = newType();
    // PASO 1: Leer tipos primero (archivo argv[2])
    if (!readTypes(argv[2], types)) {
        fprintf(stderr, "Error al leer tipos\n");
        freeQueries(q);
        freeTypes(types);
        return 1;
    }

    
    // PASO 2: Leer reclamos validando contra los tipos (archivo argv[1])
    if (!readRequest(argv[1],types,q,yMax,yMin)) {
        fprintf(stderr, "Error al leer reclamos\n");
        freeQueries(q);
        freeTypes(types);
        return 1;
    }
    if(!queriesToFile(q,"query1.csv","query1.html","query2.csv","query2.html","query3.csv","query3.html","query4.csv","query4.html","query5.csv","query5.html")){
        fprintf(stderr,"Error al exportar los archivos\n");
        freeQueries(q);
        freeTypes(types);
    }
    freeQueries(q);
    freeTypes(types);
    
    return 0;
}
