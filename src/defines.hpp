//
//  defines.hpp
//  hitoFinale
//
//  Created by Rafael Manzano on 19/12/2018.
//  Copyright © 2018 Rafael Manzano. All rights reserved.
//


/**
 * @file defines.hpp
 * @author Rafael Manzano
 * @date 19 Dic 2018
 * @brief File containing almost all defines used by the program
 */





#ifndef defines_h
#define defines_h

#define DISK_BLOCKS 32000
#define BLOCK_SIZE 1000
#define N_DISKS 4
#define MAX_SONS_PER_NODE 100
#define MAX_LABEL_LENGHT 100
#define MAX_BLOCKS_PER_FILE 10000

#define WRITE_BLOCK_CMD 1
#define READ_BLOCK_CMD 2
#define EXIT_CMD 3
#define FORMAT_CMD 4
#define GET_FREE_SECTORS_CMD 5
#define IS_FREE_SECTOR_CMD 6
#define LD_FREE_SCTRS_FLS_CMD 7
#define UPDT_FREE_SCTRS_FLS_CMD 8
#define FREE_BLCK_CMD 9
#define GET_DISK_SZ_CMD 10

#define MAIN_CMD_TAG 1
#define FMT_SIZE_TAG 2
#define OCPD_SCTRS_AMT_TAG 3
#define IS_FREE_BLCK_TAG 4
#define IS_FREE_BOOL_TAG 5
#define WRT_BLCK_TAG 6
#define WRT_DATA_TAG 7
#define READ_BLCK_TAG 8
#define READ_DATA_TAG 9
#define FREE_BLCK_TAG 10
#define GET_DISK_SZ_TAG 11


#define MASTER_ID 0
#define SLAVE_1_ID 1
#define SLAVE_2_ID 2
#define SLAVE_3_ID 3
#define SLAVE_4_ID 4

#endif /* defines_h */
