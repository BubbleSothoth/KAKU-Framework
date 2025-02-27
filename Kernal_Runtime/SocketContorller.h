#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <winsock.h>


class Listener {
private:
	const struct sockaddr* addr;
	WSADATA ws;
	SOCKET sck;
public:
	Listener(struct sockaddr* base_data) :
		addr(base_data)
	{
		WSAStartup(MAKEWORD(2, 2),&ws);
		this->sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		connect(this->sck, this->addr, sizeof(const struct sockaddr));
	}
	~Listener(void) {
		closesocket(this->sck);
	}
	void Send(char* data, size_t data_size) {
		send(this->sck, data, data_size, 0x00);
	}
};
class Sender {

};
class SocketContorller
{
	Listener listener;
	Sender sender;

};

void func() {

}