#include <NexusServerInterface.h>
#include "AvHNexusServer.h"
#include "AvHNexusTunnelToClient.h"
#include "dlls/extdll.h"
#include "dlls/util.h"

AvHNexus::TunnelToClient::TunnelToClient(void) {}
AvHNexus::TunnelToClient::~TunnelToClient(void) {}
Nexus::TunnelToClient* AvHNexus::TunnelToClient::clone(void) const { return new TunnelToClient(); }

AvHNexus::TunnelToClient* AvHNexus::TunnelToClient::getInstance(void) 
{ 
	static std::auto_ptr<AvHNexus::TunnelToClient> ptr(new AvHNexus::TunnelToClient());
	return ptr.get();
}

bool AvHNexus::TunnelToClient::insertMessage(const Nexus::ClientID local_id, const byte_string& message)
{
	std::pair<const Nexus::ClientID,const byte_string> item(local_id,message);
	messages.push_back(item);
	return true;
}

const Nexus::ClientID AvHNexus::TunnelToClient::poll(void) const
{
	Nexus::ClientID result = 0;
	if( !messages.empty() )
	{ result = messages.front().first; }
	return result;
}

bool AvHNexus::TunnelToClient::recv(const Nexus::ClientID local_id, byte_string& data)
{
	std::pair<const Nexus::ClientID,const byte_string> item = messages.front();
	if( item.first != local_id )
	{ return false; }
	data.assign(item.second);
	messages.pop_front();
	return true;
}

bool AvHNexus::TunnelToClient::send(const Nexus::ClientID local_id, const byte_string& data)
{
	return AvHNexus::send( VARS(local_id), data.c_str(), data.length() );
}