#include "lector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct typeCDT{
    char *name;
    char *code;
};

struct typevectorCDT{
    struct typeCDT *arr; //un vector de typeCDT
    int size;
};


typevector newType(){
    return calloc(1, sizeof(struct typevectorCDT));
}
static char* removeLast(char *str, int len) {
    if(str==NULL){
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
    return str;
}

static int compareTypes(const void *a, const void *b) {
    struct typeCDT *typeA = (struct typeCDT *)a;
    struct typeCDT *typeB = (struct typeCDT *)b;
    return strcmp(typeA->code, typeB->code);
}

static int binarySearchIdx(typevector types, const char *code) {
    int left = 0;
    int right = types->size - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(types->arr[mid].code, code);
        
        if (cmp == 0) {
            return mid;  // encontrado
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;  // No encontrado
}

int readTypes(const char *filename, typevector types){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", filename);
        return 0;
    }
    char line[MAX_LINE];
    int capacity = BLOCK;
    struct typeCDT* temp;
    // arranca en NULL
    temp = realloc(types->arr, capacity * sizeof(struct typeCDT));
    if (temp == NULL){
        fclose(file);
        return 0;
    }
    types->arr = temp;
    // Leer la primera línea (encabezado) y descartarla
    if (fgets(line, MAX_LINE, file) == NULL){
        fclose(file);
        return 0;  
    }
    char *s1, *s2;
    while(fgets(line, MAX_LINE, file) != NULL){
        if(types->size >= capacity){
            capacity *= 2;
            temp = realloc(types->arr, capacity * sizeof(struct typeCDT));
            if (temp == NULL) {
                fclose(file);
                return 0;
            }
            types->arr = temp;
        }
        s1 = strtok(line, DELIMITER);
        s2 = strtok(NULL, DELIMITER);
        if(s1 != NULL && s2!=NULL){
            s2 = removeLast(s2, strlen(s2));
            #ifdef CITY_NYC
            types->arr[types->size].name = strdup(s1);
            types->arr[types->size].code=strdup(s2);
            #endif
            #ifdef CITY_CHICAGO
            types->arr[types->size].name=strdup(s2);
            types->arr[types->size].code=strdup(s1);
            #endif
            if(types->arr[types->size].name == NULL){
                free(types->arr[types->size].code);
                fclose(file);
                return 0;
            }
            if(types->arr[types->size].code == NULL){
                free(types->arr[types->size].name);
                fclose(file);
                return 0;
                }
            types->size++;
            }
        }
    if (types->size < capacity) {
    struct typeCDT *new_arr = realloc(types->arr, types->size * sizeof(struct typeCDT));
    if (new_arr != NULL) {  // Solo reasignar si es distinto a NULL
        types->arr = new_arr;
    }
    }
    qsort(types->arr, types->size, sizeof(struct typeCDT), compareTypes);
    fclose(file);
    return 1;
}

void freeTypes(typevector types) {
    if (types == NULL) {
        return;
    }
    
    if (types->arr != NULL) {
        for (int i = 0; i < types->size; i++) {
            free(types->arr[i].name);
            free(types->arr[i].code);
        }
        free(types->arr);
    }
    
    free(types);
}
int readRequest(const char *filename, typevector types, queryADT q){
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: No se pudo abrir el archivo %s\n", filename);
        return 0;
    }
    char line[MAX_LINE];
    
    // Saltar encabezado
    if (fgets(line, MAX_LINE, file) == NULL) {
        fclose(file);
        return 0;
    }
    int len, idx, year, month, day, hour, quadLat, quadLong;
    char *date, *agency, *code, *status, *borough, *latitud, *longitud;
    // Leer línea por línea
    while (fgets(line, MAX_LINE, file) != NULL) {

#ifdef CITY_NYC

        date = strtok(line, DELIMITER);
        if (!date) continue; //si da NULL significa que no hay mas tokens para leer en el reclamo

        agency = strtok(NULL, DELIMITER);
        if (!agency) continue;

        code = strtok(NULL, DELIMITER);
        if (!code) continue;

        idx = binarysearchIdx(types, code);
        if (idx == -1) continue; // si idx es -1, no es un reclamo valido

        status = strtok(NULL, DELIMITER);
        if (!status) continue;

        borough = strtok(NULL, DELIMITER);
        if (!borough) continue;

        latitud = strtok(NULL, DELIMITER);
        if (!latitud) continue;

        longitud = strtok(NULL, DELIMITER);
        if (!longitud) continue;

#endif
#ifdef CITY_CHICAGO

        code = strtok(line, DELIMITER);
        if (code==NULL) continue; //si da NULL significa que no hay mas tokens para leer en el reclamo

        idx = binarysearchIdx(types, code);
        if (idx == -1) continue; // si idx es -1, no es un reclamo valido

        agency = strtok(NULL, DELIMITER);
        if (agency==NULL) continue;

        status = strtok(NULL, DELIMITER);
        if (status==NULL) continue;

        date = strtok(NULL, DELIMITER);
        if (date==NULL) continue; 

        borough = strtok(NULL, DELIMITER);
        if (borough==NULL) continue;

        latitud = strtok(NULL, DELIMITER);
        if (latitud==NULL) continue;

        longitud = strtok(NULL, DELIMITER);
        if (longitud==NULL) continue;

#endif
        longitud = removeLast(longitud, strlen(longitud));
        sscanf(date, "%d-%d-%d %d:%*d:%*d", &year, &month, &day, &hour);
        quadLat = (int)(atof(latitud)*10);
        quadLong =(int) (atof(longitud)*10);
        addToQueries(q, agency, code, status, borough, year, month, hour, day, quadLat, quadLong);
    }
    fclose(file);
    return 1;
}