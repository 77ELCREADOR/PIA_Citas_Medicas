#pragma once

/*
    ProjectRoot/
│
├── include/
│   ├── cita.h             // Structs (CitaInfo, FullTime, LimitedTime, Cita, CitaTree, Tratamiento)
│   ├── file_manager.h     // Prototypes for text/binary file reading, saving, and ID management
│   ├── sorting_searching.h// Prototypes for QuickSort, HeapSort, Binary Search, and BST functions
│   └── business_logic.h   // Prototypes for validations, capture, modify, delete, and menus
│
└── src/
    ├── main.cpp           // Only contains main(), global variables setup, and the core menu loop
    ├── file_manager.cpp   // Implements readTextFile, saveToFile, appendToFile, readBinaryFile, etc.
    ├── sorting_searching.cpp// Implements QuickSort, HeapSort, BinarySearch, and BST logic
    └── business_logic.cpp // Implements captureApp, validateDate, validateExisting, modify, delete

*/
struct LimitedTime{
    int hour;
    int minute;
    LimitedTime() : hour(0), minute(0){}

    LimitedTime(int inputHour, int inputMinute) : hour(inputHour), minute(inputMinute){}
};
struct FullTime{
    int day;
    int month;
    int year;
    LimitedTime time;

    FullTime() : day(0), month(0),year(0),time(LimitedTime()){}

    FullTime(int inputDay, int inputMonth, int inputYear, LimitedTime inputTime) : day(inputDay), month(inputMonth),year(inputYear),time(inputTime){}
};
struct Tratamiento{
    int id;
    float precio;
    char tratamiento[50];
    char descripcion[150];
    
    Tratamiento() : tratamiento{},descripcion{},id(0),precio(0.0){}
};
struct CitaInfo{
    int id;
    int tratamiento_id;
    int cantidad;
    float subtotal;
    float total;
    char nombre_paciente[50];
    FullTime cita;

    CitaInfo() : id(0),nombre_paciente{},cita(FullTime()),tratamiento_id(0),cantidad(0),subtotal(0.0),total(0.0){}
};
struct Cita{
    CitaInfo info;
    Cita *prev;
    Cita *next;
    Cita() : info(CitaInfo()), prev(nullptr),next(nullptr){}

};
struct CitaTree{
    long long timeCode;
    Cita *cita;
    CitaTree *left;
    CitaTree *right;

    CitaTree() : timeCode(0),cita(nullptr), left(nullptr), right(nullptr){}
};