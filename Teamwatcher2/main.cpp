#ifndef MAIN
#define MAIN

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include "Resource.h"
#include <ws2tcpip.h>
#include <windows.h>
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
#include "UIElement.h"
#include "Label.h"
#include "Button.h"
#include "TextBox.h"
#include "UIManager.h"
#include "Cipher.h"
#pragma comment(lib,"WS2_32")


// All of the UI
UIManager* windowUI;

// Declaring message handling functions for all the windows
LRESULT CALLBACK StartWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam); // for start window
LRESULT CALLBACK ClientWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam); // for client window
LRESULT CALLBACK ServerWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam); // for server window

// for using vector and map
using namespace std;

// Handles for all the threads
HANDLE serverSending, clientReceiving, serverReceiving, clientSending;

// rect for saving client window info
RECT Rect;

// Vector for receive and send all the pixels of the image
std::vector<uint8_t> Pixels = vector<uint8_t>();

// Vector for receive and send part of the pixels of the image
std::vector<uint8_t> ScreenPacketPixel = vector<uint8_t>();

// for checking if there is error with drawing on client screen
int draw_error = TRUE;

// generating password
LPWSTR password = LPWSTR(Password::generate(PASSWORD_SIZE));

// buffers for saving ip, port and password from the TextBoxes
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

// for saving GetText results
int gwstatIP = 0;
int gwstatPort = 0;
int gwstatPass = 0;

// for saving ip and port data
int port;
char* ip;

// for error handling
int error, lasterror = 0;

// for saving address information of connected sockets
SOCKADDR_IN InternetAddr, InternetAddrUDP;

// thread id
DWORD tid;


// Register the window class Names.
const wchar_t CLASS_NAME[] = L"Sample Window Class";
const wchar_t CLIENT_CLASS_NAME[] = L"Client Window Class";
const wchar_t SERVER_CLASS_NAME[] = L"Server Window Class";

// enum for saving all the different menues and messages
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
    NON
};

// enum of all the different control commands from client
enum Codes {
    EXIT,
    MOUSE_PRESS,
    KEY_PRESS
};


// int that saves the communication side
int commSide = NON;

// window classes of start window, class window and server window
WNDCLASS wc = { };
WNDCLASS wcClient = { };
WNDCLASS wcServer = { };

// MSG for getting the Asynchronous messages and handle them
MSG msg{};

// HWND of start window, client window and server window
HWND start_hwnd, client_hwnd, server_hwnd;

// structure for drawing the HBITMAP on the clients windows
PAINTSTRUCT ps = { 0 };

// handle to device context
HDC hDC;

// rect to save clients window info
RECT rc = { 0 };

// vector for saving all the keys pressed for the client
vector<uint32_t> keysPressed;

// HBITMAP to save the screen of the server to show in the client window
HBITMAP screenshot = NULL;

// saving the client command
int command;
// saving client's mouse coordinates
int xPos, yPos;

// for saving server and client UDP ports
int serverUDPport, clientUDPport;

// for saving the address length
int addrlen;

// flag for threads to continue or stop
BOOL threadFlag = TRUE;


// Diffie-Hellman
long long int randomValue = Cipher::random_num();
long long int sendingValue = Cipher::power(Cipher::G, randomValue, Cipher::P);

BOOL CreateAllSockets() {
    // Function for creating the TCP and UDP sockets
    // Creating UDP Socket
    sUDP = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    // Creating TCP Socket
    sTCP = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

    // Checking if created right
    if (sTCP == INVALID_SOCKET || sUDP == INVALID_SOCKET) {
        MessageBox(NULL, TEXT("Invalid Socket"), NULL, MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

BOOL CreateAllWindows(HINSTANCE currentInstance) {
    // Function to create all the windows

    // Creating the start window
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
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX, //WS_OVERLAPPEDWINDOW           // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 450,

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

    // Creating the client window
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

    // Creating the Server window
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
        WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 450,

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


void recieveMousePress(SOCKET s) {
    /*
    The function receive all the Mouse Key Press Input data,
    and simulates the input on the computer.
    */

    int xPos = 0; // for x coordinate
    int yPos = 0; // for y coordinate
    int w, h; // for width and height
    float xpos, ypos; // for real x, y coordinates (for new window)
    int code_up_down = 0; // for up or down code

    // recieve x position
    recvall(s, (char*)&xPos, sizeof(int));
    // recieve y position
    recvall(s, (char*)&yPos, sizeof(int));
    // recieve width
    recvall(s, (char*)&w, sizeof(int));
    // recieve height
    recvall(s, (char*)&h, sizeof(int));

    // calculating x and y positions
    xpos = (float)xPos / w;
    ypos = (float)yPos / h;

    // recieve up or down key
    recvall(s, (char*)&code_up_down, sizeof(int));

    xpos *= 65535; // for simulate the input
    ypos *= 65535; // for simulate the input

    // simulate the mouse input
    InputSimulator::SimulateMouseInput(code_up_down, xpos, ypos);
}

void sendallScreenshot(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to) {
    /*
    The function sends all the data of a screenshot.
    it simmilar to sendall() but for UDP sockets and sends screenshot in parts.
    */

    int sent = 0; // for saving the how many data was sent
    int sendLen = DATA_BUFSIZE * 5 + sizeof(int); // length of sending part
    int error; // saving error code
    ScreenPacketPixel.resize(sendLen); // resizing part of pixels vecotr according to part size

    // pointer to the buffer
    char* pPacket = (char*)&ScreenPacketPixel.front();

    // loop of sending the parts
    for (int i = 0; i < 1280*720*4/(DATA_BUFSIZE * 5); i++) {

        pPacket = (char*)&ScreenPacketPixel.front();
        sendLen = DATA_BUFSIZE * 5 + sizeof(int);

        // setting serial number
        memcpy(pPacket, &i, sizeof(int));
        // setting the part of the image data
        memcpy(pPacket + sizeof(int), pdata + i* (sendLen - sizeof(int)), sendLen - sizeof(int));

        // encrypting the data
        //Cipher::xor_all(pPacket, sendLen, &key, sizeof(int));

        // sending the part
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
        if (sent == 0 || sent == SOCKET_ERROR) break; // if disconnected or error occurred
    }
}

void recvPartScreenshot(SOCKET s, const char* pdata, SOCKADDR_IN from) {
    /*
    This function recieves part of screenshot and update the part in the
    Pixels vector buffer that recieved from pdata
    */

    // legth of part
    int packLen = DATA_BUFSIZE * 5;
    // saving the index
    int index = 0;
    // length of all packet
    int sendLen = DATA_BUFSIZE * 5 + sizeof(int);
    // resizing vector of part of pixels according to size
    ScreenPacketPixel.resize(sendLen);
    // saving pointer to buffer
    char* pPacket = (char*)&ScreenPacketPixel.front();

    // recieving part
    recvallUDP(s, (char*)&ScreenPacketPixel.front(), sendLen, from);

    // saving index
    memcpy(&index, pPacket, sizeof(int));

    // updating buffer according to index
    memcpy((void*)(pdata + index*packLen), pPacket + sizeof(int), sendLen - sizeof(int));
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
        // decrypting
        // Password::xor_encypt((char*)&command, sizeof(int), (char*)password, PASSWORD_SIZE);
        
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
DWORD WINAPI serverSendUDP(LPVOID lpParam)
{
    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;

    // getting screenshot and resizing it
    screenshot = Screen::GetScreenShot();
    screenshot = Screen::ResizeImage(screenshot, 1280, 720);

    // convert HBITMAP to pixels
    Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);
    int sz = Pixels.size();
    
    // sending size of vector
    sendallUDP(client, (char*)&sz, sizeof(sz), to);
    DeleteObject(screenshot);

    // Server executes continuously
    while (threadFlag) {
        // getting screenshot and resizing it
        screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1280, 720);

        // convert HBITMAP to pixels
        Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);

        // sending all screenshot
        sendallScreenshot(client, (char*)&Pixels.front(), Pixels.size() * sizeof(Pixels.front()), to);

        DeleteObject(screenshot);
        Pixels.clear();
    }
    return 1;
}

// Function that receive data from server
DWORD WINAPI clientReceiveUDP(LPVOID lpParam)
{
    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;

    // receive Pixels vector size
    int size = 0;
    recvallUDP(server, (char*)&size, sizeof(size), from);
    Pixels.resize(size);

    // Client executes continuously
    while (threadFlag) {
        // receive part of screenshot and update it in Pixels vector
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

        // sending all data from KeysPressed which saves clients input
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
    // Function for creating all UI for the screens

    windowUI = new UIManager(hwnd);
    windowUI->AddMenu(MAIN_MENU);
    windowUI->AddMenu(JOIN_MENU);
    windowUI->AddMenu(HOST_MENU);

    // getting window size and etc.
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    // Main Menu

    // Creating Title
    windowUI->AddElement(MAIN_MENU, LABEL, LPWSTR(L"Title"), LPWSTR(L"TEAM WATCHER"), width / 2 - 100, height / 8, 200, 30, NULL);

    // setting font
    windowUI->getElementByName(MAIN_MENU, LPWSTR(L"Title"))->setFont(30, FW_DEMIBOLD, LPWSTR(L"Tahoma"));

    // Creating Buttons
    windowUI->AddElement(MAIN_MENU, BUTTON, LPWSTR(L"JoinButton"), LPWSTR(L"JOIN"), width / 2 - 80, height / 8 + 100, 65, 25, JOIN_MENU);
    windowUI->AddElement(MAIN_MENU, BUTTON, LPWSTR(L"HostButton"), LPWSTR(L"HOST"), width / 2 + 15, height / 8 + 100, 65, 25, HOST_MENU);

    // Join Menu
    
    // Creating Back Button
    windowUI->AddElement(JOIN_MENU, BUTTON, LPWSTR(L"JoinBack"), LPWSTR(L"Back"), X, Y, 65, 25, MAIN_MENU);

    // Creating Labels
    windowUI->AddElement(JOIN_MENU, LABEL, LPWSTR(L"EnterIP"), LPWSTR(L"Enter Destination IP: "), X, Y + 50, 200, 25, NULL);
    windowUI->AddElement(JOIN_MENU, LABEL, LPWSTR(L"EnterPort"), LPWSTR(L"Enter Destination PORT: "), X, Y + 80, 200, 25, NULL);
    windowUI->AddElement(JOIN_MENU, LABEL, LPWSTR(L"EnterPass"), LPWSTR(L"Enter Password: "), X, Y + 110, 200, 25, NULL);

    // Creating Connect Button
    windowUI->AddElement(JOIN_MENU, BUTTON, LPWSTR(L"Connect"), LPWSTR(L"Connect"), X, Y + 160, 65, 25, CONNECTION);

    // Creating Input Boxes
    windowUI->AddElement(JOIN_MENU, TEXT_BOX, LPWSTR(L"ipText"), LPWSTR(L""), X + 180, Y + 50, 300, 20, NULL);
    windowUI->AddElement(JOIN_MENU, TEXT_BOX, LPWSTR(L"joinPortText"), LPWSTR(L""), X + 180, Y + 80, 300, 20, NULL);
    windowUI->AddElement(JOIN_MENU, TEXT_BOX, LPWSTR(L"joinPassText"), LPWSTR(L""), X + 180, Y + 110, 300, 20, NULL);

    // Host Menu

    // Getting local ip to show it
    char szHostName[255];
    gethostname(szHostName, 255);
    struct hostent* host_entry;
    host_entry = gethostbyname(szHostName);
    char* szLocalIP;
    szLocalIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);
    wchar_t local[20];
    mbstowcs(local, szLocalIP, strlen(szLocalIP) + 1);
    LPWSTR ptr = local;

    // Creating Back Button
    windowUI->AddElement(HOST_MENU, BUTTON, LPWSTR(L"hostBack"), LPWSTR(L"Back"), X, Y, 65, 25, MAIN_MENU);

    // Creating labels
    windowUI->AddElement(HOST_MENU, LABEL, LPWSTR(L"YourIP"), LPWSTR(L"Your IP: "), X, Y + 50, 200, 25, NULL);
    windowUI->AddElement(HOST_MENU, LABEL, LPWSTR(L"LocalIP"), ptr, X + 180, Y + 50, 200, 25, NULL);
    windowUI->AddElement(HOST_MENU, LABEL, LPWSTR(L"EnterDstPort"), LPWSTR(L"Enter PORT: "), X, Y + 80, 200, 25, NULL);
    windowUI->AddElement(HOST_MENU, LABEL, LPWSTR(L"YourPass"), LPWSTR(L"Your Password: "), X, Y + 110, 200, 25, NULL);
    windowUI->AddElement(HOST_MENU, LABEL, LPWSTR(L"hostPassText"), password, X + 180, Y + 110, 80, 25, NULL);

    // Creating control password buttons
    windowUI->AddElement(HOST_MENU, BUTTON, LPWSTR(L"Reset"), LPWSTR(L"Reset"), X + 340, Y + 110, 65, 25, RESET_PASSWORD);
    windowUI->AddElement(HOST_MENU, BUTTON, LPWSTR(L"Copy"), LPWSTR(L"Copy"), X + 415, Y + 110, 65, 25, COPY_PASSWORD);

    // Creating Create Server Button
    windowUI->AddElement(HOST_MENU, BUTTON, LPWSTR(L"Create"), LPWSTR(L"Create"), X, Y + 160, 65, 25, CREATION);

    // Creating input boxes
    windowUI->AddElement(HOST_MENU, TEXT_BOX, LPWSTR(L"hostPortText"), LPWSTR(L""), X + 180, Y + 80, 300, 20, CREATION);
}


int WINAPI WinMain(_In_ HINSTANCE currentInstance, _In_opt_ HINSTANCE previousInstance, _In_ LPSTR cmdLine, _In_ INT cmdShow) {
    // initializing Winsock
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed");
        return 1;
    }

    // Creating all sockets
    if (!CreateAllSockets()) {
        MessageBox(NULL, TEXT("Invalid Socket"), NULL, MB_ICONERROR);
        return 1;
    }

    // Creating all windows
    if (!CreateAllWindows(currentInstance)) {
        MessageBox(NULL, TEXT("Can't create windows"), NULL, MB_ICONERROR);
        return 1;
    }

    // Show start window
    ShowWindow(start_hwnd, cmdShow);
    UpdateWindow(start_hwnd);

    // Message Handling Function
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // cleanup and exit
    clean_exit();
    WSACleanup();

    // deleting UI
    //delete windowUI;

    return 0;
}

LRESULT CALLBACK StartWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
    // Function to handle start window messages
    switch (msg) {
    case WM_CREATE:
        // When Created 

        // Create All window UI
        createAllUI(hwnd);

        // make it all invisible
        windowUI->hideMenu(JOIN_MENU);
        windowUI->hideMenu(HOST_MENU);

        // show only main menu UI
        windowUI->showMenu(MAIN_MENU);
        break;

    case WM_COMMAND:
        // When command received
        switch (LOWORD(param))
        {
        case MAIN_MENU:
            // START MENU
            
            // make all UI invisible
            windowUI->hideMenu(JOIN_MENU);
            windowUI->hideMenu(HOST_MENU);

            // show only main menu UI
            windowUI->showMenu(MAIN_MENU);
            break;
        case HOST_MENU:
            // HOST
            // Show host menu UI elements
            windowUI->moveTo(MAIN_MENU, HOST_MENU);
            break;

        case JOIN_MENU:
            // JOIN
            // Show join menu UI elements
            windowUI->moveTo(MAIN_MENU, JOIN_MENU);
            break;

        case CONNECTION:
            // CONNECTION PROCESS
            
            // getting all the text from Input Boxes to buffers
            gwstatIP = windowUI->getElementByName(JOIN_MENU, LPWSTR(L"ipText"))->getText(ipSaved, 20);
            gwstatPort = windowUI->getElementByName(JOIN_MENU, LPWSTR(L"joinPortText"))->getText(portSaved, 20);
            gwstatPass = windowUI->getElementByName(JOIN_MENU, LPWSTR(L"joinPassText"))->getText(passSaved, 8);

            port = _wtoi(portSaved);

            // Checking port
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

            // getting and checking entered ip address
            if (0 == inet_pton(AF_INET, ip, &addr.sin_addr)) {
                MessageBox(NULL, TEXT("This is not a IPv4 address!\nIP's are in the following format:\nIPv4:(1-3 numbers).(1-3 numbers).(1-3 numbers).(1-3 numbers)"), NULL, MB_OK);
                break;
            }

            // Connect to server
            error = connect(sTCP, (sockaddr*)&addr, sizeof(addr));

            lasterror = WSAGetLastError();
            if (error != SOCKET_ERROR) { // If connection not failed
                commSide = CLIENT_MENU; // setting communication side

                // Hide start window
                ShowWindow(start_hwnd, SW_HIDE);

                // Show client window
                error = ShowWindow(client_hwnd, SW_SHOW);
                error = GetLastError();
                UpdateWindow(client_hwnd);

                // Set UDP address
                InternetAddrUDP.sin_family = AF_INET;
                InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
                InternetAddrUDP.sin_port = 0; // letting the os decide

                // bind UDP socket
                bind(sUDP, (sockaddr*)&InternetAddrUDP, sizeof(InternetAddrUDP));
                addrlen = sizeof(InternetAddrUDP);
                getsockname(sUDP, (struct sockaddr*)&InternetAddrUDP, &addrlen); // read binding

                memcpy(&serverUDPport, &InternetAddrUDP.sin_port, sizeof(serverUDPport));  // get the port number

                // Send UDP port to server
                sendall(sTCP, (char*)&serverUDPport, sizeof(int));

                // Setting ThreadFlag to TRUE
                threadFlag = TRUE;

                // Creating ClientReceiving Thread
                clientReceiving = CreateThread(NULL,
                    0,
                    clientReceiveUDP,
                    &sUDP,
                    0,
                    &tid);

                // Creating Client Sending Thread
                clientSending = CreateThread(NULL,
                    0,
                    clientSend,
                    &sTCP,
                    0,
                    &tid);
            }
            else
                MessageBox(NULL, TEXT("connection failed with error"), NULL, MB_ICONERROR); // If connection failed
            break;

        case CREATION:
            // CREATION PROCESS
            // getting all the text from Input Boxes to buffers
            gwstatPort = windowUI->getElementByName(HOST_MENU, LPWSTR(L"hostPortText"))->getText(portSaved, 20);
            gwstatPass = windowUI->getElementByName(HOST_MENU, LPWSTR(L"hostPassText"))->getText(passSaved, 8);

            port = _wtoi(portSaved);

            InternetAddr.sin_family = AF_INET;
            InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            InternetAddr.sin_port = htons(port);

            // Checking port
            if (port <= 0 || port > 65535) {
                MessageBox(NULL, TEXT("This is not a port!\nPorts are between 0-65535"), NULL, MB_OK);
                break;
            }

            // Binding the socket
            bind(sTCP, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));

            // Listening to incoming connection requests
            if (listen(sTCP, 1) == SOCKET_ERROR)
            {
                // If error occurred
                printf("listen() failed with error %d\n", WSAGetLastError());
                MessageBox(NULL, TEXT("listen() failed with error"), NULL, MB_ICONERROR);
                break;
            }
            else
            {
                // If listen completed without problems
                MessageBox(NULL, TEXT("listen() is OK!"), NULL, MB_OK);
            }

            // Creating Accept Socket
            addrlen = sizeof(to);
            sAccept = accept(sTCP, (sockaddr*)&to, &addrlen);
            addrlen = sizeof(to);

            // get accepted socket address
            error = getpeername(sAccept, (sockaddr*)&to, &addrlen);
            
            // Setting communication side as server side
            commSide = SERVER_MENU;

            // Hide start window
            ShowWindow(start_hwnd, SW_HIDE);

            // Show server window
            ShowWindow(server_hwnd, SW_SHOW);
            UpdateWindow(server_hwnd);

            InternetAddrUDP.sin_family = AF_INET;
            InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
            InternetAddrUDP.sin_port = 0; // let the os choose port

            // Binding UDP Socket
            bind(sUDP, (PSOCKADDR)&InternetAddrUDP, sizeof(InternetAddrUDP));

            // Receive client UDP port
            recvall(sAccept, (char*)&clientUDPport, sizeof(int));

            to.sin_port = clientUDPport;

            // Setting ThreadFlag to TRUE
            threadFlag = TRUE;

            // Creating Server sending Thread
            serverSending = CreateThread(NULL,
                0,
                serverSendUDP,
                &sUDP,
                0,
                &tid);

            // Creating Server receiving Thread
            serverReceiving = CreateThread(NULL,
                0,
                serverReceive,
                &sAccept,
                0,
                &tid);
            break;

        case RESET_PASSWORD:
            // RESET PASSWORD PROCESS
            // Reseting password
            password = Password::generate(PASSWORD_SIZE);
            windowUI->getElementByName(HOST_MENU, LPWSTR(L"hostPassText"))->setText(password);
            break;

        case COPY_PASSWORD:
            // COPY PASSWORD PROCESS
            // Copy password to Clipboard
            HWND hwnd1 = GetDesktopWindow();
            Clipboard::toClipboard(hwnd1, password);
            break;
        }
        break;

    case WM_DESTROY:
        // If window closed
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
        break;

    case WM_LBUTTONDOWN:
        // Push mouse press command
        keysPressed.push_back(MOUSE_PRESS);

        // Get coordinates of press
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        // Get window details
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        // Push all data
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_LEFTDOWN);
        break;

    case WM_LBUTTONUP:
        // Push mouse press command
        keysPressed.push_back(MOUSE_PRESS);

        // Get coordinates of press
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        // Get window details
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        // Push all data
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_LEFTUP);
        break;

    case WM_MBUTTONDOWN:
        // Push mouse press command
        keysPressed.push_back(MOUSE_PRESS);

        // Get coordinates of press
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        // Get window details
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        // Push all data
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_MIDDLEDOWN);
        break;

    case WM_MBUTTONUP:
        // Push mouse press command
        keysPressed.push_back(MOUSE_PRESS);
        
        // Get coordinates of press
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        // Get window details
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        // Push all data
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_MIDDLEUP);
        break;

    case WM_RBUTTONDOWN:
        // Push mouse press command
        keysPressed.push_back(MOUSE_PRESS);

        // Get coordinates of press
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        // Get window details
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        // Push all data
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_RIGHTDOWN);
        break;

    case WM_RBUTTONUP:
        // Push mouse press command
        keysPressed.push_back(MOUSE_PRESS);

        // Get coordinates of press
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        // Get window details
        GetClientRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        // Push all data
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);
        keysPressed.push_back(Rect.right - Rect.left); //width
        keysPressed.push_back(Rect.bottom - Rect.top); //height

        keysPressed.push_back(MOUSEEVENTF_RIGHTUP);
        break;

    case WM_KEYDOWN:
        // Push key press command and data
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(0);
        break;

    case WM_KEYUP:
        // Push key press command and data
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(KEYEVENTF_KEYUP);
        break;

    case WM_SYSKEYDOWN:
        // Push key press command and data
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(0);
        break;

    case WM_SYSKEYUP:
        // Push key press command and data
        keysPressed.push_back(KEY_PRESS);
        keysPressed.push_back(param);
        keysPressed.push_back(KEYEVENTF_KEYUP);
        break;

    case WM_PAINT:
        // Refreshing the image shown on client Window
        // This message is sended automatic all the time

        hDC = GetDC(hwnd);
        rc = { 0 };
        ::GetClientRect(hwnd, &rc);
        if(Pixels.size() != 0) {
            // Creating HBITMAP from Pixels
            screenshot = Screen::HBITMAPFromPixels(Pixels, 1280, 720, 32);
            // Draw HBITMAP
            draw_error = Screen::DrawBitmap(hDC, 0, 0, rc.right, rc.bottom, screenshot, SRCCOPY);
        }
        if (draw_error == FALSE)
            draw_error = GetLastError();
        DeleteObject(screenshot);
        ReleaseDC(hwnd, hDC);
        break;

    case WM_CLOSE:
        // If window closed

        // Hide Client Window
        ShowWindow(client_hwnd, SW_HIDE);

        // Show Start window
        ShowWindow(start_hwnd, SW_SHOW);
        UpdateWindow(start_hwnd);

        // Clean return to start window
        clean_exit();
        CreateAllSockets();
        break;

    case WM_DESTROY:
        // Destroying window
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
        break;

    case WM_CLOSE:
        // If window closed

        // Hide Server Window
        error = ShowWindow(server_hwnd, SW_HIDE);
        error = GetLastError();

        // Show Start window
        error = ShowWindow(start_hwnd, SW_SHOW);
        error = GetLastError();
        UpdateWindow(start_hwnd);

        // Clean return to start window
        clean_exit();
        CreateAllSockets();
        break;

    case WM_DESTROY:
        // Destroying window
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
    return 0;
}

#endif // MAIN