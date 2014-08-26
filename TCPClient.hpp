#ifndef included_TCPClient
#define included_TCPClient
#pragma once

#include <string>

class TCPClient {
public:
	~TCPClient();
	explicit TCPClient( const std::string& IPAddress, const std::string& portNumber );

	void initializeAndRun();

protected:

	std::string		m_IPAddress;
	std::string		m_PortNumber;

private:

};


#endif