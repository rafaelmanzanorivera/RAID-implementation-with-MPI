//
//  DiskSlave.cpp
//  hito_final_Slave
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//




#include <stdlib.h>
#include <stdio.h>
#include "tree.hpp"
#include <string>
#include "mpi.h"
#include "defines.hpp"




DiskSlave::DiskSlave(int id)
{
    this->slave_ID = id;
    
    //Inicializamos las estructuras dinamicas de freeSectors
    freeSectors = (bool *)malloc(diskBlocks * sizeof(bool));//TODO escalable
    for(int j = 0; j<diskBlocks;j++) freeSectors[j] = true;
    
    // blockSize = blockSize_;
    // n = n_;
    // size = size_;
    printf("Creating disk slave\n");
}

void DiskSlave::listen(){
    bool end = false;
    
    
    while(!end)
    {
        MPI_Status status;
        int command;
        MPI_Recv(&command,1,MPI_INT,MASTER_ID,MAIN_CMD_TAG, MPI_COMM_WORLD, &status);
        //mpi recieve -> command
        int a = 0;
        switch(command)
        {
            case WRITE_BLOCK_CMD:
            {
                writeBlock();
                break;
            }
                
                
            case READ_BLOCK_CMD:
            {
                readBlock();
                break;
            }
                
                
            case EXIT_CMD:
            {
                end = true;
                break;
            }
                
            case FORMAT_CMD:
            {
                format();
                break;
            }
                
            case UPDT_FREE_SCTRS_FLS_CMD:
            {
                updateFreeSectorsFiles();
                break;
            }
                
            case LD_FREE_SCTRS_FLS_CMD:
            {
                loadFreeSectorsFiles();
                break;
            }
                
            case IS_FREE_SECTOR_CMD:
            {
                isFreeSector();
                break;
            }

            case GET_FREE_SECTORS_CMD:
            {
                sendFreeSectorsAmount();
                break;
            }
                
            case GET_DISK_SZ_CMD:
            {
                sendDiskSize();
                break;
            }
                
            default:
            {
                a = 0;
                break;
            }
        }
    }
}

void DiskSlave::format()
{
    MPI_Status status;
    int size;
    MPI_Recv(&size,1,MPI_INT,MASTER_ID,FMT_SIZE_TAG, MPI_COMM_WORLD, &status);
    this->diskBlocks = size;

    printf("Slave %d: Formatting diskSlave to %d\n",slave_ID, size);
       //Inicializamos las estructuras dinamicas de freeSectors
       freeSectors = (bool *)malloc(diskBlocks * sizeof(bool));//TODO escalable
       for(int j = 0; j<diskBlocks;j++) freeSectors[j] = true;
    
       //Creamos el archivo freeSectors.dat
       freeSectors_dat = fopen("freeSectors.dat", "w+");
       if (freeSectors_dat == NULL)
       {
           fprintf(stderr, "\nError opend file\n");
           return;
       }
       fwrite(freeSectors, sizeof(bool), diskBlocks, freeSectors_dat);
    
       //Creamos el archivo disk.dat
       disk_dat = fopen ("disk.dat", "w+");
       if (disk_dat == NULL)
       {
           fprintf(stderr, "\nError opend file\n");
           return;
       }
       ftruncate(fileno(disk_dat), diskBlocks*BLOCK_SIZE);
    
       fclose(disk_dat);
       fclose(freeSectors_dat);
    
        system("ls -ls");
    
}


void DiskSlave::sendFreeSectorsAmount()
{
    //COMPROBAR FORMATEO PREVIO
    int ocupied0 = 0;
    
    for(int i = 0; i < this->diskBlocks; i++)
    {
        if(!freeSectors[i]) ocupied0++;
    }
    MPI_Send(&ocupied0,1,MPI_INT,MASTER_ID,OCPD_SCTRS_AMT_TAG,MPI_COMM_WORLD);
}


void DiskSlave::isFreeSector()
{
    MPI_Status status;
    int blockID;
    MPI_Recv(&blockID,1,MPI_INT,MASTER_ID,IS_FREE_BLCK_TAG, MPI_COMM_WORLD, &status);
    
    bool answer = freeSectors[blockID];
   // printf("Slave %d: Sector number %d is free? %d\n", slave_ID,blockID,answer);

    MPI_Send(&answer,1,MPI_C_BOOL,MASTER_ID,IS_FREE_BOOL_TAG,MPI_COMM_WORLD);
}



void DiskSlave::writeBlock()
{
    MPI_Status status;
    int blockID;
    char* blockBeg = (char*)malloc((sizeof(char)*BLOCK_SIZE)+1);
    
    //Receive block
    MPI_Recv(&blockID,1,MPI_INT,MASTER_ID,WRT_BLCK_TAG, MPI_COMM_WORLD, &status);
    //Receive data
    MPI_Recv(blockBeg,BLOCK_SIZE,MPI_CHAR,MASTER_ID, WRT_DATA_TAG, MPI_COMM_WORLD, &status);

    int diskStartPoint = blockID*BLOCK_SIZE;
    FILE *fileptr;
    long filelen;

    fileptr = fopen("disk.dat", "r+b");          // Open the file in binary mode
    fseek(fileptr, diskStartPoint, SEEK_SET);          // Jump to the block
    filelen = ftell(fileptr);
    printf("Slave %d: Im in position %ld, writing block\n", slave_ID, filelen);// Get the current byte offset in the file
    fwrite(blockBeg, 1, BLOCK_SIZE, fileptr);
    
    fclose(fileptr); // Close the file
    
    //Actualizas sectores libres
    freeSectors[blockID] = false;
    
}


void DiskSlave::readBlock()
{
    MPI_Status status;
    int blockID;
    
    //Receive desired block
    MPI_Recv(&blockID,1,MPI_INT,MASTER_ID,READ_BLCK_TAG, MPI_COMM_WORLD, &status);
    
    int diskStartPoint = blockID*BLOCK_SIZE;
    char *buffer;
    FILE *fileptr;
    
    fileptr = fopen("disk.dat", "r+b");
    if (fileptr == NULL)
    {
        fprintf(stderr, "\nError opening file (READ_BLOCK)\n");
    }
    
    fseek(fileptr, diskStartPoint, SEEK_SET);
    buffer = (char *)malloc((BLOCK_SIZE+1)*sizeof(char));
    fread(buffer, BLOCK_SIZE, 1, fileptr);
    fclose(fileptr); // Close the file
    
    //Send back data
    MPI_Send(buffer,BLOCK_SIZE,MPI_CHAR,MASTER_ID, READ_DATA_TAG, MPI_COMM_WORLD);
}


void DiskSlave::freeBlock()
{
    MPI_Status status;
    int blockID;
    
    //Receive desired block
    MPI_Recv(&blockID,1,MPI_INT,MASTER_ID,FREE_BLCK_TAG, MPI_COMM_WORLD, &status);
    
    printf("Slave %d: Freeing block %d...\n", slave_ID, blockID);
    
    freeSectors[blockID] = true;
    
}


void DiskSlave::updateFreeSectorsFiles()
{
    freeSectors_dat = fopen("freeSectors.dat", "w+");
    if (freeSectors_dat == NULL)
    {
        fprintf(stderr, "\nError opend file\n");
        return;
    }
    printf("Slave %d: Updating freeSectors file\n",slave_ID);
    
    fwrite(freeSectors, sizeof(bool), diskBlocks, freeSectors_dat);
    
    fclose(freeSectors_dat);
}


void DiskSlave::loadFreeSectorsFiles()
{
    struct stat fileStat;
    
    printf("Slave %d: Loading free sectors file\n", slave_ID);
    freeSectors_dat = fopen("freeSectors.dat", "r+b");
    if (freeSectors_dat == NULL)
    {
        fprintf(stderr, "\nError opend file\n");
        return;
    }
    
    fread(freeSectors, sizeof(bool), diskBlocks, freeSectors_dat);
    fclose(freeSectors_dat);
    

    if(!stat("freeSectors.dat",&fileStat)) diskBlocks = fileStat.st_size;
    
    printf("Diskblock size loaded, %d blocks\n", diskBlocks);
}

void DiskSlave::sendDiskSize()
{
    struct stat fileStat;
    
    if(!stat("freeSectors.dat",&fileStat)) diskBlocks = fileStat.st_size;
    
    MPI_Send(&diskBlocks,1,MPI_INT,MASTER_ID,GET_DISK_SZ_TAG,MPI_COMM_WORLD);
    
    printf("Sending actual disk size\n");
}
