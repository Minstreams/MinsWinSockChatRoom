#include "ChatRoom.h"
#include <vector>
#include <list>
#include <deque>

#define HISTORY_LIFE 60*60*3	//three hour

vector<thread> clientThreads = vector<thread>();
list<SOCKET*> clientSockets = list<SOCKET*>();
bool shutdownFlag = false;

// Prototypes
void clientThread(SOCKET clientSocket, sockaddr_in clientAddr);
void sendToAll(const string& message);
void die(const char* message);

struct MessageRecord {
	time_t timestamp;
	string message;
};
deque<MessageRecord> messageHistory = deque<MessageRecord>();



int main() {
	cout << "Echo Server" << endl;

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

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	// FIXME: How to test for error here?

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);

	if (bind(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		die("bind failed");
	}
	cout << "Server socket bound to address " << inet_ntoa(serverAddr.sin_addr) << ", port " << ntohs(serverAddr.sin_port) << endl;

	if (listen(serverSocket, 1) != 0)
	{
		die("listen failed");
	}
	cout << "Server socket listening" << endl;

	while (!shutdownFlag)
	{
		cout << "Waiting for a connection..." << endl;

		sockaddr_in clientAddr;
		int addrSize = sizeof(clientAddr);
		clientThreads.push_back(thread(clientThread, accept(serverSocket, (sockaddr*)&clientAddr, &addrSize), clientAddr));

	}

	while (!clientThreads.empty())
	{
		clientThreads.back().join();
		clientThreads.pop_back();
	}
	closesocket(serverSocket);
	WSACleanup();
	system("pause");
	return 0;
}

void clientThread(SOCKET sock, sockaddr_in clientAddr) {
	if (sock == INVALID_SOCKET)return;

	if (recvFrom(sock) > 0) {
		clientSockets.push_back(&sock);
		string name = msgStr;
		cout << "¡¾" << name << "|" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "¡¿ come" << endl;

		if (!messageHistory.empty()) {
			time_t lifeLine = time(NULL) - HISTORY_LIFE;
			while (messageHistory.front().timestamp < lifeLine) {
				messageHistory.pop_front();
			}
			for (deque<MessageRecord>::iterator itor = messageHistory.begin(); itor != messageHistory.end(); ++itor) {
				sendTo(sock, itor->message);
			}
		}

		sendToAll("¡·Welcome " + name + "!¡¶");

		while (true) {
			int count = recvFrom(sock);
			if (count <= 0) {
				cerr << "¡¾" << name << "|" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "¡¿ leaves" << endl;
				break;
			}
			string fullMsg = "¡¾" + name + "¡¿:" + msgStr;
			sendToAll(fullMsg);

			if (shutdownFlag)break;
		}
		clientSockets.remove(&sock);
	}

	closesocket(sock);
}

void sendToAll(const string& message) {
	messageHistory.push_back({ time(NULL),message });
	sendPending(message);
	for (list<SOCKET*>::iterator itor = clientSockets.begin(); itor != clientSockets.end(); ++itor)
	{
		sendTo(**itor);
	}
	cout << "¡¾Boardcast¡¿" << message << endl;
}

// Print an error message and exit.
void die(const char* message) {
	// ¡ý Don't know if it's neccessary ¡ý
	WSACleanup();
	cerr << "Error: " << message << "(WSAGetLastError() = " << WSAGetLastError() << ")" << endl;
#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	system("pause");
	exit(1);
#endif
}
