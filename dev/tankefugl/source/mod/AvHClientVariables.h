//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHClientVariables.h $
// $Date: 2002/08/02 22:02:02 $
//
//-------------------------------------------------------------------------------
// $Log: AvHClientVariables.h,v $
// Revision 1.6  2002/08/02 22:02:02  Flayra
// - Renamed centerid variable (because it tells info about everything, not just players)
//
// Revision 1.5  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_CLIENTVARIABLES_H
#define AVH_CLIENTVARIABLES_H

#include "common/cvardef.h"

extern cvar_t*	cl_cmhotkeys;
extern cvar_t*	cl_highdetail;
extern cvar_t*	cl_musicenabled;
extern cvar_t*	cl_musicdelay;
extern cvar_t*	cl_musicvolume;
extern cvar_t*	cl_particleinfo;
extern cvar_t*	cl_quickselecttime;
extern cvar_t*	cl_musicdir;

// Variables
#define kvAutoHelp			"cl_autohelp"
#define	kvCMHotKeys			"cl_cmhotkeys"
#define	kvForceDefaultFOV	"cl_forcedefaultfov"
#define kvCenterEntityID	"cl_centerentityid"
#define kvHighDetail		"cl_highdetail"
#define kvCMHotkeys			"cl_cmhotkeys"
#define kvMusicEnabled		"cl_musicenabled"
#define kvMusicDirectory	"cl_musicdirectory"
#define kvMusicDelay		"cl_musicdelay"
#define kvMusicVolume		"cl_musicvolume"
#define kvParticleInfo		"cl_particleinfo"
#define kvQuickSelectTime	"cl_quickselecttime"

#define kvDynamicLights		"cl_dynamiclights"
#define kvBuildMessages		"cl_buildmessages"

#endif
