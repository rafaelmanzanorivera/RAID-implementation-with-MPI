//
//  DiskManager.cpp
//  hito1_Distribuidos
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//

#include "tree.hpp"
#include "ListaEnlazada.h"
#include "mpi.h"


DiskManager::DiskManager()
{
    //Guardamos directorio inicial de los discos
    getcwd(disksDirectory, sizeof(disksDirectory));
    printf("Disks dir is %s\n", disksDirectory);
    
    diskBlocks = 0;
    
}

void DiskManager::remoteFormat(int size)
{
    //TODO for disk in disks
    diskBlocks = size;
    //TODO CARGAR de archivo
    
    int command = FORMAT_CMD;
    
    printf("MASTER: Sending command to 1\n");
    MPI_Send(&command,1,MPI_INT,1,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&size,1,MPI_INT,1,FMT_SIZE_TAG,MPI_COMM_WORLD);

    printf("MASTER: Sending command to 2\n");
    MPI_Send(&command,1,MPI_INT,2,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&size,1,MPI_INT,2,FMT_SIZE_TAG,MPI_COMM_WORLD);

    printf("MASTER: Sending command to 3\n");
    MPI_Send(&command,1,MPI_INT,3,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&size,1,MPI_INT,3,FMT_SIZE_TAG,MPI_COMM_WORLD);

    printf("MASTER: Sending command to 4\n");
    MPI_Send(&command,1,MPI_INT,4,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&size,1,MPI_INT,4,FMT_SIZE_TAG,MPI_COMM_WORLD);

}

int DiskManager::getFreeSectors(int disk_ID)
{   
    MPI_Status status;
    printf("MASTER: Getting disk sectors on disk %d\n", disk_ID);
    int command = GET_FREE_SECTORS_CMD;
    int freeSectors;//TODO long
    loadFreeSectorsFiles();
    MPI_Send(&command,1,MPI_INT,disk_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Recv(&freeSectors,1, MPI_INT,disk_ID, OCPD_SCTRS_AMT_TAG, MPI_COMM_WORLD,&status);
    printf("MASTER: Ocupied sectors received: %d\n", freeSectors);
    return freeSectors;
}

bool DiskManager::isFreeSector(int disk_ID, int block_ID)
{
    MPI_Status status;
    //printf("MASTER: Sector %d is free on disk %d?\n", block_ID, disk_ID+1);
    int command = IS_FREE_SECTOR_CMD;
    int blockID = block_ID;
    bool isFree;//TODO long
    
    //Send command
    MPI_Send(&command,1,MPI_INT,disk_ID+1,MAIN_CMD_TAG,MPI_COMM_WORLD);
    
    //Send block
    MPI_Send(&blockID,1,MPI_INT,disk_ID+1,IS_FREE_BLCK_TAG,MPI_COMM_WORLD);
    
    //Receive answer
    MPI_Recv(&isFree,1, MPI_INT,disk_ID+1, IS_FREE_BOOL_TAG, MPI_COMM_WORLD,&status);
    //printf("MASTER: Received: sector %d is free on disk %d: %d\n",blockID,disk_ID,isFree);
    return isFree;
}

char* DiskManager::getFileBytes(char *name)
{
    FILE *fileptr;
    char *buffer;
    long filelen;
    
    fileptr = fopen(name, "rb");          // Open the file in binary mode
    fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(fileptr);             // Get the current byte offset in the file
    rewind(fileptr);                      // Jump back to the beginning of the file
    
    buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
    fread(buffer, filelen, 1, fileptr); // Read in the entire file
    fclose(fileptr); // Close the file
    return buffer;
}

char* DiskManager::readFile(Node *fileNode)
{
   
    int i = 0;
    int buffBeg;
    int offset = -(fileNode->size%BLOCK_SIZE)+BLOCK_SIZE;
    unsigned long nBlocks = fileNode->blocks->size();
    
    char* buffer;
    char* nextWritingPoint;
    char* auxBuffer;
    
    printf("MASTER: Receiving file...\n");
    
    buffer = (char*)malloc((sizeof(char)*fileNode->size)+1);
    
    nextWritingPoint = buffer;
    
    for(i = 0, buffBeg = 0; i < nBlocks; i++, buffBeg+=BLOCK_SIZE)
    {
        if(offset>0 && (i)==nBlocks) //si estas en el ultimo bloque
        {
            printf("Is last block, size: %lld , offset: %d...\n", fileNode->size,offset);
            auxBuffer = receiveBlock(fileNode->blocks->at(i));
            memcpy(nextWritingPoint+buffBeg, auxBuffer, offset);
            
            //free(auxBuffer);
        }
        else
        {
            auxBuffer = receiveBlock(fileNode->blocks->at(i));
            memcpy(nextWritingPoint+buffBeg, auxBuffer, BLOCK_SIZE);
            //free(auxBuffer);
        }
    }
    buffer[fileNode->size]='\0';
    printf("size of buffer: %zu\n",strlen(buffer));
    
    printf("MASTER: Done receiving file...\n");


    return buffer;
    
}

char* DiskManager::receiveBlock(blockLocation blockLocation)
{
    MPI_Status status;
    int blockID = blockLocation.blockID;
    int command = READ_BLOCK_CMD;
    char* buffer = (char*)malloc(((BLOCK_SIZE)*sizeof(char)));
    
    
    printf("MASTER: Receiving block %d from disk %d\n",blockLocation.blockID,blockLocation.diskID );
    
    MPI_Send(&command,1,MPI_INT,blockLocation.diskID+1,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&blockID,1,MPI_INT,blockLocation.diskID+1,READ_BLCK_TAG,MPI_COMM_WORLD);
    MPI_Recv(buffer,1000,MPI_CHAR,blockLocation.diskID+1,READ_DATA_TAG,MPI_COMM_WORLD,&status);
    
    return buffer;
}

void DiskManager::writeFile(char *fileBytes, Node *fileNode)
{

    int i = 0;
    printf("loop check for writing\n");
    if(diskBlocks == 0) getDiskSize();
    
    //Save current dir to return later
    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory));
    chdir(disksDirectory);
    
    int buffBeg;
    int randomBlock;
    int randomDisk;
    blockLocation actualBlockLocation;
    size_t fileSize = fileNode->size;
    int offset = -(fileSize%BLOCK_SIZE)+BLOCK_SIZE;
    int neededBlocks = (fileSize+offset)/BLOCK_SIZE;
    
    printf("MASTER: Size of the file is %d\n", fileSize);
    printf("MASTER: Empty bytes at end of block %d\n", offset);
    printf("MASTER: Needed blocks %d\n", neededBlocks);
    //TODO SI SOBRAN 1000 restas un needed block
    
    randomBlock = (rand()%diskBlocks)+1;
    randomDisk = (rand()%N_DISKS); //TODO
    actualBlockLocation.blockID = randomBlock;
    actualBlockLocation.diskID = randomDisk;
    
    
    for(i = 0, buffBeg = 0; i < neededBlocks; i++, buffBeg+=BLOCK_SIZE)
    {
        //Encuentras bloque libre
        //while(!freeSectors[randomDisk][randomBlock])
        while(!isFreeSector(randomDisk, randomBlock))
        {
            randomBlock = (rand()%DISK_BLOCKS)+1;
            randomDisk = (rand()%N_DISKS);//TODO
            
            actualBlockLocation.blockID = randomBlock;
            actualBlockLocation.diskID = randomDisk;
           // printf("loop check while writing\n");
        }
        //printf("loop check for writing\n");
        //printf("Random block to save in disk %d, block %d\n", actualBlockLocation.diskID, actualBlockLocation.blockID);
        
        //Guardas los siguentes 1000 bytes en el bloque numero randomBlock
        //printf("MASTER: Sending to disk %d, block %d\n", actualBlockLocation.diskID,actualBlockLocation.blockID);
        sendBlock(fileBytes+buffBeg, actualBlockLocation);
        
        //Guardas numero de bloque en la lista de bloques del nodo del archivo
        fileNode->blocks->push_back(actualBlockLocation);
    
    }
    
    printf("MASTER: File saved in %d blocks\n", neededBlocks);
    for(int s = 0; s<fileNode->blocks->size(); s++)
    {
        printf("BlockID: %d\tDiskID: %d\n", fileNode->blocks->at(s).blockID, fileNode->blocks->at(s).diskID);
        printf("loop print for writing\n");
    }
    
    chdir(currentDirectory);
    updateFreeSectorsFiles();
    
}

void DiskManager::sendBlock(char* blockBeg, blockLocation blockLocation)
{
    //int nslave = blockLocation.diskID;
    //nslave++;

    int blockID = blockLocation.blockID;
    
    int command = WRITE_BLOCK_CMD;
    
    printf("MASTER: Sending block %d to disk %d\n",blockLocation.blockID,blockLocation.diskID );
    
    MPI_Send(&command,1,MPI_INT,blockLocation.diskID+1,MAIN_CMD_TAG,MPI_COMM_WORLD);

    MPI_Send(&blockID,1,MPI_INT,blockLocation.diskID+1,WRT_BLCK_TAG,MPI_COMM_WORLD);

    MPI_Send(blockBeg,1000,MPI_CHAR,blockLocation.diskID+1,WRT_DATA_TAG,MPI_COMM_WORLD);
}


void DiskManager::deleteFile(Node* fileNode)
{
    
    int command = FREE_BLCK_CMD;
    int blockToFree;
    int slaveOwnerOfBlock;
    
    printf("MASTER: Freeing blocks of file %s\n", fileNode->label.c_str());
    for(int i = 0; i< fileNode->blocks->size(); i++)
    {
        blockToFree = fileNode->blocks->at(i).blockID;
        slaveOwnerOfBlock = fileNode->blocks->at(i).diskID+1;

        MPI_Send(&command,1,MPI_INT,slaveOwnerOfBlock,MAIN_CMD_TAG,MPI_COMM_WORLD);
        MPI_Send(&blockToFree,1,MPI_INT,slaveOwnerOfBlock,FREE_BLCK_TAG,MPI_COMM_WORLD);
    }
    
    updateFreeSectorsFiles();
}

void DiskManager::updateFreeSectorsFiles()
{
    
    int command = UPDT_FREE_SCTRS_FLS_CMD;
    
    printf("MASTER: Sending update freeSectors files signal to slaves\n");
    
    MPI_Send(&command,1,MPI_INT,SLAVE_1_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&command,1,MPI_INT,SLAVE_2_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&command,1,MPI_INT,SLAVE_3_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&command,1,MPI_INT,SLAVE_4_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    
}


void DiskManager::loadFreeSectorsFiles()
{
    int command = LD_FREE_SCTRS_FLS_CMD;
    
    printf("MASTER: Sending load freeSectors files signal to slaves\n");
    
    MPI_Send(&command,1,MPI_INT,SLAVE_1_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&command,1,MPI_INT,SLAVE_2_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&command,1,MPI_INT,SLAVE_3_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Send(&command,1,MPI_INT,SLAVE_4_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
}


void DiskManager::getDiskSize()
{
    MPI_Status status;
    int diskSize;
    int command = GET_DISK_SZ_CMD;
    
    MPI_Send(&command,1,MPI_INT,SLAVE_1_ID,MAIN_CMD_TAG,MPI_COMM_WORLD);
    MPI_Recv(&diskSize,1000,MPI_INT,SLAVE_1_ID,GET_DISK_SZ_TAG,MPI_COMM_WORLD,&status);
    
    printf("Setting disk size to %d\n",diskSize);
    
    this->diskBlocks = diskSize;
    
}

void DiskManager::writeFiletoLocalDisk(char *buffer, char* name)
{
    FILE *fp;
    fp = fopen(name, "wb");
    if(fp == NULL)
        return;
    
    fwrite(buffer, strlen(buffer), 1, fp);
    fclose(fp);
}

DiskManager::~DiskManager()
{
   //TODO free mallocs (freeSpaces)
    
}

