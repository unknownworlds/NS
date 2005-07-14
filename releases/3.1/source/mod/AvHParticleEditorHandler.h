#ifndef AVHPARTICLEEDITORHANDLER_H
#define AVHPARTICLEEDITORHANDLER_H

#include "types.h"
#include <VGUI_InputSignal.h>
#include <VGUI_IntChangeSignal.h>
#include <VGUI_Slider.h>
#include "mod/AvHParticleTemplate.h"

using namespace vgui;

class AvHParticleEditorHandler : public InputSignal
{
public:
					AvHParticleEditorHandler();

	static void		ToggleEdit();
	static uint32	GetEditIndex();
	static bool		GetInEditMode();
	static void		InitHandlers();
	static void		SetEditIndex(uint32 inIndex);
	static void		Setup();
	
	virtual void	cursorMoved(int x,int y,Panel* panel);
	virtual void	cursorEntered(Panel* panel) {}
	virtual void	cursorExited(Panel* panel) {}
	virtual void	mousePressed(MouseCode code,Panel* panel);
	virtual void	mouseDoublePressed(MouseCode code,Panel* panel) {}
	virtual void	mouseReleased(MouseCode code,Panel* panel);
	virtual void	mouseWheeled(int delta,Panel* panel);
	virtual void	keyPressed(KeyCode code,Panel* panel)  {}
	virtual void	keyTyped(KeyCode code,Panel* panel)  {}
	virtual void	keyReleased(KeyCode code,Panel* panel)  {}
	virtual void	keyFocusTicked(Panel* panel)  {}

private:
	static uint32	sEditIndex;
	static bool		sInEditMode;
};

class AvHSliderHandler : public IntChangeSignal
{
public:
					AvHSliderHandler(const string& inSliderName, const string& inLabelName);
				
	virtual void	Init();
	virtual void	intChanged(int value, Panel* panel);

	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue) = 0;
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate) = 0;

	virtual string	GetSliderDebugInfo() const;

	virtual string	GetTextFromValue(int inValue) = 0;

	virtual bool	GetValue(int& outValue);

	virtual void	RecomputeDependencies(AvHParticleTemplate* inReloadedTemplate) {}
	
	virtual void	SetText(int inValue);

	virtual bool	Setup();

	virtual void	SetValue(int inValue);
	
private:
	string			mSliderName;
	string			mLabelName;
};

class AvHSizeHandler : public AvHSliderHandler
{
public:
					AvHSizeHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	static const int	kScalar;
};

class AvHScaleHandler : public AvHSliderHandler
{
public:
					AvHScaleHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);

private:
	static const int	kScalar;
};

class AvHGenerationRateHandler : public AvHSliderHandler
{
public:
	AvHGenerationRateHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	static const int	kScalar;
};

class AvHParticleLifetimeHandler : public AvHSliderHandler
{
public:
	AvHParticleLifetimeHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	static const int	kScalar;
};

class AvHParticleSystemLifetimeHandler : public AvHSliderHandler
{
public:
	AvHParticleSystemLifetimeHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName), mEditable(false) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	static const int	kScalar;
	bool				mEditable;
};

class AvHMaxParticlesHandler : public AvHSliderHandler
{
public:
	AvHMaxParticlesHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	static const int	kScalar;
};

class AvHDrawModeHandler : public AvHSliderHandler
{
public:
	AvHDrawModeHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) {}
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	static const int	kScalar;
};

class AvHGenVelToggleHandler : public AvHSliderHandler
{
public:
	AvHGenVelToggleHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) { this->mGenVelMode = true; }
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	bool			GetGenVelMode() const;
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
	virtual void	RecomputeDependencies(AvHParticleTemplate* inReloadedTemplate);
private:
	bool				mGenVelMode;
	static const int	kScalar;
};

class AvHGenVelShapeHandler : public AvHSliderHandler
{
public:
	AvHGenVelShapeHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) { this->mCurrentShape = PS_Point; this->mUsingEntity = false; }
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	bool			GetUsingEntity() const;
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
	
private:
	ParticleShape		mCurrentShape;
	bool				mUsingEntity;
	static const int	kScalar;
};

class AvHGenVelParamNumHandler : public AvHSliderHandler
{
public:
	AvHGenVelParamNumHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) { this->mCurrentParamNum = 1; }
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	int				GetCurrentParamNum() const;
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
	virtual void	RecomputeDependencies(AvHParticleTemplate* inReloadedTemplate);
	
private:
	int					mCurrentParamNum;
	static const int	kScalar;
	
};

class AvHGenVelParamsHandler : public AvHSliderHandler
{
public:
	AvHGenVelParamsHandler(const string& inSliderName, const string& inLabelName) : AvHSliderHandler(inSliderName, inLabelName) { memset(&this->mCurrentParams, 0, sizeof(this->mCurrentParams)); }
	virtual void	ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue);
	virtual string	GetTextFromValue(int inValue);
	virtual void	InitFromTemplate(const AvHParticleTemplate* inTemplate);
private:
	ParticleParams		mCurrentParams;
	static const int	kScalar;
	
};


#endif
