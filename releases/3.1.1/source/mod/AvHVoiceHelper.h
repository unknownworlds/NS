#ifndef AVH_VOICEHELPER_H
#define AVH_VOICEHELPER_H

#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "game_shared/voice_gamemgr.h"

class AvHVoiceHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool		CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker);
};


#endif