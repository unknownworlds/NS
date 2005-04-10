#ifndef AVHNEXUSTUNNELTOSERVER_H
#define AVHNEXUSTUNNELTOSERVER_H

#include <queue>

namespace AvHNexus
{
	class TunnelToServer : public Nexus::TunnelToServer
	{
	public:
		static TunnelToServer* getInstance(void);
		virtual ~TunnelToServer(void);

		virtual Nexus::TunnelToServer* clone(void) const;
		virtual bool send(const byte_string& data);
		virtual bool recv(byte_string& data);

		virtual bool insertMessage(const byte_string& message);	//inserted into queue of messages from server

	private:
		TunnelToServer(void);
		std::deque<const byte_string> messages;
	};
}

#endif