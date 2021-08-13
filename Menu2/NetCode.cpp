/***************************************************
* AtmosFear 2.1
* NetCode.cpp
*
* Main Network Code
*
*/
#if !NETWORKCODE_DISABLED

#include <iostream>

void InitNetwork()
{
	std::cout << "Network: Dummy Init Ok!" << std::endl;
}


void ShutdownNetwork()
{
	std::cout << "Network: Dummy Shutdown Ok!" << std::endl;
}

#else 

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x701
#endif // _WIN32_WINNT

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Hunt.h"
#include <WinSock2.h>
#include <ws2tcpip.h>

#undef WIN32_LEAN_AND_MEAN

#include <iostream>
#include <sstream>


int iResult = 0;
WSADATA l_WSAData;
std::vector<SOCKET> l_Sockets;


void InitNetwork()
{
	if (WSAStartup(MAKEWORD(2, 2), &l_WSAData) != 0)
	{
		throw std::runtime_error("Failed to initialize WinSock 2.2!");
		//ToDo: Allow gameplay even if networking is disabled
	}

	// Do other initialisation

	std::cout << "Network: Winsock 2.2 Init Ok!" << std::endl;
}


void ShutdownNetwork()
{
	WSACleanup();
	std::cout << "Network: Shutdown Ok!" << std::endl;
}


void NetworkPost(LPSTR url, LPSTR host, LPSTR data)
{
	std::stringstream packet;
	sockaddr_in		SIn;
	SOCKET			Socket;
	struct hostent* Host;

	packet << "POST " << url << " HTTP/1.1\r\n";
	packet << "Host: " << host << "\r\n";
	packet << "Content-Type: text/html\r\n";
	packet << "Content-Length: " << strlen(data) << "\r\n";
	packet << "User-Agent: Mozilla/4.0\r\n";
	packet << "\r\n";
	packet << data << "\r\n";
	packet << "\r\n\r\n";

#ifdef _DEBUG
	std::cout << "---\n" << packet.str() << "---" << std::endl;
#endif

	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
	{
		throw std::runtime_error("Network: Invalid Socket!\r\nvoid NetworkPost();");
		return;
	}

	SIn.sin_family = AF_INET;
	SIn.sin_port = htons((WORD)80);

	Host = gethostbyname(host);
	if (Host == NULL)
	{
		throw std::runtime_error("Network: Invalid Host!\r\nvoid NetworkPost();");
		return;
	}

	SIn.sin_addr.s_addr = *((int*)*Host->h_addr_list);

	if (connect(Socket, (const struct sockaddr*) & SIn, sizeof(sockaddr_in)) == -1)
	{
		throw std::runtime_error("Network: Failed to connect!\r\nvoid NetworkPost();");
		return;
	}

	send(Socket, packet.str().c_str(), (int)packet.str().length(), 0);

	while (true)
	{
		char Buf;
		//memset(Buf, 0, 64);
		int len = recv(Socket, &Buf, 1, 0);

		// -- If closed gracefully (no error)
		if (len == 0) break;
		if (len < 0) { std::cout << "\nrecv error!" << std::endl; break; }
		if (len > 0) std::cout << Buf;
	}
	std::cout << std::endl;

	closesocket(Socket);
}

void NetworkGet(LPSTR url, LPSTR host, LPSTR data)
{
	std::stringstream packet;
	sockaddr_in		SIn;
	SOCKET			Socket;
	struct hostent* Host;

	packet << "GET " << url << " HTTP/1.1\r\n";
	packet << "Host: " << host << "\r\n";
	packet << "Content-Type: text/html\r\n";
	packet << "User-Agent: Mozilla/4.0\r\n";
	packet << "\r\n";
	packet << "\r\n";

#ifdef _DEBUG
	std::cout << "---\n" << packet.str() << "---" << std::endl;
#endif

	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == INVALID_SOCKET)
	{
		throw std::runtime_error("Network: Invalid Socket!\r\nvoid NetworkPost();");
		return;
	}

	SIn.sin_family = AF_INET;
	SIn.sin_port = htons((WORD)80);

	Host = gethostbyname(host);
	if (Host == NULL)
	{
		throw std::runtime_error("Network: Invalid Host!\r\nvoid NetworkPost();");
		return;
	}

	SIn.sin_addr.s_addr = *((int*)*Host->h_addr_list);

	if (connect(Socket, (const struct sockaddr*) & SIn, sizeof(sockaddr_in)) == -1)
	{
		throw std::runtime_error("Network: Failed to connect!\r\nvoid NetworkPost();");
		return;
	}

	send(Socket, packet.str().c_str(), (int)packet.str().length(), 0);

	while (true)
	{
		char Buf;
		//memset(Buf, 0, 64);
		int len = recv(Socket, &Buf, 1, 0);

		// -- If closed gracefully (no error)
		if (len == 0) break;
		if (len < 0) { std::cout << "\nrecv error!" << std::endl; break; }
		if (len > 0) std::cout << Buf;
	}

	std::cout << std::endl;

	closesocket(Socket);
}

void NetworkConnect(LPSTR ip)
{
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip, "25671", &hints, &result);
	if (iResult != 0) {
		std::stringstream ess;
		ess << "getaddrinfo failed: " << iResult;
		throw std::runtime_error(ess.str());
		return;
	}

	l_Sockets.push_back(INVALID_SOCKET);

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	l_Sockets[l_Sockets.size() - 1] = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (l_Sockets[l_Sockets.size() - 1] == INVALID_SOCKET) {
		freeaddrinfo(result);
		std::stringstream ess;
		ess << "Error at socket(): " << WSAGetLastError();
		throw std::runtime_error(ess.str());
		return;
	}

	// Connect to server.
	iResult = connect(l_Sockets[l_Sockets.size() - 1], ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(l_Sockets[l_Sockets.size() - 1]);
		l_Sockets[l_Sockets.size() - 1] = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (l_Sockets[l_Sockets.size() - 1] == INVALID_SOCKET)
	{
		throw std::runtime_error("Unable to connect to server!\n");
		return;
	}
}

#endif
