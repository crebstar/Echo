#ifndef includedUDPServer
#define includedUDPServer
#pragma once

#include <string>

class UDPServer {
public:
	~UDPServer();
	explicit UDPServer( const std::string& ipAddress, const std::string& portNumber );

	void initializeAndRun();

protected:

	std::string			m_IPAddress;
	std::string			m_PortNumber;

private:


};


#endif