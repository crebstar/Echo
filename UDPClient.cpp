#include "UDPClient.hpp"

#include <stdio.h>
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


UDPClient::~UDPClient() {

}


UDPClient::UDPClient( const std::string& ipAddress, const std::string& portNumber ) {

	m_IPAddress = ipAddress;
	m_PortNumber = portNumber;
}


void UDPClient::initializeAndRun() {

	printf( "Initializing UDP Client...\n" );

	WSAData wsaData;
	int wsResult = WSAStartup( MAKEWORD( 2,2 ), &wsaData ) ;

	if ( wsResult != 0 ) {

		printf("WSAStartup failed with error code: %d\n", wsResult );
		return;
	}

	SOCKET connectSocket = INVALID_SOCKET;
	addrinfo* result = nullptr;
	addrinfo hints;

	ZeroMemory( &hints, sizeof( hints ) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; 
	hints.ai_protocol = IPPROTO_UDP;

	wsResult = getaddrinfo( m_IPAddress.c_str(), m_PortNumber.c_str(), &hints, &result );

	if ( wsResult != 0 ) {

		printf( "getaddrinfo failed with error: %d\n", wsResult );
		WSACleanup();
		return;
	}

	addrinfo* ptr = nullptr;
	for ( ptr = result; ptr != nullptr; ptr = ptr->ai_next ) {

		connectSocket = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
		if ( connectSocket == INVALID_SOCKET ) {

			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		/*
		wsResult = connect( connectSocket, ptr->ai_addr, (int) ptr->ai_addrlen );
		if ( wsResult == SOCKET_ERROR ) {

			closesocket( connectSocket );
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
		*/
	}

	freeaddrinfo( result );

	if ( connectSocket == INVALID_SOCKET ) {

		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	//setup address structure
	sockaddr_in serverAddr;
	int serverAddrLength = sizeof( serverAddr );

	memset( (char *) &serverAddr, 0, serverAddrLength );
	serverAddr.sin_family = AF_INET;
	 
	serverAddr.sin_port = htons( ( unsigned short ) atoi( m_PortNumber.c_str() ) );
	serverAddr.sin_addr.S_un.S_addr = inet_addr( m_IPAddress.c_str() );

	const int recBufferLength = 512;
	char recvbuf[ recBufferLength ];

	do {

		// Send an initial buffer
		std::cout << std::endl;
		std::cout << "Type message to send to server here ( Type '0' to exit ): ";

		std::string inputString;
		getline( std::cin, inputString );
		std::cin.clear();

		if ( inputString == "0" ) {

			break;

		} else {

			wsResult = sendto( connectSocket, inputString.c_str(), inputString.length(), 0, (sockaddr*) &serverAddr, serverAddrLength ); // For UDP use sendTo

			if (wsResult == SOCKET_ERROR) {

				printf( "sendto failed with error: %d\n", WSAGetLastError() );
				closesocket( connectSocket );
				WSACleanup();
			}
		}

		int sizeOfSocketAddress = sizeof( *result );
		wsResult = recvfrom( connectSocket, recvbuf, recBufferLength, 0, (sockaddr*) &serverAddr, &serverAddrLength ); // For UDP use recvfrom

		if ( wsResult > 0 ) {

			printf( "Num Bytes received: %d\n", wsResult );
			printf( "Message Received From Server: %s", recvbuf );

		} else if ( wsResult == 0 ) {

			printf( "Connection closed\n" );

		} else {
			printf( "recvfrom function call failed with error number: %d\n", WSAGetLastError() );
		}

		inputString.clear();

	} while ( wsResult > 0 );

	closesocket( connectSocket );
	WSACleanup();

	printf( "UDP Client is concluding...\n\n" );
}