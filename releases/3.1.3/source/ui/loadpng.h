//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef LOADPNG_H
#define LOADPNG_H

#include "bitmappng.h"
#include "vgui_inputstream.h"

BitmapPNG* LoadPNG(char const *pFilename);
BitmapPNG* LoadPNG(uchar* data, int length);
BitmapPNG* LoadPNG(vgui::InputStream& stream);

#endif // VGUI_LOADTGA_H
