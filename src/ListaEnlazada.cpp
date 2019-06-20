#include "ListaEnlazada.h"
#include "cassert"
using namespace std;

NodoLista * ListaEnlazada::getNodo (int posicion) {
	assert(n > 0);
	assert(posicion >= 0 && posicion <= n-1);

	NodoLista *resultado = NULL;
	resultado = lista;

	for(int i = 0; i < posicion; i++)
	{
        resultado = resultado->siguienteNodo;
	}
	
	return (resultado);
}

ListaEnlazada::ListaEnlazada() {
	lista = NULL; 
	n=0;
	
}

Node* ListaEnlazada::getValor(int posicion) {
	assert(n > 0);
	assert (posicion >= 0 && posicion <= n-1); 
	
	NodoLista *nodoResultado = getNodo (posicion);
    
	return (nodoResultado->elemento);
}

void ListaEnlazada::setValor(int posicion, Node* nuevoValor)
{
	assert(n > 0);
	assert (posicion >= 0 && posicion <= n-1); 
	getNodo(posicion)->elemento = nuevoValor;
}

int ListaEnlazada::getN() {
	return (n);
}

void ListaEnlazada::insertar (int posicion, Node* nuevoValor) {
	assert (posicion >= 0 && posicion <= n); 

	// Creamos el nuevo nodo
	NodoLista *nuevoNodo = new NodoLista;
	nuevoNodo->elemento = nuevoValor;

	// Lista vacia
	if (n==0)
    {
        lista = nuevoNodo;
        nuevoNodo->siguienteNodo = NULL;

    }
    
	// Si la lista no está vacía
	else {
		
        if(posicion == 0)
        {
            nuevoNodo->siguienteNodo = lista;
            lista = nuevoNodo;
        }
        else
        {
            NodoLista *nodoSiguiente;
            NodoLista *nodoAnterior;
            
            nodoAnterior = getNodo(posicion-1);
            nodoSiguiente = nodoAnterior->siguienteNodo;
            
            nodoAnterior->siguienteNodo = nuevoNodo;
            nuevoNodo->siguienteNodo = nodoSiguiente;
        }
		
	} 
	n++;

}


void ListaEnlazada::eliminar (int posicion) {
	assert(n > 0);
	assert (posicion >= 0 && posicion <= n-1); 

    
    NodoLista *nodoAEliminar = NULL;;

	if (n == 1)
    {
        nodoAEliminar = getNodo(posicion);
        lista = NULL;
    }
    
	else {
        
        if(posicion == 0)
        {
            NodoLista *nodoSiguiente = lista->siguienteNodo;
            nodoAEliminar = lista;
            lista = nodoSiguiente;
        }
	
        else
        {
            NodoLista *nodoAnterior = getNodo(posicion-1);
            NodoLista *nodoAEliminar = nodoAnterior->siguienteNodo;
            
            nodoAnterior->siguienteNodo=nodoAEliminar->siguienteNodo;
        }
		
	}
    
	delete(nodoAEliminar);

	n--;

}

void ListaEnlazada::concatenar(ListaEnlazada *listaAConcatenar) {

    NodoLista *lastNodo = getNodo(getN()-1);
    lastNodo->siguienteNodo = listaAConcatenar->lista;
    
    n += listaAConcatenar->getN();
}

Node* ListaEnlazada::buscar(string elementoABuscar) {
	int posicion = 0;
	bool encontrado = false;
    Node* nodoBuscado = NULL;

	
	while (!encontrado && posicion < n) {
		
		if (getValor(posicion)->label.compare(elementoABuscar) == 0)
        {
            encontrado = true;
            nodoBuscado = getValor(posicion);
        }
		posicion++;
	}

	
	if (encontrado) return (nodoBuscado);
	else return (NULL);
}

int ListaEnlazada::buscarPosicion(string elementoABuscar)
{
    int posicion = 0;
    bool encontrado = false;
    
    while (!encontrado && posicion < n)
    {
        
        if (getValor(posicion)->label.compare(elementoABuscar) == 0)
        {
            encontrado = true;
            return posicion;
        }
        posicion++;
    }
    
    return -1;
}




ListaEnlazada::~ListaEnlazada() {
	while (n>0) eliminar(0);
}

