#include "Tick.h"

TickReceiver::TickReceiver() {
	isConnected = false;
	peerID = rand();
	timeSyncThread = std::thread(&TickReceiver::receiveTime, this);
	timeSyncRunning = true;
	timeSyncThread.detach();
}

//Need to add bind ip address.
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
	//Need to setup socket timestamping
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		spdlog::error("Bind failed with error code : %d", WSAGetLastError());
		return;
	}

	isConnected = true;

}

Time TickReceiver::getTime() {
	std::lock_guard<std::mutex> lock(timeSyncMutex);
	return {
		incomingTime,
		timeSync.IsSynchronized()
	};
}


void TickReceiver::receiveTime() {
	int retry_count = 0;
	int count = 0;
	while (timeSyncRunning) {
		if (isConnected) {

			int recv_len;
			std::vector<char> buf;
			buf.resize(1024);
			sockaddr_in client;
			int addr_len = sizeof(sockaddr_in);
			recv_len = recvfrom(sock, &buf[0], 1024, 0, (sockaddr*)&client, &addr_len);
			if (recv_len == SOCKET_ERROR) {
				spdlog::error("recvfrom() failed with error code : %d", WSAGetLastError());
				retry_count++;
				if (retry_count > 10) {
					spdlog::error("Unable to receive time from TWS.");
					timeSyncRunning = false;
					return;
				}
			}

			if (recv_len > 0) {
				retry_count = 0;
				uint8_t isSync = 0;
				std::memcpy(&isSync, &buf[0], sizeof(uint8_t));
				if (isSync) {
					TimeSyncPacket p;
					std::memcpy(&p, &buf[0], sizeof(TimeSyncPacket));

					spdlog::debug("Received time sync packet: ID: {}, Time: {}", p.peerID, p.time);
					Counter24 c(p.time);
					timeSync.OnPeerMinDeltaTS24(c);
					continue;
				}
				TimePacket p;
				std::memcpy(&p, &buf[0], sizeof(TimePacket));
				setTime(p.time);
			}
		}
		count++;
	}
}

void TickReceiver::setTime(uint32_t time) {
	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
	uint64_t stime = timeSync.FromLocalTime23(us.count(), time);
	std::lock_guard<std::mutex> lock(timeSyncMutex);
	incomingTime = stime;
}
