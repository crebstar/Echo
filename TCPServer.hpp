#ifndef included_TCPServer
#define included_TCPServer
#pragma once

#include <string>

class TCPServer {
public:
	~TCPServer();
	explicit TCPServer( const std::string& ipAddress, const std::string& portNumber );

	void initializeAndRun();

protected:

	std::string			m_IPAddress;
	std::string			m_PortNumber;

private:

};

#endif