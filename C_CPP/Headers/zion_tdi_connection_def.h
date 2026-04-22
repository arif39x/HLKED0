/*
 *      name: ZION system TDI Hook Def.
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once
#define HTONS(a)  (((0xFF&a)<<8) + ((0xFF00&a)>>8))
#define INETADDR(a, b, c, d) (a + (b<<8) + (c<<16) + (d<<24))

#define ZION_TCP_INFO			1
#define ZION_UDP_INFO			0
#define ZION_ALL_ADDR			0xFFFFFFFF
#define ZION_ALL_PORT			0xFFFFu
#define CMD_SUB_TDI_CONNECTION_HIDE	0x0001u

#define CMD_TDI_CONNECTION ((CMD_MAJOR_TDI << 16) | CMD_SUB_TDI_CONNECTION_HIDE)

#pragma pack(1)
/*
typedef struct _ZION_CONNECTION_INFO {
DWORD Type;
DWORD SrcAddr; 
USHORT SrcPort; 
DWORD DstAddr; 
USHORT DstPort; 
} ZION_CONNECTION_INFO, *PZION_CONNECTION_INFO;
*/
typedef struct _ZION_TDI_CONNECTION_CMD_DATA
{
	DWORD Type;
	DWORD SrcAddr; 
	USHORT SrcPort; 
	DWORD DstAddr; 
	USHORT DstPort;

} ZION_TDI_CONNECTION_CMD_DATA, *PZION_TDI_CONNECTION_CMD_DATA;
#pragma pack()