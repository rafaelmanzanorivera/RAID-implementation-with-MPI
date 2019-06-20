//
//  arbol.cpp
//  Hito1_SistemasDistribuidos
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//

#include "tree.hpp"
#include "ListaEnlazada.h"

Tree::Tree()
{
    root = new Node();
    
    root->father = NULL;
    root->tree = this;
    root->label = "/";
    root->isDirectory = true;
    root->size = 4096;
    root->sons = (ListaEnlazada*)new ListaEnlazada();
    
    diskManager = new DiskManager();
    
    currentDirectory = root;
    lastNodeId = root->ID;
}



void Node::deleteSubtree()
{
    ListaEnlazada *sons = this->sons;
    ListaEnlazada *aux = NULL;
    
    int i;
    
     printf("INI delete\n");
    //Guardas hijos si tiene
    if(this->sons == NULL)
    {
        delete this;
        return;
    }
    
    if(this->sons->getN() != 0)
    {
        aux = new ListaEnlazada();
        
        //Guardamos en lista auxiliar
        for (i = 0; i<sons->getN(); i++)
        {
            aux->insertar(i, sons->getValor(i));
            printf("saving son\n");
        }
        
        //Eliminas raiz
        printf( "deleting current\n");
        this->tree->nNodes--;
        delete this;
         //LLamada recursiva a borrar hijos
        for (i = 0; i<aux->getN(); i++)
        {
            printf( "deleting son ---> REC\n");
            aux->getValor(i)->deleteSubtree();
            
        }
    }
    else
    {
        printf( "deleting lasto");
        this->tree->nNodes--;
        delete this;
        
    }
    
    delete aux;
}



void Tree::saveTree()
{
    FILE *outfile;
    
    //Save current dir to return later
    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory));
    chdir(this->diskManager->disksDirectory);
    
    // open file for writing
    outfile = fopen ("tree.dat", "w+");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nError opend file\n");
        return;
    }
    
    //Call save recursive
    this->root->saveNode(outfile);
    
    //Check
    if(fwrite != 0)
        printf("contents to file written successfully !\n");
    else
        printf("error writing file !\n");
    
    // close file
    fclose (outfile);
    
    chdir(currentDirectory);
}
void Node::saveNode(FILE *outfile)
{
    
    serializableNode newNode;

    
    //Conviertes info relevante a struct
    
    newNode.nodeId = this->ID;
    
    //Root
    if(!this->label.compare("/")) newNode.fatherId = -1;
    else newNode.fatherId = this->father->ID;
    
    newNode.level = this->level;
    newNode.isDirectory = this->isDirectory;
    newNode.size = this->size;
    newNode.lastModified = this->lastModified;
    
    strcpy(newNode.label, this->label.c_str());
    
    if(this->isDirectory)
    {
        memset(newNode.sons, -1, MAX_SONS_PER_NODE);
        newNode.nSons = this->sons->getN();
        for(int i = 0; i<this->sons->getN();i++)
        {
            newNode.sons[i] = this->sons->getValor(i)->ID;
        }
        newNode.nBlocks = -1;
    }
    else
    {
        //TODO inicializar con memset(newNode.diskBlocks, -1, 100);
        newNode.nBlocks = this->blocks->size();
        for(int i = 0; i<this->blocks->size();i++)
        {
            newNode.diskBlocks[i].blockID = this->blocks->at(i).blockID;
            newNode.diskBlocks[i].diskID = this->blocks->at(i).diskID;
        }
        newNode.nSons = -1;
    }
    
    printf("Saving object %s\n",newNode.label);
    //Guardas struct en archivo
    fwrite (&newNode, sizeof(serializableNode), 1, outfile);
    
    
    //LLamas recursivamente
    if(this->isDirectory)
    {
        for (int i = 0; i<this->sons->getN(); i++)
        {
            this->sons->getValor(i)->saveNode(outfile);
        }
    }
}


void Tree::loadTree()
{
    FILE *infile;
    serializableNode input;
    Node* newNode;
    
    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory));
    chdir(this->diskManager->disksDirectory);
    
    // open file for reading
    infile = fopen ("tree.dat", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nError opening file\n");
        return;
    }
    
    //load disk information
    this->diskManager->loadFreeSectorsFiles();
    
    // read file contents till end of file
    while(fread(&input, sizeof(serializableNode), 1, infile))
    {
        printf("Node ID: %d\n",input.nodeId);
        printf("Father Id: %d\n",input.fatherId);
        printf("label: %s\n",input.label);
        printf("nSons: %d\n",input.nSons);
        if(input.isDirectory)
        {
            for(int i = 0; i<input.nSons;i++)
            {
                printf("    Son ID: %d\n",input.sons[i]);
            }
        }
        
        printf("nBlocks %lu\n", input.nBlocks);
        if(!input.isDirectory)
        {
            for(int i = 0; i<input.nBlocks;i++)
            {
                printf("    BlockID: %d\tDiskID %d\n ",input.diskBlocks[i].blockID,input.diskBlocks[i].diskID);
            }
        }
        
               
        printf("\n\n");
        
        if(input.nodeId != 0)
        {
            newNode = new Node();
            
            newNode->tree = this;
            
            newNode->ID = input.nodeId;
            newNode->father = this->searchNodeInTree(input.fatherId);
            newNode->label = string(input.label);
            newNode->level = input.level;
            newNode->isDirectory = input.isDirectory;
            newNode->size = input.size;
            
            //Si es directorio cargas hijos
            if(input.isDirectory)
            {
                newNode->sons = new ListaEnlazada();
                newNode->blocks = NULL;
            }
            
            //Si es fichero cargas informacion de los bloques
            else
            {
                newNode->blocks = new vector<blockLocation>;
                newNode->sons = NULL;
                
                for(int i = 0; i<input.nBlocks;i++)
                {
                    newNode->blocks->push_back(input.diskBlocks[i]);
                }
            }
            
            
            newNode->father->sons->insertar(0, newNode);
            this->nNodes++;
            
        }
        
    }
    
    // close file
    fclose (infile);
    
    // Return to source directory
    chdir(currentDirectory);
}

void Node::loadNode(Node* nodeToLoad)
{

    //Lees struct de archivo
    //Inicializas nuevo nodo con info leida
    
}

Node* Tree::searchNodeInTree(int nodeID)
{
    Node *searchedNode;
    
    searchedNode = this->root->findNode(nodeID);
    return searchedNode;
}

Node* Node::findNode(int ID)
{
    int i;
    
    if(this->ID == ID)
        return this;
    
    else
        for(i = 0; i<this->sons->getN(); i++)
        {
            return this->sons->getValor(i)->findNode(ID);
        }
    
    return NULL;
    
}

Tree::~Tree()
{
    if(root != NULL)
    {
        root->deleteSubtree();
        root = NULL;
        nNodes = 0;
    }
    
}
