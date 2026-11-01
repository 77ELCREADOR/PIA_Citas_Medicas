#pragma once
#include "cita.h"
#include <string>

//Funciones de excritura
void saveBinID(int ID);
void appendToFile(Cita *temp);
//Funciones de lectura
void readTextFile(Cita *&start, Cita *&end);
void readTreatments(Tratamiento catalogo[7]);
void readBinaryFile(Cita *&start, Cita *&end);
int readBinID();
void destroyTree(CitaTree *&root);
void saveToBinary(Cita *start);
void saveToFile(Cita *start);
