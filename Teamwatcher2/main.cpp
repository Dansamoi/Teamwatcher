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
#include "Screen.h"
#include <string>
#include <vector>
#include "InputSimulator.h"
#pragma comment(lib,"WS2_32")

LRESULT CALLBACK StartWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);
LRESULT CALLBACK ClientWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);
LRESULT CALLBACK ServerWindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);

using namespace std;

HANDLE serverSending, clientReceiving, serverReceiving, clientSending;

int threadExitFlag = 1;

RECT Rect;

std::vector<uint8_t> Pixels = vector<uint8_t>();
BYTE* bufferImage;

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

HWND client_hwnd, server_hwnd, hScreenWindow;

PAINTSTRUCT ps = { 0 };
HDC hDC;
RECT rc = { 0 };

int wError;

vector<INT> keysPressed;
vector<char*> messages;

HBITMAP screenshot = NULL;

int command;
float xPos, yPos;

void clean_exit() {
    closesocket(sUDP);
    closesocket(sTCP);
    closesocket(sAccept);
    WSACleanup();
    switch (commSide) {
    case CLIENT_MENU:
        TerminateThread(clientReceiving, 0);
        break;
    case HOST_MENU:
        TerminateThread(serverSending, 0);
        break;
    }
}

void sendall(SOCKET s, const char* pdata, int buflen) {
    int sent = 0;
    while (buflen > 0) {
        sent = send(s, pdata, buflen, 0);
        if (sent == -1 || sent == 0) break;
        pdata += sent;
        buflen -= sent;
    }
}

void sendallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN to) {
    int sent = 0;
    int sendLen = DATA_BUFSIZE * 10;
    int error;
    while (buflen > 0) {
        if (buflen < sendLen) 
            sendLen = buflen;
        sent = sendto(s, pdata, sendLen, 0, (sockaddr*)&to, sizeof(to));
        if (sent == -1) {
            error = WSAGetLastError();
            continue;
        }
        if (sent == 0) break;
        pdata += sent;
        buflen -= sent;
    }
}

void recvall(SOCKET s, const char* pdata, int buflen) {
    int recieved = 0;
    while (buflen > 0) {
        recieved = recv(s, (char*)pdata, buflen, 0);
        if (recieved == -1 || recieved == 0) break;
        pdata += recieved;
        buflen -= recieved;
    }
}

void recvallUDP(SOCKET s, const char* pdata, int buflen, SOCKADDR_IN from) {
    int sizeOfFrom = sizeof(from);
    int recieved = 0;
    int recvLen = DATA_BUFSIZE * 10;
    int packAmount = 1280 * 720 * 4 / (DATA_BUFSIZE * 10);
    int error;
    if (buflen < recvLen) {
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
    else 
    {
        for (int i = 0; i < packAmount; i++) {
            recieved = recvfrom(s, (char*)pdata, recvLen, 0, (sockaddr*)&from, &sizeOfFrom);
            if (recieved == -1) {
                error = WSAGetLastError();
                continue;
            }
            pdata += DATA_BUFSIZE*10;
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

void get_command(SOCKET s, char* command) {
    if (strcmp(command, "-1") == 0) return;
    char buffer[128];
    std::string result;
    FILE* pipe = _popen(command, "r"); // creating a pipe to the program
    if (!pipe) {
        return;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) { // reading from the pipe
        result += buffer;
    }
    _pclose(pipe);
    std::string length = std::string(6 - std::to_string(result.length()).length(), '0') + std::to_string(result.length());
    send(s, length.c_str(), length.length(), 0);
    sendall(s, result.c_str(), result.length());
}

void download(SOCKET s) {
    char path[1024];
    ZeroMemory(path, 1024);
    recv(s, path, 1024, 0); // receiving the path to the file
    if (strcmp(path, "-1") == 0) {
        return;
    }

    struct _stat64 stat;
    if (_stat64(path, &stat) != 0) { // checking if the file exists
        send(s, "-1", 2, 0);
        return;
    }
    __int64 filesize = stat.st_size; // getting the file size
    std::string filesize_str = std::to_string(filesize);
    std::string filesize_str_length = std::string(2 - std::to_string(filesize_str.length()).length(), '0') + std::to_string(filesize_str.length());
    send(s, filesize_str_length.c_str(), filesize_str_length.length(), 0);
    send(s, filesize_str.c_str(), filesize_str.length(), 0); // sending the file size

    char command[1024];
    ZeroMemory(command, 1024);
    recv(s, command, 1024, 0); // receiving and executing the hash command
    get_command(s, command);

    FILE* fp = fopen(path, "rb"); // trying to open the file
    if (fp == NULL) {
        send(s, "0", 1, 0);
        return;
    }
    send(s, "1", 1, 0);

    char data[4096];
    int read = 0;
    do { // loop to send the contents of the file
        ZeroMemory(data, 4096);
        read = fread(data, 1, 4096, fp);
        sendall(s, data, read);
    } while (read == 4096);

    fclose(fp); // closing the file handle
}

void upload(SOCKET s) {
    char filesize_str[32];
    ZeroMemory(filesize_str, 32);
    recv(s, filesize_str, 32, 0); // receiving the size of the file to be uploaded
    if (strcmp(filesize_str, "-1") == 0) {
        return;
    }

    int filesize;
    sscanf_s(filesize_str, "%d", &filesize); // converting it to an int

    char path[1024];
    ZeroMemory(path, 1024);
    recv(s, path, 1024, 0); // receiving the path to where the file will be uploaded

    FILE* fp = fopen(path, "wb"); // opening the destination file
    if (fp == NULL) {
        send(s, "0", 1, 0);
        return;
    }
    send(s, "1", 1, 0);

    int received = 0, wrote = 0, buflen;
    char data[4096];
    while (filesize - received > 0) { // loop to receive all the contents of the uploaded file
        ZeroMemory(data, 4096);
        buflen = __min(4096, filesize - received);
        received += recv(s, data, buflen, MSG_WAITALL);
        wrote += fwrite(data, 1, buflen, fp);
    }

    fclose(fp); // closing the file handle 

    char command[1024];
    ZeroMemory(command, 1024);
    recv(s, command, 1024, 0); // receiving and executing the hash command 
    get_command(s, command);
}


//void sendScreen(SOCKET s);
//void recieveScreen(SOCKET s);
//void sendKeyPress(SOCKET s);
void recieveKeyPress(SOCKET s) {
    int code = 0;
    int up_down = 0;

    recvall(s, (char*)&code, sizeof(int));
    recvall(s, (char*)&up_down, sizeof(int));

    InputSimulator::SimulateKeyInput(code, up_down);

}

//void sendMousePress(SOCKET s);
void recieveMousePress(SOCKET s) {
    int xPos = 0;
    int yPos = 0;
    int code_up_down = 0;


    recvall(s, (char*)&xPos, sizeof(int));
    recvall(s, (char*)&yPos, sizeof(int));
    recvall(s, (char*)&code_up_down, sizeof(int));

    InputSimulator::SimulateMouseInput(code_up_down, xPos * GetSystemMetrics(SM_CXSCREEN), yPos * GetSystemMetrics(SM_CYSCREEN));


}

// Function that receive data
// from client
DWORD WINAPI serverReceive(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    char buffer[sizeof(int)] = { 0 };
    int command;

    // Created client socket

    SOCKET client = *(SOCKET*)lpParam;

    // Server executes continuously
    while (true) {

        // If received buffer gives
        // error then return -1
        if (recv(client, buffer, sizeof(int), 0)
            == SOCKET_ERROR) {
            cout << "recv function failed with error "
                << WSAGetLastError() << endl;
            return -1;
        }

        // char to int
        sscanf_s(buffer, "%d", &command);

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
    //char buffer[1920 * 1080] = { 0 };

    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;
    //screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1920, 1080);
    //bufferImage = Screen::GetPixelsFromHBITMAP(screenshot);
    //int size = sizeof(bufferImage);
    //sendall(client, (char*)&size, sizeof(int));

    // Server executes continuously
    while (true) {
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

        // If server exit
        //if (strcmp(buffer, "exit") == 0) {
        //    cout << "Thank you for using the application"
        //        << endl;
        //    break;
        //}
    }
    return 1;
}

DWORD WINAPI serverSendUDP(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    //char buffer[1920 * 1080] = { 0 };

    // Created client socket
    SOCKET client = *(SOCKET*)lpParam;
    //screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1920, 1080);
    //bufferImage = Screen::GetPixelsFromHBITMAP(screenshot);
    //int size = sizeof(bufferImage);
    //sendall(client, (char*)&size, sizeof(int));

    // Server executes continuously
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
    while (true) {
        //clock_t next_cycle = clock();
        //double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
        //last_cycle = next_cycle;
        screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1280, 720);
        Screen::HBITMAPToPixels(screenshot, Pixels, 1280, 720, 32);
        //bufferImage = Screen::GetPixelsFromHBITMAP(screenshot);
        // Input message server
        // wants to send to client

        // If sending failed
        // return -1
        //int sz = Pixels.size();

        //sendallUDP(client, (char*)&sz, sizeof(sz), to);
        sendallUDP(client, (char*)&Pixels.front(), Pixels.size() * sizeof(Pixels.front()), to);
        //delete[] bufferImage;
        DeleteObject(screenshot);
        Pixels.clear();
        while (clock() - last_cycle < delay);
        last_cycle = clock();

        //waitKey(FRAME_INTERVAL);

        // If server exit
        //if (strcmp(buffer, "exit") == 0) {
        //    cout << "Thank you for using the application"
        //        << endl;
        //    break;
        //}
    }
    return 1;
}

// Function that receive data from server
DWORD WINAPI clientReceive(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    //char buffer[1920 * 1080] = { 0 };
    //BYTE bufferImage[1920 * 1080] = { 0 };

    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;
    int size = 0;
    // Client executes continuously
    while (true) {

        // If received buffer gives
        // error then return -1
        //DeleteObject(screenshot);
        //Pixels.clear();
        size = 0;
        recvall(server, (char*)&size, sizeof(size));
        if (size == 0) continue;
        Pixels.resize(size);
        //if (recv(server, (char*)&Pixels.front(), size*sizeof(Pixels.front()), 0) == SOCKET_ERROR) {
        //    cout << "recv function failed with error: "
        //        << WSAGetLastError()
        //        << endl;
        //    //return -1;
        //}
        recvall(server, (char*)&Pixels.front(), size * sizeof(Pixels.front()));
        if (!Pixels.empty())
            DeleteObject(screenshot);
            screenshot = Screen::HBITMAPFromPixels(Pixels, 1280, 720, 32);
            Pixels.clear();

        // If Server exits
        //if (strcmp(buffer, "exit") == 0) {
        //    cout << "Server disconnected."
        //        << endl;
        //    return 1;
       // }

        // Print the message
        // given by server that
        // was stored in buffer
        //cout << "Server: " << buffer << endl;

        // Clear buffer message
        //memset(buffer, 0, sizeof(buffer));
        //memset(bufferImage, 0, sizeof(bufferImage));
    }
    return 1;
}

DWORD WINAPI clientReceiveUDP(LPVOID lpParam)
{
    // Created buffer[] to
    // receive message
    //char buffer[1920 * 1080] = { 0 };
    //BYTE bufferImage[1920 * 1080] = { 0 };

    // Created server socket
    SOCKET server = *(SOCKET*)lpParam;
    int size = 0;
    recvallUDP(server, (char*)&size, sizeof(size), from);
    Pixels.resize(size);
    // Client executes continuously
    while (true) {

        // If received buffer gives
        // error then return -1
        //DeleteObject(screenshot);
        //Pixels.clear();
        //if (recv(server, (char*)&Pixels.front(), size*sizeof(Pixels.front()), 0) == SOCKET_ERROR) {
        //    cout << "recv function failed with error: "
        //        << WSAGetLastError()
        //        << endl;
        //    //return -1;
        //}
        recvallUDP(server, (char*)&Pixels.front(), size * sizeof(Pixels.front()), from);

        if (!Pixels.empty())
            DeleteObject(screenshot);

        screenshot = Screen::HBITMAPFromPixels(Pixels, 1280, 720, 32);

        // If Server exits
        //if (strcmp(buffer, "exit") == 0) {
        //    cout << "Server disconnected."
        //        << endl;
        //    return 1;
       // }

        // Print the message
        // given by server that
        // was stored in buffer
        //cout << "Server: " << buffer << endl;

        // Clear buffer message
        //memset(buffer, 0, sizeof(buffer));
        //memset(bufferImage, 0, sizeof(bufferImage));
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
        if (keysPressed.size() != 0)
        {
            sendall(server, (char*)&keysPressed[0], 4);

            keysPressed.erase(keysPressed.begin());

        }

        // If client exit
        if (strcmp(buffer, "exit") == 0) {
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

    hScreenWindow = CreateWindow(
        L"Static",
        NULL,
        WS_VISIBLE | WS_CHILD | SS_BITMAP | WS_BORDER,
        400, 60, 1280, 720,
        hwnd,
        NULL, NULL, NULL
    );
    client_menu.push_back(hScreenWindow);
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

    ShowWindow(start_hwnd, SW_HIDE);

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
        //WSAAsyncSelect(sAccept, server_hwnd, HOST_MSG_NOTIFICATION, (FD_ACCEPT | FD_WRITE | FD_CONNECT | FD_READ | FD_CLOSE));
        ShowWindow(server_hwnd, cmdShow);
        UpdateWindow(server_hwnd);

        InternetAddrUDP.sin_family = AF_INET;
        InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
        InternetAddrUDP.sin_port = 6000;

        bind(sUDP, (PSOCKADDR)&InternetAddrUDP, sizeof(InternetAddrUDP));

        to.sin_family = AF_INET;
        //to.sin_addr.s_addr = htonl(INADDR_ANY);
        to.sin_addr.s_addr = inet_addr("127.0.0.1");
        to.sin_port = 5000;

        serverSending = CreateThread(NULL,
            0,
            serverSendUDP,
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

        //WSAAsyncSelect(sTCP, client_hwnd, CLIENT_MSG_NOTIFICATION, (FD_ACCEPT | FD_WRITE | FD_READ | FD_CLOSE));
        ShowWindow(client_hwnd, cmdShow);
        UpdateWindow(client_hwnd);

        InternetAddrUDP.sin_family = AF_INET;
        InternetAddrUDP.sin_addr.s_addr = htonl(INADDR_ANY);
        InternetAddrUDP.sin_port = 5000;

        bind(sUDP, (PSOCKADDR)&InternetAddrUDP, sizeof(InternetAddrUDP));

        clientReceiving = CreateThread(NULL,
            0,
            clientReceiveUDP,
            &sUDP,
            0,
            &tid);

        clientSending = CreateThread(NULL,
            0,
            clientSend,
            &sTCP,
            0,
            &tid);

        while (GetMessage(&msgServer, nullptr, 0, 0)) {
            TranslateMessage(&msgServer);
            DispatchMessage(&msgServer);
        }

        WaitForSingleObject(clientReceiving, INFINITE);
        WaitForSingleObject(clientSending, INFINITE);

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

            //WSAAsyncSelect(sTCP, hwnd, CLIENT_MSG_NOTIFICATION, FD_CONNECT | FD_READ | FD_CLOSE);

            error = connect(sTCP, (sockaddr*)&addr, sizeof(addr));
            lasterror = WSAGetLastError();
            if (error != SOCKET_ERROR)
                commSide = CLIENT_MENU;
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
            sAccept = accept(sTCP, NULL, NULL);
            //WSAAsyncSelect(sTCP, hwnd, HOST_MSG_NOTIFICATION, FD_ACCEPT | FD_CONNECT | FD_READ | FD_CLOSE);
            commSide = SERVER_MENU;
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
            clean_exit();
            break;
        }
        break;

    case CLIENT_MSG_NOTIFICATION:         //Is a message being sent?
    {
        wError = WSAGETSELECTERROR(lparam);
        switch (WSAGETSELECTEVENT(lparam))               //If so, which one is it?
        {
        case FD_CONNECT:
            //Connection was made successfully

            //lasterror = WSAGetLastError();

            if (error == SOCKET_ERROR && wError) {
                MessageBox(NULL, TEXT("failed to create a connection"), NULL, MB_ICONERROR);
                closesocket(sTCP);
                sTCP = INVALID_SOCKET;
                //WSACleanup();
                //return 1;
            }
            else {
                MessageBox(NULL, TEXT("Client connected - The Client"), NULL, MB_OK);
                commSide = CLIENT_MENU;
            }
            break;

        case FD_READ:
            //Incoming data; get ready to receive
            break;

        case FD_CLOSE:
            //Lost the connection
            clean_exit();
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
            clean_exit();
            PostQuitMessage(0);
            return 0;
            break;
        }
        break;

    case WM_LBUTTONDOWN:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetWindowRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        xPos /= Rect.right - Rect.left;
        yPos /= Rect.bottom - Rect.top;
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);

        keysPressed.push_back(MOUSEEVENTF_LEFTDOWN);
        break;

    case WM_LBUTTONUP:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetWindowRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        xPos /= Rect.right - Rect.left;
        yPos /= Rect.bottom - Rect.top;
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);

        keysPressed.push_back(MOUSEEVENTF_LEFTUP);
        break;

    case WM_MBUTTONDOWN:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetWindowRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        xPos /= Rect.right - Rect.left;
        yPos /= Rect.bottom - Rect.top;
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);

        keysPressed.push_back(MOUSEEVENTF_MIDDLEDOWN);
        break;

    case WM_MBUTTONUP:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetWindowRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        xPos /= Rect.right - Rect.left;
        yPos /= Rect.bottom - Rect.top;
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);

        keysPressed.push_back(MOUSEEVENTF_MIDDLEUP);
        break;

    case WM_RBUTTONDOWN:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        GetWindowRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        xPos /= Rect.right - Rect.left;
        yPos /= Rect.bottom - Rect.top;
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);

        keysPressed.push_back(MOUSEEVENTF_RIGHTDOWN);
        break;

    case WM_RBUTTONUP:
        keysPressed.push_back(MOUSE_PRESS);
        xPos = LOWORD(lparam);
        yPos = HIWORD(lparam);

        Rect;

        GetWindowRect(hwnd, &Rect);
        MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);

        xPos /= Rect.right - Rect.left;
        yPos /= Rect.bottom - Rect.top;
        keysPressed.push_back(xPos);
        keysPressed.push_back(yPos);

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
        //screenshot = Screen::ResizeImage(Screen::GetScreenShot(),1920,1080);
        //Screen::DrawBitmap(hDC, 0, 0, rc.right, rc.bottom, Screen::GetScreenShot(), SRCCOPY);

        //screenshot = Screen::ResizeImage(Screen::GetScreenShot(), 1920, 1080);
        //Screen::HBITMAPToPixels(screenshot, Pixels, 1920, 1080, 32);
        //bufferImage = Screen::GetPixelsFromHBITMAP(screenshot);
        //DeleteObject(screenshot);
        //screenshot = Screen::HBITMAPFromPixels(Pixels, 1920, 1080, 32);



        //Screen::CreateHBITMAPfromPixels(screenshot, bufferImage);
        //delete[] bufferImage;
        //SetBitmapBits(screenshot, sizeof(bufferImage), bufferImage);
        if (screenshot != NULL)
            Screen::DrawBitmap(hDC, 0, 0, rc.right, rc.bottom, screenshot, SRCCOPY);
        DeleteObject(screenshot);
        ReleaseDC(hwnd, hDC);
        break;

        //case WM_NCHITTEST:
        //    return HTCAPTION;

    case WM_DESTROY:
        clean_exit();
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
            //buffer[DATA_BUFSIZE] = { 0 };

            // Created client socket
            //if (recv(sAccept, buffer, DATA_BUFSIZE, 0)
            //    == SOCKET_ERROR) {
            //    cout << "recv function failed with error "
            //        << WSAGetLastError() << endl;
            //    return -1;
            //}

            break;

        case FD_CLOSE:
            //Lost the connection
            clean_exit();
            PostQuitMessage(0);
            return 0;
            break;
        }
        break;

    case WM_DESTROY:
        clean_exit();
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, msg, param, lparam);
    }
}

#endif // MAIN

