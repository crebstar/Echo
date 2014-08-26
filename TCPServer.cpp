#include "TCPServer.hpp"

#include <stdio.h>
#include <iostream>

// String ops
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#define WIN32_LEAN_AND_MEAN
#include <windows.h>



TCPServer::~TCPServer() {

}


TCPServer::TCPServer( const std::string& ipAddress, const std::string& portNumber ) {

	m_IPAddress = ipAddress;
	m_PortNumber = portNumber;
}


void TCPServer::initializeAndRun() {

	printf( "\n\nAttempting to create TCP Server with IP: %s and Port: %s \n", m_IPAddress.c_str(), m_PortNumber.c_str() );

	WSAData wsaData;
	int winSockResult = 0;

	SOCKET listenSocket = INVALID_SOCKET;
	
	struct addrinfo* result = nullptr;
	struct addrinfo hints;
	
	winSockResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( winSockResult != 0 ) {

		printf( "WSAStartup failed with error number: %d\n", winSockResult );
		return;
	}

	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= IPPROTO_TCP;
	hints.ai_flags		= AI_PASSIVE;

	
	winSockResult = getaddrinfo( m_IPAddress.c_str(), m_PortNumber.c_str(), &hints, &result );
	if ( winSockResult != 0 ) {

		printf( "getaddrinfo function call failed with error number: %d\n", winSockResult );
		WSACleanup();

		return;
	}

	// Initialize the ListenSocket ( Connect, Bind, then listen )
	listenSocket = socket( result->ai_family, result->ai_socktype, result->ai_protocol );
	if ( listenSocket == INVALID_SOCKET ) {

		printf( "socket function call failed with error number: %ld\n", WSAGetLastError() );

		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	winSockResult = bind( listenSocket, result->ai_addr, static_cast<int>( result->ai_addrlen ) );

	if ( winSockResult == SOCKET_ERROR ) {

		printf( "Bind to listenSocket failed with error number: %d\n", WSAGetLastError() );

		freeaddrinfo( result );
		closesocket( listenSocket );
		WSACleanup();

		return;
	}

	freeaddrinfo(result);

	winSockResult = listen( listenSocket, SOMAXCONN );
	if ( winSockResult == SOCKET_ERROR ) {

		printf( "Listen failed with error number: %d\n", WSAGetLastError() );
		closesocket( listenSocket );
		WSACleanup();

		return;
	}

	// Initialize the ClientSocket ( Only supporting one client right now )
	SOCKET clientSocket = INVALID_SOCKET;
	
	clientSocket = accept( listenSocket, nullptr, nullptr ); // Blocking call...

	if ( clientSocket == INVALID_SOCKET ) {

		printf( "accept function call failed ( Cannot connect with client ) with error number: %d\n", WSAGetLastError() );

		closesocket( listenSocket );
		WSACleanup();
		return;
	}

	closesocket( listenSocket );

	// Start Receiving Packets From Client
	int winSockSendResult;
	char recvbuf[ 512 ];
	ZeroMemory( recvbuf, sizeof( recvbuf ) );
	int recvbuflen = 512;

	do {

		winSockResult = recv( clientSocket, recvbuf, recvbuflen, 0 );
	
		if ( winSockResult > 0 ) {

			std::string receivedString( recvbuf );
			receivedString.resize( recvbuflen );

			printf( "Num Bytes received: %d\n", winSockResult );
			printf( "Message Received: %s \n", receivedString.c_str() );

			// Send the buffer received back to the client (Echo functionality)
			winSockSendResult = send( clientSocket, receivedString.c_str(), receivedString.length(), 0 );

			if ( winSockSendResult == SOCKET_ERROR ) {

				printf( "send function call failed with error number: %d\n", WSAGetLastError() );
				closesocket( clientSocket );
				WSACleanup();

				return;
			}

			printf( "Num Bytes sent: %d\n", winSockSendResult );

		} else if ( winSockResult == 0 ) {

			printf( "The connection with client is shutting down\n" );

		} else {

			printf( "recv function call failed with error number: %d\n", WSAGetLastError() );

			closesocket( clientSocket );
			WSACleanup();
			return;
		}

		memset( recvbuf,'\0', 512 );

	} while ( winSockResult > 0 );

	// shutdown the connection since we're done
	winSockResult = shutdown( clientSocket, SD_SEND );

	if ( winSockResult == SOCKET_ERROR ) {

		printf("shutdown failed with error: %d\n", WSAGetLastError());

		closesocket( clientSocket );
		WSACleanup();

		return;
	}

	// cleanup
	closesocket( clientSocket );
	WSACleanup();

	printf( "TCP Server has finished executing\n\n" );
}