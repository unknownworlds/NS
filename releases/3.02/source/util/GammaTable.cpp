#include "util/GammaTable.h"

// Include windows gamma functions
#include <windows.h>
#include "winuser.h"

GammaTable::GammaTable()
{
	this->mSlope = 1.0f;
	this->InitializeToFlat();
	this->mDirect3DMode = false;

	#ifdef USE_DIRECTX_8
	this->m3DDevice = NULL;
	memset(&this->mDirect3DGammaRamp, 0, sizeof(D3DGAMMARAMP));
	#endif
}

GammaTable::~GammaTable()
{
}

float GammaTable::GetGammaSlope() const
{
	return this->mSlope;
}

#ifdef USE_DIRECTX_8
IDirect3DDevice8* GammaTable::GetDirect3DDevice()
{
	if(!this->m3DDevice)
	{
		// Create DX8 object
		IDirect3D8* theIDX8 = Direct3DCreate8(D3D_SDK_VERSION);
		if(theIDX8)
		{
			//IUnknown::QueryInterface(IID_IDirectDrawGammaControl)
			
			HWND theHWND = (HWND)0x109823;

			// Create the device
			D3DPRESENT_PARAMETERS thePresentParameters;

			thePresentParameters.Windowed = TRUE;
			thePresentParameters.BackBufferWidth = thePresentParameters.BackBufferHeight = 0;
			thePresentParameters.hDeviceWindow = NULL;

			HRESULT theResult = theIDX8->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, theHWND, D3DCREATE_FPU_PRESERVE, &thePresentParameters, &this->m3DDevice);
			int a = 0;
			switch(theResult)
			{
			case D3DERR_INVALIDCALL: // The method call is invalid. For example, a method's parameter may have an invalid value. 
				a = 1;
				break;
			case D3DERR_NOTAVAILABLE: // This device does not support the queried technique. 
				a = 1;
				break;
			case D3DERR_OUTOFVIDEOMEMORY: // Direct3D does not have enough display memory to perform the operation. 
				a = 1;
				break;
			case D3D_OK:
				a = 1;
				break;
			}
		}
	}

	return this->m3DDevice;
}
#endif

void GammaTable::InitializeToFlat()
{
	this->mSlope = 1.0f;
	
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 256; i++)
		{
			int theBaseOffset = j*256 + i;
			uint8 theNewColor = (uint8)i;
			uint16 theNewWord = theNewColor << 8;
			
			((uint16*)this->mBaseData)[theBaseOffset] = theNewWord;
			((uint16*)this->mSlopeData)[theBaseOffset] = theNewWord;
		}
	}
}

bool GammaTable::InitializeFromVideoState()
{
	bool theSuccess = false;

	if(!this->mDirect3DMode)
	{
		HDC theDC = GetDC(NULL);
		if(theDC != 0)
		{
			// Read current settings in
			if(GetDeviceGammaRamp(theDC, this->mBaseData) == TRUE)
			{
				// Copy to base data also
				memcpy(this->mSlopeData, this->mBaseData, kGammaTableSize*sizeof(char));
		
				// This may not actually be one, but we treat one as the base state always
				this->mSlope = 1.0f;
		
				theSuccess = true;
			}
		
			if(!ReleaseDC(NULL, theDC))
			{
				// emit error about leak
			}
		}
	}
	
	// Try the DirectX way
	if(!theSuccess)
	{
		#ifdef USE_DIRECTX_8
		IDirect3DDevice8* the3DDevice = this->GetDirect3DDevice();
		if(the3DDevice)
		{
			// TODO: Copy data into base data and slope data
			the3DDevice->GetGammaRamp(&this->mDirect3DGammaRamp);
			ASSERT(kGammaTableSize == sizeof(D3DGAMMARAMP));
			memcpy(this->mBaseData, &this->mDirect3DGammaRamp, kGammaTableSize);
			memcpy(this->mSlopeData, &this->mDirect3DGammaRamp, kGammaTableSize);

			this->mDirect3DMode = true;
		}
		#endif
	}
	
	return theSuccess;
}

bool GammaTable::InitializeToVideoState()
{
	bool theSuccess = false;

	if(!this->mDirect3DMode)
	{
		HDC theDC = GetDC(NULL);
		if(theDC != 0)
		{
            // Multi-monitor support returns false for some reason, even though it seems to work
            #define SM_CMONITORS 80
            bool theIgnoreErrorCode = (GetSystemMetrics(SM_CMONITORS) > 1);
            bool theSetGammaRamp = (SetDeviceGammaRamp(theDC, this->mSlopeData) == TRUE);

			// Restore original gamma ramp
			if(theSetGammaRamp || theIgnoreErrorCode)
			{
				theSuccess = true;
			}

			if(!ReleaseDC(NULL, theDC))
			{
				// emit error about leak
			}
		}
	}

	#ifdef USE_DIRECTX_8
	if(this->mDirect3DMode)
	{
		IDirect3DDevice8* the3DDevice = this->GetDirect3DDevice();
		if(the3DDevice)
		{
			memcpy(&this->mDirect3DGammaRamp, this->mSlopeData, kGammaTableSize);
			the3DDevice->SetGammaRamp(D3DSGR_CALIBRATE, &this->mDirect3DGammaRamp);
		}
	}
	#endif
	
	return theSuccess;
}

void GammaTable::ProcessSlope(float inSlope)
{
	this->mSlope = inSlope;

	// Steepen and saturate, ala Q3
	uint8 kMaxValue = uint8(-1);
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 256; i++)
		{
			int theBaseOffset = j*256 + i;
			uint16 theWord = ((uint16*)this->mBaseData)[theBaseOffset];
			uint8 theBaseColor = (theWord & 0xFF00) >> 8;
			uint8 theNewColor = (uint8)min(theBaseColor*inSlope, (float)kMaxValue);
			uint16 theNewWord = theNewColor << 8;
			((uint16*)this->mSlopeData)[theBaseOffset] = theNewWord;
		}
	}
}
