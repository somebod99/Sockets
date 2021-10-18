#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int buffsize = 4096;

sockaddr CreateSockAdd(std::string ip, int port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
	return *reinterpret_cast<sockaddr*>(&addr);
}

int main()
{
	try
	{
		WSADATA wsa_data;

		int result;
		result = WSAStartup(MAKEWORD(2, 2), (LPWSADATA)&wsa_data);

		if (result)
		{
			std::string err = "Error wsa startup with error code: " + std::to_string(result) + "\n";
			throw std::exception(err.c_str());
		}

		SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (client_socket == INVALID_SOCKET)
		{
			std::string err = "Error creating socket: " + std::to_string(WSAGetLastError()) + "\n";

			WSACleanup();

			throw std::exception(err.c_str());
		}

		std::string ip;
		int port;

		std::cout << "ip: ";
		std::cin >> ip;
		std::cout << "port: ";
		std::cin >> port;

		sockaddr server_info = CreateSockAdd(ip, port);

		result = connect(client_socket, &server_info, sizeof(server_info));

		if (result == SOCKET_ERROR)
		{
			std::string err = "Error connect: " + std::to_string(WSAGetLastError()) + "\n";

			WSACleanup();

			throw std::exception(err.c_str());
		}

		std::cout << "Connection made sucessfully\n";

		std::string req;
		std::cout << "Enter request: ";
		std::cin.ignore();
		std::getline(std::cin, req);

		result = send(client_socket, req.c_str(), req.size() + 1, 0);

		if (result == SOCKET_ERROR)
		{
			std::string err = "Error sending request: " + std::to_string(WSAGetLastError()) + "\n";
			
			WSACleanup();

			throw std::exception(err.c_str());
		}

		char resp[buffsize];

		result = recv(client_socket, resp, buffsize, 0);

		if (result > 0)
		{
			std::cout << "Responce: " << resp << '\n';
		}
		else if (result == 0)
		{
			std::cout << "Connection lost\n";
		}
		else
		{
			std::string err = "Error getting responce: " + std::to_string(WSAGetLastError()) + "\n";

			WSACleanup();

			throw std::exception(err.c_str());
		}

		closesocket(client_socket);
		WSACleanup();
	}
	catch (std::exception err)
	{
		std::cout << err.what() << '\n';
	}

	system("pause");
	return 0;
}