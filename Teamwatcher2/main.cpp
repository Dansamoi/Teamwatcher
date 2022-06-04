#ifndef MAIN
#define MAIN

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include "Resource.h"
#include <ws2tcpip.h>
#include <windows.h>
#include "CreateUI.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <iostream>
#include "Password.h"
#include "Clipboard.h"
#include "Data.h"
#include "Screen.h"
#include <string>
#include <vector>
#include "InputSimulator.h"
#pragma comment(lib,"WS2_32")

// Message handling functions for all the windows
LRESULT CALLBACK StartWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam); // for start window
LRESULT CALLBACK ClientWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam); // for client window
LRESULT CALLBACK ServerWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam); // for server window

using namespace std;

HINSTANCE hInstance;
HICON hIcon;

// Handles for all the threads
HANDLE serverSending, clientReceiving, serverReceiving, clientSending;

int threadExitFlag = 1;

RECT Rect;

// Vector for receive and send all the pixels of the image
std::vector<uint8_t> Pixels = vector<uint8_t>();

std::vector<uint8_t> ScreenPacketPixel = vector<uint8_t>();;
int SerialNum = TRUE;

BYTE* bufferImage; // delete this

HWND ipText;
HWND joinPortText;
HWND joinPassText;
HWND hostPortText;
HWND hostPassText;
LPWSTR password = LPWSTR(Password::generate(PASSWORD_SIZE));
wchar_t ipSaved[20];
wchar_t portSaved[20];
wchar_t passSaved[20];

// UDP socket for sending and recieving the image pixels
SOCKET sUDP;

// TCP socket for server listening for connections and for client key press input sending
SOCKET sTCP;

// TCP socket for server receive client key press input
SOCKET sAccept;

// SOCKADDR_IN struct to save information of client UDP socket for the server and server UDP socket for the client
SOCKADDR_IN from, to;

int gwstatIP = 0;
int gwstatPort = 0;
int gwstatPass = 0;

int port;
char* ip;

int error, lasterror = 0;

SOCKADDR_IN InternetAddr, InternetAddrUDP;
ULONG NonBlock;
DWORD Flags;
DWORD SendBytes;
DWORD RecvBytes;

DWORD tid;

HWND chatText;

char buffer[DATA_BUFSIZE] = { 0 };

// Register the window class Names.
const wchar_t CLASS_NAME[] = L"Sample Window Class";
const wchar_t CLIENT_CLASS_NAME[] = L"Client Window Class";
const wchar_t SERVER_CLASS_NAME[] = L"Server Window Class";

enum MenuNumbers {
    BASE = 0,
    MAIN_MENU = 1,
    JOIN_MENU = 2,
    HOST_MENU = 3,
    CONNECTION = 4,
    CREATION = 5,
    RESET_PASSWORD = 6,
    COPY_PASSWORD = 7,
    CLIENT_MENU = 8, SERVER_MENU = 9,
    START_MENU = 10,
    //CLIENT_MSG_NOTIFICATION = 103,
    //HOST_MSG_NOTIFICATION = 104,
    NON
};

enum Codes {
    EXIT,
    MOUSE_PRESS,
    KEY_PRESS
};

map<int, vector<HWND>> Menus;

int commSide = NON;

WNDCLASS wc = { };
WNDCLASS wcClient = { };
WNDCLASS wcServer = { };

MSG msg{};
MSG msgClient{};
MSG msgServer{};

HWND start_hwnd, client_hwnd, server_hwnd, hScreenWindow;

PAINTSTRUCT ps = { 0 };
HDC hDC;
RECT rc = { 0 };

int wError;

vector<uint32_t> keysPressed;

HBITMAP screenshot = NULL;

int command;
int xPos, yPos;

int serverUDPport, clientUDPport;
int addrlen;

BOOL threadFlag = TRUE;

BOOL CreateAllSockets() {
    // Creating TCP Socket and UDP socket
    sUDP = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    sTCP = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sTCP == INVALID_SOCKET || sUDP == INVALID_SOCKET) {
        MessageBox(NULL, TEXT("Invalid Socket"), NULL, MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

BOOL CreateAllWindows(HINSTANCE currentInstance) {
    // Register the start window class.
    wc.hInstance = currentInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hIcon = LoadIcon(currentInstance, MAKEINTRESOURCE(IDI_TEAMWATCHER2));
    wc.lpfnWndProc = StartWindowProcessMessages;

    RegisterClass(&wc);

    // Create the start window.
    start_hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"TeamWatcher",                      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        wc.hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (start_hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Could not create window"), NULL, MB_ICONERROR);
        return FALSE;
    }


    // Register the client window class.
    wcClient.hInstance = currentInstance;
    wcClient.lpszClassName = CLIENT_CLASS_NAME;
    wcClient.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcClient.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcClient.hIcon = LoadIcon(currentInstance, MAKEINTRESOURCE(IDI_TEAMWATCHER2));
    wcClient.lpfnWndProc = ClientWindowProcessMessages;

    RegisterClass(&wcClient);

    // Create the CLIENT window.
    client_hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLIENT_CLASS_NAME,                     // Window class
        L"TeamWatcher-Client",                      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        wcClient.hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (client_hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Could not create window"), NULL, MB_ICONERROR);
        return FALSE;
    }

    // Register the SERVER window class.
    wcServer.hInstance = currentInstance;
    wcServer.lpszClassName = SERVER_CLASS_NAME;
    wcServer.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcServer.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wcServer.hIcon = LoadIcon(currentInstance, MAKEINTRESOURCE(IDI_TEAMWATCHER2));
    wcServer.lpfnWndProc = ServerWindowProcessMessages;

    RegisterClass(&wcServer);

    // Create the SERVER window.
    server_hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        SERVER_CLASS_NAME,                     // Window class
        L"TeamWatcher-HOST",                      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        wcServer.hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (server_hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Could not create window"), NULL, MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

void clean_exit() {
    /* 
    The function called before exiting
    from the program for clean exit.
    */
    //closing all sockets
    closesocket(sTCP);
    closesocket(sAccept);
    closesocket(sUDP);
    //closesocket(sAccept);

    threadFlag = FALSE;
    keysPressed.clear();
    ScreenPacketPixel.clear();
    Pixels.clear();

    // closing threads according to communication side
    switch (commSide) {
    case CLIENT_MENU:
        TerminateThread(clientReceiving, 0);
        TerminateThread(clientSending, 0);
        //shutdown(sTCP, SD_BOTH);
        closesocket(sTCP);
        break;
    case SERVER_MENU:
        //shutdown(sAccept, SD_BOTH);
        TerminateThread(serverReceiving, 0);
        TerminateThread(serverSending, 0);
        error = closesocket(sAccept);
        error = WSAGetLastError();
        closesocket(sTCP);
        break;
    }
}

void sendall(SOCKET s, const char* pdata, int buflen) {
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

void sendallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to) {
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

void recvall(SOCKET s, const char* pdata, int buflen) {
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

void recvallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from) {
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

void recvallUDPOld(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from) {
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

//void sendScreen(SOCKET s);
//void recieveScreen(SOCKET s);
//void sendKeyPress(SOCKET s);
void recieveKeyPress(SOCKET s) {
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

//void sendMousePress(SOCKET s);
void recieveMousePress(SOCKET s) {
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


void sendallScreenshot(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to) {
    /*

    */

    int sent = 0;
    int sendLen = DATA_BUFSIZE * 5 + sizeof(int);
    int error;
    ScreenPacketPixel.resize(sendLen);

    char* pPacket = (char*)&ScreenPacketPixel.front();

    for (int i = 0; i < 1280*720*4/(DATA_BUFSIZE * 5); i++) {
        pPacket = (char*)&ScreenPacketPixel.front();
        sendLen = DATA_BUFSIZE * 5 + sizeof(int);
        memcpy(pPacket, &i, sizeof(int));
        memcpy(pPacket + sizeof(int), pdata + i* (sendLen - sizeof(int)), sendLen - sizeof(int));

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

void recvPartScreenshot(SOCKET s, const char* pdata, SOCKADDR_IN from) {
    /*

    */

    int packLen = DATA_BUFSIZE * 5;
    int index = 0;
    int sendLen = DATA_BUFSIZE * 5 + sizeof(int);
    ScreenPacketPixel.resize(sendLen);
    char* pPacket = (char*)&ScreenPacketPixel.front();

    recvallUDP(s, (char*)&ScreenPacketPixel.front(), sendLen, from);

    memcpy(&index, pPacket, sizeof(int));
    memcpy((void*)(pdata + index*packLen), pPacket + sizeof(int), sendLen - sizeof(int));
    
    //recvallUDP(s, (char*)&index, sizeof(int), from);
    //recvallUDP(s, pdata + index * packLen, sizeof(packLen), from);
}

// Function that receive data
// from client
DWORD WINAPI serverReceive(LPVOID lpParam)
{
    /* 
    Function that receive data from client
    */

    // buffer to save command
    int command;

    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;

    // Server executes continuously
    while (threadFlag) {

        // If received buffer gives
        // error then return -1
        if (recv(client, (char*)&command, sizeof(int), 0)
            == SOCKET_ERROR) {
            cout << "recv function failed with error "
                << WSAGetLastError() << endl;
            return -1;
        }
        //Password::xor_encypt((char*)&command, sizeof(int), (char*)password, PASSWORD_SIZE);
        // execute command (exit = 0, register mouse press = 1, register key press = 2)
        switch (command)
        {
        case EXIT:
            clean_exit();
            break;

        case MOUSE_PRESS:
            recieveMousePress(client);
            break;

        case KEY_PRESS:
            recieveKeyPress(client);
            break;
        }

    }
    return 1;
}

// Function that sends data to client
DWORD WINAPI serverSend(LPVOID lpParam)
{
    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;

    // Server executes continuously
    while (threadFlag) {
        screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1280, 720);
        Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);
        //bufferImage = Screen::GetPixelsFromHBITMAP(screenshot);
        // Input message server
        // wants to send to client

        // If sending failed
        // return -1
        int sz = Pixels.size();
        sendall(client, (char*)&sz, sizeof(sz));
        sendall(client, (char*)&Pixels.front(), Pixels.size() * sizeof(Pixels.front()));

        //delete[] bufferImage;
        DeleteObject(screenshot);
        Pixels.clear();
    }
    return 1;
}


DWORD WINAPI serverSendUDP(LPVOID lpParam)
{


    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;

    double delay = 1 / 30 * CLOCKS_PER_SEC;
    clock_t last_cycle = clock();
    screenshot = Screen::GetScreenShot();
    double now = (clock() - last_cycle) / (double)CLOCKS_PER_SEC;;
    last_cycle = clock();
    screenshot = Screen::ResizeImage(screenshot, 1280, 720);
    now = (clock() - last_cycle) / (double)CLOCKS_PER_SEC;;
    Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);
    int sz = Pixels.size();
    sendallUDP(client, (char*)&sz, sizeof(sz), to);
    DeleteObject(screenshot);
    while (threadFlag) {

        screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1280, 720);
        Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);

        sendallUDP(client, (char*)&Pixels.front(), Pixels.size() * sizeof(Pixels.front()), to);
        //delete[] bufferImage;
        DeleteObject(screenshot);
        Pixels.clear();
        while (clock() - last_cycle < delay);
        last_cycle = clock();
    }
    return 1;
}

DWORD WINAPI serverSendUDPNew(LPVOID lpParam)
{
    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;
    double delay = 1 / 30 * CLOCKS_PER_SEC;
    clock_t last_cycle = clock();
    screenshot = Screen::GetScreenShot();
    double now = (clock() - last_cycle) / (double)CLOCKS_PER_SEC;;
    last_cycle = clock();
    screenshot = Screen::ResizeImage(screenshot, 1280, 720);
    now = (clock() - last_cycle) / (double)CLOCKS_PER_SEC;;
    Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);
    int sz = Pixels.size();
    sendallUDP(client, (char*)&sz, sizeof(sz), to);
    DeleteObject(screenshot);
    while (threadFlag) {

        last_cycle = clock();
        screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1280, 720);
        last_cycle = clock();
        Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);

        now = (clock() - last_cycle) / (double)CLOCKS_PER_SEC;;
        last_cycle = clock();

        sendallScreenshot(client, (char*)&Pixels.front(), Pixels.size() * sizeof(Pixels.front()), to);

        now = (clock() - last_cycle) / (double)CLOCKS_PER_SEC;;
        //delete[] bufferImage;
        DeleteObject(screenshot);
        Pixels.clear();
    }
    return 1;
}

// Function that receive data from server
DWORD WINAPI clientReceive(LPVOID lpParam)
{

    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;
    int size = 0;
    // Client executes continuously
    while (threadFlag) {

        size = 0;
        recvall(server, (char*)&size, sizeof(size));
        if (size == 0) continue;
        Pixels.resize(size);
        recvall(server, (char*)&Pixels.front(), size * sizeof(Pixels.front()));
        if (!Pixels.empty())
            DeleteObject(screenshot);
            screenshot = Screen::HBITMAPFromPixels(Pixels, 1280, 720, 32);
            Pixels.clear();

    }
    return 1;
}

DWORD WINAPI clientReceiveUDP(LPVOID lpParam)
{
    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;
    int size = 0;
    recvallUDP(server, (char*)&size, sizeof(size), from);
    Pixels.resize(size);
    // Client executes continuously
    while (threadFlag) {
        recvallUDP(server, (char*)&Pixels.front(), size * sizeof(Pixels.front()), from);

        if (!Pixels.empty())
            DeleteObject(screenshot);

        screenshot = Screen::HBITMAPFromPixels(Pixels, 1280, 720, 32);
    }
    return 1;
}

DWORD WINAPI clientReceiveUDPNew(LPVOID lpParam)
{

    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;
    int size = 0;
    recvallUDP(server, (char*)&size, sizeof(size), from);
    Pixels.resize(size);
    // Client executes continuously
    while (threadFlag) {
        recvPartScreenshot(server, (char*)&Pixels.front(), from);

    }
    return 1;
}

// Function that sends data to server
DWORD WINAPI clientSend(LPVOID lpParam)
{
    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;

    // Client executes continuously
    while (threadFlag) {

        // Input message client
        // wants to send to server
        if (!keysPressed.empty())
        {
            //Password::xor_encypt((char*)&keysPressed[0], sizeof(int), (char*)password, PASSWORD_SIZE);
            sendall(server, (char*)&keysPressed[0], sizeof(int));

            keysPressed.erase(keysPressed.begin());

        }
    }
    return 1;
}

void createAllUI(HWND hwnd) {
    vector<HWND> main_menu, join_menu, host_menu;// , server_menu, client_menu;

    // Main Menu
    main_menu.push_back(CreateUI::CreateTextBox(L"TEAM WATCHER", X, Y, 300, 25, hwnd));
    main_menu.push_back(CreateUI::CreateButton(L"JOIN", X, Y + 60, 65, 25, hwnd, JOIN_MENU));
    main_menu.push_back(CreateUI::CreateButton(L"HOST", X + 80, Y + 60, 65, 25, hwnd, HOST_MENU));
    Menus[MAIN_MENU] = main_menu;

    // Join Menu
    join_menu.push_back(CreateUI::CreateButton(L"Back", X, Y, 65, 25, hwnd, MAIN_MENU));
    join_menu.push_back(CreateUI::CreateTextBox(L"Enter Destination IP: ", X, Y + 30, 200, 25, hwnd));
    join_menu.push_back(CreateUI::CreateTextBox(L"Enter Destination PORT: ", X, Y + 60, 200, 25, hwnd));
    join_menu.push_back(CreateUI::CreateTextBox(L"Enter Password: ", X, Y + 90, 200, 25, hwnd));
    join_menu.push_back(CreateUI::CreateButton(L"Connect", X, Y + 120, 65, 25, hwnd, CONNECTION));
    ipText = CreateUI::CreateInputBox(L"", X + 180, Y + 30, 300, 20, hwnd);
    joinPortText = CreateUI::CreateInputBox(L"", X + 180, Y + 60, 300, 20, hwnd);
    joinPassText = CreateUI::CreateInputBox(L"", X + 180, Y + 90, 300, 20, hwnd);
    join_menu.push_back(ipText);
    join_menu.push_back(joinPortText);
    join_menu.push_back(joinPassText);
    Menus[JOIN_MENU] = join_menu;

    // Host Menu
    char szHostName[255];
    gethostname(szHostName, 255);
    struct hostent* host_entry;
    host_entry = gethostbyname(szHostName);
    char* szLocalIP;
    szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
    wchar_t local[20];
    mbstowcs(local, szLocalIP, strlen(szLocalIP) + 1);
    LPWSTR ptr = local;

    host_menu.push_back(CreateUI::CreateButton(L"Back", X, Y, 65, 25, hwnd, MAIN_MENU));
    host_menu.push_back(CreateUI::CreateTextBox(L"Your IP is: ", X, Y + 30, 200, 25, hwnd));
    host_menu.push_back(CreateUI::CreateTextBox(ptr, X + 180, Y + 30, 200, 25, hwnd));
    host_menu.push_back(CreateUI::CreateTextBox(L"Enter PORT: ", X, Y + 60, 200, 25, hwnd));
    host_menu.push_back(CreateUI::CreateTextBox(L"Your Password: ", X, Y + 90, 200, 25, hwnd));
    host_menu.push_back(CreateUI::CreateButton(L"Reset", X + 340, Y + 90, 65, 25, hwnd, RESET_PASSWORD));
    host_menu.push_back(CreateUI::CreateButton(L"Copy", X + 415, Y + 90, 65, 25, hwnd, COPY_PASSWORD));
    host_menu.push_back(CreateUI::CreateButton(L"Create", X, Y + 120, 65, 25, hwnd, CREATION));
    hostPortText = CreateUI::CreateInputBox(L"", X + 180, Y + 60, 300, 20, hwnd);
    hostPassText = CreateUI::CreateTextBox(password, X + 180, Y + 90, 80, 25, hwnd);
    host_menu.push_back(hostPortText);
    host_menu.push_back(hostPassText);
    Menus[HOST_MENU] = host_menu;
}

void invisible() {
    for (auto v : Menus) {
        for (auto e : v.second) {
            ShowWindow(e, SW_HIDE);
        }
    }
}

void visible(int menuNum) {
    for (auto e : Menus[menuNum]) {
        ShowWindow(e, SW_SHOW);
    }
}


int WINAPI WinMain(_In_ HINSTANCE currentInstance, _In_opt_ HINSTANCE previousInstance, _In_ LPSTR cmdLine, _In_ INT cmdShow) {
    // initializing Winsock
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed");
        return 1;
    }

    if (!CreateAllSockets()) {
        MessageBox(NULL, TEXT("Invalid Socket"), NULL, MB_ICONERROR);
        return 1;
    }

    if (!CreateAllWindows(currentInstance)) {
        MessageBox(NULL, TEXT("Can't create windows"), NULL, MB_ICONERROR);
        return 1;
    }

    ShowWindow(start_hwnd, cmdShow);
    UpdateWindow(start_hwnd);

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    clean_exit();
    WSACleanup();

    //WaitForSingleObject(serverSending, INFINITE);
    //WaitForSingleObject(serverReceiving, INFINITE);
    //WaitForSingleObject(clientReceiving, INFINITE);
    //WaitForSingleObject(clientSending, INFINITE);

    //ShowWindow(start_hwnd, SW_HIDE);
    /*
    switch (commSide) {
    case SERVER_MENU:

        ShowWindow(server_hwnd, cmdShow);
        UpdateWindow(server_hwnd);

        InternetAddrUDP.sin_family = AF_INET;
        InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
        InternetAddrUDP.sin_port = 0; // let the os choose port

        bind(sUDP, (PSOCKADDR)&InternetAddrUDP, sizeof(InternetAddrUDP));

        recvall(sAccept, (char*)&clientUDPport, sizeof(int));

        to.sin_port = clientUDPport;

        serverSending = CreateThread(NULL,
            0,
            serverSendUDPNew,
            &sUDP,
            0,
            &tid);


        serverReceiving = CreateThread(NULL,
            0,
            serverReceive,
            &sAccept,
            0,
            &tid);

        while (GetMessage(&msgServer, nullptr, 0, 0)) {
            TranslateMessage(&msgServer);
            DispatchMessage(&msgServer);
        }

        WaitForSingleObject(serverSending, INFINITE);
        WaitForSingleObject(serverReceiving, INFINITE);

        CloseWindow(server_hwnd);
        break;

    case CLIENT_MENU:
        ShowWindow(client_hwnd, cmdShow);
        UpdateWindow(client_hwnd);

        InternetAddrUDP.sin_family = AF_INET;
        InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
        InternetAddrUDP.sin_port = 0; // letting the os decide
        //serverUDPport = 5000;

        bind(sUDP, (sockaddr*)&InternetAddrUDP, sizeof(InternetAddrUDP));
        addrlen = sizeof(InternetAddrUDP);
        getsockname(sUDP, (struct sockaddr*)&InternetAddrUDP, &addrlen); // read binding

        memcpy(&serverUDPport, &InternetAddrUDP.sin_port, sizeof(serverUDPport));  // get the port number

        sendall(sTCP, (char*)&serverUDPport, sizeof(int));

        clientReceiving = CreateThread(NULL,
            0,
            clientReceiveUDPNew,
            &sUDP,
            0,
            &tid);

        clientSending = CreateThread(NULL,
            0,
            clientSend,
            &sTCP,
            0,
            &tid);

        while (GetMessage(&msgClient, nullptr, 0, 0)) {
            TranslateMessage(&msgClient);
            DispatchMessage(&msgClient);
        }

        WaitForSingleObject(clientReceiving, INFINITE);
        WaitForSingleObject(clientSending, INFINITE);

        CloseWindow(client_hwnd);
        break;
    }

    */
    return 0;
}

LRESULT CALLBACK StartWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
    case WM_CREATE:
        createAllUI(hwnd);
        invisible();
        visible(MAIN_MENU);
        break;

    case WM_COMMAND:
        switch (LOWORD(param))
        {
        case MAIN_MENU:
            // START MENU
            invisible();
            visible(MAIN_MENU);
            break;
        case HOST_MENU:
            // HOST
            invisible();
            visible(HOST_MENU);
            break;

        case JOIN_MENU:
            // JOIN
            invisible();
            visible(JOIN_MENU);
            break;

        case CONNECTION:
            // CONNECTION PROCESS
            gwstatIP = GetWindowText(ipText, ipSaved, 20);
            gwstatPort = GetWindowText(joinPortText, portSaved, 20);
            gwstatPass = GetWindowText(joinPassText, passSaved, 20);

            MessageBox(hwnd, ipSaved, portSaved, MB_OK);

            port = _wtoi(portSaved);

            if (port <= 0 || port > 65535) {
                MessageBox(NULL, TEXT("This is not a port!\nPorts are between 0-65535"), NULL, MB_OK);
                break;
            }

            ip = new char[20];
            memset(ip, 0, 20);

            wcstombs(ip, ipSaved, wcslen(ipSaved));

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            if (0 == inet_pton(AF_INET, ip, &addr.sin_addr)) {
                MessageBox(NULL, TEXT("This is not a IPv4 address!\nIP's are in the following format:\nIPv4:(1-3 numbers).(1-3 numbers).(1-3 numbers).(1-3 numbers)"), NULL, MB_OK);
                break;
            }

            error = connect(sTCP, (sockaddr*)&addr, sizeof(addr));
            lasterror = WSAGetLastError();
            if (error != SOCKET_ERROR) {
                //commSide = CLIENT_MENU;
                ShowWindow(start_hwnd, SW_HIDE);
                error = ShowWindow(client_hwnd, SW_SHOW);
                error = GetLastError();
                UpdateWindow(client_hwnd);

                InternetAddrUDP.sin_family = AF_INET;
                InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
                InternetAddrUDP.sin_port = 0; // letting the os decide
                //serverUDPport = 5000;

                bind(sUDP, (sockaddr*)&InternetAddrUDP, sizeof(InternetAddrUDP));
                addrlen = sizeof(InternetAddrUDP);
                getsockname(sUDP, (struct sockaddr*)&InternetAddrUDP, &addrlen); // read binding

                memcpy(&serverUDPport, &InternetAddrUDP.sin_port, sizeof(serverUDPport));  // get the port number

                sendall(sTCP, (char*)&serverUDPport, sizeof(int));

                threadFlag = TRUE;

                clientReceiving = CreateThread(NULL,
                    0,
                    clientReceiveUDPNew,
                    &sUDP,
                    0,
                    &tid);

                clientSending = CreateThread(NULL,
                    0,
                    clientSend,
                    &sTCP,
                    0,
                    &tid);
            }
            else
                MessageBox(NULL, TEXT("connection failed with error"), NULL, MB_ICONERROR);
            break;

        case CREATION:
            // CREATION PROCESS
            gwstatPort = GetWindowText(hostPortText, portSaved, 20);
            gwstatPass = GetWindowText(hostPassText, passSaved, 20);

            //MessageBox(hwnd, passSaved, portSaved, MB_OK);

            port = _wtoi(portSaved);

            InternetAddr.sin_family = AF_INET;
            InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            InternetAddr.sin_port = htons(port);

            if (port <= 0 || port > 65535) {
                MessageBox(NULL, TEXT("This is not a port!\nPorts are between 0-65535"), NULL, MB_OK);
                break;
            }

            bind(sTCP, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));

            if (listen(sTCP, 1) == SOCKET_ERROR)
            {
                printf("listen() failed with error %d\n", WSAGetLastError());
                MessageBox(NULL, TEXT("listen() failed with error"), NULL, MB_ICONERROR);
                break;
            }
            else
            {
                MessageBox(NULL, TEXT("listen() is OK!"), NULL, MB_OK);
            }
            addrlen = sizeof(to);
            sAccept = accept(sTCP, (sockaddr*)&to, &addrlen);
            addrlen = sizeof(to);
            error = getpeername(sAccept, (sockaddr*)&to, &addrlen);
            //WSAAsyncSelect(sTCP, hwnd, HOST_MSG_NOTIFICATION, FD_ACCEPT | FD_CONNECT | FD_READ | FD_CLOSE);
            //commSide = SERVER_MENU;

            ShowWindow(start_hwnd, SW_HIDE);

            ShowWindow(server_hwnd, SW_SHOW);
            UpdateWindow(server_hwnd);

            InternetAddrUDP.sin_family = AF_INET;
            InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
            InternetAddrUDP.sin_port = 0; // let the os choose port

            bind(sUDP, (PSOCKADDR)&InternetAddrUDP, sizeof(InternetAddrUDP));

            recvall(sAccept, (char*)&clientUDPport, sizeof(int));

            to.sin_port = clientUDPport;

            threadFlag = TRUE;

            serverSending = CreateThread(NULL,
                0,
                serverSendUDPNew,
                &sUDP,
                0,
                &tid);


            serverReceiving = CreateThread(NULL,
                0,
                serverReceive,
                &sAccept,
                0,
                &tid);
            break;

        case RESET_PASSWORD:
            // RESET PASSWORD PROCESS
            password = Password::generate(PASSWORD_SIZE);
            SetWindowText(hostPassText, password);
            break;

        case COPY_PASSWORD:
            // COPY PASSWORD PROCESS
            HWND hwnd1 = GetDesktopWindow();
            Clipboard::toClipboard(hwnd1, password);
            break;
        }
        break;

    case WM_DESTROY:
        clean_exit();
        WSACleanup();
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
}

LRESULT CALLBACK ClientWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
    case WM_CREATE:
        //createAllUI(hwnd);
        //invisible();
        //visible(CLIENT_MENU);
        break;

    case WM_LBUTTONDOWN:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_LEFTDOWN);
        break;

    case WM_LBUTTONUP:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_LEFTUP);
        break;

    case WM_MBUTTONDOWN:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_MIDDLEDOWN);
        break;

    case WM_MBUTTONUP:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_MIDDLEUP);
        break;

    case WM_RBUTTONDOWN:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_RIGHTDOWN);
        break;

    case WM_RBUTTONUP:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        Rect;

        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_RIGHTUP);
        break;

    case WM_KEYDOWN:
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(0);
        break;

    case WM_KEYUP:
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(KEYEVENTF_KEYUP);
        break;

    case WM_SYSKEYDOWN:
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(0);
        break;

    case WM_SYSKEYUP:
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(KEYEVENTF_KEYUP);
        break;

    case WM_PAINT:
        hDC = GetDC(hwnd);
        rc = { 0 };
        ::GetClientRect(hwnd, &rc);
        if(Pixels.size() != 0)
            screenshot = Screen::HBITMAPFromPixels(Pixels, 1280, 720, 32);
            SerialNum = Screen::DrawBitmap(hDC, 0, 0, rc.right, rc.bottom, screenshot, SRCCOPY);
        if (SerialNum == FALSE)
            SerialNum = GetLastError();
        DeleteObject(screenshot);
        ReleaseDC(hwnd, hDC);
        break;

    case WM_CLOSE:
        ShowWindow(client_hwnd, SW_HIDE);
        ShowWindow(start_hwnd, SW_SHOW);
        UpdateWindow(start_hwnd);
        clean_exit();
        CreateAllSockets();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
    return 0;
}

LRESULT CALLBACK ServerWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
    case WM_CREATE:
        //createAllUI(hwnd);
        //invisible();
        //visible(SERVER_MENU);
        break;

    case WM_CLOSE:
        error = ShowWindow(server_hwnd, SW_HIDE);
        error = GetLastError();
        error = ShowWindow(start_hwnd, SW_SHOW);
        error = GetLastError();
        UpdateWindow(start_hwnd);
        clean_exit();
        CreateAllSockets();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
    return 0;
}

#endif // MAIN

