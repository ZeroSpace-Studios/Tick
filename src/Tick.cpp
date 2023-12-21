#include "Tick.h"

void TickReceiver::setup()
{
#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(0x0101, &wsaData)) {
		spdlog::error("Unable to start windows sockets.");
		return;
	}
#endif // _WIN32
	if (sock = socket(AF_INET, SOCK_DGRAM, 0) == INVALID_SOCKET)
	{
		spdlog::error("Could not create socket : %d", WSAGetLastError());
	};

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(10354);

	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		spdlog::error("Bind failed with error code : %d", WSAGetLastError());
		return;
	}

	isConnected = true;

}
