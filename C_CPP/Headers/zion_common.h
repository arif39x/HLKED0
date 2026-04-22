/*
 *      name: ZION system common header
 *      type: Kernel Module
 *
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */


#pragma once



#define ZION_SERVER_MODE
//if Defined, meaning the code runs in Ring 0

#define ZION_HIDE_SERVER




#define trace(str,...) DbgPrint("[%d:%s()]"str, __LINE__,__FUNCTION__, __VA_ARGS__)

#ifdef ZION_DEBUG_VERSION
#define safe_trace(str,...) trace(str,__VA_ARGS__)
#else
#define safe_trace (void)
#endif



