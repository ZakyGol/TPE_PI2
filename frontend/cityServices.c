#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../backend/queries.h"
#include "lector.h"
#include "fromQueryToFile.h"
#define YEAR_MIN_DEFAULT 1900
#define YEAR_MAX_DEFAULT 2100

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s requests.csv types.csv [yMin] [yMax]\n", prog);
}

int main(int argc, char *argv[]) {

    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Uso: %s <archivo_reclamos> <archivo_tipos>\n", argv[0]);
        return 1;
    }

    const char *requestsPath = argv[1];
    const char *typesPath    = argv[2];

    int yMin, yMax;
    yMin=YEAR_MIN_DEFAULT;
    yMax=YEAR_MAX_DEFAULT;

    if (argc == 4) {
        yMin = atoi(argv[3]); //convierte el argumento a numero
        if (yMin <= 0) {
            fprintf(stderr, "Invalid yMin\n");
            return 1;
        }
        yMax = YEAR_MAX_DEFAULT; //es de ese año en adelante
    }

    if (argc == 5) {
        yMin = atoi(argv[3]);
        yMax = atoi(argv[4]);
        if (yMin <= 0 || yMax <= 0 || yMax < yMin) {
            fprintf(stderr, "Error en el rango de años\n");
            return 1;
        }   
    }   

    /* Backend */
    queryADT queries = newQueries();
    if (!queries) {
        fprintf(stderr, "Error por falta de memoria\n");
        return 1;
    }

    /* Frontend: types */
    typevector types = newType();
    if (!types) {
        fprintf(stderr, "Error por falta de memoria\n");
        freeQueries(queries);
        return 1;
    }

    if (!readTypes(typesPath, types)) {
        fprintf(stderr, "Error al leer los archivos de types\n");
        freeTypes(types);
        freeQueries(queries);
        return 1;
    }

    if (!readRequest(requestsPath, types, queries, yMin, yMax)) {
        fprintf(stderr, "Error al leer los archivos de reclamos\n");
        freeTypes(types);
        freeQueries(queries);
        return 1;
    }

    if(!queriesToFile(queries,
                    "query1.csv","query1.html",
                    "query2.csv","query2.html",
                    "query3.csv","query3.html",
                    "query4.csv","query4.html",
                    "query5.csv","query5.html")){
        fprintf(stderr,"Error al exportar los archivos\n");
        freeQueries(queries);
        freeTypes(types);
    }
    freeTypes(types);
    freeQueries(queries);

    return 0;
}   