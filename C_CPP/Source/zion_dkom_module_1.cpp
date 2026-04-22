/*
 *      name: ZION system DKOM Driver User
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_dkom_module.h"


#include "zion_dkom_eprocess.h"


STD_EXPORT(ZIONRESULT) DKOM_HideDriverByName(WCHAR *DriverFileName)
{
	PZION_CS_PACKET ptrZION_MODULE_Packet;
	PZION_DKOM_MODULE_CMD_DATA ptrDKOMMODULECMDData;
	ptrZION_MODULE_Packet = (PZION_CS_PACKET)malloc(sizeof(ZION_CS_PACKET));
	if (NULL == ptrZION_MODULE_Packet)
	{
		printf("ZION: Memory alloc failed in DKOM_HideDriverByName\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	ptrDKOMMODULECMDData = (PZION_DKOM_MODULE_CMD_DATA)malloc(sizeof(ZION_DKOM_MODULE_CMD_DATA));
	if (NULL == ptrDKOMMODULECMDData)
	{
		printf("ZION: Memory alloc failed in DKOM_HideDriverByName\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	if (ZION_OK != ZION_SetDKOMMODULECMDData(ptrDKOMMODULECMDData,
		DriverFileName))
	{
		printf("ZION: Error in DKOM_HideDriverByName\n");
		return ZION_ERROR;
	}
	if (ZION_OK != ZION_SetPacketValue(ptrZION_MODULE_Packet,
		CMD_DKOM_MODULE,
		ptrDKOMMODULECMDData,
		sizeof(ZION_DKOM_MODULE_CMD_DATA)))
	{
		printf("ZION: Error in DKOM_HideDriverByName\n");
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
		free(ptrDKOMMODULECMDData);
		return ZION_ERROR;
	}
	printf("ZION: DKOM HIDE MODULE %s Succeed\n", DriverFileName);
	free(ptrZION_MODULE_Packet);
	free(ptrDKOMMODULECMDData);
	return ZION_OK;
	//ZION_SetPacketValue(ptrZION_SSDT_Packet, CMD_SSDT_HOOK, 
}
ZIONRESULT ZION_SetDKOMMODULECMDData(PZION_DKOM_MODULE_CMD_DATA data,
							   WCHAR *DriverFileName)
{
	wcscpy_s(data->DriverFileName,MAX_DRIVER_FILE_NAME ,DriverFileName); 
	return ZION_OK;
}