#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#define textIP(a) (uint32_t)a[0] << "." << (uint32_t)a[1] << "." << (uint32_t)a[2] << "." << (uint32_t)a[3]

#if defined __WINDOWS__ || defined WIN32

#else
	#include "stdint.h"
#endif

//#define NETWORKMESSAGE_MAXSIZE 16768
#define NETWORKMESSAGE_MAXSIZE 15360


#if defined WIN32 || defined __WINDOWS__

#include "windows.h"

#define EWOULDBLOCK WSAEWOULDBLOCK

#ifndef __GNUC__
#pragma comment( lib, "Ws2_32.lib" )
typedef unsigned long uint32_t;
typedef signed long int32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
#endif

#define OTSYS_SLEEP(t) Sleep(t)
/*
#define OTSYS_SLEEP(uint32_t t){
	Sleep(t);
}
*/

#else

#include <time.h>
#include <sys/timeb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef SOCKET
#define SOCKET int
#endif

#ifndef closesocket
#define closesocket close
#endif

#ifndef SOCKADDR
#define SOCKADDR sockaddr
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

inline void OTSYS_SLEEP(int t)
{
	timespec tv;
	tv.tv_sec  = t / 1000;
	tv.tv_nsec = (t % 1000)*1000000;
	nanosleep(&tv, NULL);
}

#endif

#endif
