#ifndef _LIST_H
#define _LIST_H

#define TRUE_ true
#define FALSE_ false

// Clase que define un elemento de una lista enlazada.
// Atributos:
// int key : clave que permite identificar al elemento.
// void *item : el valor del elemento.
//
// Metodos:
// ListElement(void *itemPointer, int sortKey) : Constructor de la clase, permite inicializar una
// nueva instancia de la clase ListElement.
//
// 	Parametros:
// 	void * itemPointer : Puntero al elemento a insertar.
// 	int sortKey : clave para la inserción ordenada del elemento.
class ListElement {
   public:
     ListElement(void *itemPointer, int sortKey);  
     ListElement *next;		
     int key;		    	
     void *item; 	    	
};

// Clase que permite representar una lista enlazada simple de elementos ListElement.
// Atributos:
// ListElement *first: Representa el primer elemento de la lista.
// ListElement *last: Representa el ultimo elemento de la lista.
//
// Metodos:
// bool IsEmpty(): Permite saber si la lista está vacia.
// void Insert(void *item, int sortKey): Permite insertar el elemento identificado por item y sortKey.
// void * Remove(int keyPointer): Permite retornar el elemento identificado por keyPointer.
class List {
  public:
    List();		
    ~List();		

    bool IsEmpty();	
    void Insert(void *item, int sortKey);
    void *Remove(int *keyPointer); 	

  private:
    ListElement *first;  	
    ListElement *last;		
};
#endif 

