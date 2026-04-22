/*
 *      name: ZION system Packet helper func
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_packet_helper.h"


ZIONRESULT ZION_SetPacketValue(PZION_CS_PACKET Packet,
							   DWORD cmd,
							   PVOID ptrData,
							   size_t s_datasize)
{
	if ((NULL == Packet) || (s_datasize > MAX_ZION_DATA_SIZE))
	{
		printf("ZION: Argu error in ZION_SetPacketValue\n");
		return ZION_ERROR;
	}
	Packet->dwVersion = ZION_SERVER_VERSION;
	Packet->dwCmd = cmd;
	
	memcpy(Packet->pData,ptrData, s_datasize);
	return ZION_OK;

}