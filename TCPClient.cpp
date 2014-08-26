#include "TCPClient.hpp"

#include <stdio.h>
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


TCPClient::~TCPClient() {

}


TCPClient::TCPClient( const std::string& IPAddress, const std::string& portNumber ) {

	m_IPAddress = IPAddress;
	m_PortNumber = portNumber;
}


void TCPClient::initializeAndRun() {

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
	hints.ai_socktype = SOCK_STREAM; // TCP ( UDP is DGRAM )
	hints.ai_protocol = IPPROTO_TCP; // TCP ( UDP is _UDP )

	wsResult = getaddrinfo( m_IPAddress.c_str(), m_PortNumber.c_str(), &hints, &result );

	if ( wsResult != 0 ) {

		printf("getaddrinfo failed with error: %d\n", wsResult );
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

		// Connect to server
		wsResult = connect( connectSocket, ptr->ai_addr, (int) ptr->ai_addrlen );
		if ( wsResult == SOCKET_ERROR ) {

			closesocket( connectSocket );
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;

	}

	freeaddrinfo( result );

	if ( connectSocket == INVALID_SOCKET ) {

		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	const int recBufferLength = 512;
	char recvbuf[ recBufferLength ];

	do {

		// Send an initial buffer
		std::cout << std::endl;
		std::cout << "Type message to send to server here ( Type '0' to exit ): ";

		std::string inputString;
		getline( std::cin, inputString );

		if ( inputString == "0" ) {

			break;

		} else {

			wsResult = send( connectSocket, inputString.c_str(), inputString.length(), 0 ); // For UDP use sendTo

			if (wsResult == SOCKET_ERROR) {

				printf( "send failed with error: %d\n", WSAGetLastError() );
				closesocket( connectSocket );
				WSACleanup();
			}
		}

		wsResult = recv( connectSocket, recvbuf, recBufferLength, 0 ); // For UDP use recvfrom

		if ( wsResult > 0 ) {

			printf( "Num Bytes received: %d\n", wsResult );
			printf( "Message Received From Server: %s", recvbuf );

		} else if ( wsResult == 0 ) {

			printf( "Connection closed\n" );

		} else {
			printf( "recv function call failed with error number: %d\n", WSAGetLastError() );
		}

		inputString.clear();

	} while ( wsResult > 0 );


	wsResult = shutdown( connectSocket, SD_SEND );
	if ( wsResult == SOCKET_ERROR ) {

		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket( connectSocket );
		WSACleanup();
		return;
	}

	printf( "TCP Client is concluding...\n\n" );
}

