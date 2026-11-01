#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include "../include/file_manager.h"
#include "../include/sorting_searching.h"
#include "../include/business_logic.h"

using namespace std;

//Funciones de excritura
void saveBinID(int ID)
{
    ofstream binaryWriter("files/citas_ID.bin", ios::binary | ios::out);
    binaryWriter.write((char *)&ID, sizeof(int));
    binaryWriter.close();
}
void appendToFile(Cita *temp)
{
    ofstream fileWriter("files/citas_vigentes.txt", ios::app);

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
//Funciones de lectura
void readTextFile(Cita *&start, Cita *&end)
{
    string tempText = "";
    ifstream fileReader("files/citas_vigentes.txt");
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
void readTreatments(Tratamiento catalogo[7])
{
    ifstream archivo("files/tratamientos.txt");
    for (int i = 0; i < 7; i++)
    {
        archivo >> catalogo[i].id >> catalogo[i].tratamiento >> catalogo[i].descripcion >> catalogo[i].precio;
    }
}
void readBinaryFile(Cita *&start, Cita *&end)
{
    ifstream binaryReader("files/citas_eliminadas.dat", ios::binary | ios::in);

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
int readBinID()
{
    int ID = 1;
    ifstream binaryReader("files/citas_ID.bin", ios::binary | ios::in);
    binaryReader.read((char *)&ID, sizeof(int));
    binaryReader.close();
    return ID;
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
void saveToBinary(Cita *App)
{
    ofstream binaryWriter("files/citas_eliminadas.dat", ios::binary | ios::app);
    binaryWriter.write((char *)App, sizeof(Cita));
    binaryWriter.close();
}
void saveToFile(Cita *start)
{
    ofstream fileWriter("files/citas_vigentes.txt");
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