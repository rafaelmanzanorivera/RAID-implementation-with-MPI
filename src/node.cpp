//
//  nodo.cpp
//  Hito1_SistemasDistribuidos
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//

#include "tree.hpp"
#include "ListaEnlazada.h"
int Node::counterNodesID =0;

Node::Node()
{
    this->ID = counterNodesID++;
    this->lastModified = this->getTimeRaw();
}

int Node::getLevel()
{
    if(this->father == NULL) return 1;
    else return 1 + this->father->getLevel();
}




Node* Node::addChild(string label,bool isDirectory)
{
    //Creamos nuevo nodo
    Node *newNode = new Node();
    
    
    //Inicializamos nodo
    newNode->tree = this->tree;
    newNode->father = this;
    newNode->label = label;
    newNode->level = this->getLevel()+1;
    newNode->isDirectory = isDirectory;
    
    //añador nuevo nodo a lista del padre
    this->sons->insertar(0, newNode);
    
    if(isDirectory)
    {
        newNode->sons = new ListaEnlazada();
        newNode->size = 4096;
    }
    
    else
    {
        newNode->sons = NULL;
        newNode->size = 0;//TODO
        newNode->blocks = new vector<blockLocation>;
    }
    
    //Actualizamos numero de nodos del arbol
    this->tree->nNodes++;
    
    
    return newNode;
}

void Node::addNode(Node *newNode, Node *father)
{
    father->sons->insertar(0, newNode);
    this->tree->nNodes++;
}

long int Node::getTimeRaw()
{
    struct timeval tv;
    struct tm* ptm;
    long int milliseconds;
    
    /* Obtain the time of day, and convert it to a tm struct. */
    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);

    milliseconds = tv.tv_usec / 1000;

    return milliseconds;
}


string Node::getTimeFormatted()
{
    struct timeval tv;
    struct tm* ptm;
    char time_string[40];
    long milliseconds;
    string time;
    
    /* Obtain the time of day, and convert it to a tm struct. */
    gettimeofday (&tv, NULL);
    ptm = localtime (&tv.tv_sec);
    /* Format the date and time, down to a single second. */
    strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
    /* Compute milliseconds from microseconds. */
    milliseconds = tv.tv_usec / 1000;
    /* Print the formatted time, in seconds, followed by a decimal point
     and the milliseconds. */
    //printf ("%s.%03ld\n", time_string, milliseconds);

    time = time_string;
    
    return time;
    
    
}

string Node::getPath()
{
    if(this->father == NULL)
        return "/";
    else
        return this->father->getPath().append(label).append("/");
}




Node::~Node()
{
    if(this->isDirectory)
        delete sons;
    else
    {
        this->tree->diskManager->deleteFile(this);
        delete blocks;
    }
}
