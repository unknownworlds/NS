#ifndef SPRITEPANEL_H
#define SPRITEPANEL_H

#include "vgui_Panel.h"
#include "ui/GammaAwareComponent.h"
#include "ui/ReloadableComponent.h"
#include "types.h"

class SpritePanel : public vgui::Panel, public ReloadableComponent, public GammaAwareComponent
{
public:
					SpritePanel(const string& inBaseSpriteName, const string& inRenderMode);

	virtual void	NotifyGammaChange(float inGammaSlope);
					
	virtual void	SetVAlignment(const string& inAlignment);

	virtual void	VidInit(void);

protected:
	int				GetNumSpritesAcross();
	int				GetNumSpritesDown();
	virtual void	paint();
	virtual void	paintBackground();

private:
	string			mBaseSpriteName;
	string			mRenderMode;
	string			mVAlignment;
	int				mSpriteHandle;
	float			mGammaSlope;

};

#endif
