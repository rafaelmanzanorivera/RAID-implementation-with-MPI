//
//  terminal.cpp
//  Hito1_SistemasDistribuidos
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//

#include "terminal.hpp"
#include"ListaEnlazada.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>


using namespace std;

Terminal::Terminal()
{
    tree = new Tree();
    tree->loadTree();
    //chdir("hito1_Distribuidos");

}


void Terminal::run(){
    bool exit=false;
    //int command=-1;
    command_t command;
    
    command.arguments = new vector<char*>();
    
    
    while(!exit){
        readCommand(&command);
        runCommand(&command);
        
    };
}


void Terminal:: readCommand(command_t *command)
{
    char* linea=new char [1024];
    char separador[5]=" \n\n\0";
    char* token=NULL;
    char* tokenSpaced=NULL;

    setvbuf(stdout, NULL, _IONBF, 0);
    
    //leer terminal
    
    while(token == NULL)
    {
        //cout << this->tree->currentDirectory->getPath() << "> ";
        printf("%s> ", this->tree->currentDirectory->getPath().c_str());
        fgets(linea, 1023, stdin);
        command->arguments->clear();
        token=strtok(linea, separador);
    }
    
    //Add space to token (Corrects cda as valid)
    size_t len = strlen(token);
    tokenSpaced = new char[len+2];
    strcpy(tokenSpaced, token);
    tokenSpaced[len] = ' ';
    tokenSpaced[len+1] = '\0';
    
    command->type=getCommandType(tokenSpaced);
    
    while(token!=NULL)
    {
        token=strtok(NULL, separador);
        command->arguments->push_back(token);
    }
}


int Terminal::getCommandType(char *comando)
{
    if(comando != NULL)
    {
        if(!strncmp("cd ", comando, 3))
            return CMD_CD;
        if(!strncmp("mkdir ", comando, 6))
            return CMD_MKDIR;
        if(!strncmp("exit", comando, 4))
            return CMD_EXIT;
        if(!strncmp("ls", comando, 2))
            return CMD_LS;
        if(!strncmp("pwd", comando, 3))
            return CMD_PWD;
        if(!strncmp("lls", comando, 3))
            return CMD_LLS;
        if(!strncmp("rmdir ", comando, 6))
            return CMD_RMDIR;
        if(!strncmp("rm ", comando, 3))
            return CMD_RM;
        if(!strncmp("upload ", comando, 7))
            return CMD_UPLOAD;
        if(!strncmp("lpwd", comando, 4))
            return CMD_LPWD;
        if(!strncmp("lcd ", comando, 4))
            return CMD_LCD;
        if(!strncmp("cp ", comando, 3))
            return CMD_CP;
        if(!strncmp("mv ", comando, 3))
            return CMD_MV;
        if(!strncmp("cat ", comando, 4))
            return CMD_CAT;
        if(!strncmp("format ", comando, 7))
            return CMD_FORMAT;
        if(!strncmp("diskinfo", comando, 8))
            return CMD_DISKINFO;
        if(!strncmp("download ", comando, 9))
            return CMD_DOWNLOAD;
        
    }
    return -1;
}


void Terminal::runCommand(command_t* command)
{
    switch(command->type)
    {
        case CMD_CD:
            this->cd_CMD(command);
            break;
        case CMD_MKDIR:
            this->mkdir_CMD(command);
            break;
        case CMD_EXIT:
            this->exit_CMD(command);
            break;
        case CMD_LS:
            this->ls_CMD(command);
            break;
        case CMD_LLS:
            this->lls_CMD(command);
            break;
        case CMD_PWD:
            this->pwd_CMD(command);
            break;
        case CMD_RM:
            this->rm_CMD(command);
            break;
        case CMD_RMDIR:
            this->rmdir_CMD(command);
            break;
        case CMD_UPLOAD:
            this->upload_CMD(command);
            break;
        case CMD_LPWD:
            this->lpwd_CMD(command);
            break;
        case CMD_LCD:
            this->lcd_CMD(command);
            break;
        case CMD_CP:
            this->cp_CMD(command);
            break;
        case CMD_MV:
            this->mv_CMD(command);
            break;
        case CMD_CAT:
            this->cat_CMD(command);
            break;
        case CMD_FORMAT:
            this->format_CMD(command);
            break;
        case CMD_DISKINFO:
            this->diskInfo_CMD(command);
            break;
        case CMD_DOWNLOAD:
            this->download_CMD(command);
            break;
        
            
        default:
            cout << "Command not found" << endl;
            break;
    }
}


void Terminal::cd_CMD(command_t *cmd)
{
    int res;
    
    if(cmd->arguments->size() != 2)
    {
        printf("Usage: cd <target>\n");
        return;
    }
    
    char *destinyDirectory = cmd->arguments->at(0);
    res = this->changeDistributedDirectory(destinyDirectory);
    if(res == -1)
    {
        printf("cd: No such directory\n");
    }
    else if(res == -3)
    {
        printf("cd: This directory is empty\n");
    }
    
    
}

int Terminal::changeDistributedDirectory(char *destinyDirectory)
{
    //TODO directorio existe? //TODO: No se puede cd .. en /
    Node* startingDirectory = this->tree->currentDirectory;
    string d;
    
    if(destinyDirectory == NULL) return 0;
   
    d.append(destinyDirectory);
    
    //Si es absoluto y empieza por / -> vas a raiz
    if(!strncmp("/", destinyDirectory, 1))
    {
        tree->currentDirectory = tree->root;
    }
    
    //Si el cambio es solo a /
    if(!strncmp("/", destinyDirectory, 1) && strlen(destinyDirectory)==1)
    {
        tree->currentDirectory = tree->root;
        return 0;
    }
    
    //Si el cambio es solo a ..
    if(!strncmp("..", destinyDirectory, 2) && strlen(destinyDirectory)==2)
    {
        if(tree->currentDirectory->father != NULL)
            tree->currentDirectory = tree->currentDirectory->father;
        return 0;
    }
    
    if(!strncmp(".", destinyDirectory, 1) && strlen(destinyDirectory)==1)
    {
        return 0;
    }
    
    //Si es un directorio concreto o una secuencia de directorios
    if(d.find("/") != -1 && d.size() > 1)
    {
        char *str = destinyDirectory;
        char * pch;
        
        pch = strtok (str,"/");
        while (pch != NULL)
        {
            int res = innerChangeDistributedDirectory(pch);
            if(res == -1)
            {
                this->tree->currentDirectory = startingDirectory;
                return -1;
            }
            else if(res == -3)
            {
                return -3;
            }
            pch = strtok (NULL, "/");
        }
    }
    else
        return innerChangeDistributedDirectory(destinyDirectory);
    
    return 0;
}

int Terminal::innerChangeDistributedDirectory(char* targetDirectory)
{
    char *destinyDirectory = targetDirectory;
    
    if(destinyDirectory == NULL) return -2;
    
    string d;
    d.append(destinyDirectory);
    
    if(!strncmp(".", destinyDirectory, 1) && strlen(destinyDirectory)==1)
    {
        return 0;
    }
    
    if(!strncmp("..", destinyDirectory, 2))
    {
        if(tree->currentDirectory->father != NULL)
            tree->currentDirectory = tree->currentDirectory->father;
        return 0;
    }
    
    //Caso directorio concreto
    //Si el directorio no esta vacio
    if(tree->currentDirectory->sons->getN() != 0)
    {
        string d;
        d.append(destinyDirectory);
        
        Node *finalDirectory = nullptr;
        
        //Buscamos hijo con mismo nombre y devolvemos el puntero al Nodo hijo
        finalDirectory = tree->currentDirectory->sons->buscar(d);
        if(finalDirectory != NULL)
            this->tree->currentDirectory = finalDirectory;
        else
        {
            return -1;
        }
        
    }
    else
    {
        return -1;
    }
    return 0;
}

void Terminal::ls_CMD(command_t *cmd)
{
    Node *currentDirectory = this->tree->currentDirectory;
    ListaEnlazada *childArray = this->tree->currentDirectory->sons;
    
    Node *child;
    
    if(childArray->getN() > 0)
    {
        child = childArray->getValor(0);
        //DIR tmp 4096 Mon 2011-10-17 19:04:27
        for( int i = 0; i < childArray->getN(); i++)
        {
            cout << ((childArray->getValor(i)->isDirectory) ? "DIR" : "FILE") ;
           // cout << " " << childArray->getValor(i)->label.substr(0,childArray->getValor(i)->label.length()-1);
            cout << " " << childArray->getValor(i)->label;
            cout << " " << childArray->getValor(i)->size;
            cout << " " << childArray->getValor(i)->getTimeFormatted() << endl;
            
        }
    }
    
}

void Terminal::mkdir_CMD(command_t *cmd)
{
    if(cmd->arguments->size() != 2)
    {
        printf("Usage: mkdir <Dir name>\n");
        return;
    }
    char *destinyDirectory = cmd->arguments->at(0);
    string d;
    d.append(destinyDirectory);
    
    if(!isValidName(d)) return;
    
    this->tree->currentDirectory->addChild(destinyDirectory, true);
}

void Terminal::pwd_CMD(command_t *cmd)
{
    Node* current = this->tree->currentDirectory;
    string path;
    path = current->getPath();
    cout << path << endl;
}

void Terminal::lls_CMD(command_t *cmd)
{
    int status;
    char *args[2];
    
    system("ls -ls");
}

void Terminal::rm_CMD(command_t *cmd)
{
    //Si es una ruta
    //Guardas directorio inicial
    //Cambias al directorio final
    
    Node *startingDirectory = this->tree->currentDirectory;
    Node *targetNode = NULL;
    string target;
    string targetPath, targetFile;
    int chOut,posFatherList;
    char* auxStr;
    
    
    //Comprobamos que hay dos argumentos
    if(cmd->arguments->size() != 2)
    {
        printf("Usage: rm <file>\n");
        return;
    }

    target.append(cmd->arguments->at(0));

    targetPath = getPath(target);
    targetFile = getFilename(target);
    
    cout << "targetPath " << targetPath << endl;
    cout << "targetFile " << targetFile << endl;
    
    //Comprobar que los directorios existen
    auxStr = strdup(targetPath.c_str());
    chOut = changeDistributedDirectory(auxStr);
    if(chOut == -1 || chOut == -3)
    {
        cout << "rm: No such directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    targetNode = tree->currentDirectory->sons->buscar(targetFile);
    if(targetNode == NULL)
    {
        cout << "rm: No such file in directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    if(targetNode->isDirectory)
    {
        cout << "rm: " << targetNode->label << " is a directory, use rmdir\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    //Borramos referencia del padre //TODO COMPROBAR QUE EL PADRE NO SE HAYA BORRADO YA SI BORRADO RECURSIVO
    posFatherList = this->tree->currentDirectory->sons->buscarPosicion(targetNode->label);
    this->tree->currentDirectory->sons->eliminar(posFatherList);
    
    this->tree->currentDirectory = startingDirectory;
    
    delete targetNode;
    
}


void Terminal::rmdir_CMD(command_t *cmd)
{
    int posicion;
    string d;
    d.append(cmd->arguments->at(0));
    Node *searchedNode = nullptr;
    
    if(cmd->arguments->size() != 2)
    {
        printf("rmdir: Usage: rmdir <target> \n");
        return;
    }
    
    
    
    //Buscamos el nodo a borrar
    searchedNode = tree->currentDirectory->sons->buscar(d);
    
    //Borramos nodo y sus hijos
    if(searchedNode == NULL)
        return;
    
    if(!searchedNode->isDirectory)
    {
        printf("rmdir: Trying to rmdir a file, use rm. \n");
        return;
    }
    
    searchedNode->deleteSubtree();
    
    //Borramos referencia del padre
    posicion = this->tree->currentDirectory->sons->buscarPosicion(d);
    this->tree->currentDirectory->sons->eliminar(posicion);
    
    
}
void Terminal::upload_CMD(command_t *cmd)
{

    string g,target, targetPath, targetFile;
    g.append(cmd->arguments->at(0));
    char *auxStr;
    //Save current dir to return later
    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory));
    
    if(cmd->arguments->size() != 2)
    {
        printf("Usage: upload <target>\n");
        return;
    }
    
    target.append(cmd->arguments->at(0));
    
    targetPath = getPath(target);
    targetFile = getFilename(target);
    
    cout << "targetPath " << targetPath << endl;
    cout << "targetFile " << targetFile << endl;
    
    //Comprobar que los directorios existen
    auxStr = strdup(targetPath.c_str());
    chdir(auxStr);
    
    if(!isValidName(targetFile))return;
    
    
    this->uploadRecursive(targetFile);
    this->tree->saveTree();
    
    chdir(currentDirectory);
    return;
}

int Terminal::getLocalFilesget(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

void Terminal::uploadRecursive(string name)
{
    //TODO Check if uploading valid names
    
    char cwd[1024];
    Node* newNode;
    struct stat fileStat;
    string dir = string(".");
    vector<string> files = vector<string>();
    //string name = nameA.substr(0,nameA.length()-1);
    
    char *cstr = new char[name.length() + 1];
    strcpy(cstr, name.c_str());
    
    if(!stat(name.c_str(),&fileStat))
    {
        if (S_ISDIR(fileStat.st_mode))
        {
            cout<<"Creating dir node " << name << endl;
            newNode = this->tree->currentDirectory->addChild(name, true);
        }
        else
        {
            cout<<"Creating file node " << name << endl;
            newNode = this->tree->currentDirectory->addChild(name, false);
            newNode->size = fileStat.st_size;
            
            //Guardas en disco
            this->tree->diskManager->writeFile(this->tree->diskManager->getFileBytes(cstr),newNode);
        }
        
        //newNode->lastModified = fileStat.st_ctimespec.tv_nsec;
        
        
        //lcd /Users/rafaelmanzano/Desktop
        
        if (S_ISDIR(fileStat.st_mode)) //TODO probar directorio vacio
        {
            //cout<<"As " << name << "is dir, going for subdirectories---- " << endl;
            this->getLocalFilesget(name,files);
            
            for (unsigned int i = 0;i < files.size();i++)
            {
                getcwd(cwd, sizeof(cwd));
                chdir(name.c_str());
                Node * inidir = this->tree->currentDirectory;
                this->tree->currentDirectory = newNode;
                if(files[i].substr(0,1).compare(".") != 0)
                {
                    //cout<<"Uploading " << files[i] <<endl;
                    uploadRecursive(files[i]);
                    //cout<<"Uploading " << files[i] <<endl;
                }
                this->tree->currentDirectory = inidir;
                chdir(cwd);
                
            }
            
        }
        
        
    }
    else
    {
        cout << "Error" << endl;
    }
}
void Terminal::lpwd_CMD(command_t *cmd)
{
    //TODO
    system("pwd");
}
void Terminal::lcd_CMD(command_t *cmd)
{
    char cwd[1024];
    
    string g;
    g.append(cmd->arguments->at(0));
    chdir(g.c_str());
    g = g.substr(0,g.size()-1);
    
    chdir(g.c_str());
   
    getcwd(cwd, sizeof(cwd));
}
void Terminal::cp_CMD(command_t *cmd)
{
    Node *startingDirectory = this->tree->currentDirectory;
    Node *sourceNode = NULL;
    Node *targetNode = NULL;
    string source, target;
    string sourcePath, sourceFile, targetPath, targetFile;
    int chOut;
    char* auxStr;
    char* sourceBytes;
    
    cout << cmd->arguments->size() << endl;;
    
    //Comprobamos que hay dos argumentos
    if(cmd->arguments->size() != 3)
    {
        printf("Usage: cp <source> <target>\n");
        return;
    }
    source.append(cmd->arguments->at(0));
    target.append(cmd->arguments->at(1));
    
    sourcePath = getPath(source);
    targetPath = getPath(target);
    sourceFile = getFilename(source);
    targetFile = getFilename(target);
    
    
    if(!targetPath.compare("."))
    {
        targetPath.clear();
        targetPath.append(this->tree->currentDirectory->getPath());
    }
    
    if(!sourcePath.compare("."))
    {
        sourcePath.clear();
        sourcePath.append(this->tree->currentDirectory->getPath());
    }
    
    
    cout << "sourcePath " << sourcePath << endl;
    cout << "targetPath " << targetPath << endl;
    cout << "sourceFile " << sourceFile << endl;
    cout << "targetFile " << targetFile << endl;
    
    //Comprobar que los directorios existen
    auxStr = strdup(sourcePath.c_str());
    chOut = changeDistributedDirectory(auxStr);
    if(chOut == -1 || chOut == -3)
    {
        cout << "cp: No such source directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    sourceNode = tree->currentDirectory->sons->buscar(sourceFile);
    if(sourceNode == NULL)
    {
        cout << "cp: No such file in source directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    auxStr = strdup(targetPath.c_str());
    chOut = changeDistributedDirectory(auxStr);
    if(chOut == -1 || chOut == -3)
    {
        cout << "cp: No such target directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }

    //Comprobar que nombre final es valido en el directorio final
    if(!isValidName(targetFile))
    {
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    //Creas nuevo nodo
    targetNode = this->tree->currentDirectory->addChild(targetFile, false);
    
    //Copiar datos del nodo origen al nodo final
    targetNode->size = sourceNode->size;
    sourceBytes = this->tree->diskManager->readFile(sourceNode);
    this->tree->diskManager->writeFile(sourceBytes, targetNode);
    
    this->tree->currentDirectory = startingDirectory;
    //TODO
}
void Terminal::mv_CMD(command_t *cmd)
{
    Node *startingDirectory = this->tree->currentDirectory;
    Node *sourceNode = NULL;
    Node *targetNode = NULL;
    string source, target;
    string sourcePath, sourceFile, targetPath, targetFile;
    int chOut;
    char* auxStr;
    char* sourceBytes = nullptr;
    
    //cout << cmd->arguments->size() << endl;;
    
    //Comprobamos que hay dos argumentos
    if(cmd->arguments->size() != 3)
    {
        printf("Usage: mv <sourceFile> <targetFile>\n");
        return;
    }
    source.append(cmd->arguments->at(0));
    target.append(cmd->arguments->at(1));

    sourcePath = getPath(source);
    targetPath = getPath(target);
    sourceFile = getFilename(source);
    targetFile = getFilename(target);
    
    
    if(!targetPath.compare("."))
    {
        targetPath.clear();
        targetPath.append(this->tree->currentDirectory->getPath());
    }
    
    if(!sourcePath.compare("."))
    {
        sourcePath.clear();
        sourcePath.append(this->tree->currentDirectory->getPath());
    }
        
    
    
    cout << "sourcePath " << sourcePath << endl;
    cout << "targetPath " << targetPath << endl;
    cout << "sourceFile " << sourceFile << endl;
    cout << "targetFile " << targetFile << endl;
    
    //Comprobar que los directorios existen
    auxStr = strdup(sourcePath.c_str());
    chOut = changeDistributedDirectory(auxStr);
    if(chOut == -1 || chOut == -3)
    {
        cout << "cp: No such source directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    //Guardas nodo del directorio origen
    sourceNode = tree->currentDirectory->sons->buscar(sourceFile);
    if(sourceNode == NULL)
    {
        cout << "cp: No such file in source directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    //Cambiamos a directorio final
    auxStr = strdup(targetPath.c_str());
    chOut = changeDistributedDirectory(auxStr);
    if(chOut == -1 || chOut == -3)
    {
        cout << "cp: No such target directory\n";
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    //Comprobar que nombre final es valido en el directorio final
    if(!isValidName(targetFile))
    {
        this->tree->currentDirectory = startingDirectory;
        return;
    }
    
    if(!sourceNode->isDirectory)
    {
        //FICHEROS
        //Creas nuevo nodo
        printf("Current dir is %s",this->tree->currentDirectory->getPath().c_str());
        targetNode = this->tree->currentDirectory->addChild(targetFile, false);
        //Copiar datos del nodo origen al nodo final
        targetNode->size = sourceNode->size;
        sourceBytes = this->tree->diskManager->readFile(sourceNode);
        printf("File recieved\n Writing new file...");
        this->tree->diskManager->writeFile(sourceBytes, targetNode);
        printf("Deleting source node...\n");
    }
    else
    {
        printf("mv: Usage: mv <sourceFile> <targetFile>\n");
        return;
    }
    
    
    auxStr = strdup(sourcePath.c_str());
    chOut = changeDistributedDirectory(auxStr);
    
    
    
    printf("Deleting father reference node...\n");
    int posFatherList = this->tree->currentDirectory->sons->buscarPosicion(sourceNode->label);
    this->tree->currentDirectory->sons->eliminar(posFatherList);
    
    this->tree->currentDirectory = startingDirectory;
    delete sourceNode;
    

    //Borramos referencia del padre //TODO COMPROBAR QUE EL PADRE NO SE HAYA BORRADO YA SI BORRADO RECURSIVO
    
    free(sourceBytes);
}

void Terminal::cat_CMD(command_t *cmd)
{
    string d;
    d.append(cmd->arguments->at(0));
    Node *searchedNode = nullptr;
    
    //Buscamos el nodo a leer
    searchedNode = tree->currentDirectory->sons->buscar(d);

    if(searchedNode != NULL)
        printf("%s",this->tree->diskManager->readFile(searchedNode));
    else
        cout << d << ": No such file or directory\n"; //TODO encaps
}

void Terminal::download_CMD(command_t *cmd)
{
    string d;
    d.append(cmd->arguments->at(0));
    Node *searchedNode = nullptr;
    char* fileBytes;
    char *name;
    
    if(cmd->arguments->size() != 2)
    {
        printf("download: Usage: download <target>\n");
        return;
    }
    
    //Buscamos el nodo a leer
    searchedNode = tree->currentDirectory->sons->buscar(d);

    if(searchedNode == NULL)
    {
        cout << d << ": No such file or directory\n"; //TODO encaps
        return;
    }
    name = (char*)searchedNode->label.c_str();
    printf("Searched node %s found\n",searchedNode->label.c_str());
    fileBytes = this->tree->diskManager->readFile(searchedNode);
    this->tree->diskManager->writeFiletoLocalDisk(fileBytes, name);
    printf("download: File written to local disk\n");
}

void Terminal::format_CMD(command_t *cmd)
{
    if(cmd->arguments->size() != 2)
    {
        printf("Format: Usage: format <size>\n");
    }
    

    int n = atoi(cmd->arguments->at(0));
    int size = n/4;
    printf("Formatting remote disks of size %d\n", size);
    //tree->diskManager.format(n);
    tree->diskManager->remoteFormat(size);
    
}

void Terminal::exit_CMD(command_t *cmd)
{
    printf("Saving tree\n");
    this->tree->saveTree();
    printf("Updating FreeSectorsFiles\n");
    this->tree->diskManager->updateFreeSectorsFiles();
    exit(1);
}

void Terminal::diskInfo_CMD(command_t *cmd)
{
    
    printf("Ocupied blocks on disk 0: %d\n", this->tree->diskManager->getFreeSectors(1));
    printf("Ocupied blocks on disk 1: %d\n", this->tree->diskManager->getFreeSectors(2));
    printf("Ocupied blocks on disk 2: %d\n", this->tree->diskManager->getFreeSectors(3));
    printf("Ocupied blocks on disk 3: %d\n", this->tree->diskManager->getFreeSectors(4));
    
}

bool Terminal::isValidName(string name)
{
    if(!name.compare("..") || !name.compare(".") || (this->tree->currentDirectory->sons->buscarPosicion(name) != -1))
    {
        cout<<"Not valid name"<<endl;
        return false;
    }
    else
        return true;
}

string Terminal::getPath(string string)
{
//    std::cout << "Splitting: " << string << '\n';
    std::size_t found = string.find_last_of("/\\");
//    std::cout << " path: " << string.substr(0,found) << '\n';
//    std::cout << " file: " << string.substr(found+1) << '\n';
    if(string.find("/") == -1)
    {
        return ".";
    }
    
    return string.substr(0,found);
    
    
}

string Terminal::getFilename(string string)
{
//    std::cout << "Splitting: " << string << '\n';
    std::size_t found = string.find_last_of("/\\");
//    std::cout << " path: " << string.substr(0,found) << '\n';
//    std::cout << " file: " << string.substr(found+1) << '\n';
    
    if(string.find("/") == -1)
    {
        return string;
    }
    return string.substr(found+1);
}
