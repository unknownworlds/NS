#ifndef AVHNEXUSCLIENT_H
#define AVHNEXUSCLIENT_H

namespace AvHNexus
{
	bool send(const unsigned char* data, const size_t length);
	bool recv(const unsigned char* data, const size_t length);

	void startup(void);
	void shutdown(void);

	bool login(const string& name, const string& password);
	bool logout(void);
}

#endif