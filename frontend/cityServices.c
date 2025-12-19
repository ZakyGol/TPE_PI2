#include <stdio.h>
#include <stdlib.h>

#include "lector.h"
#include "../backend/queries.h" 
#include "fromQueryToFile.h"

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s requests.csv types.csv [yearFrom] [yearTo]\n", prog);
}

int main(int argc, char *argv[]) {

    if (argc < 3 || argc > 5) {
        usage(argv[0]);
        return 1;
    }

    const char *requestsPath = argv[1];
    const char *typesPath    = argv[2];

    int yearFrom = -1, yearTo = -1;

    if (argc == 4) {
        yearFrom = atoi(argv[3]); //convierte el argumento a numero
        if (yearFrom <= 0) {
            fprintf(stderr, "Invalid yearFrom\n");
            return 1;
        }
        yearTo = -1; //es de ese año en adelante
    }

    if (argc == 5) {
        yearFrom = atoi(argv[3]);
        yearTo = atoi(argv[4]);
        if (yearFrom <= 0 || yearTo <= 0 || yearTo < yearFrom) {
            fprintf(stderr, "Invalid year range\n");
            return 1;
        }   
    }   

    /* Backend */
    queryADT queries = newQueries();
    if (!queries) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }

    /* Frontend: types */
    typevector types = newType();
    if (!types) {
        fprintf(stderr, "Out of memory\n");
        freeQueries(queries);
        return 1;
    }

    if (!readTypes(typesPath, types)) {
        fprintf(stderr, "Error reading types file\n");
        freeTypes(types);
        freeQueries(queries);
        return 1;
    }

    /* Frontend: requests (una sola pasada) */
    if (!readRequest(requestsPath, types, queries, yearFrom, yearTo)) {
        fprintf(stderr, "Error reading requests file\n");
        freeTypes(types);
        freeQueries(queries);
        return 1;
    }

    /* Output */
    queriesToFile(queries,
              "q1.csv", "q1.html",
              "q2.csv", "q2.html",
              "q3.csv", "q3.html",
              "q4.csv", "q4.html",
              "q5.csv", "q5.html");

    freeTypes(types);
    freeQueries(queries);

    return 0;
}
