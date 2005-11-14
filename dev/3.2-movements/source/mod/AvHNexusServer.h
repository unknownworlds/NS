#ifndef AVHNEXUSSERVER_H
#define AVHNEXUSSERVER_H

struct edict_s;
typedef struct edict_s edict_t;
struct entvars_s;
typedef struct entvars_s entvars_t;

namespace AvHNexus
{
	bool send(entvars_t* const pev, const unsigned char* data, const size_t length);
	bool recv(entvars_t* const pev, const char* data, const size_t length);

	void handleUnauthorizedJoinTeamAttempt(const edict_t* edict, const unsigned char team_index);
	string getNetworkID(const edict_t* edict);

	void performSpeedTest(void);
	void processResponses(void);

	void setGeneratePerformanceData(const edict_t* edict, const bool generate);
	bool getGeneratePerformanceData(void);

	bool isRecordingGame(void);
	void startGame(void);
	void cancelGame(void);
	void finishGame(void);

	void startup(void);
	void shutdown(void);
}

#endif