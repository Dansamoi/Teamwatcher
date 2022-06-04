#include "DataTransferHandler.h"

void DataTransferHandler::sendall(SOCKET s, const char* pdata, int buflen)
{
    /*
    The function sends all the data according to buflen.
    send() can send just part of the data, sendall() assures
    it sends all of it.
    */

    int sent = 0;
    while (buflen > 0) {
        sent = send(s, pdata, buflen, 0);
        if (sent == -1 || sent == 0) break; // 0 - closed socket, -1 - SOCKET_ERROR
        pdata += sent;
        buflen -= sent;
    }
}

void DataTransferHandler::sendallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to)
{
    /*
    The function sends all the data according to buflen.
    it simmilar to sendall() but for UDP sockets.
    sendto() can send just part of the data, sendallUDP() assures
    it sends all of it.
    */

    int sent = 0;
    int sendLen = DATA_BUFSIZE * 10;
    int error;
    while (buflen > 0) {
        if (buflen < sendLen)
            sendLen = buflen;
        sent = sendto(s, pdata, sendLen, 0, (sockaddr*)&to, sizeof(to));
        if (sent == -1) {
            error = WSAGetLastError();
            break;
        }
        if (sent == 0) break;
        pdata += sent;
        buflen -= sent;
    }
}

void DataTransferHandler::recvall(SOCKET s, const char* pdata, int buflen)
{
    /*
    The function receive all the data according to buflen.
    recv() can receive just part of the data, recvall() assures
    it receives all of it.
    */

    int recieved = 0;
    while (buflen > 0) {
        recieved = recv(s, (char*)pdata, buflen, 0);
        if (recieved == SOCKET_ERROR || recieved == 0)
            break; // 0 - closed socket, -1 - SOCKET_ERROR
        pdata += recieved;
        buflen -= recieved;
    }
}

void DataTransferHandler::recvallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from)
{
    /*
    The function receive all the data according to buflen.
    it simmilar to recvall() but for UDP sockets.
    recvfrom() can receive just part of the data, recvallUDP() assures
    it receives all of it.
    */

    int sizeOfFrom = sizeof(from);
    int recieved = 0;
    int recvLen = DATA_BUFSIZE * 10;
    int error;
    if (buflen < recvLen) {
        while (buflen > 0) {
            recieved = recvfrom(s, (char*)pdata, buflen, 0, (sockaddr*)&from, &sizeOfFrom);
            if (recieved == SOCKET_ERROR) {
                error = WSAGetLastError();
                break;
            }
            if (recieved == 0)
                break;
            pdata += recieved;
            buflen -= recieved;
        }
    }
    else
    {
        while (buflen > 0) {
            recvLen = DATA_BUFSIZE * 10;
            if (buflen < recvLen) recvLen = buflen;
            recieved = recvfrom(s, (char*)pdata, recvLen, 0, (sockaddr*)&from, &sizeOfFrom);
            if (recieved == SOCKET_ERROR) {
                error = WSAGetLastError();
                continue;
            }
            pdata += recieved;
            buflen -= recieved;
        }

    }
}

void DataTransferHandler::recvallUDPOld(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from)
{
    /*
    The function receive all the data according to buflen.
    it simmilar to recvall() but for UDP sockets.
    recvfrom() can receive just part of the data, recvallUDP() assures
    it receives all of it.
    */

    int sizeOfFrom = sizeof(from);
    int recieved = 0;
    int recvLen = DATA_BUFSIZE * 10;
    int packAmount = 1280 * 720 * 4 / (DATA_BUFSIZE * 10);
    int error;
    if (buflen < recvLen) {
        while (buflen > 0) {
            recieved = recvfrom(s, (char*)pdata, buflen, 0, (sockaddr*)&from, &sizeOfFrom);
            if (recieved == -1) {
                error = WSAGetLastError();
                continue;
            }
            pdata += recieved;
            buflen -= recieved;
        }
    }
    else
    {
        for (int i = 0; i < packAmount; i++) {
            recieved = recvfrom(s, (char*)pdata, recvLen, 0, (sockaddr*)&from, &sizeOfFrom);
            if (recieved == -1) {
                error = WSAGetLastError();
                continue;
            }
            pdata += DATA_BUFSIZE * 10;
            buflen -= DATA_BUFSIZE * 10;
        }

    }
    recvLen = buflen;
    while (buflen > 0) {
        recieved = recvfrom(s, (char*)pdata, recvLen, 0, (sockaddr*)&from, &sizeOfFrom);
        if (recieved == -1) {
            error = WSAGetLastError();
            continue;
        }
        pdata += recieved;
        buflen -= recieved;
    }
}

void DataTransferHandler::recieveKeyPress(SOCKET s)
{
    /*
    The function receive all the Key Press Input data,
    and simulates the input on the computer.
    */

    int code = 0;
    int up_down = 0;

    // receive key code
    recvall(s, (char*)&code, sizeof(int));

    // receive if the key up or down code
    recvall(s, (char*)&up_down, sizeof(int));

    // simulate the input
    InputSimulator::SimulateKeyInput(code, up_down);
}

void DataTransferHandler::recieveMousePress(SOCKET s)
{
    int xPos = 0;
    int yPos = 0;
    int w, h;
    float xpos, ypos;
    int code_up_down = 0;


    recvall(s, (char*)&xPos, sizeof(int));
    recvall(s, (char*)&yPos, sizeof(int));
    recvall(s, (char*)&w, sizeof(int));
    recvall(s, (char*)&h, sizeof(int));

    xpos = (float)xPos / w;
    ypos = (float)yPos / h;

    recvall(s, (char*)&code_up_down, sizeof(int));

    xpos *= 65535; // */ GetSystemMetrics(SM_CXFULLSCREEN)
    ypos *= 65535; // */ GetSystemMetrics(SM_CYFULLSCREEN)

    InputSimulator::SimulateMouseInput(code_up_down, xpos, ypos);


}

void DataTransferHandler::sendallScreenshot(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to)
{
    /*

    */
    std::vector<uint8_t> ScreenPacketPixel;
    int sent = 0;
    int sendLen = DATA_BUFSIZE * 5 + sizeof(int);
    int error;
    ScreenPacketPixel.resize(sendLen);

    char* pPacket = (char*)&ScreenPacketPixel.front();

    for (int i = 0; i < 1280 * 720 * 4 / (DATA_BUFSIZE * 5); i++) {
        pPacket = (char*)&ScreenPacketPixel.front();
        sendLen = DATA_BUFSIZE * 5 + sizeof(int);
        memcpy(pPacket, &i, sizeof(int));
        memcpy(pPacket + sizeof(int), pdata + i * (sendLen - sizeof(int)), sendLen - sizeof(int));

        while (sendLen > 0) {
            sent = sendto(s, pPacket, sendLen, 0, (sockaddr*)&to, sizeof(to));
            if (sent == SOCKET_ERROR) {
                error = WSAGetLastError();
                break;
            }
            if (sent == 0)
                break;
            pPacket += sent;
            sendLen -= sent;
        }
        if (sent == 0 || sent == SOCKET_ERROR) break;
    }
}

void DataTransferHandler::recvPartScreenshot(SOCKET s, const char* pdata, SOCKADDR_IN from)
{
    /*

    */
    std::vector<uint8_t> ScreenPacketPixel;
    int packLen = DATA_BUFSIZE * 5;
    int index = 0;
    int sendLen = DATA_BUFSIZE * 5 + sizeof(int);
    ScreenPacketPixel.resize(sendLen);
    char* pPacket = (char*)&ScreenPacketPixel.front();

    recvallUDP(s, (char*)&ScreenPacketPixel.front(), sendLen, from);

    memcpy(&index, pPacket, sizeof(int));
    memcpy((void*)(pdata + index * packLen), pPacket + sizeof(int), sendLen - sizeof(int));

    //recvallUDP(s, (char*)&index, sizeof(int), from);
    //recvallUDP(s, pdata + index * packLen, sizeof(packLen), from);
}
