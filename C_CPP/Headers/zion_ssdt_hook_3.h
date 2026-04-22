/*
 *      name: SSDT HOOK MODULE
 *      type: tool
 *
 *      Org Author: Liang Chen
 *      Version : 0.2
 */

typedef struct _ZION_Service_Descriptor_Entry {
    unsigned int *ServiceTableBase;
    unsigned int *ServiceCounterTableBase; 
    unsigned int NumberOfServices;
    unsigned char *ParamTableBase;
} ZION_Service_Descriptor_Entry, *PZION_Service_Descriptor_Entry;

extern PZION_Service_Descriptor_Entry KeServiceDescriptorTable;
