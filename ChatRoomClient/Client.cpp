#include "../ChatRoomServer/ChatRoom.h"
#include <conio.h>

#define SERVERIP "111.229.94.88"

string name;

char inputBuffer[MAX_MSG_SIZE];
bool exitFlag = false;

// Prototypes
void recvMessage(const SOCKET& sock);
void die(const char* message);

int main() {
	cout << "╒==========================================╕" << endl;
	cout << "├----WELCOME to Minke's Chat Room Online---┤" << endl;
	cout << "╘==========================================╛" << endl;


	WSADATA w;
	int error = WSAStartup(0x0202, &w);
	if (error != 0)
	{
		die("WSAStartup failed");
	}
	if (w.wVersion != 0x0202)
	{
		die("Wrong WinSock version");
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	// FIXME: How to check for errors from socket?

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVERPORT);
	addr.sin_addr.s_addr = inet_addr(SERVERIP);

	cout << "Enter your name: ";
	getline(cin, name);
	cout << "┌──────────────────────────────────────────┐" << endl;
	cout << "｜Connecting to Server......               ｜" << endl;

	if (connect(sock, (const sockaddr*)&addr, sizeof addr) == SOCKET_ERROR)
	{
		die("connect failed");
	}
	cout << "｜Connected!                               ｜" << endl;
	cout << "｜Here are chat records within 3 hours↓   ｜" << endl;
	cout << "└──────────────────────────────────────────┘" << endl;

	sendTo(sock, name);

	thread recvMessageThread = thread(recvMessage, sock);

	while (true)
	{
		cout << "\r" << "【" + name + "】:";
		memset(inputBuffer, 0, MAX_MSG_SIZE);
		char ch;
		int index = 0;
		while ((ch = getch()) != '\r') {
			switch (ch)
			{
			case 8:	//Backspace
				if (index > 0) {
					index--;
					inputBuffer[index] = 0;
					cout << "\b \b";
				}
				break;
			case 27://invalid char
			case 32:
				break;
			default:
				if (index < MAX_MSG_SIZE - 1) {
					inputBuffer[index] = ch;
					index++;
					cout << ch;
				}
				break;
			}
		}

		string line = string(inputBuffer);

		if (exitFlag)break;
		if (line.empty())continue;

		sendTo(sock, line);
	}
	closesocket(sock);
	WSACleanup();

	return 0;
}

void recvMessage(const SOCKET& sock) {
	while (true) {
		int count = recvFrom(sock);
		if (count <= 0) {
			std::cerr << "Server closed connection" << std::endl;
			break;
		}
		cout << "\r\t\t\t\t\r" << msgStr << "                                      " << endl << "【" + name + "】:" << inputBuffer;
	}
	exitFlag = true;
}

// Print an error message and exit.
void die(const char* message) {
	cerr << "【Error: " << message << "(WSAGetLastError() = " << WSAGetLastError() << ")】" << endl;
	exit(1);
}