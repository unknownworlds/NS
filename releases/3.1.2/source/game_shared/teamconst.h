//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: teamconst.h $
// $Date: 2002/07/23 16:49:42 $
//
//-------------------------------------------------------------------------------
// $Log: teamconst.h,v $
// Revision 1.2  2002/07/23 16:49:42  Flayra
// - Added document header, new player constant
//
//===============================================================================
#ifndef TEAM_CONST_H
#define TEAM_CONST_H

#define MAX_PLAYERS			32//voogru: Why was this 64?!

#define MAX_PLAYERS_PER_TEAM	16

//#define MAX_TEAMS			64
#define MAX_TEAMS			6

#define MAX_TEAM_NAME		16

#define MAX_TEAMNAME_LENGTH	16

#define TEAMPLAY_TEAMLISTLENGTH		MAX_TEAMS*MAX_TEAMNAME_LENGTH

#endif