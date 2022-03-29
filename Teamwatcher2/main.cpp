#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "CreateUI.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <iostream>
#pragma comment(lib,"WS2_32")
#define X 20
#define Y 20

LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam);

using namespace std;

HWND textfield;
HWND ipText;
HWND portText;
wchar_t ipSaved[20];
wchar_t portSaved[20];

enum MenuNumbers {MAIN_MENU = 1, JOIN_MENU = 2, HOST_MENU = 3, CONNECTION = 4};

map<int, vector<HWND>> Menus;

void createAllUI(HWND hwnd) {
    vector<HWND> main_menu;
    
    // Main Menu
    main_menu.push_back(CreateUI::CreateTextBox(L"TEAM WATCHER", X, Y, 300, 25, hwnd));
    main_menu.push_back(CreateUI::CreateButton(L"JOIN", X, Y + 60, 65, 25, hwnd, JOIN_MENU));
    main_menu.push_back(CreateUI::CreateButton(L"HOST", X + 80, Y + 60, 65, 25, hwnd, HOST_MENU));
    Menus[MAIN_MENU] = main_menu;

    // Join Menu
    vector<HWND> join_menu;
    join_menu.push_back(CreateUI::CreateButton(L"Back", X, Y, 65, 25, hwnd, MAIN_MENU));
    join_menu.push_back(CreateUI::CreateTextBox(L"Enter Destination IP: ", X, Y + 30, 200, 25, hwnd));
    join_menu.push_back(CreateUI::CreateTextBox(L"Enter Destination PORT: ", X, Y + 60, 200, 25, hwnd));
    join_menu.push_back(CreateUI::CreateButton(L"Connect", X, Y + 90, 65, 25, hwnd, CONNECTION));
    ipText = CreateUI::CreateInputBox(L"", X + 180, Y + 30, 300, 20, hwnd);
    portText = CreateUI::CreateInputBox(L"", X + 180, Y + 60, 300, 20, hwnd);
    join_menu.push_back(ipText);
    join_menu.push_back(portText);
    Menus[JOIN_MENU] = join_menu;

    // Host Menu
    vector<HWND> host_menu;
    host_menu.push_back(CreateUI::CreateButton(L"Back", X, Y, 65, 25, hwnd, MAIN_MENU));
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

    // Creating TCP Socket and UDP socket
    SOCKET sUDP = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0);
    SOCKET sTCP = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (sTCP == INVALID_SOCKET || sUDP == INVALID_SOCKET) {
        printf("invalid socket\n");
        return 1;
    }

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };
    wc.hInstance = currentInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpfnWndProc = WindowProcessMessages;

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Sample",                      // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        wc.hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        MessageBox(NULL, TEXT("Could not create window"), NULL, MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, cmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return 0;

}

LRESULT CALLBACK WindowProcessMessages(HWND hwnd, UINT msg, WPARAM param, LPARAM lparam) {
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
            int gwstatIP = 0;
            int gwstatPort = 0;
            gwstatIP = GetWindowText(ipText, ipSaved, 20);
            gwstatPort = GetWindowText(portText, portSaved, 20);

            MessageBox(hwnd, ipSaved, portSaved, MB_OK);
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