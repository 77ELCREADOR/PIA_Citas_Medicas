#include <iostream>
#include <string>
#include "../include/file_manager.h"
#include "../include/sorting_searching.h"
#include "../include/business_logic.h"

using namespace std;

//Funciones para el árbol
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
void addNodeTree(CitaTree *&root, Cita *Node)
{
    CitaTree *newNode = new CitaTree();

    newNode->timeCode = getDatesforComparison(Node->info.cita);
    newNode->cita = Node;
    newNode->left = NULL;
    newNode->right = NULL;

    findSpotForTree(root, newNode);
}
void setTree(Cita *start, CitaTree *&root)
{
    while (start != NULL)
    {
        addNodeTree(root, start);
        start = start->next;
    }
}
//Elimina una cita en particular, borrando su nodo de la lista y el árbol
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
//Funciones para la lista
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
//Funciones relacionadas con el Quicksort
void swapCitas(Cita *a, Cita *b)
{
    CitaInfo temp = a->info;
    a->info = b->info;
    b->info = temp;
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
void QuickSortCitaByName(Cita *start, Cita *end)
{
    if (start != NULL && end != NULL && start != end && start->prev != end)
    {
        Cita *lockedPivot = partition(start, end);
        QuickSortCitaByName(start, lockedPivot->prev);
        QuickSortCitaByName(lockedPivot->next, end);
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
//Funciones relacionadas con el HeapSort
Cita *getNode(Cita *start, int index)
{
    for (int i = 0; i < index && start != NULL; i++)
    {
        start = start->next;
    }
    return start;
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
//Funciones para Búsqueda binaria
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
