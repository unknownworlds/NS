/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// Geiger.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "mod/AvHNetworkMessages.h"

DECLARE_MESSAGE(m_Geiger, Geiger )

int CHudGeiger::Init(void)
{
	HOOK_MESSAGE( Geiger );

	m_iGeigerRange = 0;
	m_iFlags = 0;

	//gHUD.AddHudElem(this);

	srand( (unsigned)time( NULL ) );

	return 1;
};

int CHudGeiger::VidInit(void)
{
	return 1;
};

int CHudGeiger::MsgFunc_Geiger(const char *pszName,  int iSize, void *pbuf)
{
	//update geiger data
	NetMsg_GeigerRange( pbuf, iSize, m_iGeigerRange );
	m_iGeigerRange <<= 2;
	m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudGeiger::Draw (float flTime)
{
	return 1;
}
