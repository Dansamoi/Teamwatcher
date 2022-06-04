#pragma once
#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <winsock2.h>
#include "Data.h"
#include "InputSimulator.h"
#include <vector>
#pragma comment(lib,"WS2_32")

class DataTransferHandler
{
public:
    void sendall(SOCKET s, const char* pdata, int buflen);

    void sendallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to);

    void recvall(SOCKET s, const char* pdata, int buflen);

    void recvallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from);

    void recvallUDPOld(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from);

    void recieveKeyPress(SOCKET s);

    void recieveMousePress(SOCKET s);

    void sendallScreenshot(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to);

    void recvPartScreenshot(SOCKET s, const char* pdata, SOCKADDR_IN from);

};

#endif // DATA_TRANSFER_H