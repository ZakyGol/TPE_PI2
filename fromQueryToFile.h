#ifndef FROM_QUERY_TO_FILE_H
#define FROM_QUERY_TO_FILE_H

#include "queries.h" 

void queriesToFile(queryADT q, 
        const char *csvFile1, const char *htmlFile1, 
        const char *csvFile2, const char *htmlFile2, 
        const char *csvFile3, const char *htmlFile3, 
        const char *csvFile4, const char *htmlFile4,
        const char *csvFile5, const char *htmlFile5);

#endif