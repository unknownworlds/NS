#ifndef AVHNEXUSTUNNELTOCLIENT_H
#define AVHNEXUSTUNNELTOCLIENT_H

struct entvars_s;
typedef struct entvars_s entvars_t;

#include <queue>

namespace AvHNexus
{
	class TunnelToClient : public Nexus::TunnelToClient
	{
	public:
		static TunnelToClient* getInstance(void);
		virtual ~TunnelToClient(void);

		virtual Nexus::TunnelToClient* clone(void) const; //necessary so that we don't "slice" on copy
		virtual const Nexus::ClientID	poll(void) const;  //returns next local ClientID to recv or 0 if none available
		virtual bool			send(const Nexus::ClientID local_id, const byte_string& data);
		virtual bool			recv(const Nexus::ClientID local_id, byte_string& data);

		virtual bool insertMessage(const Nexus::ClientID local_id, const byte_string& message); //inserted into queue of messages from clients

	private:
		TunnelToClient(void);
		std::deque<std::pair<const Nexus::ClientID,const byte_string> > messages;
	};
}

#endif