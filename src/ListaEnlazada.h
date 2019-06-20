/**
 * @file ListaEnlazada.h
 * @author Rafael Manzano
 * @date 19 Dic 2018
 * @brief File containing the linked list used by the program
 */


#include "Nodo.h"
#include "tree.hpp"
#ifndef ListaEnlazada_h
#define ListaEnlazada_h


 /**
  * @brief Linked list that store Node elements
  */
class ListaEnlazada
{
	NodoLista *lista; /**Node pointer to a NodoLista struct */

	int n; /**Number of elements in the list */

	/**
	* @brief Method that return a NodoLista by its position
	* @param int with the NodoLista's position
	* @return A NodoLista pointer
	*/
	NodoLista * getNodo (int posicion);

public:

	ListaEnlazada();

	/**
	* @brief Method that return a Node by its position
	* @param int with the Node's position
	* @return A Node pointer
	*/
	Node* getValor(int posicion);

	/**
	* @brief Method that return a Node by its position
	* @param int with the Node's position
	* @return A Node pointer
	*/
	void setValor(int posicion, Node* nuevoValor);

	/**
	* @brief Method that return the number of elements in the list
	* @return Int, the number os elements in the list
	*/
	int getN(); 

	/**
	* @brief Method that insert an element in a position
	* @param position int with the Node's position
	* @param nuevoValor 2 Node pointer with the new Node
	*/
	void insertar (int posicion, Node* nuevoValor);

	/**
	* @brief Method that remove an element in a position
	* @param int with the Node's position
	*/
	void eliminar (int posicion);

	/**
	* @brief Method that concat that list with another one
	* @param New list to concat
	*/
	void concatenar(ListaEnlazada *listaAConcatenar);

	/**
	* @brief Method that search an element by its name
	* @param string with the element's name
	* @return Node pointer 
	*/
    Node* buscar(string elementoABuscar);

	/**
	* @brief Search a element position by its name
	* @param string with the element's name
	* @return Int, the position
	*/
    int buscarPosicion(string elementoABuscar);

	~ListaEnlazada();
};

#endif

