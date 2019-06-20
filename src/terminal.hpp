//
//  terminal.hpp
//  Hito1_SistemasDistribuidos
//
//  Created by Rafael Manzano on 8/10/18.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//



/**
 * @file terminal.hpp
 * @author Rafael Manzano
 * @date 19 Dic 2018
 * @brief File containing the terminal used by the program
 */




#ifndef terminal_hpp
#define terminal_hpp

#include <iostream>
#include <stdio.h>
#include "tree.hpp"
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define CMD_CD 1
#define CMD_MKDIR 2
#define CMD_LS 3
#define CMD_EXIT 4
#define CMD_PWD 5
#define CMD_LLS 6
#define CMD_RM 7
#define CMD_RMDIR 8
#define CMD_UPLOAD 9
#define CMD_LPWD 10
#define CMD_LCD 11
#define CMD_CP 12
#define CMD_MV 13
#define CMD_CAT 14
#define CMD_FORMAT 15
#define CMD_DISKINFO 16
#define CMD_DOWNLOAD 17


/**
 * @brief Struct that contain a command to use in the Terminal
 */
typedef struct command_t
{
    int type; /**< Some documentation for first. */
    std::vector<char*> *arguments; /**< Some documentation for first. */
} command_t;


/**
 * @brief Class that implement a console to use our file system and run commands
 */
class Terminal
{
    
    public:
        Tree* tree; /**Tree pointer with the file system tree */
        Terminal(); 

		/**
		* @brief Method that run the terminal
		*/
        void run();

		/**
		* @brief Method that read a command
		* @param Command to read
		*/
        void readCommand(command_t* comando);

		/**
		* @brief Method that run a command
		* @param Command to read
		*/
        void runCommand(command_t* comando);

		/**
		* @brief Method that get the type of a command
		* @param Command to read in char pointer
		* @return The type of the command
		*/
        int getCommandType(char* comando);
        void cd_CMD(command_t *cmd);
        void ls_CMD(command_t *cmd);
        void mkdir_CMD(command_t *cmd);
        void pwd_CMD(command_t *cmd);
        void lls_CMD(command_t *cmd);
    
        void rm_CMD(command_t *cmd);
        void rmdir_CMD(command_t *cmd);
        void upload_CMD(command_t *cmd);
        void lpwd_CMD(command_t *cmd);
        void lcd_CMD(command_t *cmd);
        void cp_CMD(command_t *cmd);
        void mv_CMD(command_t *cmd);
        void cat_CMD(command_t *cmd);
        void format_CMD(command_t *cmd);
        void exit_CMD(command_t *cmd);
        void diskInfo_CMD(command_t *cmd);
        void download_CMD(command_t *cmd);
    

		/**
		* @brief Method that change a directory from a distributed system
		* @param Directory's name
		*/
        int changeDistributedDirectory(char* targetDirectory);


        int innerChangeDistributedDirectory(char* targetDirectory);
    
		/**
		* @brief Method that say if a name is valid
		* @param string name
		*/
        bool isValidName(string name);

		/**
		* @brief Method that say if a name exists
		* @param string names
		*/
        bool exists(string name);
    
		/**
		* @brief Method that upload the entire file system
		* @param string nameA
		*/
        void uploadRecursive(string nameA);


        int getLocalFilesget(string dir, vector<string> &files);
    
		/**
		* @brief Method that return the path
		* @param string 
		*/
        string getPath(string string);

		/**
		* @brief Method that return the name of the File
		* @param string
		*/
        string getFilename(string string);
    
    
    
        
    
};

#endif /* terminal_hpp */
