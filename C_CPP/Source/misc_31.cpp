#include "includes.h"
#include "externs.h"

DWORD XSplitBuffer(char *buffer, void *LinesArray)
{
//	dbg("XSplitBuffer()");

	int i = 0, j = strlen(buffer);

	DWORD index = -1;

	char *lines[MAXLINES] = {0};
	memset(lines, 0, sizeof(lines));

	if (j > 0)
	{
		index = 0;
		lines[index++] = buffer;
		for (i = 0; i < j; i++) 
			if (buffer[i] == '\r' || buffer[i] == '\n') 
				buffer[i] = 0;//0x03

		//Now that all cr/lf have been converted to NULL, save the pointers...
		for (i = 0; i < j; i++)
		{
			if (!buffer[i] && buffer[i+1])
			{
				if (index < MAXLINES) 
					lines[index++] = &buffer[i+1]; 
				else 
					break;
			}
		}

//		for (DWORD x = 0; x < index; x++)
//			if (lines[x])
//				dbg("(sub func) :: XSplitBuffer() line[%d]: %s", x, lines[x]);

		if (LinesArray)
			memcpy(LinesArray, lines, sizeof(lines));

	}

	return index;
}


DWORD XGetFileSize(char *Path)
{
	HANDLE XFile = CreateFile(Path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (XFile != INVALID_HANDLE_VALUE)
	{
		DWORD XSize = GetFileSize(XFile, NULL);

		CloseHandle(XFile);

		return XSize;
	}
	else
		return -1; //maybe return -1
}

char *XConvertSize(double Size)
{
    double SizePointer;
    static char SizeExt[512] = {0};
 
    _snprintf(SizeExt, sizeof(SizeExt), "(%.2f Bytes)", Size);
    SizePointer = Size / 1024.0; 
    if (SizePointer < 1024.0 && Size > 1024.0)
	{ 
        Size = SizePointer;
        _snprintf(SizeExt, sizeof(SizeExt), "(%.2f KB)", SizePointer);
        return SizeExt;
    }
    else if (SizePointer >= 1024.0 && Size > 1024.0)
	{
        SizePointer = SizePointer / 1024.0;
        if (SizePointer < 1024.0)
		{ 
            _snprintf(SizeExt, sizeof(SizeExt), "(%.2f MB)", SizePointer);
            return SizeExt; 
        }
        else
		{
            SizePointer = SizePointer / 1024.0;
            _snprintf(SizeExt, sizeof(SizeExt), "(%.2f GB)", SizePointer);
            return SizeExt; 
        }
    }
    else return SizeExt;
}


bool XCheckNTBased()
{
	return (GetVersion() >= 0x80000000)?(FALSE):(TRUE);
}

#ifdef DEBUG_SYSTEM
void dbg(char *Message, ...)
{
	if (!Message)
		return;

	char dbgbuf[DATA_BUFFER*8] = {0}, date[70] = {0}, time[70] = {0};

	va_list va;
	va_start(va, Message);
//	vsprintf(dbgbuf, Message, va);
	_vsnprintf(dbgbuf, sizeof(dbgbuf), Message, va);
	va_end(va);
	strncat(dbgbuf, "\r\n", sizeof(dbgbuf));

	GetDateFormat(0x409, 0, 0, "dd/MM/yy", date, sizeof(date));
	GetTimeFormat(0x409, 0, 0, "HH:mm:ss", time, sizeof(time));

	#ifdef CRYPT_DEBUG
	Crypto(dbgbuf, 8);
	#endif

//	CreateMutex(NULL, FALSE, dbgbuf); // for working in SandBox mode
	OutputDebugString(dbgbuf);

	#ifdef DEBUG_CONSOLE
	printf("%s", dbgbuf);
	#endif

	return;
}
#endif

void XCrypto(char *String, int Key)
{
	int i, length;
	if (!String)
		return;

	length = strlen(String);
	for (i = 0; i < length; i++)
	{
			*String ^= Key;
			String++;
	}
		return;
}


char *XGetUptime(DWORD Time)
{
	static char buffer[64] = {0};

	DWORD Total = GetTickCount() / 1000 - Time;
	DWORD Days = Total / 86400;
	DWORD Hours = (Total % 86400) / 3600;
	DWORD Minutes = ((Total % 86400) % 3600) / 60;
	DWORD Seconds = ((Total % 86400) % 3600) % 60;
	
	_snprintf(buffer, sizeof(buffer), "%dday(s) %dhour(s) %dminut(s) %dsecond(s)", Days, Hours, Minutes, Seconds);

	return buffer;
}

void XKeyEvent(BYTE key, bool caps)
{
	if (caps)
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0), FALSE?KEYEVENTF_KEYUP:0, 0);
        keybd_event(key, MapVirtualKey(key, 0), FALSE?KEYEVENTF_KEYUP:0, 0);
		keybd_event(key, MapVirtualKey(key, 0), TRUE?KEYEVENTF_KEYUP:0, 0);
	if (caps)
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0), TRUE?KEYEVENTF_KEYUP:0, 0);
}


void *XHeapAlloc(DWORD size)
{
	void *memory = 0;

	if (size > 0)
		memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);

	if (!memory)
	{
		#ifdef DEBUG_SYSTEM
		dbg("Memory Allocation Error!");
		#endif
		return NULL;
	}

	return memory;
}

bool XHeapFree(void *memory)
{
	if (memory)
	{
		HeapFree(GetProcessHeap(), 0, memory);
		memory = 0;
	}
	else
		return false;

	return true;;
}

char *xcommaI64(unsigned __int64 number)
{
	const int size = 50;
	int counter = 0;

	static char str[size];
	memset(str, 0, sizeof(str));

	char tempStr[size];
	char *ptr1=tempStr, *ptr2;
  
	do
	{
		*ptr1++ = (char)((number % 10) + '0');
		number /= 10;
		if (number &&  !(++counter % 3))
			*ptr1++ = ',';
	} while (number);
  
	for( --ptr1, ptr2 = str; ptr1 >= tempStr; --ptr1)
		*ptr2++ = *ptr1;
	*ptr2 = 0;
  
	return str;
}

char *XGetDriveType(char *Drive)
{
	switch(GetDriveType(Drive))
	{
		case DRIVE_UNKNOWN:
			return ("Unknown");
		case DRIVE_NO_ROOT_DIR:
			return ("Invalid");
		case DRIVE_FIXED:
			return ("Disk");
		case DRIVE_REMOTE:
			return ("Network");
		case DRIVE_CDROM:
			return ("CD-Rom");
		case DRIVE_RAMDISK:
			return ("RAM");
		case DRIVE_REMOVABLE:
			return ("Removable");
		default:
			return ("?");
	}
}

#define GET_UINT32(n,b,i)                       \
{                                               \
    (n) = ( (uint32) (b)[(i)    ]       )       \
        | ( (uint32) (b)[(i) + 1] <<  8 )       \
        | ( (uint32) (b)[(i) + 2] << 16 )       \
        | ( (uint32) (b)[(i) + 3] << 24 );      \
}

#define PUT_UINT32(n,b,i)                       \
{                                               \
    (b)[(i)    ] = (uint8) ( (n)       );       \
    (b)[(i) + 1] = (uint8) ( (n) >>  8 );       \
    (b)[(i) + 2] = (uint8) ( (n) >> 16 );       \
    (b)[(i) + 3] = (uint8) ( (n) >> 24 );       \
}

void md5_starts(md5_context *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
}

void md5_process(md5_context *ctx, uint8 data[64])
{
    uint32 X[16], A, B, C, D;

    GET_UINT32( X[0],  data,  0 );
    GET_UINT32( X[1],  data,  4 );
    GET_UINT32( X[2],  data,  8 );
    GET_UINT32( X[3],  data, 12 );
    GET_UINT32( X[4],  data, 16 );
    GET_UINT32( X[5],  data, 20 );
    GET_UINT32( X[6],  data, 24 );
    GET_UINT32( X[7],  data, 28 );
    GET_UINT32( X[8],  data, 32 );
    GET_UINT32( X[9],  data, 36 );
    GET_UINT32( X[10], data, 40 );
    GET_UINT32( X[11], data, 44 );
    GET_UINT32( X[12], data, 48 );
    GET_UINT32( X[13], data, 52 );
    GET_UINT32( X[14], data, 56 );
    GET_UINT32( X[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a,b,c,d,k,s,t)                                \
{                                                       \
    a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];

#define F(x,y,z) (z ^ (x & (y ^ z)))

    P( A, B, C, D,  0,  7, 0xD76AA478 );
    P( D, A, B, C,  1, 12, 0xE8C7B756 );
    P( C, D, A, B,  2, 17, 0x242070DB );
    P( B, C, D, A,  3, 22, 0xC1BDCEEE );
    P( A, B, C, D,  4,  7, 0xF57C0FAF );
    P( D, A, B, C,  5, 12, 0x4787C62A );
    P( C, D, A, B,  6, 17, 0xA8304613 );
    P( B, C, D, A,  7, 22, 0xFD469501 );
    P( A, B, C, D,  8,  7, 0x698098D8 );
    P( D, A, B, C,  9, 12, 0x8B44F7AF );
    P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
    P( B, C, D, A, 11, 22, 0x895CD7BE );
    P( A, B, C, D, 12,  7, 0x6B901122 );
    P( D, A, B, C, 13, 12, 0xFD987193 );
    P( C, D, A, B, 14, 17, 0xA679438E );
    P( B, C, D, A, 15, 22, 0x49B40821 );

#undef F

#define F(x,y,z) (y ^ (z & (x ^ y)))

    P( A, B, C, D,  1,  5, 0xF61E2562 );
    P( D, A, B, C,  6,  9, 0xC040B340 );
    P( C, D, A, B, 11, 14, 0x265E5A51 );
    P( B, C, D, A,  0, 20, 0xE9B6C7AA );
    P( A, B, C, D,  5,  5, 0xD62F105D );
    P( D, A, B, C, 10,  9, 0x02441453 );
    P( C, D, A, B, 15, 14, 0xD8A1E681 );
    P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
    P( A, B, C, D,  9,  5, 0x21E1CDE6 );
    P( D, A, B, C, 14,  9, 0xC33707D6 );
    P( C, D, A, B,  3, 14, 0xF4D50D87 );
    P( B, C, D, A,  8, 20, 0x455A14ED );
    P( A, B, C, D, 13,  5, 0xA9E3E905 );
    P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
    P( C, D, A, B,  7, 14, 0x676F02D9 );
    P( B, C, D, A, 12, 20, 0x8D2A4C8A );

#undef F
    
#define F(x,y,z) (x ^ y ^ z)

    P( A, B, C, D,  5,  4, 0xFFFA3942 );
    P( D, A, B, C,  8, 11, 0x8771F681 );
    P( C, D, A, B, 11, 16, 0x6D9D6122 );
    P( B, C, D, A, 14, 23, 0xFDE5380C );
    P( A, B, C, D,  1,  4, 0xA4BEEA44 );
    P( D, A, B, C,  4, 11, 0x4BDECFA9 );
    P( C, D, A, B,  7, 16, 0xF6BB4B60 );
    P( B, C, D, A, 10, 23, 0xBEBFBC70 );
    P( A, B, C, D, 13,  4, 0x289B7EC6 );
    P( D, A, B, C,  0, 11, 0xEAA127FA );
    P( C, D, A, B,  3, 16, 0xD4EF3085 );
    P( B, C, D, A,  6, 23, 0x04881D05 );
    P( A, B, C, D,  9,  4, 0xD9D4D039 );
    P( D, A, B, C, 12, 11, 0xE6DB99E5 );
    P( C, D, A, B, 15, 16, 0x1FA27CF8 );
    P( B, C, D, A,  2, 23, 0xC4AC5665 );

#undef F

#define F(x,y,z) (y ^ (x | ~z))

    P( A, B, C, D,  0,  6, 0xF4292244 );
    P( D, A, B, C,  7, 10, 0x432AFF97 );
    P( C, D, A, B, 14, 15, 0xAB9423A7 );
    P( B, C, D, A,  5, 21, 0xFC93A039 );
    P( A, B, C, D, 12,  6, 0x655B59C3 );
    P( D, A, B, C,  3, 10, 0x8F0CCC92 );
    P( C, D, A, B, 10, 15, 0xFFEFF47D );
    P( B, C, D, A,  1, 21, 0x85845DD1 );
    P( A, B, C, D,  8,  6, 0x6FA87E4F );
    P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
    P( C, D, A, B,  6, 15, 0xA3014314 );
    P( B, C, D, A, 13, 21, 0x4E0811A1 );
    P( A, B, C, D,  4,  6, 0xF7537E82 );
    P( D, A, B, C, 11, 10, 0xBD3AF235 );
    P( C, D, A, B,  2, 15, 0x2AD7D2BB );
    P( B, C, D, A,  9, 21, 0xEB86D391 );

#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
}

void md5_update(md5_context *ctx, uint8 *input, uint32 length)
{
    uint32 left, fill;

    if(!length)
		return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += length;
    ctx->total[0] &= 0xFFFFFFFF;

    if(ctx->total[0] < length)
        ctx->total[1]++;

    if(left && length >= fill)
    {
        memcpy((void *)(ctx->buffer + left), (void *)input, fill);
        md5_process(ctx, ctx->buffer);
        length -= fill;
        input  += fill;
        left = 0;
    }

    while(length >= 64)
    {
        md5_process(ctx, input);
        length -= 64;
        input  += 64;
    }

    if(length)
        memcpy((void *)(ctx->buffer + left), (void *)input, length);
}

static uint8 md5_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void md5_finish(md5_context *ctx, uint8 digest[16])
{
    uint32 last, padn;
    uint32 high, low;
    uint8 msglen[8];

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_UINT32( low,  msglen, 0 );
    PUT_UINT32( high, msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    md5_update( ctx, md5_padding, padn );
    md5_update( ctx, msglen, 8 );

    PUT_UINT32( ctx->state[0], digest,  0 );
    PUT_UINT32( ctx->state[1], digest,  4 );
    PUT_UINT32( ctx->state[2], digest,  8 );
    PUT_UINT32( ctx->state[3], digest, 12 );
}


#pragma warning(disable: 4035)
unsigned __int64 GetCycleCount(void)
{
	_asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}
#pragma warning(default: 4035)

int cpuspeed(void)
{
	unsigned __int64 startcycle;
	unsigned __int64 speed, num, num2;

	do {
		startcycle = GetCycleCount();
		Sleep(1000);
		speed = ((GetCycleCount()-startcycle)/100000)/10;
	} while (speed > 1000000); // if speed is 1000GHz+, then something probably went wrong so we try again =P

	// guess 'real' cpu speed by rounding raw cpu speed (something like 601mhz looks kinda tacky)
	num = speed % 100;
	num2 = 100;
	if (num < 80) num2 = 75;
	if (num < 71) num2 = 66;
	if (num < 55) num2 = 50;
	if (num < 38) num2 = 33;
	if (num < 30) num2 = 25;
	if (num < 10) num2 = 0;
	speed = (speed-num)+num2;

	return (int)speed;
}



char *XMD5Str(char *string, char *buffer, bool Standart)
{
	if (!string || !buffer)
		return ("");;

    md5_context ctx;
    unsigned char md5sum[16] = {0};

	md5_starts(&ctx);

    md5_update(&ctx, (unsigned char *)string, strlen(string));

	md5_finish(&ctx, md5sum);

    for(int j = 0; j < 16; j++)
		sprintf(buffer + j * 2, "%02x", (Standart)?(md5sum[j]):(md5sum[j]));

	#ifdef DEBUG_SYSTEM
	dbg("XMD5Str(): %s", buffer);
	#endif

	return buffer;
}

char *XCalculateMD5(char *string, char *buffer)
{
	if (!buffer)
		return NULL;

    md5_context ctx;
    unsigned char md5sum[16] = {0};

	md5_starts(&ctx);

    md5_update(&ctx, (unsigned char *)string, strlen(string));

	md5_finish(&ctx, md5sum);

    for(int j = 0; j < 16; j++)
		sprintf(buffer + j * 2, "%02x", md5sum[j]); 

	return buffer;
}
