//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: build.h $
// $Date: 2002/11/22 22:09:10 $
//
//-------------------------------------------------------------------------------
// $Log: build.h,v $
// Revision 1.17  2002/11/22 22:09:10  Flayra
// - Updated version #
//
// Revision 1.16  2002/11/22 21:07:55  Flayra
// - Removed dev build, use DEBUG instead
//
// Revision 1.15  2002/11/15 19:09:57  Flayra
// - Reworked network metering to be easily toggleable
//
// Revision 1.14  2002/11/12 02:20:25  Flayra
// - Updated version number
//
// Revision 1.13  2002/11/06 01:37:07  Flayra
// - Regular update
//
// Revision 1.12  2002/11/03 04:54:45  Flayra
// - Regular update
//
// Revision 1.11  2002/10/24 21:09:04  Flayra
// - No longer needed
//
// Revision 1.10  2002/10/20 16:33:31  Flayra
// - Regular update
//
// Revision 1.9  2002/10/16 20:48:18  Flayra
// - Regular update
//
// Revision 1.8  2002/10/03 19:07:12  Flayra
// - Profiling switch
// - Regular game version update
//
// Revision 1.7  2002/09/23 21:51:39  Flayra
// - Regular update
//
// Revision 1.6  2002/09/09 19:38:34  Flayra
// - Tried implementing DirectX version of gamma ramp support
//
// Revision 1.5  2002/08/16 02:21:37  Flayra
// - Re-enabled selection
//
// Revision 1.4  2002/08/09 00:14:59  Flayra
// - Removed selection prediction
//
// Revision 1.3  2002/05/23 02:41:53  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef BUILD_H
#define BUILD_H

// Build options
#define CLIENT_WEAPONS
#define AVH_PREDICT_SELECT

// Build in code to help playtest.  Choose neither, AVH_PLAYTEST_BUILD, or AVH_PLAYTEST_BUILD _and_ AVH_LAN_PLAYTEST_BUILD
#ifdef DEBUG
#define AVH_PLAYTEST_BUILD
//#define AVH_LAN_PLAYTEST_BUILD
#endif

// Build in code for security.  This forces the server to uplink and only allows authenticated players to join the server.
//#ifndef DEBUG
//#define AVH_SECURE_PRERELEASE_BUILD
//#endif

// When you're doing a final secure build of your client .dll and you're ready to
// send the binary to Valve to be processed, you need to change this to 1 and rebuild
//#define FINAL_VAC_BUILD

// Define this when debugging on a different machine in a "live" environment (ie, trying to catch crashes)
//#define AVH_EXTERNAL_BUILD

//#define USE_DIRECTX_8

// May not be working completely
//#define SKULK_VIEW_ROTATION

// Mapper build?
//#define AVH_MAPPER_BUILD

// Sends profiling info to log
//#define PROFILE_BUILD

// Use multiple weapons sounds for upgrades
//#define AVH_UPGRADE_SOUNDS

// Define to track and meter network messages, don't define to use basic HL networking
//#define USE_NETWORK_METERING

// Define to have server report server and player data to master server
//#define USE_UPP

#endif