//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "vgui.h"
#include "bitmappng.h"
#include <memory.h>

BitmapPNG::BitmapPNG(int width, int height, uchar* data)
{
	setSize(width,height); //double-check that this allocates the memory we need!
	memcpy(_rgba,data,width*height*4);
}

BitmapPNG::~BitmapPNG(void) {}


