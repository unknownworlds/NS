//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: UIUtil.h $
// $Date: 2002/05/23 04:03:05 $
//
//-------------------------------------------------------------------------------
// $Log: UIUtil.h,v $
// Revision 1.8  2002/05/23 04:03:05  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef UI_UTIL_H
#define UI_UTIL_H

#include "types.h"
#include "textrep/TRDescription.h"
#include "vgui_keycode.h"
#include "VGUI_Color.h"

namespace vgui
{
    class Color;
}

void UIDrawVariableBarSpriteHoles(int inSprite, int inX, int inY, float inPercentage, float inGammaSlope, bool inTrueHolesFalseAdditive = true, float inSecondaryPercentage = -1);
void UIGetPosition(const TRDescription& inDesc, float& outXPos, float& outYPos, int& outWidth, int& outHeight);
void UIStringToAlignment(const string& inString, int outAlignment);
void UIStringToColor(const string& inString, vgui::Color& outColor);
char UIKeyCodeToChar(vgui::KeyCode inKeyCode);
string UINameToSprite(const string& inName, int inScreenWidth, bool inNoRes = false);
vgui::Color UIBlendColor(const vgui::Color& inBaseColor, const vgui::Color& inDestColor, float inParametricPercentage);

void vguiSimpleLine(int x0, int y0, int x1, int y1, int r, int g, int b, int a);
void vguiSimpleBox(int x0, int y0, int x1, int y1, int r, int g, int b, int a);

#endif