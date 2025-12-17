#include <stdio.h>
#include <stdlib.h>

#include "lector.h"
#include "queries.h"
#include "fromQueryToFile.h"

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s requests.csv types.csv [yearFrom] [yearTo]\n", prog);
}

int main(int argc, char *argv[]) {

    if (argc != 3 && argc != 4 && argc != 5) {
        usage(argv[0]);
        return 1;
    }

    const char *requestsPath = argv[1];
    const char *typesPath    = argv[2];

    int yearFrom = -1, yearTo = -1;

    if (argc >= 4) {
        yearFrom = atoi(argv[3]);
        if (yearFrom <= 0) {
            fprintf(stderr, "Invalid yearFrom\n");
            return 1;
        }
        yearTo = yearFrom;
    }

    if (argc == 5) {
        yearTo = atoi(argv[4]);
        if (yearTo < yearFrom) {
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
    if (!writeAllQueries(queries)) {
        fprintf(stderr, "Error writing output files\n");
        freeTypes(types);
        freeQueries(queries);
        return 1;
    }

    freeTypes(types);
    freeQueries(queries);

    return 0;
}