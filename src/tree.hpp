//
//  tree.hpp
//  Hito1_SistemasDistribuidos
//
//  Created by Rafael Manzano on 2/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//


/**
 * @file tree.hpp
 * @author Rafael Manzano
 * @date 19 Dic 2018
 * @brief File containing the classes Tree, DiskManager, Node and DiskSlave, and structs blockLocation and serializableNode
 */





#ifndef tree_hpp
#define tree_hpp

class ListaEnlazada;
class DiskManager;

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sys/stat.h>
#include <sys/time.h>
#include <string>
#include "defines.hpp"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>





using namespace std;

class Tree;


/**
 * @brief Struct that contain the identification of a block
 */
typedef struct blockLocation
{
    int blockID; 
    int diskID; 
} blockLocation;


/**
 * @brief Class that implement a Node of a file system
 */
class Node
{
    
    private:
    
        static int counterNodesID; /** Class atribute, counter of nodes */
        long int getTimeRaw();
    
    
    
    public:
    
        Tree *tree; /** root of file system */
        Node *father; /**father Node */
        ListaEnlazada *sons; /**List of nodes that are their sons */
        string label; /**Name */
        unsigned int ID; 
        unsigned int level; /** Height in the tree */
        bool isDirectory; /**True if is directory, false if is file */
        off_t size; /**Node content's size */
        long lastModified; 
    
        vector<blockLocation> *blocks; /**Blocks that occupies */
    
        void saveNode(FILE *outfile); 
        void loadNode(Node* nodeToLoad);

        Node *addChild(string label, bool isDirectory);//TODO assert padre es directorio
        void addNode(Node *newNode, Node *father);
        Node* findNode(int ID);
        string getTimeFormatted();
        void deleteSubtree();
        string getPath();
    
        Node();
        ~Node();
    
        int getLevel();
    
    
};



/**
 * @brief Class that control the disk
 */
class DiskManager
{
public:
    
    FILE *disk_dat[N_DISKS]; 
    FILE *freeSectors_dat[N_DISKS]; 
    char disksDirectory[1024];
    
    long diskBlocks; 
    
    DiskManager();
    
    bool **freeSectors; 
    
    void format(int size);
    void remoteFormat(int size);

    int getFreeSectors(int disk_ID);
    bool isFreeSector(int disk_ID, int block_ID);
    
    void writeFile(char* fileBytes,Node* fileNode);
    char* readFile(Node* fileNode);
    
    void writeBlock(char* blockBeg, blockLocation blockLocation);
    char* readBlock(blockLocation blockLocation);

    void sendBlock(char* blockBeg, blockLocation blockLocation);
    char* receiveBlock(blockLocation blockLocation);


    
    void deleteFile(Node* fileNode);
    
    void updateFreeSectorsFiles();
    void loadFreeSectorsFiles();
    
    void getDiskSize();
    
    ~DiskManager();
    
    char* getFileBytes(char* name);
    void writeFiletoLocalDisk(char* buffer,char* name);
    
};

/**
 * @brief Class that store the structure of the file system
 */
class Tree
{
    unsigned int lastNodeId; 
    
public:
    unsigned int nNodes = 0; 
    Tree();
    Node *root; 
    Node *currentDirectory; 
    DiskManager *diskManager;
    
    
    void saveTree();
    void loadTree();
    
    Node* searchNodeInTree(int nodeID);
    
    void updateNode();
    
    ~Tree();
};


/**
 * @brief struct that store the information of a node in order to serialize it
 */
struct serializableNode
{
    unsigned int nodeId;
    int fatherId; 
    unsigned int level; 
    bool isDirectory; 
    off_t size;
    long lastModified; 
    char label[MAX_LABEL_LENGHT];
    int nSons;
    int sons[MAX_SONS_PER_NODE]; 
    unsigned long nBlocks; 
    blockLocation diskBlocks[MAX_BLOCKS_PER_FILE]; 
};

/**
 * @brief Class that manage the disk by the order of the DiskManager
 */
class DiskSlave {
    
public:
    
    DiskSlave(int id);
    
    int slave_ID;
    FILE *disk_dat; 
    FILE *freeSectors_dat;
    int size; 
    int blockSize;
    int n; /** ndisks */
    
    bool *freeSectors; 
    long diskBlocks; 

    void sendFreeSectorsAmount();//ret int
    
    void isFreeSector();//ret bool
    
    void sendDiskSize();
    
    void listen();
    
    void format();
    
    void writeBlock();
    
    void readBlock();
    
    void freeBlock();
    
    void updateFreeSectorsFiles();
    
    void loadFreeSectorsFiles();
    
};


#endif /* arbol_hpp */
