//
// Property of Charlie Cleveland, flayra@overmind.org
//

#ifndef UIHUD_H
#define UIHUD_H

//#include "cl_dll/hud.h"
#include "cl_dll/chud.h"
#include "textrep/TRDescription.h"
#include "ui/UIManager.h"
#include "vgui_InputSignal.h"
#include "vgui_Scheme.h"
#include "vgui_BitmapTGA.h"
#include "cl_dll/vgui_SchemeManager.h"

class UIFactory;
struct fmod_instance_struct;
typedef fmod_instance_struct FMOD_INSTANCE;
typedef struct FSOUND_STREAM FSOUND_STREAM;

// Initializes self from text file
class UIHud : public CHud
{
public:
    // Takes over memory of factory, deletes it when it is done
	UIHud(const string& inFilename, UIFactory* inFactory);
    virtual ~UIHud(void);

	// Accessors for returning the UI manager
	UIManager& GetManager(void);
	const UIManager& GetManager(void) const;

	// Override default behaviors
	virtual void	Init(void);

    virtual void	LoadSchemes(void);

    // The header that is saved out before saving out our ui.txt
    virtual string	GetTextHeader(void) const;

	bool			InitializeScheme(const char* inName, Scheme* inScheme);

	void			ClearStream(FSOUND_STREAM*& ioStream);
	
	bool			PlaySong(string& inSongName, int& ioVolume, bool inLooping, FSOUND_STREAM*& outStream, int& outChannel, int& outBytesInSong, float inTimeElapsed = -1);

	virtual void	PlayRandomSong(void);

    virtual bool    PlayInternetStream(const string& inStreamName, string& outError);

    virtual void    StopInternetStream();

    virtual void	PostUIInit(void);

	virtual void	ResetGame();

	virtual void	VidInit(void);

	virtual void	Think(void);

	virtual int		Redraw(float flTime, int intermission);

    virtual void	ToggleEditMode(void);

    virtual void	ToggleMouse(void);

	virtual int		UpdateClientData(client_data_t *cdata, float time);

	virtual void	SetMusicAllowed(bool inState);

	virtual void	SetUsingVGUI(bool inState);
					
	virtual void	InitializeSound(void);
					
	virtual void	Shutdown(void);
	
	virtual void	StopMusic(void);

	virtual void	UpdateMusic(float inCurrentTime);
    
    virtual bool    UpdateInternetStream(float inCurrentTime, string& outError);

    FMOD_INSTANCE*  GetFMOD();

protected:
    virtual bool    Update(float inCurrentTime, string& outError);

private:
	bool			GetIsTimeForSong(float inCurrentTime) const;

	bool			PickRandomSong(string& outRelativeSongName) const;

	virtual void	ShutdownMusic(void);

    const string		mFilename;
    static const string UITextHeader;
	bool				mInitted;

	bool				mSoundInitialized;
	bool				mMusicEnabled;
	bool				mMusicAllowed;
	bool				mSongIsPlaying;
	int					mBytesInCurrentSong;
	float				mTimeSongEnded;
	int					mRandomSecondsBetweenSongs;
	int					mCurrentVolume;
	int					mCurrentChannel;
    int                 mInternetStreamChannel;

	TRDescriptionList	mDescriptionList;

	UIManager			mManager;

    BitmapTGA*			mArrowBitmap;
    Cursor*				mArrowCursor;

	FSOUND_STREAM*		mCurrentSongStream;
    FSOUND_STREAM*		mCurrentInternetStream;

protected:
	CSchemeManager*		mSchemeManager;

    FMOD_INSTANCE*      mFMOD;

};

#endif