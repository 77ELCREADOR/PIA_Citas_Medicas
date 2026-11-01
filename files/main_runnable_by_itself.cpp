#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <regex>
#include <sstream>
#include "../include/cita.h"

using namespace std;

void printAppointmentsMenu(Cita *start, Cita *end, int &listState, CitaTree *&root, bool &treeCorrupted, Tratamiento catalogo[7]);
void appendToFile(Cita *temp);
void printAppointmentInfo(Cita *app, Tratamiento catalogo[7]);
void printChronological(CitaTree *&root, Tratamiento catalogo[7]);
void addNodeTree(CitaTree *&root, Cita *Node);
void readTextFile(Cita *&start, Cita *&end);
void readTreatments(Tratamiento catalogo[7]);
void readBinaryFile(Cita *&start, Cita *&end);
void modificarCita(Cita *start, Cita *end, Cita *&found, CitaTree *&root, Tratamiento catalogo[7]);
int readBinID();
void saveBinID(int ID);
void addApp(Cita *&start, Cita *&end, Cita *tempApp);
void capitalizeWords(string &text);
void captureApp(Cita *start, Cita *end, Cita *&App, Tratamiento catalogo[7], CitaTree *root);
bool expiredDate(int day, int month, int year, int hour, int minute);
bool validateDate(int day, int month, int year, int hour, int minute, long long appTimeCode);
bool validateExisting(long long timeCode, CitaTree *root);
void closingApp(Cita *&start, CitaTree *&root);
void destroyTree(CitaTree *&root);
void saveToBinary(Cita *start);
void saveToFile(Cita *start);
// Quicksort algo, order my 2 dll by name
void swapCitas(Cita *a, Cita *b);
Cita *partition(Cita *start, Cita *end);
void QuickSortCitaByName(Cita *start, Cita *end);
bool containsName(string name, string input);
Cita *BinarySearch(Cita *start, Cita *end, string nombre);
Cita *BinarySearchID(Cita *start, Cita *end, int ID);

bool foundID(Cita *&found, string name, int id);

void printFoundDates(Cita *found, string name);
void eliminarCita(Cita *&start, Cita *&found, Cita *&end, CitaTree *&root);

// Heapsort
Cita *getNode(Cita *current, int index);
void heapifyID(Cita *start, int n, int i);
void HeapSortCitaByID(Cita *start);
int getCount(Cita *start);
long long getDatesforComparison(FullTime time);
void findSpotForTree(CitaTree *&root, CitaTree *Node);
void searchByID(Cita *&start, Cita *&end, CitaTree *&root, int &listState, bool &treeCorrupted, Cita *&found, Tratamiento catalogo[7]);
void searchByName(Cita *&start, Cita *&end, CitaTree *&root, int &listState, bool &treeCorrupted, Cita *&found, Tratamiento catalogo[7]);
void searchMenu(Cita *&start, Cita *&end, CitaTree *&root, Tratamiento catalogo[7], int &listState, bool &treeCorrupted);

void removeTreeNode(long long timeCode, CitaTree *&root);
void setTree(Cita *start, CitaTree *&root);

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
    bool isDirty = false, isRunning = true, isInMenu = true, isInSubMenu = true;
    string name = "";
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
                captureApp(start, end, newApp, catalogo, root);
                newApp->info.id = ++global_id;
                addApp(start, end, newApp);
                appendToFile(newApp);
                saveBinID(global_id);
                addNodeTree(root, newApp);
                if (listState == 2)
                {
                    listState = 0;
                }
                cout << "Se ha agregado la nueva cita con éxito!\n";
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
void setTree(Cita *start, CitaTree *&root)
{
    while (start != NULL)
    {
        addNodeTree(root, start);
        start = start->next;
    }
}
void readBinaryFile(Cita *&start, Cita *&end)
{
    ifstream binaryReader("../files/citas_eliminadas.dat", ios::binary | ios::in);

    if (!binaryReader.is_open())
    {
        return;
    }

    Cita *tempApp = new Cita();

    while (binaryReader.read((char *)tempApp, sizeof(Cita)))
    {

        addApp(start, end, tempApp);

        tempApp = new Cita();
    }

    delete tempApp;
    binaryReader.close();
}
void searchMenu(Cita *&start, Cita *&end, CitaTree *&root, Tratamiento catalogo[7], int &listState, bool &treeCorrupted)
{
    Cita *found = NULL;
    bool isInMenu = true;
    char option = {};
    int id = 0;

    while (isInMenu)
    {
        cout << "Selecciona una de las siguientes opciones:\nA-Buscar por nombre\nB-Buscar por ID\nC-Regresar al menú anterior\n";
        cin >> option;
        if (cin.fail() || cin.peek() != '\n')
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Favor de introducur un caracter válido\n";
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (toupper(option))
            {
            case 'A':
                searchByName(start, end, root, listState, treeCorrupted, found, catalogo);
                break;
            case 'B':
                searchByID(start, end, root, listState, treeCorrupted, found, catalogo);
                break;
            case 'C':
                isInMenu = false;
                cout << "Regresando al menú principal...\n";
                break;
            default:
                break;
            }
        }
    }
}
void searchByName(Cita *&start, Cita *&end, CitaTree *&root, int &listState, bool &treeCorrupted, Cita *&found, Tratamiento catalogo[7])
{
    string name = "";
    bool isInMenu = true;
    char option = {};
    int id = 0;

    while (true)
    {
        if (listState != 2)
        {
            QuickSortCitaByName(start, end);
            listState = 2;
            treeCorrupted = true;
        }
        cout << "Favor de introducir un nombre para la búsqueda, empezando por el apellido\n";
        getline(cin, name);
        regex namePattern(R"(^[A-Za-z]{2,}( [A-Za-z]{2,})?$)");
        if (regex_match(name, namePattern))
        {
            capitalizeWords(name);
            found = BinarySearch(start, end, name);
            if (found != NULL)
            {
                cout << "Encontramos los siguientes resultados:\n";
                printFoundDates(found, name);
                while (true)
                {
                    cout << "Favor de introducir el ID de la cita con la que vamos a trabajar:\n";
                    cin >> id;
                    if (cin.fail() || cin.peek() != '\n')
                    {
                        cout << "Solamente se aceptan números\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else if (!foundID(found, name, id))
                    {
                        cout << "No se ha encontrado ese ID dentro de la lista proporcionada\n";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else
                    {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        isInMenu = true;
                        while (isInMenu)
                        {
                            cout << "Selecciona lo que quieras hacer con la cita de " << found->info.nombre_paciente << " \nA-Eliminar\nB-Modificar\nC-Regresar\n";
                            cin >> option;
                            if (cin.fail() || cin.peek() != '\n')
                            {
                                cout << "Introduce un caracter solamente \n";
                                cin.clear();
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            }
                            else
                            {
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                switch (toupper(option))
                                {
                                case 'A':
                                    eliminarCita(start, found, end, root);
                                    cout << "Se ha eliminado la cita satisfactoriamente\n";
                                    break;
                                case 'B':
                                    modificarCita(start, end, found, root, catalogo);
                                    listState = 0;
                                    cout << "Estado final de la cita:\n";
                                    cout << "ID: " << found->info.id << "\tNombre: " << found->info.nombre_paciente << "\t" << found->info.cita.month << "/" << found->info.cita.day << "/" << found->info.cita.year << " " << found->info.cita.time.hour << ":" << found->info.cita.time.minute << "\n";
                                    break;
                                case 'C':
                                    isInMenu = false;
                                    cout << "Regresando a menú de búsqueda...\n";
                                    break;
                                default:
                                    cout << "Favor de seleccionar una opción válida\n";
                                    break;
                                }
                                found = NULL;
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
            else
            {
                cout << "No se encontraron resultados con ese nombre\n";
            }
        }
        else
        {
            cout << "Favor de sólo emplear letras y máximo un espacio\n";
        }
    }
}
void eliminarCita(Cita *&start, Cita *&found, Cita *&end, CitaTree *&root)
{
    long long timeCode = getDatesforComparison(found->info.cita);

    if (start == NULL)
    {
        return;
    }
    if (end == found && start == found)
    {
        start = NULL;
        end = NULL;
    }
    else if (start == found && found != end)
    {
        start = found->next;
        start->prev = NULL;
    }
    else if (end == found)
    {
        end = found->prev;
        end->next = NULL;
    }
    else
    {
        Cita *prevTemp = found->prev;
        Cita *nextTemp = found->next;
        prevTemp->next = nextTemp;
        nextTemp->prev = prevTemp;
    }
    removeTreeNode(timeCode, root);
    saveToBinary(found);
    saveToFile(start);
    delete found;
}
void searchByID(Cita *&start, Cita *&end, CitaTree *&root, int &listState, bool &treeCorrupted, Cita *&found, Tratamiento catalogo[7])
{
    bool isInMenu = true;
    int id = 0;
    char option = {};
    while (true)
    {
        cout << "Favor de introducir el ID de la cita:\n";
        cin >> id;
        if (cin.fail() || cin.peek() != '\n')
        {
            cout << "Favor de usar sólo números\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (listState != 1)
            {
                HeapSortCitaByID(start);
                listState = 1;
                treeCorrupted = true;
            }
            found = BinarySearchID(start, end, id);
            if (found != NULL)
            {
                cout << "Se ha encontrado la cita con ID " << id << ":\n";
                cout << "ID: " << found->info.id << "\tNombre: " << found->info.nombre_paciente << "\t" << found->info.cita.month << "/" << found->info.cita.day << "/" << found->info.cita.year << " " << found->info.cita.time.hour << ":" << found->info.cita.time.minute << "\n";
                isInMenu = true;
                while (isInMenu)
                {
                    cout << "Selecciona lo que quieras hacer con la cita de ID: " << id << " \nA-Eliminar\nB-Modificar\nC-Regresar\n";
                    cin >> option;
                    if (cin.fail() || cin.peek() != '\n')
                    {
                        cout << "Introduce un caracter solamente \n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else
                    {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        switch (toupper(option))
                        {
                        case 'A':
                            eliminarCita(start, found, end, root);
                            cout << "Se ha eliminado la cita satisfactoriamente\n";
                            break;
                        case 'B':
                            modificarCita(start, end, found, root, catalogo);
                            cout << "Se ha modificado la cita con éxito!:\n";
                            cout << "ID: " << found->info.id << "\tNombre: " << found->info.nombre_paciente << "\t" << found->info.cita.month << "/" << found->info.cita.day << "/" << found->info.cita.year << " " << found->info.cita.time.hour << ":" << found->info.cita.time.minute << "\n";
                            break;
                        case 'C':
                            isInMenu = false;
                            cout << "Regresando a menú de búsqueda...\n";
                            break;
                        default:
                            cout << "Favor de seleccionar una opción válida\n";
                            break;
                        }
                        found = NULL;
                        break;
                    }
                }
                break;
            }
            else
            {
                cout << "No se han encontrado resultados para ese ID\n";
            }
        }
        break;
    }
}
void readTreatments(Tratamiento catalogo[7])
{
    ifstream archivo("../files/tratamientos.txt");
    for (int i = 0; i < 7; i++)
    {
        archivo >> catalogo[i].id >> catalogo[i].tratamiento >> catalogo[i].descripcion >> catalogo[i].precio;
    }
}
void printAppointmentsMenu(Cita *start, Cita *end, int &listState, CitaTree *&root, bool &treeCorrupted, Tratamiento catalogo[7])
{
    Cita *temp = start;
    char option = {};
    bool isInMenu = true;

    if (start == NULL)
    {
        cout << "No hay citas activas\n";
        return;
    }

    while (isInMenu)
    {
        cout << "Seleccione como quiere visualizar las citas activa:\nA-Por ID de cita\nB-Por nombre\nC-Por fecha\nD-Regresar al menú principal\n";
        cin >> option;
        if (cin.fail() || cin.peek() != '\n')
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Favor de introducir un caracter válido\n";
        }
        else
        {
            switch (toupper(option))
            {
            case 'A':
                if (listState != 1)
                {
                    HeapSortCitaByID(start);
                    listState = 1;
                    treeCorrupted = true;
                }
                temp = start;
                while (temp != NULL)
                {
                    printAppointmentInfo(temp, catalogo);
                    temp = temp->next;
                }
                break;
            case 'B':
                if (listState != 2)
                {
                    QuickSortCitaByName(start, end);
                    listState = 2;
                    treeCorrupted = true;
                }
                temp = start;
                while (temp != NULL)
                {
                    printAppointmentInfo(temp, catalogo);
                    temp = temp->next;
                }
                break;
            case 'C':
                if (treeCorrupted)
                {
                    destroyTree(root);
                    setTree(start,root);
                    treeCorrupted = false;
                }
                printChronological(root, catalogo);
                break;
            case 'D':
                isInMenu = false;
                break;
            default:
                cout << "Favor de seleccionar una opción válida\n";
                break;
            }
        }
    }
}
void printAppointmentInfo(Cita *app, Tratamiento catalogo[7])
{
    if (app == NULL)
    {
        return;
    }

    // Convert the 1-7 ID into a 0-6 array index
    int t_index = app->info.tratamiento_id - 1;

    cout << "-----------------------------------------------------------------\n";
    cout << " FOLIO (ID):  " << app->info.id << "\n";
    cout << " PACIENTE:    " << app->info.nombre_paciente << "\n";
    cout << " FECHA:       " << app->info.cita.month << "/"
         << app->info.cita.day << "/"
         << app->info.cita.year << " a las "
         << app->info.cita.time.hour << ":"
         << (app->info.cita.time.minute < 10 ? "0" : "") << app->info.cita.time.minute << "\n";
    cout << " TRATAMIENTO: " << catalogo[t_index].tratamiento << "\n";
    cout << " DESCRIPCION: " << catalogo[t_index].descripcion << "\n";
    cout << " DETALLES:    " << app->info.cantidad << " unidad(es) a $" << catalogo[t_index].precio << " c/u\n";
    cout << " SUBTOTAL:    $" << app->info.subtotal << "\n";
    cout << " TOTAL (IVA): $" << app->info.total << "\n";
    cout << "-----------------------------------------------------------------\n";
}
void addNodeTree(CitaTree *&root, Cita *Node)
{
    CitaTree *newNode = new CitaTree();

    newNode->timeCode = getDatesforComparison(Node->info.cita);
    newNode->cita = Node;
    newNode->left = NULL;
    newNode->right = NULL;

    findSpotForTree(root, newNode);
}
void findSpotForTree(CitaTree *&root, CitaTree *Node)
{
    if (root == NULL)
    {
        root = Node;
        return;
    }
    if (root->timeCode < Node->timeCode)
    {
        findSpotForTree(root->right, Node);
    }
    else if (root->timeCode > Node->timeCode)
    {
        findSpotForTree(root->left, Node);
    }
    else
    {
        cout << "La fecha ya existe, no se admiten duplicados\n";
        delete Node;
        return;
    }
}
long long getDatesforComparison(FullTime time)
{
    long long year = (long long)time.year * 100000000;
    long long month = (long long)time.month * 1000000;
    long long day = (long long)time.day * 10000;
    long long hour = (long long)time.time.hour * 100;
    long long minute = (long long)time.time.minute;

    long long timeCode = year + month + day + hour + minute;
    return timeCode;
}
void printChronological(CitaTree *&root, Tratamiento catalogo[7])
{
    // Caso base, no quedan más elementos en el árbol
    if (root == NULL)
    {
        return;
    }
    // Desciende por la izquierda hasta ser detenido por el caso base
    printChronological(root->left, catalogo);
    // Imprime la información
    printAppointmentInfo(root->cita, catalogo);
    // Desciende por la derecha
    printChronological(root->right, catalogo);
    /*
        Un ejemplo de un caso:
        Empieza en el nodo principal, el cual no es nulo, sigue por la izquierda, la función principal se detiene ya que se vuelve a llamar a si misma.
        Sigue hasta ser detenido por el caso base, cuando finalmente regresa, por fin puede imprimir el código y checa el lado derecho, sigue hasta el caso base.
        Una vez que termina de recorrer el lado derecho, por fin continúa la función que la llamo, la cual sigue el paso recursivo hasta llegar a la raíz.
        Al llegar a la raíz por fin se imprime la raíz y se pasa a descender por la derecha, siguiendo los mismos pasos descritos anteriormente, checará la izquierda,
        seguirá por ahí hasta llegar al final y regresará, listo para pasar al siguiente elemento de la derecha hasta llegar al último elemento, el cual, naturalmente será el más grande.
    */
}
int readBinID()
{
    int ID = 1;
    ifstream binaryReader("../files/citas_ID.bin", ios::binary | ios::in);
    binaryReader.read((char *)&ID, sizeof(int));
    binaryReader.close();
    return ID;
}
void saveBinID(int ID)
{
    ofstream binaryWriter("../files/citas_ID.bin", ios::binary | ios::out);
    binaryWriter.write((char *)&ID, sizeof(int));
    binaryWriter.close();
}
void captureApp(Cita *start, Cita *end, Cita *&App, Tratamiento catalogo[7], CitaTree *root)
{
    string tempText = "";
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, cantidad = 0, tratamiento_id = 0;
    float precio = 0.0;

    while (true)
    {
        cout << "Introduce el nombre del paciente empezando por los apellidos:\n";
        regex namePattern(R"(^[A-Za-z]{2,} [A-Za-z]{2,}$)");
        getline(cin, tempText);
        if (cin.fail())
        {
            cout << "Algo salió mal\n";
            cin.clear();
        }
        else if (tempText.length() > 49 || tempText.length() <= 0)
        {
            cout << "Favor de introducir un nombre de entre 0 y 50 caracteres\n";
        }
        else if (!regex_match(tempText, namePattern))
        {
            cout << "Favor de introducir un sólo apellido y nombre\n";
        }
        else
        {
            capitalizeWords(tempText);
            strcpy(App->info.nombre_paciente, tempText.c_str());
            break;
        }
    }
    while (true)
    {
        regex pattern("^\\d{1,2}/\\d{1,2}/\\d{4} \\d{2}:\\d{2}$");
        cout << "Introduce la fecha de la cita usando el siguiente formato: MM/DD/AAAA HH:MM\n";
        getline(cin, tempText);
        if (cin.fail())
        {
            cout << "Algo salio mal\n";
            cin.clear();
        }
        else if (regex_match(tempText, pattern))
        {
            stringstream ss(tempText);
            getline(ss, tempText, '/');
            month = stoi(tempText);
            getline(ss, tempText, '/');
            day = stoi(tempText);
            getline(ss, tempText, ' ');
            year = stoi(tempText);
            getline(ss, tempText, ':');
            hour = stoi(tempText);
            getline(ss, tempText);
            minute = stoi(tempText);

            LimitedTime inputTime = LimitedTime(hour, minute);
            FullTime inputDate = FullTime(day, month, year, inputTime);
            long long timeCodeNew = getDatesforComparison(inputDate);

            if (validateDate(day, month, year, hour, minute, timeCodeNew) && validateExisting(timeCodeNew, root))
            {
                App->info.cita.year = year;
                App->info.cita.month = month;
                App->info.cita.day = day;
                App->info.cita.time.hour = hour;
                App->info.cita.time.minute = minute;
                break;
            }
        }
        else
        {
            cout << "Favor de seguir el formato correcto\n";
        }
    }
    while (true)
    {
        cout << "Selecciona uno de las siguientes opciones de tratamiento (introduce el número asignado):\n";
        for (int i = 0; i < 7; i++)
        {
            cout << i + 1 << "-" << catalogo[i].tratamiento << "\t$" << catalogo[i].precio << "\n";
        }
        cin >> tratamiento_id;
        if (cin.fail() || cin.peek() != '\n')
        {
            cout << "Algo salió mal\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (tratamiento_id < 1 || tratamiento_id > 7)
        {
            cout << "Favor de introducir un código de tratamiento válido\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            App->info.tratamiento_id = catalogo[tratamiento_id - 1].id;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    while (true)
    {
        cout << "Introduce la cantidad del tratamiento:\n";
        cin >> cantidad;
        if (cin.fail() || cin.peek() != '\n')
        {
            cout << "Introduce un valor válido\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (cantidad <= 0 || cantidad > 99)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Favor de introducir una cantidad de tratamiento de entre 1 y 99 unidades\n";
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            App->info.cantidad = cantidad;
            break;
        }
    }
    App->info.subtotal = catalogo[tratamiento_id - 1].precio * App->info.cantidad;
    App->info.total = App->info.subtotal * 1.16;
}
void capitalizeWords(string &text)
{
    for (int i = 0; i < text.length(); i++)
    {
        if (i == 0 || text[i - 1] == ' ')
        {
            text[i] = toupper(text[i]);
        }
        else
        {
            text[i] = tolower(text[i]);
        }
    }
}
bool expiredDate(int day, int month, int year, int hour, int minute)
{
    time_t t = time(0);
    tm *now = localtime(&t);

    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    int currentHour = now->tm_hour;
    int currentMinute = now->tm_min;
    if (year < currentYear)
    {
        return true;
    }
    if (month < currentMonth && year == currentYear)
    {
        return true;
    }
    if (day < currentDay && year == currentYear && month == currentMonth)
    {
        return true;
    }
    if (hour < currentHour && day == currentDay && year == currentYear && month == currentMonth)
    {
        return true;
    }
    if (minute < currentMinute && hour == currentHour && day == currentDay && year == currentYear && month == currentMonth)
    {
        return true;
    }
    return false;
}
bool validateDate(int day, int month, int year, int hour, int minute, long long appTimeCode)
{
    time_t t = time(0);
    struct tm timeinfo = {0};
    tm *now = localtime(&t);

    now->tm_min += 10;
    mktime(now);

    LimitedTime nowTime = LimitedTime(now->tm_hour, now->tm_min);
    FullTime nowDate = FullTime(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, nowTime);

    long long nowTimeCode = getDatesforComparison(nowDate);

    if (appTimeCode < nowTimeCode)
    {
        cout << "No se admiten citas en el pasado, reserva con 10 minutos de antelación\n";
        return false;
    }
    if (appTimeCode > 212000000000)
    {
        cout << "El doctor ha visto el futuro, muere en el año 2120, no se permiten citas después de su fallecimiento\n";
        return false;
    }

    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    mktime(&timeinfo);

    if (timeinfo.tm_mday != day)
    {
        cout << "Favor de introducir una fecha válida\n";
        return false;
    }
    if (timeinfo.tm_wday == 0 || timeinfo.tm_wday == 6)
    {
        cout << "El consultorio solo abre entre semana\n";
        return false;
    }
    if (hour > 17 || hour < 8)
    {
        cout << "El consultorio abre de 08 a 18 (La última cita disponible es a las 17:30\n";
        return false;
    }
    if (minute != 0 && minute != 30)
    {
        cout << "Sólo se puede agendar una cita cada 30 minutos\n";
        return false;
    }
    return true;
}
bool validateExisting(long long timeCode, CitaTree *root)
{
    if (root == NULL)
    {
        return true;
    }
    if (root->timeCode < timeCode)
    {
        return validateExisting(timeCode, root->right);
    }
    else if (root->timeCode > timeCode)
    {
        return validateExisting(timeCode, root->left);
    }
    else
    {
        cout << "No hay cupo en ese horario\n";
        return false;
    }
}
void readTextFile(Cita *&start, Cita *&end)
{
    string tempText = "";
    ifstream fileReader("../files/citas_vigentes.txt");
    // Si no se encuentra el archivo, regresa al ciclo principal como si nada.
    if (!fileReader.is_open())
    {
        return;
    }
    while (getline(fileReader, tempText, '|'))
    {
        // Si el archivo de texto se encuentra vacío o agrega un salto de línea fantasma al final, previene que el sistema muera tratando de convertir un espacio vacío a int
        if (tempText.empty() || tempText == "\n")
            break;
        // Crea un nodo, se usa App por Appointment y lee el id
        Cita *tempApp = new Cita();
        tempApp->info.id = stoi(tempText);
        // Siguiendo el formato de nuestro texto, lee el nombre del paciente
        getline(fileReader, tempText, '{');
        strcpy(tempApp->info.nombre_paciente, tempText.c_str());
        // Lee la fecha {xx/xx/xx|xx:xx}
        getline(fileReader, tempText, '/');
        tempApp->info.cita.month = stoi(tempText);
        getline(fileReader, tempText, '/');
        tempApp->info.cita.day = stoi(tempText);
        getline(fileReader, tempText, '|');
        tempApp->info.cita.year = stoi(tempText);
        getline(fileReader, tempText, ':');
        tempApp->info.cita.time.hour = stoi(tempText);
        getline(fileReader, tempText, '}');
        tempApp->info.cita.time.minute = stoi(tempText);
        // Lee el ID del tratamiento
        getline(fileReader, tempText, '|');
        tempApp->info.tratamiento_id = stoi(tempText);
        getline(fileReader, tempText, '|');
        tempApp->info.cantidad = stoi(tempText);
        // Finaliza con la información de la cita
        getline(fileReader, tempText, '|');
        tempApp->info.subtotal = stof(tempText);
        getline(fileReader, tempText, '\n');
        tempApp->info.total = stof(tempText);

        if (expiredDate(tempApp->info.cita.day, tempApp->info.cita.month, tempApp->info.cita.year, tempApp->info.cita.time.hour, tempApp->info.cita.time.minute))
        {
            saveToBinary(tempApp);
            delete tempApp;
        }
        else
        {
            addApp(start, end, tempApp);
        }
    }
    fileReader.close();
}
void saveToFile(Cita *start)
{
    ofstream fileWriter("../files/citas_vigentes.txt");
    Cita *temp = start;
    if (!fileWriter.is_open())
    {
        cout << "Algo salió mal\n";
        return;
    }
    while (temp != NULL)
    {
        fileWriter << temp->info.id << "|";
        fileWriter << temp->info.nombre_paciente << "{";
        fileWriter << temp->info.cita.month << "/";
        fileWriter << temp->info.cita.day << "/";
        fileWriter << temp->info.cita.year << "|";
        fileWriter << temp->info.cita.time.hour << ":";
        fileWriter << temp->info.cita.time.minute << "}";
        fileWriter << temp->info.tratamiento_id << "|";
        fileWriter << temp->info.cantidad << "|";
        fileWriter << temp->info.subtotal << "|";
        fileWriter << temp->info.total << "\n";
        temp = temp->next;
    }
    fileWriter.close();
}
void appendToFile(Cita *temp)
{
    ofstream fileWriter("../files/citas_vigentes.txt", ios::app);

    if (fileWriter.is_open())
    {
        fileWriter << temp->info.id << "|";
        fileWriter << temp->info.nombre_paciente << "{";
        fileWriter << temp->info.cita.month << "/";
        fileWriter << temp->info.cita.day << "/";
        fileWriter << temp->info.cita.year << "|";
        fileWriter << temp->info.cita.time.hour << ":";
        fileWriter << temp->info.cita.time.minute << "}";
        fileWriter << temp->info.tratamiento_id << "|";
        fileWriter << temp->info.cantidad << "|";
        fileWriter << temp->info.subtotal << "|";
        fileWriter << temp->info.total << "\n";
        fileWriter.close();
    }
}
void closingApp(Cita *&start, CitaTree *&root)
{
    while (start != NULL)
    {
        Cita *tempStart = start;
        start = tempStart->next;
        delete tempStart;
    }
    destroyTree(root);
}
void destroyTree(CitaTree *&root)
{
    if (root == NULL)
    {
        return;
    }

    destroyTree(root->left);
    destroyTree(root->right);
    delete root;

    root = NULL;
}
void addApp(Cita *&start, Cita *&end, Cita *tempApp)
{
    // En un nuevo elemento, el siguiente siempre será nulo
    tempApp->next = NULL;
    if (start == NULL)
    {
        start = tempApp;
        end = tempApp;
        tempApp->prev = NULL;
        // En caso de que sea el primer elemento capturado, se editan los punteros start y end para apuntar al nuevo miembro
    }
    else
    {
        tempApp->prev = end;
        end->next = tempApp;
        end = tempApp;
        // El primer miembro no se toca, se edita el anterior miembro final para que ahora apunte al nuevo y el nuevo apunta al anterior más nuevo en su apuntador previo, después, se actualiza el puntero del final
    }
}
void saveToBinary(Cita *App)
{
    ofstream binaryWriter("../files/citas_eliminadas.dat", ios::binary | ios::app);
    binaryWriter.write((char *)App, sizeof(Cita));
    binaryWriter.close();
}
Cita *partition(Cita *start, Cita *end)
{
    Cita *i = start->prev;
    Cita *j = start;
    Cita *pivot = end;

    while (j != end)
    {
        if (string(j->info.nombre_paciente) < string(pivot->info.nombre_paciente) && i == NULL)
        {
            i = start;
            swapCitas(i, j);
        }
        else if (string(j->info.nombre_paciente) < string(pivot->info.nombre_paciente) && i != NULL)
        {
            i = i->next;
            swapCitas(i, j);
        }
        j = j->next;
    }
    if (i != NULL)
    {
        i = i->next;
        swapCitas(i, pivot);
    }
    else
    {
        i = start;
        swapCitas(i, pivot);
    }
    return i;
}
void swapCitas(Cita *a, Cita *b)
{
    CitaInfo temp = a->info;
    a->info = b->info;
    b->info = temp;
}
void QuickSortCitaByName(Cita *start, Cita *end)
{
    if (start != NULL && end != NULL && start != end && start->prev != end)
    {
        Cita *lockedPivot = partition(start, end);
        QuickSortCitaByName(start, lockedPivot->prev);
        QuickSortCitaByName(lockedPivot->next, end);
    }
}
Cita *BinarySearch(Cita *start, Cita *end, string nombre)
{
    if (start == NULL || end == NULL || start->prev != end)
    {
        return NULL;
    }
    else if (start == end && containsName(string(start->info.nombre_paciente), nombre))
    {
        return start;
    }
    else if (start == end && !containsName(string(start->info.nombre_paciente), nombre))
    {
        return NULL;
    }
    Cita *tempStart = start;
    Cita *tempEnd = end;
    while (tempStart != tempEnd && tempStart->prev != tempEnd)
    {
        tempStart = tempStart->next;
        tempEnd = tempEnd->prev;
    }
    if (containsName(string(tempStart->info.nombre_paciente), nombre))
    {
        while (tempStart->prev != NULL)
        {
            tempStart = tempStart->prev;
            if (!containsName(string(tempStart->info.nombre_paciente), nombre))
            {
                tempStart = tempStart->next;
                break;
            }
        }
        return tempStart;
    }
    else if (string(tempStart->info.nombre_paciente) < nombre)
    {
        return BinarySearch(tempStart->next, end, nombre);
    }
    else if (string(tempStart->info.nombre_paciente) > nombre)
    {
        return BinarySearch(start, tempStart->prev, nombre);
    }
    return NULL;
}
Cita *BinarySearchID(Cita *start, Cita *end, int ID)
{
    if (start == NULL || end == NULL || start->prev != end)
    {
        return NULL;
    }
    Cita *tempStart = start;
    Cita *tempEnd = end;
    while (tempStart != tempEnd && tempStart->prev != tempEnd)
    {
        tempStart = tempStart->next;
        tempEnd = tempEnd->prev;
    }
    if (tempStart->info.id == ID)
    {
        return tempStart;
    }
    else if (tempStart->info.id < ID)
    {
        return BinarySearchID(tempStart->next, end, ID);
    }
    else if (tempStart->info.id > ID)
    {
        return BinarySearchID(start, tempStart->prev, ID);
    }
    return NULL;
}
bool containsName(string name, string input)
{
    if (input.length() > name.length())
    {
        return false;
    }
    for (int i = 0; i < input.length(); i++)
    {
        if (name[i] != input[i])
        {
            return false;
        }
    }
    return true;
}
void printFoundDates(Cita *found, string name)
{
    if (found != NULL)
    {
        for (int i = 0; i < 60; i++)
        {
            cout << "-";
        }
        cout << "\n";
        while (found != NULL && containsName(string(found->info.nombre_paciente), name))
        {
            cout << "ID: " << found->info.id << "\tNombre: " << found->info.nombre_paciente << "\t" << found->info.cita.month << "/" << found->info.cita.day << "/" << found->info.cita.year << " " << found->info.cita.time.hour << ":" << found->info.cita.time.minute << "\n";
            found = found->next;
        }
    }
}
bool foundID(Cita *&found, string name, int id)
{
    Cita *temp = found;
    if (temp != NULL)
    {
        while (temp != NULL && containsName(string(temp->info.nombre_paciente), name))
        {
            if (temp->info.id == id)
            {
                found = temp;
                return true;
            }
            temp = temp->next;
        }
    }
    return false;
}
void modificarCita(Cita *start, Cita *end, Cita *&found, CitaTree *&root, Tratamiento catalogo[7])
{
    char option = {};
    string tempText = "";
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, cantidad = 0, tratamiento_id = 0;
    float precio = 0.0;
    while (true)
    {
        cout << "Selecciona una de las opciones a modificar:\nA-Nombre del paciente\nB-Hora de cita\nC-Tratamiento\nD-Regresar\n";
        cin >> option;
        if (cin.fail() || cin.peek() != '\n')
        {
            cin.clear();
            cout << "Por favor introduce un caracter válido\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch (toupper(option))
            {
            case 'A':
                while (true)
                {
                    cout << "Introduce el nombre del paciente empezando por los apellidos:\n";
                    regex namePattern(R"(^[A-Za-z]{2,} [A-Za-z]{2,}$)");
                    getline(cin, tempText);
                    if (cin.fail())
                    {
                        cout << "Algo salió mal\n";
                        cin.clear();
                    }
                    else if (tempText.length() > 49 || tempText.length() <= 0)
                    {
                        cout << "Favor de introducir un nombre de entre 0 y 50 caracteres\n";
                    }
                    else if (!regex_match(tempText, namePattern))
                    {
                        cout << "Favor de introducir un sólo apellido y nombre\n";
                    }
                    else
                    {
                        capitalizeWords(tempText);
                        strcpy(found->info.nombre_paciente, tempText.c_str());
                        cout << "Se ha editado el nombre satisfadctoriamente\n";
                        saveToFile(start);
                        break;
                    }
                }
                break;
            case 'B':
                while (true)
                {
                    regex pattern("^\\d{1,2}/\\d{1,2}/\\d{4} \\d{2}:\\d{2}$");
                    cout << "Introduce la fecha de la cita usando el siguiente formato: MM/DD/AAAA HH:MM\nIntroduce la misma fecha para volver sin realizar cambios\n";
                    getline(cin, tempText);
                    if (cin.fail())
                    {
                        cout << "Algo salio mal\n";
                        cin.clear();
                    }
                    else if (regex_match(tempText, pattern))
                    {
                        stringstream ss(tempText);
                        getline(ss, tempText, '/');
                        month = stoi(tempText);
                        getline(ss, tempText, '/');
                        day = stoi(tempText);
                        getline(ss, tempText, ' ');
                        year = stoi(tempText);
                        getline(ss, tempText, ':');
                        hour = stoi(tempText);
                        getline(ss, tempText);
                        minute = stoi(tempText);

                        LimitedTime inputTime = LimitedTime(hour, minute);
                        FullTime inputDate = FullTime(day, month, year, inputTime);

                        long long timeCodeNew = getDatesforComparison(inputDate);
                        long long timeCodeOld = getDatesforComparison(found->info.cita);

                        if (timeCodeNew == timeCodeOld)
                        {
                            cout << "Has proporcionado la misma fecha, no se realizaron cambios\n";
                            break;
                        }
                        else if (validateDate(day, month, year, hour, minute, timeCodeNew) && validateExisting(timeCodeNew, root))
                        {
                            removeTreeNode(timeCodeOld, root);

                            found->info.cita.year = year;
                            found->info.cita.month = month;
                            found->info.cita.day = day;
                            found->info.cita.time.hour = hour;
                            found->info.cita.time.minute = minute;

                            addNodeTree(root, found);

                            saveToFile(start);
                            cout << "Se ha editado la fecha de la cita satisfactoriamente\n";

                            break;
                        }
                    }
                    else
                    {
                        cout << "Favor de seguir el formato correcto\n";
                    }
                }
                break;
            case 'C':
                while (true)
                {
                    cout << "Selecciona uno de las siguientes opciones de tratamiento (introduce el número asignado):\n";
                    for (int i = 0; i < 7; i++)
                    {
                        cout << i + 1 << "-" << catalogo[i].tratamiento << "\t$" << catalogo[i].precio << "\n";
                    }
                    cin >> tratamiento_id;
                    if (cin.fail() || cin.peek() != '\n')
                    {
                        cout << "Algo salió mal\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else if (tratamiento_id < 1 || tratamiento_id > 7)
                    {
                        cout << "Favor de introducir un código de tratamiento válido\n";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else
                    {
                        found->info.tratamiento_id = catalogo[tratamiento_id - 1].id;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;
                    }
                }
                while (true)
                {
                    cout << "Introduce la cantidad del tratamiento:\n";
                    cin >> cantidad;
                    if (cin.fail() || cin.peek() != '\n')
                    {
                        cout << "Introduce un valor válido\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else if (cantidad <= 0 || cantidad > 99)
                    {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Favor de introducir una cantidad de tratamiento de entre 1 y 99 unidades\n";
                    }
                    else
                    {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        found->info.cantidad = cantidad;
                        break;
                    }
                }
                found->info.subtotal = catalogo[tratamiento_id - 1].precio * found->info.cantidad;
                found->info.total = found->info.subtotal * 1.16;
                cout << "Se ha editado el tratamiento satisfactoriamente\n";
                saveToFile(start);

                break;
            case 'D':
                return;
                break;
            default:
                break;
            }
        }
    }
}
void removeTreeNode(long long timeCode, CitaTree *&root)
{
    if (root == NULL)
    {
        return;
    }
    if (timeCode < root->timeCode)
    {
        removeTreeNode(timeCode, root->left);
    }
    else if (timeCode > root->timeCode)
    {
        removeTreeNode(timeCode, root->right);
    }
    else if (root->left == NULL)
    {
        CitaTree *temp = root;
        root = root->right;
        delete temp;
    }
    else if (root->right == NULL)
    {
        CitaTree *temp = root;
        root = root->left;
        delete temp;
    }
    else
    {
        CitaTree *temp = root->right;
        while (temp->left != NULL)
        {
            temp = temp->left;
        }
        root->timeCode = temp->timeCode;
        root->cita = temp->cita;
        removeTreeNode(root->timeCode, root->right);
    }
    return;
}
Cita *getNode(Cita *start, int index)
{
    for (int i = 0; i < index && start != NULL; i++)
    {
        start = start->next;
    }
    return start;
}
void HeapSortCitaByID(Cita *start)
{
    int n = getCount(start);
    for (int i = (n / 2) - 1; i >= 0; i--)
    {
        heapifyID(start, n, i);
    }
    for (int i = n - 1; i >= 1; i--)
    {
        Cita *node_0 = getNode(start, 0);
        Cita *node_i = getNode(start, i);
        swapCitas(node_0, node_i);
        heapifyID(start, i, 0);
    }
}
int getCount(Cita *start)
{
    int counter = 0;
    Cita *temp = start;
    while (temp != NULL)
    {
        temp = temp->next;
        counter++;
    }
    return counter;
}
void heapifyID(Cita *start, int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    Cita *node_largest = getNode(start, largest);
    Cita *node_i = node_largest;

    if (left < n)
    {
        Cita *node_left = getNode(start, left);

        if (node_left->info.id > node_largest->info.id)
        {
            largest = left;
            node_largest = node_left;
        }
    }
    if (right < n)
    {
        Cita *node_right = getNode(start, right);

        if (node_right->info.id > node_largest->info.id)
        {
            largest = right;
            node_largest = node_right;
        }
    }
    if (largest != i)
    {
        swapCitas(node_i, node_largest);
        heapifyID(start, n, largest);
    }
}