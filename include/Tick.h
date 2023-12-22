#pragma once
#include <spdlog/spdlog.h>
#include <thread>
#include "TimeSync/TimeSync.h"

#ifdef _WIN32
#pragma comment(lib,"Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mstcpip.h>
#include <Windows.h>
#endif // _WIN32

#pragma pack(push, 1)
typedef struct TimePacket {
	uint8_t isPeerSync = false;
	uint32_t peerID;
	uint32_t time;
} TimePacket;
#pragma pack(pop)

#pragma(push, 1)
typedef struct TimeSyncPacket {
	uint8_t isPeerSync = true;
	uint32_t peerID;
	uint32_t time;
} TimeSyncPacket;
#pragma(pop)

typedef struct Time {
	uint64_t time;
	bool sync;
};

class Tick {
public:
	Tick();
	~Tick();

	// We need to setup UDP networking here
	// We need to setup a callback to send a time signal
	// We need to specify if we are going to be a server or a client
	// We need to be able to bind to multicast addresses
	// Needs to be microsend accurate

	virtual void setup(const std::string& ip, const int port) = 0;
	virtual void sendTime() = 0;
	virtual void receiveTime() = 0;


protected:
	uint64_t incomingTime = 0;
	bool isConnected = false;
	TimeSynchronizer timeSync;
	std::thread timeSyncThread;
	std::atomic_bool timeSyncRunning = false;
	std::mutex timeSyncMutex;
};

class TickReceiver : public Tick {

public:
	TickReceiver();
	~TickReceiver();

	//Need to add bind IP
	void setup() override;
	Time getTime();

private:
#ifdef _WIN32
	SOCKET sock;
	sockaddr_in addr;
#endif // _WIN32

	uint32_t peerID = 0;

	void receiveTime() override;
	void setTime(uint32_t time);
};

class TickSender : public Tick {
public:
	TickSender();
	~TickSender();

	void setup(const std::string& ip, const int port) override;
	void sendTime() override;

private:
#ifdef _WIN32
	SOCKET sock;
	sockaddr_in addr;
#endif // _WIN32
	uint32_t peerID = 0;

};