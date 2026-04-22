#pragma once

typedef struct _DEVICE_EXTENSION 
{
	union
	{
		PDEVICE_OBJECT AttachedToDevice;		//绑定的设备
		PDEVICE_OBJECT FileSystem;
	};
	
	PDEVICE_OBJECT PhysicDevice;				//实际的设备
	UNICODE_STRING DeviceName;					//设备名称
	WCHAR		   DeviceNameBuffer[512];	
} DEVICE_EXTENSION, *PDEVICE_EXTENSION, *PHOOK_EXTENSION;

extern PDEVICE_OBJECT  deviceObject;