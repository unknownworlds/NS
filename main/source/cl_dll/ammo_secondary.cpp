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
// ammo_secondary.cpp
//
// implementation of CHudAmmoSecondary class
//

#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>

int CHudAmmoSecondary :: Init( void )
{
	gHUD.AddHudElem(this);
	m_HUD_ammoicon = 0;

	for ( int i = 0; i < MAX_SEC_AMMO_VALUES; i++ )
		m_iAmmoAmounts[i] = -1;  // -1 means don't draw this value

	Reset();

	return 1;
}

void CHudAmmoSecondary :: Reset( void )
{
	m_fFade = 0;
}

int CHudAmmoSecondary :: VidInit( void )
{
	return 1;
}

int CHudAmmoSecondary :: Draw(float flTime)
{
	if ( (gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
		return 1;

	// draw secondary ammo icons above normal ammo readout
	int a, x, y, r, g, b, AmmoWidth;
	gHUD.GetPrimaryHudColor(r, g, b);
	a = (int) max( MIN_ALPHA, m_fFade );
	if (m_fFade > 0)
		m_fFade -= (gHUD.m_flTimeDelta * 20);  // slowly lower alpha to fade out icons
	ScaleColors( r, g, b, a );

	AmmoWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	y = ScreenHeight() - (gHUD.m_iFontHeight*4);  // this is one font height higher than the weapon ammo values
	x = ScreenWidth() - AmmoWidth;

	if ( m_HUD_ammoicon )
	{
		// Draw the ammo icon
		x -= (gHUD.GetSpriteRect(m_HUD_ammoicon).right - gHUD.GetSpriteRect(m_HUD_ammoicon).left);
		y -= (gHUD.GetSpriteRect(m_HUD_ammoicon).top - gHUD.GetSpriteRect(m_HUD_ammoicon).bottom);

		SPR_Set( gHUD.GetSprite(m_HUD_ammoicon), r, g, b );
		SPR_DrawAdditive( 0, x, y, &gHUD.GetSpriteRect(m_HUD_ammoicon) );
	}
	else
	{  // move the cursor by the '0' char instead, since we don't have an icon to work with
		x -= AmmoWidth;
		y -= (gHUD.GetSpriteRect(gHUD.m_HUD_number_0).top - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).bottom);
	}

	// draw the ammo counts, in reverse order, from right to left
	for ( int i = MAX_SEC_AMMO_VALUES-1; i >= 0; i-- )
	{
		if ( m_iAmmoAmounts[i] < 0 )
			continue; // negative ammo amounts imply that they shouldn't be drawn

		// half a char gap between the ammo number and the previous pic
		x -= (AmmoWidth / 2);

		// draw the number, right-aligned
		x -= (gHUD.GetNumWidth( m_iAmmoAmounts[i], DHN_DRAWZERO ) * AmmoWidth);
		gHUD.DrawHudNumber( x, y, DHN_DRAWZERO, m_iAmmoAmounts[i], r, g, b );

		if ( i != 0 )
		{
			// draw the divider bar
			x -= (AmmoWidth / 2);
			FillRGBA(x, y, (AmmoWidth/10), gHUD.m_iFontHeight, r, g, b, a);
		}
	}

	return 1;
}
