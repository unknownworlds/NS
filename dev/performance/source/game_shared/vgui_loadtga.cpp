//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "../cl_dll/wrect.h"
#include "../cl_dll/cl_dll.h"
#include "vgui.h"
#include "vgui_loadtga.h"
#include "vgui_inputstream.h"


#include "ui/MemoryInputStream.h"

vgui::BitmapTGA* vgui_LoadTGANoInvertAlpha(char const *pFilename)
{ return vgui_LoadTGA(pFilename,false); }

vgui::BitmapTGA* vgui_LoadTGA(char const *pFilename, bool bInvertAlpha)
{
	int nLength = 0;
	uchar* pData = gEngfuncs.COM_LoadFile((char*)pFilename, 5, &nLength);

	if( !pData )
		return NULL;

	MemoryInputStream stream(pData,nLength);
	vgui::BitmapTGA *pRet = new vgui::BitmapTGA(&stream, bInvertAlpha);
	gEngfuncs.COM_FreeFile(pData);

	return pRet;
}

