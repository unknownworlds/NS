#include "ui/UIHud.h"
#include "textrep/TRFactory.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/hud.h"
#include "vgui_Scheme.h"
#include "vgui_Panel.h"
#include "mod/AvHClientVariables.h"
#include "vgui_App.h"

// for FindFirst and FindNext
#include <windows.h>
#pragma warning(push)
#pragma warning(disable: 311)
#include <fmoddyn.h>
#include <fmod_errors.h>
#pragma warning(pop)

using namespace vgui;

extern BitmapTGA* LoadTGA( const char* pImageName );

// Initialize hud from specified input file
UIHud::UIHud(const string& inFilename, UIFactory* inFactory) : CHud(), mFilename(inFilename), mManager(inFactory)
{

    this->mFMOD = NULL;

	// Set flag false so we load the first time
	this->mInitted = false;
	this->mSoundInitialized = false;
	this->mMusicEnabled = false;
	this->mMusicAllowed = false;
	this->mCurrentSongStream = NULL;
    this->mCurrentInternetStream = NULL;
	this->mBytesInCurrentSong = 0;
	this->mTimeSongEnded = -1;
	this->mSongIsPlaying = false;
	this->mRandomSecondsBetweenSongs = 0;
	this->mCurrentVolume = 0;
	this->mCurrentChannel = -1;
    this->mInternetStreamChannel = -1;

    // Init cursor variables
    this->mArrowBitmap = NULL;
    this->mArrowCursor = NULL;

	if(TRFactory::ReadDescriptions(mFilename, this->mDescriptionList))
	{
	}
	else
	{
		// TODO: Emit error
	}

    this->mSchemeManager = NULL;
}

UIHud::~UIHud(void)
{
	this->ShutdownMusic();
}

bool UIHud::GetIsTimeForSong(float inCurrentTime) const
{
	bool theTimeForNewSong = false;

	if(this->mMusicEnabled && this->mMusicAllowed && !this->mSongIsPlaying)
	{
		float theTimeElapsedSinceSongStopped = inCurrentTime - this->mTimeSongEnded;
		if((this->mTimeSongEnded == -1) || (theTimeElapsedSinceSongStopped > this->mRandomSecondsBetweenSongs))
		{
			theTimeForNewSong = true;
		}
	}

	return theTimeForNewSong;
}

UIManager& UIHud::GetManager(void)
{
	return this->mManager;
}

const UIManager& UIHud::GetManager(void) const
{
	return this->mManager;
}

string UIHud::GetTextHeader(void) const
{
    return "Default header\r\n";
}

void UIHud::Init(void)
{
	CHud::Init();
}

void UIHud::InitializeSound(void)
{

    char theFileName[2][_MAX_PATH];

    sprintf(theFileName[0], "%s/fmod.dll", getModDirectory());
    sprintf(theFileName[1], "fmod.dll");
    
    for (int i = 0; i < 2 && mFMOD == NULL; ++i)
    {

        mFMOD = FMOD_CreateInstance(theFileName[i]);
    
        // Check sound version
    
        if (mFMOD != NULL && mFMOD->FSOUND_GetVersion() == FMOD_VERSION)
	    {
		    // Init sound
		    if (mFMOD->FSOUND_Init(44100, 32, 0))
		    {
			    this->mSoundInitialized = true;
		    }
		    else
		    {
			    char theErrorMessage[512];
			    sprintf(theErrorMessage, "say %s", FMOD_ErrorString(mFMOD->FSOUND_GetError()));
			    //ClientCmd(theErrorMessage);
		    }
	    }
        else
        {
            FMOD_FreeInstance(mFMOD);
            mFMOD = NULL;
        }
    
    }

}


bool UIHud::InitializeScheme(const char* inName, Scheme* inScheme)
{
	bool theSuccess = false;

	// TODO: How to detect if invalid scheme handle?
	SchemeHandle_t hPrimaryScheme = this->mSchemeManager->getSchemeHandle( inName );
	{
		int r, g, b, a;

		// font
		inScheme->setFont( Scheme::sf_primary1, this->mSchemeManager->getFont(hPrimaryScheme) );

		// text color
		this->mSchemeManager->getFgColor( hPrimaryScheme, r, g, b, a );
		inScheme->setColor(Scheme::sc_primary1, r, g, b, a );		// sc_primary1 is non-transparent orange

		// background color (transparent black)
		this->mSchemeManager->getBgColor( hPrimaryScheme, r, g, b, a );
		inScheme->setColor(Scheme::sc_primary3, r, g, b, a );

		// armed foreground color
		this->mSchemeManager->getFgArmedColor( hPrimaryScheme, r, g, b, a );
		inScheme->setColor(Scheme::sc_secondary2, r, g, b, a );

		// armed background color
		this->mSchemeManager->getBgArmedColor( hPrimaryScheme, r, g, b, a );
		inScheme->setColor(Scheme::sc_primary2, r, g, b, a );

		//!! need to get this color from scheme file
		// used for orange borders around buttons
		this->mSchemeManager->getBorderColor( hPrimaryScheme, r, g, b, a );
		inScheme->setColor(Scheme::sc_secondary1, r, g, b, a );

		theSuccess = true;
	}
	return theSuccess;
}

void UIHud::LoadSchemes(void)
{
    // Create scheme manager, we have screen dimensions by now
    this->mSchemeManager = new CSchemeManager(ScreenWidth(), ScreenHeight());

	Scheme* pScheme = App::getInstance()->getScheme();
	this->InitializeScheme("PieMenuScheme", pScheme);
//	this->InitializeScheme("TechButtonScheme", pScheme);
//	this->InitializeScheme("ChainOfCommandScheme", pScheme);
//	this->InitializeScheme("TitleFont", pScheme);
//	this->InitializeScheme("Heading1", pScheme);
}

bool UIHud::PickRandomSong(string& outRelativeSongName) const
{
	WIN32_FIND_DATA		theFindData;
	HANDLE				theFileHandle;
	StringList			theSongList;
	bool				theFoundSong = false;
	size_t				theNumSongs;

	// Find random song in directory
	string thePath;
	if(strcmp(cl_musicdir->string, ""))
	{
		thePath = cl_musicdir->string;
	}
	else
	{
		thePath = string(getModDirectory()) + string("\\") + string(kMusicDirectory);
	}

	string theFileQualifier = thePath + string("\\*.mp3");
	theFileHandle = FindFirstFile(theFileQualifier.c_str(), &theFindData);
	if (theFileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			theSongList.push_back(thePath + string("\\") + string(theFindData.cFileName));
		} 
		while(FindNextFile(theFileHandle, &theFindData));
	
        FindClose(theFileHandle);
        theFileHandle = INVALID_HANDLE_VALUE;
    
    }

	// Pick a random song in the list
	theNumSongs = theSongList.size();
	if(theNumSongs > 0)
	{
		int theSongOffset = gEngfuncs.pfnRandomLong( 0, (long)theNumSongs -1 );
		outRelativeSongName = string(theSongList[theSongOffset]);
		theFoundSong = true;
	}
	
	return theFoundSong;
}

void UIHud::ClearStream(FSOUND_STREAM*& ioStream)
{
	// TODO: Check error codes to make sure we don't leak?
    if (mSoundInitialized)
    {
    	mFMOD->FSOUND_Stream_Stop(ioStream);
    	mFMOD->FSOUND_Stream_Close(ioStream);
    }
	ioStream = NULL;
}

bool UIHud::PlaySong(string& inSongName, int& ioVolume, bool inLooping, FSOUND_STREAM*& outStream, int& outChannel, int& outBytesInSong, float inTimeElapsed)
{
	bool theSuccess = false;

	// Replace '/' with '\\'
	std::replace(inSongName.begin(), inSongName.end(), '/', '\\');
	
	// Load song
	char theSongName[255];
	strcpy(theSongName, inSongName.c_str());
	
	// Open
	unsigned int thePlayMode = FSOUND_STEREO;//FSOUND_NORMAL;
	thePlayMode |= (inLooping ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);

	if(!this->mSoundInitialized)
	{
		this->InitializeSound();
	}

    if (mSoundInitialized)
    {
    
	    // If alt-tabbing away at exactly the wrong time, music initialization can fail, don't crash
	    //ASSERT(this->mSoundInitialized);

	    outStream = mFMOD->FSOUND_Stream_Open(theSongName, thePlayMode, /*FSOUND_NONBLOCKING*/ 0, 0);
	    if(outStream)
	    {
		    outChannel = mFMOD->FSOUND_Stream_Play(FSOUND_FREE, outStream);
		    if (outChannel >= 0)
		    {
			    // Set song progress
			    if(inTimeElapsed != -1)
			    {
				    int theOffsetInMS = inTimeElapsed*1000;
				    int theSongTimeInMS = mFMOD->FSOUND_Stream_GetLengthMs(outStream);

				    if((theOffsetInMS > theSongTimeInMS) && inLooping)
				    {
					    theOffsetInMS %= theSongTimeInMS;	
				    }
				    
				    mFMOD->FSOUND_Stream_SetTime(outStream, theOffsetInMS);
			    }
			    
			    // Set volume to half way.  TODO: This should use a real music volume variable
			    mFMOD->FSOUND_SetPan(outChannel, FSOUND_STEREOPAN);
			    
			    ioVolume = min(max(0, ioVolume), 255);
			    mFMOD->FSOUND_SetVolume(outChannel, ioVolume);

			    outBytesInSong = mFMOD->FSOUND_Stream_GetLength(outStream);

			    theSuccess = true;
		    }
		    else
		    {
		    }
	    }
	    else
	    {
		    // TODO: Print out this error message somewhere
		    char theErrorMessage[512];
		    sprintf(theErrorMessage, "say %s", FMOD_ErrorString(mFMOD->FSOUND_GetError()));
	    }
    
    }

	return theSuccess;
}

bool UIHud::PlayInternetStream(const string& inStreamName, string& outError)
{
    bool theSuccess = false;

    // Initialize sound system
    if(!this->mSoundInitialized)
    {
        this->InitializeSound();
    }

    // If initialized
    if(this->mSoundInitialized)
    {
        // If steam is currently playing, clear/stop it
            // Reset stream channel
		if(this->mInternetStreamChannel != -1)
			gHUD.StopInternetStream();

		// If stream channel is reset
		if(this->mInternetStreamChannel == -1)
        {
            // Set buffer size and properties
            mFMOD->FSOUND_Stream_SetBufferSize(100);
            mFMOD->FSOUND_Stream_Net_SetBufferProperties(64000, 60, 80);

            // Open stream for playing
            this->mCurrentInternetStream = mFMOD->FSOUND_Stream_Open(inStreamName.c_str(), FSOUND_NORMAL | FSOUND_NONBLOCKING, 0, 0);
            if(!this->mCurrentInternetStream)
            {
                outError = FMOD_ErrorString(mFMOD->FSOUND_GetError());
            }
            else
            {
                theSuccess = true;
            }
        }
    }

    return theSuccess;
}

void UIHud::StopInternetStream()
{
    this->ClearStream(this->mCurrentInternetStream);
    
    this->mInternetStreamChannel = -1;
}

bool UIHud::UpdateInternetStream(float inCurrentTime, string& outError)
{
    bool theSuccess = true;

    // If we're playing a stream
    if(this->mCurrentInternetStream)
    {
        if(this->mInternetStreamChannel < 0)
        {
            this->mInternetStreamChannel = mFMOD->FSOUND_Stream_PlayEx(FSOUND_FREE, this->mCurrentInternetStream, NULL, TRUE);
            mFMOD->FSOUND_SetPaused(this->mInternetStreamChannel, FALSE);
            
            //if (channel != -1)
            //{
            //    FSOUND_Stream_Net_SetMetadataCallback(this->mCurrentInternetStream, metacallback, 0);
           // }
        }

        // If we get an error, add it as a tooltip
        int theOpenState = mFMOD->FSOUND_Stream_GetOpenState(this->mCurrentInternetStream);
        if((theOpenState == -1) || (theOpenState == -3))
        {
            //printf("\nERROR: failed to open stream!\n");
            //printf("SERVER: %s\n", );
            //break;

            outError = mFMOD->FSOUND_Stream_Net_GetLastServerStatus();
            theSuccess = false;
			gHUD.StopStream();
        }
    
        int read_percent = 0, driver = 0, channel = -1, status = 0, bitrate;
        unsigned int flags;
        mFMOD->FSOUND_Stream_Net_GetStatus(this->mCurrentInternetStream, &status, &read_percent, &bitrate, &flags);
    }

    return theSuccess;
}

void UIHud::PlayRandomSong(void)
{
	// Pick random song first
	string theRelativeSongName;
	if(this->PickRandomSong(theRelativeSongName))
	{
		// Remove current song if any
		if(this->mCurrentSongStream)
		{
			this->StopMusic();
		}

		// Load song
//		char theSongName[255];
//		strcpy(theSongName, theRelativeSongName.c_str());
//		this->mCurrentSongStream = FSOUND_Stream_OpenMpeg(theSongName, FSOUND_NORMAL | FSOUND_LOOP_OFF);
//		if(this->mCurrentSongStream)
//		{
//			// Play default song
//			//FMUSIC_PlaySong(mod);
//			
//			this->mCurrentChannel = FSOUND_Stream_Play(FSOUND_FREE, this->mCurrentSongStream);
//			if (this->mCurrentChannel < 0)
//			{
//				//	MessageBox(hwnd, "Error.  Cannot start song", "Playing a song", MB_ICONHAND|MB_OK|MB_SYSTEMMODAL);
//			}
//			else
//			{
//				// Set volume to half way.  TODO: This should use a real music volume variable
//				FSOUND_SetPan(this->mCurrentChannel, FSOUND_STEREOPAN);
//
		int theVolume = (int)(cl_musicvolume->value);
		//				this->mBytesInCurrentSong = FSOUND_Stream_GetLength(this->mCurrentSongStream);
		
		if(this->PlaySong(theRelativeSongName, theVolume, false, this->mCurrentSongStream, this->mCurrentChannel, this->mBytesInCurrentSong))
		{
			this->mCurrentVolume = theVolume;
			int theDelay = (int)(cl_musicdelay->value);
			this->mRandomSecondsBetweenSongs = gEngfuncs.pfnRandomLong(0, theDelay);
			//this->mTimeSongEnded = -1.0f;
			this->mSongIsPlaying = true;
			
		}
//				this->mCurrentVolume = (int)(cl_musicvolume->value);
//				this->mCurrentVolume = min(max(0, this->mCurrentVolume), 255);
//				cl_musicvolume->value = this->mCurrentVolume;
//				FSOUND_SetVolume(this->mCurrentChannel, this->mCurrentVolume);
//
//				this->mBytesInCurrentSong = FSOUND_Stream_GetLength(this->mCurrentSongStream);
//				this->mRandomSecondsBetweenSongs = gEngfuncs.pfnRandomLong(5, 90);
//				this->mTimeSongEnded = 0.0f;
//				this->mSongIsPlaying = true;
//			}
//		}
//		else
//		{
//			// TODO: Print out this error message somewhere
//			char theErrorMessage[512];
//			sprintf(theErrorMessage, "say %s", FMOD_ErrorString(FSOUND_GetError()));
//		}
	}
}


void UIHud::PostUIInit(void)
{
}

int UIHud::Redraw(float flTime, int intermission)
{
	int rc = 0;
	
	rc = CHud::Redraw(flTime, intermission);
	
	// Now that the file has been parsed, pass it off to the UI manager to build everything
	// This has to happen after CHud::VidInit because the components depend on the screen resolution to
	// be sized properly
	if(!this->mInitted)
	{
        this->LoadSchemes();
		
		if(this->mManager.Initialize(this->mDescriptionList, this->mSchemeManager))
		{
            this->PostUIInit();
			
			this->mInitted = true;
		}
		else
		{
			// TODO: Emit error
        }
		
		// Initialize music the first time through
		this->InitializeSound();
    }

    string theErrorString;
    this->Update(flTime, theErrorString);

	return rc;
}

void UIHud::ResetGame()
{
	this->mTimeSongEnded = -1;
	this->mRandomSecondsBetweenSongs = 0;
}

void UIHud::SetMusicAllowed(bool inState)
{
	this->mMusicAllowed = inState;
}

void UIHud::SetUsingVGUI(bool inState)
{
	this->mManager.SetUsingVGUI(inState);
}

void UIHud::Shutdown(void)
{
	this->ShutdownMusic();
}

void UIHud::ShutdownMusic(void)
{
	if(this->mSoundInitialized)
	{
		this->StopMusic();

        this->StopInternetStream();

		mFMOD->FSOUND_Close();

        FMOD_FreeInstance(mFMOD);
        mFMOD = NULL;
        
        this->mSoundInitialized = false;

	}
}

void UIHud::StopMusic()
{
	if(this->mCurrentSongStream && this->mSoundInitialized)
	{
		this->ClearStream(this->mCurrentSongStream);
		this->mSongIsPlaying = false;
		//this->mMusicEnabled = false;
		//this->mMusicAllowed = false;
	}
}

void UIHud::Think(void)
{
	CHud::Think();
}

void UIHud::ToggleEditMode(void)
{
    // For now this also changes us into edit mode
    if(this->mManager.InMouseMode())
    {
        if(this->mManager.ToggleEditMode())
        {
            
            // Now save this bad-boy out!
            this->mManager.Save(this->mFilename, this->GetTextHeader());
        }
    }
}

void UIHud::ToggleMouse(void)
{
    // For now this also changes us into edit mode
    this->mManager.ToggleMouse();
}

bool UIHud::Update(float inCurrentTime, string& outError)
{
    this->mManager.Update(inCurrentTime);

	this->UpdateMusic(inCurrentTime);

    bool theSuccess = this->UpdateInternetStream(inCurrentTime, outError);

    return theSuccess;
}

int UIHud::UpdateClientData(client_data_t *cdata, float time)
{
	return CHud::UpdateClientData(cdata, time);
}

void UIHud::UpdateMusic(float inCurrentTime)
{
	bool theJustTurnedOnMusic = false;

	// When song is done, wait a time, then pick another song
	if(this->mSoundInitialized)
	{
		if(!this->mMusicEnabled && this->mMusicAllowed)
		{
			this->mMusicEnabled = true;
			theJustTurnedOnMusic = true;
		}
		else if(this->mMusicEnabled && !this->mMusicAllowed)
		{
			this->StopMusic();
		}

		if(this->mSongIsPlaying && ((int)(cl_musicvolume->value) != this->mCurrentVolume))
		{
			this->mCurrentVolume = (int)(cl_musicvolume->value);
			this->mCurrentVolume = min(max(0, this->mCurrentVolume), 255);
			cl_musicvolume->value = this->mCurrentVolume;
		 	mFMOD->FSOUND_SetVolume(this->mCurrentChannel, this->mCurrentVolume);
		}

		// If the current song is done and a random time has elapsed since OR
		// we just turned on music
		if(theJustTurnedOnMusic || this->GetIsTimeForSong(inCurrentTime))
		{
			// Play a random song
			this->PlayRandomSong();
		}

		if(this->mMusicEnabled && (this->mCurrentSongStream != NULL))
		{
			if((int)(mFMOD->FSOUND_Stream_GetPosition(this->mCurrentSongStream)) >= this->mBytesInCurrentSong)
			{
				this->mTimeSongEnded = inCurrentTime;
				this->mSongIsPlaying = false;
			}
		}
//		if(this->mTimeSongEnded != 0.0f && thi)
//		{
//			float theTimeElapsedSinceSongStopped = inCurrentTime - this->mTimeSongEnded;
//			if(theTimeElapsedSinceSongStopped > this->mRandomSecondsBetweenSongs)
//			{
//				this->PlayRandomSong();
//			}
//		}
	}
}

void UIHud::VidInit(void)
{
	CHud::VidInit();

	GetManager().VidInit();
}

FMOD_INSTANCE* UIHud::GetFMOD()
{
    return mFMOD;
}

