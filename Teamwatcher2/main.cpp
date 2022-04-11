#ifndef MAIN
#define MAIN

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
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
#pragma comment(lib,"WS2_32")

LRESULT CALLBACK StartWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);
LRESULT CALLBACK ClientWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);
LRESULT CALLBACK ServerWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);

using namespace std;

HWND textfield;
HWND ipText;
HWND joinPortText;
HWND joinPassText;
HWND hostPortText;
HWND hostPassText;
LPWSTR password = Password::generate(PASSWORD_SIZE);
wchar_t ipSaved[20];
wchar_t portSaved[20];
wchar_t passSaved[20];

SOCKET sUDP;
SOCKET sTCP;

SOCKET sAccept;


int gwstatIP = 0;
int gwstatPort = 0;
int gwstatPass = 0;

int port;
char* ip;

int error;

SOCKADDR_IN InternetAddr;
ULONG NonBlock;
DWORD Flags;
DWORD SendBytes;
DWORD RecvBytes;

DWORD tid;
HANDLE t1, t2;

HWND chatText;

char buffer[DATA_BUFSIZE] = { 0 };

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
    CLIENT_MSG_NOTIFICATION = 103, 
    HOST_MSG_NOTIFICATION = 104, 
    NON
};

map<int, vector<HWND>> Menus;

int commSide = NON;

WNDCLASS wc = { };
WNDCLASS wcClient = { };
WNDCLASS wcServer = { };

MSG msg{};
MSG msgClient{};
MSG msgServer{};

HWND client_hwnd, server_hwnd;

// Function that receive data
// from client
DWORD WINAPI serverReceive(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    char buffer[DATA_BUFSIZE] = { 0 };

    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;

    // Server executes continuously
    while (true) {

        // If received buffer gives
        // error then return -1
        if (recv(client, buffer, DATA_BUFSIZE, 0)
            == SOCKET_ERROR) {
            cout << "recv function failed with error "
                << WSAGetLastError() << endl;
            return -1;
        }

        // If Client exits
        if (strcmp(buffer, "exit") == 0) {
            cout << "Client Disconnected."
                << endl;
            break;
        }

        // Print the message
        // given by client that
        // was stored in buffer
        cout << "Client: " << buffer << endl;
        //MessageBox(NULL, LPWSTR(buffer), NULL, MB_ICONERROR);

        // Clear buffer message
        memset(buffer, 0, DATA_BUFSIZE);
    }
    return 1;
}

// Function that sends data to client
DWORD WINAPI serverSend(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    char buffer[DATA_BUFSIZE] = { 0 };

    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;

    // Server executes continuously
    while (true) {

        // Input message server
        // wants to send to client
        gets_s(buffer);
        wcstombs(buffer, portSaved, wcslen(portSaved));

        // If sending failed
        // return -1
        if (send(client, buffer, DATA_BUFSIZE, 0) == SOCKET_ERROR) {
            cout << "send failed with error "
                << WSAGetLastError() << endl;
            return -1;
        }

        // If server exit
        if (strcmp(buffer, "exit") == 0) {
            cout << "Thank you for using the application"
                << endl;
            break;
        }
    }
    return 1;
}

// Function that receive data from server
DWORD WINAPI clientReceive(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    char buffer[DATA_BUFSIZE] = { 0 };

    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;

    // Client executes continuously
    while (true) {

        // If received buffer gives
        // error then return -1
        if (recv(server, buffer,
            DATA_BUFSIZE, 0)
            == SOCKET_ERROR) {
            cout << "recv function failed with error: "
                << WSAGetLastError()
                << endl;
            return -1;
        }

        // If Server exits
        if (strcmp(buffer, "exit") == 0) {
            cout << "Server disconnected."
                << endl;
            return 1;
        }

        // Print the message
        // given by server that
        // was stored in buffer
        cout << "Server: " << buffer << endl;

        // Clear buffer message
        memset(buffer, 0, DATA_BUFSIZE);
    }
    return 1;
}

// Function that sends data to server
DWORD WINAPI clientSend(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    char buffer[DATA_BUFSIZE] = { 0 };

    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;

    // Client executes continuously
    while (true) {

        // Input message client
        // wants to send to server
        gets_s(buffer);

        // If sending failed
        // return -1
        if (send(server,
            buffer,
            DATA_BUFSIZE, 0)
            == SOCKET_ERROR) {
            cout << "send failed with error: "
                << WSAGetLastError() << endl;
            return -1;
        }

        // If client exit
        if (strcmp(buffer, "exit")
            == 0) {
            cout << "Thank you for using the application"
                << endl;
            break;
        }
    }
    return 1;
}

void createAllUI(HWND hwnd) {
    
    vector<HWND> main_menu, join_menu, host_menu, server_menu, client_menu;

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

    client_menu.push_back(CreateUI::CreateTextBox(L"Chat", X, Y + 30, 200, 25, hwnd));
    chatText = CreateUI::CreateInputBox(L"", X, Y + 60, 300, 600, hwnd);
    client_menu.push_back(chatText);
    Menus[CLIENT_MENU] = client_menu;

    server_menu.push_back(CreateUI::CreateTextBox(L"Chat", X, Y + 30, 200, 25, hwnd));
    chatText = CreateUI::CreateInputBox(L"", X, Y + 60, 300, 600, hwnd);
    server_menu.push_back(chatText);
    Menus[SERVER_MENU] = server_menu;
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

    // Creating TCP Socket and UDP socket
    sUDP = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    sTCP = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sTCP == INVALID_SOCKET || sUDP == INVALID_SOCKET) {
        MessageBox(NULL, TEXT("Invalid Socket"), NULL, MB_ICONERROR);
        return 1;
    }

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };
    wc.hInstance = currentInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpfnWndProc = StartWindowProcessMessages;

    RegisterClass(&wc);

    // Create the start window.
    HWND start_hwnd = CreateWindowEx(
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
        return 0;
    }

    ShowWindow(start_hwnd, cmdShow);
    UpdateWindow(start_hwnd);

    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (commSide != NON) break;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseWindow(start_hwnd);

    switch (commSide) {
    case SERVER_MENU:
        // Register the window class.
        wcServer.hInstance = currentInstance;
        wcServer.lpszClassName = SERVER_CLASS_NAME;
        wcServer.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcServer.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wcServer.lpfnWndProc = ServerWindowProcessMessages;

        RegisterClass(&wcServer);

        // Create the start window.
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
            return 0;
        }
        WSAAsyncSelect(sAccept, server_hwnd, HOST_MSG_NOTIFICATION, (FD_ACCEPT | FD_WRITE | FD_CONNECT | FD_READ | FD_CLOSE));
        ShowWindow(server_hwnd, cmdShow);
        UpdateWindow(server_hwnd);

        while (GetMessage(&msgServer, nullptr, 0, 0)) {
            TranslateMessage(&msgServer);
            DispatchMessage(&msgServer);
        }

        CloseWindow(server_hwnd);
        break;

    case CLIENT_MENU:
        // Register the window class.
        wcClient.hInstance = currentInstance;
        wcClient.lpszClassName = CLIENT_CLASS_NAME;
        wcClient.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcClient.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wcClient.lpfnWndProc = ClientWindowProcessMessages;

        RegisterClass(&wcClient);

        // Create the start window.
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
            return 0;
        }
        WSAAsyncSelect(sTCP, client_hwnd, CLIENT_MSG_NOTIFICATION, (FD_ACCEPT | FD_WRITE | FD_READ | FD_CLOSE));
        ShowWindow(client_hwnd, cmdShow);
        UpdateWindow(client_hwnd);

        while (GetMessage(&msgClient, nullptr, 0, 0)) {
            TranslateMessage(&msgClient);
            DispatchMessage(&msgClient);
        }

        CloseWindow(client_hwnd);
        break;
    }


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
            gwstatPass= GetWindowText(joinPassText, passSaved, 20);

            MessageBox(hwnd, ipSaved, portSaved, MB_OK);

            port = _wtoi(portSaved);
            
            ip = new char[20];
            memset(ip, 0, 20);

            wcstombs(ip, ipSaved, wcslen(ipSaved));

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            inet_pton(AF_INET, ip, &addr.sin_addr);

            WSAAsyncSelect(sTCP, hwnd, CLIENT_MSG_NOTIFICATION, FD_CONNECT | FD_READ | FD_CLOSE);

            error = connect(sTCP, (sockaddr*)&addr, sizeof(addr));

            /*
            if (error == SOCKET_ERROR) {
                MessageBox(NULL, TEXT("failed to create a connection"), NULL, MB_ICONERROR);
                closesocket(sTCP);
                WSACleanup();
                //return 1;
            }
            */
            //commSide = CLIENT_MENU;
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

            bind(sTCP, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));

            if (listen(sTCP, 5) == SOCKET_ERROR)
            {
                printf("listen() failed with error %d\n", WSAGetLastError());
                MessageBox(NULL, TEXT("listen() failed with error"), NULL, MB_ICONERROR);
                //return 1;
            }
            else
            {
                MessageBox(NULL, TEXT("listen() is OK!"), NULL, MB_OK);
            }

            WSAAsyncSelect(sTCP, hwnd, HOST_MSG_NOTIFICATION, FD_ACCEPT | FD_CONNECT | FD_READ | FD_CLOSE);

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
        PostQuitMessage(0);
        return 0;

    case HOST_MSG_NOTIFICATION:         //Is a message being sent?
        switch (WSAGETSELECTEVENT(lparam))               //If so, which one is it?
        {
        case FD_ACCEPT:
            //Connection request was made
            sAccept = accept(sTCP, NULL, NULL);
            if (sAccept == INVALID_SOCKET) {
                wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
                closesocket(sTCP);
                WSACleanup();
                return 1;
            }
            else
            {
                wprintf(L"Client connected.\n");
                MessageBox(NULL, TEXT("Client connected"), NULL, MB_OK);
                WSAAsyncSelect(sAccept, hwnd, HOST_MSG_NOTIFICATION, FD_CONNECT | FD_READ | FD_CLOSE);
                //commSide = SERVER_MENU;
            }
            commSide = SERVER_MENU;
            break;

        case FD_READ:
            //Incoming data; get ready to receive
            break;

        case FD_CLOSE:
            //Lost the connection
            break;
        }
        break;

    case CLIENT_MSG_NOTIFICATION:         //Is a message being sent?
    {
        switch (WSAGETSELECTEVENT(lparam))               //If so, which one is it?
        {
        case FD_CONNECT:
            //Connection was made successfully
            MessageBox(NULL, TEXT("Client connected - The Client"), NULL, MB_OK);
            commSide = CLIENT_MENU;
            break;

        case FD_READ:
            //Incoming data; get ready to receive
            break;

        case FD_CLOSE:
            //Lost the connection
            break;
        }
    }
    break;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
}

LRESULT CALLBACK ClientWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
    case WM_CREATE:
        createAllUI(hwnd);
        invisible();
        visible(CLIENT_MENU);
        break;

    case CLIENT_MSG_NOTIFICATION:         //Is a message being sent?
        switch (WSAGETSELECTEVENT(lparam))               //If so, which one is it?
        {
        case FD_ACCEPT:
            //Connection request was made
            break;

        case FD_CONNECT:
            //Connection was made successfully
            break;

        case FD_WRITE:

            break;

        case FD_READ:
            //Incoming data; get ready to receive
            break;

        case FD_CLOSE:
            //Lost the connection
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
}

LRESULT CALLBACK ServerWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
    switch (msg) {
    case WM_CREATE:
        createAllUI(hwnd);
        invisible();
        visible(SERVER_MENU);
        break;

    case HOST_MSG_NOTIFICATION:         //Is a message being sent?
        switch (WSAGETSELECTEVENT(lparam))               //If so, which one is it?
        {
        case FD_ACCEPT:
            //Connection request was made
            break;

        case FD_CONNECT:
            //Connection was made successfully
            break;

        case FD_WRITE:

            break;

        case FD_READ:
            //Incoming data; get ready to receive
            buffer[DATA_BUFSIZE] = { 0 };

            // Created client socket
            if (recv(sAccept, buffer, DATA_BUFSIZE, 0)
                == SOCKET_ERROR) {
                cout << "recv function failed with error "
                    << WSAGetLastError() << endl;
                return -1;
            }


            break;

        case FD_CLOSE:
            //Lost the connection
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
}

#endif // MAIN