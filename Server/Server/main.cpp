#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <string>

#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int port = 2011;
constexpr int buffsize = 4096;

void TextEdit(std::string& text, const std::string& ip, const std::string& port)
{
	std::string fill = " [ " + ip + ":" + port + " ] ";

	int i = 0;
	int count_dot = 0;

	while (text[i] != '\0')
	{
		while (text[i] != '.' && text[i] != '\0')
			++i;

		if (text[i] == '\0')
		{
			text.insert(i, fill);
			break;
		}

		while (text[i + 1] != '\0' && text[i + 1] == '.')
			++i;

		text.insert(i + 1, fill);

		++count_dot;
		i += fill.size() + 1;
	}
}

int main()
{
	try
	{
		WSADATA wsa_data;

		int result;
		result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

		if (result)
		{
			std::string err = "Error wsa startup with error code: " + std::to_string(result) + "\n";
			throw std::exception(err.c_str());
		}

		SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (server_socket == INVALID_SOCKET)
		{
			std::string err = "Error creating socket: " + std::to_string(WSAGetLastError()) + "\n";

			WSACleanup();

			throw std::exception(err.c_str());
		}

		sockaddr_in server_info;
		server_info.sin_family = AF_INET;
		server_info.sin_port = htons(port);
		server_info.sin_addr.S_un.S_addr = INADDR_ANY;

		result = bind(server_socket, (LPSOCKADDR)&server_info, sizeof(server_info));

		if (result == SOCKET_ERROR)
		{
			std::string err = "Error binding: " + std::to_string(WSAGetLastError()) + '\n';

			WSACleanup();

			throw std::exception(err.c_str());
		}

		char host[256];
		char host_name[1024];

		if (!gethostname(host_name, 1024))
		{
			if (LPHOSTENT lphost = gethostbyname(host_name))
				strcpy_s(host, inet_ntoa(*((in_addr*)lphost->h_addr_list[0])));
		}

		std::cout << "====Server started===\n";
		std::cout << "IP: " << host << "\nport: " << htons(server_info.sin_port) << "\n\n";

		while (true)
		{
			result = listen(server_socket, 10);

			if (result == SOCKET_ERROR)
			{
				std::string err = "Error listening: " + std::to_string(WSAGetLastError()) + '\n';

				WSACleanup();

				throw std::exception(err.c_str());
			}

			SOCKET client_socket;
			sockaddr_in from;
			int sizeof_from = sizeof(from);

			client_socket = accept(server_socket, (sockaddr*)&from, &sizeof_from);

			if (client_socket == INVALID_SOCKET)
			{
				std::string err = "Error accepting: " + std::to_string(WSAGetLastError()) + '\n';

				WSACleanup();

				throw std::exception(err.c_str());
			}

			std::cout << "New connection from: " << inet_ntoa(from.sin_addr) << ", port: " << htons(from.sin_port) << '\n';

			char req[buffsize];

			result = recv(client_socket, req, buffsize, 0);

			if (result == SOCKET_ERROR || req[0] == -52)
			{
				std::string err = "Error getting request: " + std::to_string(WSAGetLastError()) + '\n';

				WSACleanup();

				throw std::exception(err.c_str());
			}
			
			std::cout << "Geted data: " << req << '\n';

			std::string resp(req);

			TextEdit(resp, host, std::to_string(port));

			result = send(client_socket, resp.c_str(), resp.size() + 1, 0);

			if (result == SOCKET_ERROR)
			{
				std::string err = "Error sending responce: " + std::to_string(WSAGetLastError()) + '\n';

				WSACleanup();

				throw std::exception(err.c_str());
			}

			closesocket(client_socket);

			std::cout << "Connection closed\n\n";
		}
	}
	catch (std::exception err)
	{
		std::cout << err.what() << '\n';
	}

	return 0;
}