#pragma once
#include "tree.hpp"


/**
 * @file Nodo.h
 * @author Rafael Manzano
 * @date 19 Dic 2018
 * @brief File containing the struct NodoLista
 */



/**
 * @brief struct that store a Node and the next NodoLista. Used by a linked list
 */
struct NodoLista
{
	Node* elemento; /** El elemento que guarda el nodo */
	NodoLista *siguienteNodo; /** Puntero al siguiente nodo de la lista enlazada */
};


