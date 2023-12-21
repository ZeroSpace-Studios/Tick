#pragma once
#include <spdlog/spdlog.h>
#include <thread>
#include "TimeSync/TimeSync.h"

#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif // _WIN32

class Tick {
public:
	Tick();
	~Tick();

	// We need to setup UDP networking here
	// We need to setup a callback to send a time signal
	// We need to specify if we are going to be a server or a client
	// We need to be able to bind to multicast addresses
	// Needs to be microsend accurate

	virtual void setup() = 0;
	virtual void sendTime() = 0;
	virtual void receiveTime() = 0;


protected:
	uint64_t timeinFlight;
	bool isConnected;
	TimeSynchronizer timeSync;
};

class TickReceiver : public Tick {

public:
	TickReceiver();
	~TickReceiver();

	void setup() override;
	void sendTime() override;

private:
#ifdef _WIN32
	SOCKET sock;
	sockaddr_in addr;
#endif // _WIN32


};