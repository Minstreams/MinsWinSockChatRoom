/* Common definition & functions for Chat room Server & Client*/
#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERVERPORT 5555
#define SIZE_BUF_SIZE 8
#define MAX_MSG_SIZE 2048

// message buffer
extern int msgSize;
extern char sizeBuffer[SIZE_BUF_SIZE];
extern char msgBuffer[MAX_MSG_SIZE];
extern string msgStr;

void ClearBuffer();
void sendPending(const string& message);
void sendTo(const SOCKET& sock);
void sendTo(const SOCKET& sock, const string& message);
int recvFrom(const SOCKET& sock);