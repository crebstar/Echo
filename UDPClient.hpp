#ifndef includedUDPClient
#define includedUDPClient
#pragma once

#include <string>

class UDPClient {
public:
	~UDPClient();
	explicit UDPClient( const std::string& ipAddress, const std::string& portNumber );

	void initializeAndRun();

protected:

	std::string				m_IPAddress;
	std::string				m_PortNumber;

private:


};


#endif