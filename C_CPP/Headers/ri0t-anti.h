DWORD __forceinline IsInsideVPC_exceptionFilter(LPEXCEPTION_POINTERS ep)
{
  PCONTEXT ctx = ep->ContextRecord;
  ctx->Ebx = -1;                        // Not running VPC
  ctx->Eip += 4;                        // skip past the "call VPC" opcodes
  return EXCEPTION_CONTINUE_EXECUTION;
}
bool IsVPC()
{
    bool bVPCIsPresent = FALSE;

    __try
    {
        _asm push ebx
        _asm mov  ebx, 0 // It will stay ZERO if VPC is running
        _asm mov  eax, 1 // VPC function number
        _asm __emit 0Fh
        _asm __emit 3Fh
        _asm __emit 07h
        _asm __emit 0Bh
        _asm test ebx, ebx
        _asm setz [bVPCIsPresent]
        _asm pop ebx
    }

    __except(IsInsideVPC_exceptionFilter(GetExceptionInformation()))
    {
    }
    return bVPCIsPresent;
}
bool IsVMware2()
{
    bool bVMWareIsPresent = TRUE;
    __try
    {
        __asm
        {
            push   edx
            push   ecx
            push   ebx

            mov    eax, 'VMXh'
            mov    ebx, 0        // any value but not the MAGIC VALUE
            mov    ecx, 10        // get VMWare version
            mov    edx, 'VX'    // port number

            in     eax, dx        // read port
                                 // on return EAX returns the VERSION
            cmp    ebx, 'VMXh'    // is it a reply from VMWare?
            setz   [bVMWareIsPresent]    // set return value

            pop    ebx
            pop    ecx
            pop    edx
        }
    }

    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        bVMWareIsPresent = FALSE;
    }

    return bVMWareIsPresent;
}

bool IsAnubis()
{
    char File[MAX_PATH];
    bool Present = FALSE;

    if (strstr(File, "C:\\InsideTm\\"))
        Present = TRUE;
    return Present;
}

bool IsNorman()
{
    CHAR szUserName[MAX_PATH];
    DWORD dwUserNameSize = sizeof(szUserName);
    GetUserName(szUserName, &dwUserNameSize);
    if(!strcmp(szUserName, "CurrentUser"))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool IsSunbelt()
{
    CHAR szFileName[MAX_PATH];
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    if(!strcmp(szFileName, "C:\\file.exe"))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool IsVirtualPC()
{
    __try
    {
        __asm
        {
            mov eax, 1
            _emit 0x0F
            _emit 0x3F
            _emit 0x07
            _emit 0x0B
            _emit 0xC7
            _emit 0x45
            _emit 0xFC
            _emit 0xFF
            _emit 0xFF
            _emit 0xFF
            _emit 0xFF
        }
    }
    __except(1)
    {
        return FALSE;
    }
    return TRUE;
}

bool IsVMware()
{
    DWORD _EBX;
    __try
    {
        __asm
        {
            push ebx
            mov eax, 0x564D5868
            mov ebx, 0x8685D465
            mov ecx, 0x0A
            mov dx, 0x5658
            in eax, dx
            mov _EBX, ebx
            pop ebx
        }
    }
    __except(1)
    {
        return FALSE;
    }
    return _EBX == 0x564D5868;
}

bool IsInVM()
{
    if (IsAnubis() || IsVMware() || IsVMware2() || IsVPC() || IsNorman() || IsSunbelt()
		|| IsVirtualPC())
        return TRUE;

    return FALSE;
}