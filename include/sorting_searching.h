#pragma once
#include "cita.h"
#include <string>

//Elimina una cita en particular, borrando su nodo de la lista y el árbol
void eliminarCita(Cita *&start, Cita *&found, Cita *&end, CitaTree *&root);
//Funciones para el árbol
void addNodeTree(CitaTree *&root, Cita *Node);
void removeTreeNode(long long timeCode, CitaTree *&root);
void setTree(Cita *start, CitaTree *&root);
void findSpotForTree(CitaTree *&root, CitaTree *Node);
long long getDatesforComparison(FullTime time);
//Funciones para la lista
void addApp(Cita *&start, Cita *&end, Cita *tempApp);
//Funciones relacionadas con el Quicksort
void swapCitas(Cita *a, Cita *b);
Cita *partition(Cita *start, Cita *end);
void QuickSortCitaByName(Cita *start, Cita *end);
bool foundID(Cita *&found, std::string name, int id);
//Funciones relacionadas con el HeapSort
Cita *getNode(Cita *current, int index);
void heapifyID(Cita *start, int n, int i);
void HeapSortCitaByID(Cita *start);
int getCount(Cita *start);
//Funciones para Búsqueda binaria
Cita *BinarySearch(Cita *start, Cita *end, std::string nombre);
Cita *BinarySearchID(Cita *start, Cita *end, int ID);
