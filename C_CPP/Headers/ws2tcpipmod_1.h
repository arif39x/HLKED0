struct in6_addr {
    union {
        u_char Byte[16];
        u_short Word[8];
    } u;
};

struct sockaddr_in6_old {
    short   sin6_family;        /* AF_INET6 */
    u_short sin6_port;          /* Transport level port number */
    u_long  sin6_flowinfo;      /* IPv6 flow information */
    struct in6_addr sin6_addr;  /* IPv6 address */
};

typedef union sockaddr_gen{
        struct sockaddr Address;
        struct sockaddr_in  AddressIn;
        struct sockaddr_in6_old AddressIn6;
} sockaddr_gen;

/* Structure to keep interface specific information */

typedef struct _INTERFACE_INFO
{
    u_long      iiFlags;        /* Interface flags */
    sockaddr_gen    iiAddress;      /* Interface address */
    sockaddr_gen    iiBroadcastAddress;     /* Broadcast address */
    sockaddr_gen    iiNetmask;      /* Network mask */
} INTERFACE_INFO, FAR * LPINTERFACE_INFO;

#define SIO_GET_INTERFACE_LIST  _IOR('t', 127, u_long)