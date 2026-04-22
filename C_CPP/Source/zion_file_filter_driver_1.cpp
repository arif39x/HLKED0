/*
 *      name: ZION system File Filter User
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_file_filter_driver.h"


STD_EXPORT(ZIONRESULT) Filter_Hide_File(WCHAR *FileName, DWORD type)
{
	PZION_CS_PACKET ptrZION_MODULE_Packet;
	PZION_FILE_FILTER_CMD_DATA ptrFileFilterCMDData;
	ptrZION_MODULE_Packet = (PZION_CS_PACKET)malloc(sizeof(ZION_CS_PACKET));
	if (NULL == ptrZION_MODULE_Packet)
	{
		printf("ZION: Memory alloc failed in Filter_Hide_File\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	ptrFileFilterCMDData = (PZION_FILE_FILTER_CMD_DATA)malloc(sizeof(ZION_FILE_FILTER_CMD_DATA));
	if (NULL == ptrFileFilterCMDData)
	{
		printf("ZION: Memory alloc failed in Filter_Hide_File\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	if (ZION_OK != ZION_SetFileFilterCMDData(ptrFileFilterCMDData,
		FileName))
	{
		printf("ZION: Error in Filter_Hide_File\n");
		return ZION_ERROR;
	}
	if (ZION_OK != ZION_SetPacketValue(ptrZION_MODULE_Packet,
		type,
		ptrFileFilterCMDData,
		sizeof(ZION_FILE_FILTER_CMD_DATA)))
	{
		printf("ZION: Error in Filter_Hide_File\n");
		return ZION_ERROR;
	}

	DWORD dwReceived;
	//Maybe need to judge whether g_hZionServer_Dev is NULL
	if (!DeviceIoControl(g_hZionServer_Dev,
		IOCTL_ZION_CS_PKT_CMD,
		(LPVOID) ptrZION_MODULE_Packet,
		sizeof(ZION_CS_PACKET),
		NULL,
		0,
		&dwReceived,
		NULL))
	{
		printf("ZION: Device IO CTRL Error in DKOM_HideDriverByName\n");
		free(ptrZION_MODULE_Packet);
		free(ptrFileFilterCMDData);
		return ZION_ERROR;
	}
	
	free(ptrZION_MODULE_Packet);
	free(ptrFileFilterCMDData);
	return ZION_OK;
	//ZION_SetPacketValue(ptrZION_SSDT_Packet, CMD_SSDT_HOOK, 
}
ZIONRESULT ZION_SetFileFilterCMDData(PZION_FILE_FILTER_CMD_DATA data,
							   WCHAR *FileName)
{
	wcscpy_s(data->HideFileName,MAX_FILTER_FILE_NAME ,FileName); 
	return ZION_OK;
}