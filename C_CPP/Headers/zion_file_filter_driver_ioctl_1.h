/*
 *      name: ZION system filter driver Def.
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once
#define MAX_FILTER_FILE_NAME 100
#define CMD_SUB_HIDE_FILE	0x0001u
#define CMD_SUB_HIDE_DIRECTORY	 0x0002u

#define CMD_FILE_FILTER ((CMD_MAJOR_FILE_FILTER << 16) | CMD_SUB_HIDE_FILE)
#define CMD_DIRECTORY_FILTER ((CMD_MAJOR_FILE_FILTER << 16) | CMD_SUB_HIDE_DIRECTORY)

typedef struct _ZION_FILE_FILTER_CMD_DATA
{
	WCHAR HideFileName[MAX_FILTER_FILE_NAME];
}ZION_FILE_FILTER_CMD_DATA, *PZION_FILE_FILTER_CMD_DATA;