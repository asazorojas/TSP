#include <stdlib.h>
#include "list.h"

// Constructor de la clase ListElement, permite inicializar un elemento de la lista.
// itemPointer : la referencia al objeto que se quiere insertar.
// sortKey : la clave del objeto a insertar.
ListElement::ListElement(void *itemPointer, int sortKey)
{
     item = itemPointer;
     key = sortKey;
     next = NULL;	
}

// Constructor de la clase List, permite inicializar la lista enlazada.
// Inicialmente inicializar la cabeza y cola de la lista.
List::List()
{ 
    first = last = NULL; 
}

// Destructor de la clase List, permite remover todos los elementos de la lista.
List::~List()
{ 
    while (Remove(NULL) != NULL);	 
}

// Permite verificar si la lista está vacia.
bool List::IsEmpty() 
{ 
    if (first == NULL)
        return TRUE_;
    else
	return FALSE_; 
}

// Permite insertar un elemento en la lista enlazada.
// item : el elemento a insertar.
// sortKey : la clave del objeto a insertar.
void List::Insert(void *item, int sortKey)
{
    // Se instancia un nuevo elemento con los parametros.
    ListElement *element = new ListElement(item, sortKey);
    ListElement *pointer;

    // Si la lista esta vacia, el primer y ultimo elemento son los mismos.
    if (IsEmpty()) {	
        first = element;
        last = element;
    // Si no está vacia, se compara su clave con la del primer elemento de la lista, si es menor se inserta en la cabeza de la lista.
    } else if (sortKey < first->key) {	
	element->next = first;
	first = element;
    } else {
        // Si la clave es mayor, buscamos un lugar donde insertar el elemento.	    
        for (pointer = first; pointer->next != NULL; pointer = pointer->next) {
            if (sortKey < pointer->next->key) {
		element->next = pointer->next;
	        pointer->next = element;
		return;
	    }
	}
	last->next = element;
	last = element;
    }
}

// Permite remover un elemento de la lista enlazada.
// keyPointer : la referencia al objeto que se quiere remover.
void *List::Remove(int *keyPointer)
{
    // El primer elemento de la lista.	
    ListElement *element = first;
    // El elemento a retornar.
    void *thing;

    // Si la lista está vacia se retorna null.
    if (IsEmpty()) 
	return NULL;

    // Se asigna el elemento.
    thing = first->item;
    // Si el primer elemento es igual al ultimo de la lista se hacen null las referencias.
    if (first == last) {
        first = NULL;
	last = NULL;
    } else {
	// se asigna el elemento siguiente.    
        first = element->next;
    }
    // si el puntero no es null
    if (keyPointer != NULL)
        *keyPointer = element->key;
    // Se borra el elemento
    delete element;
    // Se retorna el elemento deseado.
    return thing;
}

