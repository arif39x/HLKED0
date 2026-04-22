
#define SHTDN_REASON_MAJOR_SYSTEM	0x00050000 // ripped from reason.h
#define SHTDN_REASON_MINOR_HUNG		0x00000005

#define SHUTDOWN_SYSTEM_HUNG		(SHTDN_REASON_MAJOR_SYSTEM|SHTDN_REASON_MINOR_HUNG) // for XP,2k only ignored on 9x
#define EWX_SYSTEM_REBOOT			(EWX_REBOOT | EWX_FORCE)
#define EWX_FORCE_SHUTDOWN			(EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCE)
#define EWX_FORCE_LOGOFF			(EWX_LOGOFF|EWX_FORCE)

DWORD XSplitBuffer(char *buffer, void *LinesArray);

DWORD XGetFileSize(char *Path);

char *XConvertSize(double Size);


bool XCheckNTBased();

#ifdef DEBUG_SYSTEM
void dbg(char *Message, ...);
#endif

char *XGetUptime(DWORD startup);

void *XHeapAlloc(DWORD size);

bool XHeapFree(void *memory);


char *XGetDriveType(char *Drive);


#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
    uint32 total[2];
    uint32 state[4];
    uint8 buffer[64];
}
md5_context;

void md5_starts(md5_context *ctx);
void md5_update(md5_context *ctx, uint8 *input, uint32 length);
void md5_finish(md5_context *ctx, uint8 digest[16]);

bool XSelfCheck();

unsigned __int64 GetCycleCount(void);
int cpuspeed(void);

char *XMD5Str(char *string, char *buffer, bool Standart);

char *XCalculateMD5(char *string, char *buffer);

char *xcommaI64(unsigned __int64 number);