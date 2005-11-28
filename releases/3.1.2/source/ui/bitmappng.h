//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef BITMAPPNG_H
#define BITMAPPNG_H

#include "vgui_bitmap.h"

class VGUIAPI BitmapPNG : public vgui::Bitmap
{
public:
	BitmapPNG(int width, int height, uchar* data);
	virtual ~BitmapPNG(void);
private:
};

#endif