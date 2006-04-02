#include <NexusClientInterface.h>
#include "AvHNexusClient.h"
#include "AvHNexusTunnelToServer.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHNexus::TunnelToServer::TunnelToServer(void) {}
AvHNexus::TunnelToServer::~TunnelToServer(void) {}
Nexus::TunnelToServer* AvHNexus::TunnelToServer::clone(void) const { return new TunnelToServer(); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHNexus::TunnelToServer* AvHNexus::TunnelToServer::getInstance(void) 
{ 
	static std::auto_ptr<AvHNexus::TunnelToServer> ptr(new AvHNexus::TunnelToServer());
	return ptr.get();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHNexus::TunnelToServer::insertMessage(const byte_string& message)
{
	messages.push_back(message);
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHNexus::TunnelToServer::recv(byte_string& data)
{
	if( messages.empty() )
	{ return false; }
	data.assign(messages.front());
	messages.pop_front();
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AvHNexus::TunnelToServer::send(const byte_string& data)
{
	return AvHNexus::send( data.c_str(), data.length() );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
