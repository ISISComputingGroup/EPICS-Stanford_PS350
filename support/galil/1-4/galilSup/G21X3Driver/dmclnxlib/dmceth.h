#ifndef INCLUDE_ETHERNET
#define INCLUDE_ETHERNET

#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define MAKEWORD(a, b) ((WORD)(((UCHAR)(a)) | ((WORD)((UCHAR)(b))) << 8))
#endif

#if !defined(WIN32) && !defined(SOCKET)
   #define SOCKET int
#endif

#ifndef SOCKADDR_IN
   #define SOCKADDR_IN struct sockaddr_in
#endif

#ifndef SOCKET_ERROR
   #define SOCKET_ERROR -1
#endif


#define MAX_WINSOCK_UDP_WRITE       400
#define MAX_WINSOCK_TCP_WRITE       400
#define MULTICAST_IPADDRESS         "239.255.19.56"
#define GALIL_TCPIP_PORT            5000
#define GALIL_TCPIP_PORT_MESSAGES   5005

extern long ETHERNETReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead);
extern long ETHERNETWriteData(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten);
extern long ETHERNETClear(int iIndex);
extern int ETHERNETCharAvailableInput(int iIndex);
extern long OpenSocket(int iIndex);
extern long CloseSocket(int iIndex);

long ReadSocket(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead);
long ReadSocketMulticast(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead);
long WriteSocket(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten);
long WriteSocketMulticast(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten);
int FillBootpRep(unsigned char* data, int length, ULONG ulIPAddress, PUSHORT pusModel, PUSHORT pusSerialNumber);
int WillReadBlock(SOCKET sock);

#endif /* INCLUDE_ETHERNET */
