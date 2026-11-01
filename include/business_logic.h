#pragma once
#include "cita.h"
#include <string>

//Funciones para navegar menús
void captureApp(Cita *start, Cita *end, Cita *&App, Tratamiento catalogo[7], CitaTree *root);
void printAppointmentsMenu(Cita *start, Cita *end, int &listState, CitaTree *&root, bool &treeCorrupted, Tratamiento catalogo[7]);
void modificarCita(Cita *start, Cita *end, Cita *&found, CitaTree *&root, Tratamiento catalogo[7]);
void printFoundDates(Cita *found, std::string name);
void searchByID(Cita *&start, Cita *&end, CitaTree *&root, int &listState, bool &treeCorrupted, Cita *&found, Tratamiento catalogo[7]);
void searchByName(Cita *&start, Cita *&end, CitaTree *&root, int &listState, bool &treeCorrupted, Cita *&found, Tratamiento catalogo[7]);
void searchMenu(Cita *&start, Cita *&end, CitaTree *&root, Tratamiento catalogo[7], int &listState, bool &treeCorrupted);
//Funciones para imprimir datos
void printAppointmentInfo(Cita *app, Tratamiento catalogo[7]);
void printChronological(CitaTree *&root, Tratamiento catalogo[7]);
//Funciones de validación
void capitalizeWords(std::string &text);
bool expiredDate(int day, int month, int year, int hour, int minute);
bool validateDate(int day, int month, int year, int hour, int minute, long long appTimeCode);
bool validateExisting(long long timeCode, CitaTree *root);
void closingApp(Cita *&start, CitaTree *&root);
bool containsName(std::string name, std::string input);

