/*
 *      name: ZION system Command Packet Def.
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */

#pragma once



//Packet


//TODO: This value needs change
#define MAX_ZION_DATA_SIZE         4096


#define CMD_MAJOR_SERVER            0x0001u
//TODO: Will be changed
#define CMD_MAJOR_HOOK              0x0002u
#define CMD_MAJOR_MEMORY            0x0003u          
#define CMD_MAJOR_DKOM				0x0004u
#define CMD_MAJOR_TDI				0x0005u
#define CMD_MAJOR_FILE_FILTER		0x0006u
#include <pshpack1.h>
typedef struct _ZION_CS_PACKET
{
    DWORD   dwVersion; 
    DWORD   dwType;
    union {
        DWORD dwCmd;
        struct _cmd{
           WORD    wMajorCmd;
           WORD    wSubCmd;
        } cmd;
    } ;

    DWORD   dwDataSize;
    union 
    {
        struct _flag_bit{
            DWORD   BIT_DIRECTION   :1;
            DWORD   BIT_LAST        :1;
            DWORD   BIT_RESERVED    :30;
        } flag_bit;
        DWORD   dwflag;
    } ;
    BYTE  pData[MAX_ZION_DATA_SIZE];

} ZION_CS_PACKET, *PZION_CS_PACKET;

#include <poppack.h>



