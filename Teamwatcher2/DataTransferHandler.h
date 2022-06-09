#pragma once
#ifndef DATA_TRANSFER_H
#define DATA_TRANSFER_H

#include <winsock2.h>
#include "Data.h"
#include "InputSimulator.h"
#include <vector>
#pragma comment(lib,"WS2_32")

class DataTransferHandler
    // Class with different send recv functionality
{
public:
    // send all data using TCP
    void sendall(SOCKET s, const char* pdata, int buflen);

    // send all data using UDP
    void sendallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to);

    // receive all data using TCP
    void recvall(SOCKET s, const char* pdata, int buflen);

    // receive all data using UDP
    void recvallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from);

    // receive all data using UDP
    void recvallUDPOld(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from);

    // receive key press using TCP
    void recieveKeyPress(SOCKET s);

    // receive mouse press using TCP
    void recieveMousePress(SOCKET s);

    // send all screenshot using UDP
    void sendallScreenshot(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to);

    // receive part of screenshot using UDP
    void recvPartScreenshot(SOCKET s, const char* pdata, SOCKADDR_IN from);

};

#endif // DATA_TRANSFER_H