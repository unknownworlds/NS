#ifndef GAMMATABLE_H
#define GAMMATABLE_H

#include "types.h"

#ifdef USE_DIRECTX_8
#include "D3d8.h"
#undef SERVER_EXECUTE
#endif

const uint32 kGammaTableSize = 2*256*3;

class GammaTable
{
public:
					GammaTable();
	virtual			~GammaTable();

	float			GetGammaSlope() const;
	
	void			InitializeToFlat();

	bool			InitializeFromVideoState();

	bool			InitializeToVideoState();

	void			ProcessSlope(float inSlope);

private:
	#ifdef USE_DIRECTX_8
	IDirect3DDevice8*	GetDirect3DDevice();
	IDirect3DDevice8*	m3DDevice;
	D3DGAMMARAMP		mDirect3DGammaRamp;
	#endif

	bool			mDirect3DMode;

	char			mBaseData[kGammaTableSize];
	char			mSlopeData[kGammaTableSize];

	float			mSlope;
};

#endif