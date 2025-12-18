#ifndef LECTOR_H
#define LECTOR_H

#include "../backend/queries.h" 

#define MAX_LINE 1024
#define DELIMITER ";"
#define BLOCK 250

typedef struct typevectorCDT * typevector;

// creo la estructura para almacenar los tipos de reclamo
typevector newType(void);
// leo del archivo filename los tipos de reclamo y los almacento en la estructura types
int readTypes(const char *filename, typevector types);
//libero la estuctura de tipos de reclamo
void freeTypes(typevector types);
/*  Leo los reclamos del archivo filename, 
    los comparo con los tipos de reclamo y 
    los añado a las estuctura de las queries*/
int readRequest(const char *filename, typevector types, queryADT q);
#endif
