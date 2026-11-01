#include <iostream>
#include <fstream>
#include <string>
#include "../include/cita.h"
#include "../include/file_manager.h"
#include "../include/sorting_searching.h"
#include "../include/business_logic.h"
#include <cstdlib> 

/*
g++ *.cpp -o citas_app
*/
using namespace std;

int main()
{
    Cita *start = NULL;
    Cita *end = NULL;
    Cita *newApp = NULL;
    CitaTree *root = NULL;
    Tratamiento catalogo[7] = {};
    int listState = 1;
    bool treeCorrupted = false;
    char option;
    bool isRunning = true;
    int global_id = readBinID();

    readTextFile(start, end);
    HeapSortCitaByID(start);
    setTree(start, root);
    readTreatments(catalogo);

    while (isRunning)
    {
        cout << "Bienvenido al menú de citas\nElige la acción que quieras realizar hoy:\nA-Agendar Cita\nB-Buscar cita\nC-Lista de citas vigentes\nD-Salir\nE-Limpiar pantallas\n";
        cin >> option;
        if (cin.fail() || cin.peek() != '\n')
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (toupper(option))
            {
            case 'A':
                newApp = new Cita();
                captureApp(start, end, newApp, catalogo, root,global_id,listState);
                break;
            case 'B':
                searchMenu(start, end, root, catalogo, listState, treeCorrupted);
                break;
            case 'C':
                printAppointmentsMenu(start, end, listState, root, treeCorrupted, catalogo);
                break;
            case 'D':
                isRunning = false;
                cout << "\033[2J\033[3J\033[1;1H";
                break;
            case 'E':
                cout << "\033[2J\033[3J\033[1;1H";
                break;
            default:
                cout << "Favor de ingresar una opción válida\n";
                break;
            }
        }
    }
    if (listState != 1)
    {
        HeapSortCitaByID(start);
        listState = 1;
        treeCorrupted = true;
    }
    saveToFile(start);
    closingApp(start, root);
    return 0;
}
