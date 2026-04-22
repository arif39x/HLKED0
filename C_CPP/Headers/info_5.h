#pragma once

typedef struct info_t
{
    char   Country     [4];
    char   ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
    char   UserName    [UNLEN + 1];
    ulong  Uptime;
    ulong  Ram;
    ulong  MajorVersion;
    ulong  MinorVersion;
    ulong  ServicePack;
    float  ProcSpeed;
    ushort Procs;
    BOOL   IsAdmin;
} info_t;

void info_all          ( info_t* Info );
void info_uptime       ( info_t* Info );
void info_country      ( info_t* Info );
void info_computer_name( info_t* Info );
void info_user_name    ( info_t* Info );
void info_os_version   ( info_t* Info );
void info_ram          ( info_t* Info );
void info_proc_count   ( info_t* Info );
void info_proc_speed   ( info_t* Info );
void info_is_admin     ( info_t* Info );