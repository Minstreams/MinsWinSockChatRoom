#include "ChatRoom.h"
int msgSize;
char sizeBuffer[SIZE_BUF_SIZE];
char msgBuffer[MAX_MSG_SIZE];
string msgStr;

void ClearBuffer() {
	memset(sizeBuffer, 0, SIZE_BUF_SIZE);
	memset(msgBuffer, 0, MAX_MSG_SIZE);
}

void sendPending(const string& message) {
	ClearBuffer();

	msgSize = message.size() + 1;
	string sizeStr = to_string(msgSize);
	memcpy(sizeBuffer, sizeStr.c_str(), sizeStr.length());
	memcpy(msgBuffer, message.c_str(), msgSize);
}

void sendTo(const SOCKET& sock) {
	send(sock, sizeBuffer, SIZE_BUF_SIZE, 0);
	send(sock, msgBuffer, msgSize, 0);
}

void sendTo(const SOCKET& sock, const string& message) {
	sendPending(message);
	sendTo(sock);
}

int recvFrom(const SOCKET& sock) {
	ClearBuffer();

	int count = recv(sock, sizeBuffer, SIZE_BUF_SIZE, 0);
	if (count <= 0) {
		return count;
	}
	msgSize = atoi(sizeBuffer);
	count = recv(sock, msgBuffer, msgSize, 0);
	msgStr = string(msgBuffer);
	return count;
}