#include "util/nowarnings.h"
#include "mod/AvHParticleEditorHandler.h"
#include "cl_dll/chud.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "mod/AvHConstants.h"
#include "mod/AvHParticleTemplateClient.h"
#include "mod/AvHParticleSystemManager.h"
#include "VGUI_Label.h"
#include "util/STLUtil.h"
//#include "ui/SliderPlus.h"
#include "game_shared/vgui_slider2.h"

uint32 AvHParticleEditorHandler::sEditIndex = 0;
bool AvHParticleEditorHandler::sInEditMode = false;

extern AvHParticleTemplateListClient	gParticleTemplateList;

AvHSizeHandler						gSizeHandler(kPSESizeSlider, kPSESizeLabel);
AvHScaleHandler						gScaleHandler(kPSEScaleSlider, kPSEScaleLabel);
AvHGenerationRateHandler			gGenerationRateHandler(kPSEGenerationRateSlider, kPSEGenerationRateLabel);
AvHParticleLifetimeHandler			gParticleLifetimeHandler(kPSEParticleLifetimeSlider, kPSEParticleLifetimeLabel);
AvHParticleSystemLifetimeHandler	gParticleSystemLifetimeHandler(kPSEParticleSystemLifetimeSlider, kPSEParticleSystemLifetimeLabel);
AvHMaxParticlesHandler				gMaxParticlesHandler(kPSEMaxParticlesSlider, kPSEMaxParticlesLabel);
AvHDrawModeHandler					gDrawModeHandler(kPSEDrawModeSlider, kPSEDrawModeLabel);
AvHGenVelToggleHandler				gGenVelToggleHandler(PSEGenVelToggleSlider, kPSEGenVelToggleLabel);
AvHGenVelShapeHandler				gGenVelShapeHandler(kPSEGenVelShapeSlider, kPSEGenVelShapeLabel);
AvHGenVelParamNumHandler			gGenVelParamNumHandler(kPSEGenVelParamNumSlider, kPSEGenVelParamNumLabel);
AvHGenVelParamsHandler				gGenVelParamsHandler(kPSEGenVelParamValueSlider, kPSEGenVelParamValueLabel);

const int AvHSizeHandler::kScalar = 20;
const int AvHScaleHandler::kScalar = 30;
const int AvHGenerationRateHandler::kScalar = 5;
const int AvHParticleLifetimeHandler::kScalar = 20;
const int AvHParticleSystemLifetimeHandler::kScalar = 5;
const int AvHMaxParticlesHandler::kScalar = 2;
const int AvHDrawModeHandler::kScalar = 1;
const int AvHGenVelToggleHandler::kScalar = 250;
const int AvHGenVelShapeHandler::kScalar = 100;
const int AvHGenVelParamNumHandler::kScalar = 1;
const int AvHGenVelParamsHandler::kScalar = 1;

AvHParticleTemplate* GetEdittedParticleTemplate()
{
	AvHParticleTemplate* theTemplate = NULL;
	
	theTemplate = gParticleTemplateList.GetTemplateAtIndex(AvHParticleEditorHandler::GetEditIndex());
	
	return theTemplate;
}

AvHParticleEditorHandler::AvHParticleEditorHandler()
{
}

void AvHParticleEditorHandler::InitHandlers()
{
	gSizeHandler.Init();
	gScaleHandler.Init();
	gGenerationRateHandler.Init();
	gParticleLifetimeHandler.Init();
	gParticleSystemLifetimeHandler.Init();
	gMaxParticlesHandler.Init();
	gDrawModeHandler.Init();
	gGenVelToggleHandler.Init();
	gGenVelShapeHandler.Init();
	gGenVelParamNumHandler.Init();
	gGenVelParamsHandler.Init();
}

void AvHParticleEditorHandler::ToggleEdit()
{
	if(!gHUD.GetInTopDownMode() && gHUD.GetServerVariableFloat("sv_cheats"))
	{
		if(!sInEditMode)
		{
			if(gHUD.SwitchUIMode(EDITPS_MODE))
			{
				gHUD.ToggleMouse();
				
				AvHParticleEditorHandler::InitHandlers();
				
				sInEditMode = true;
			}
		}
		else
		{
			if(gHUD.SwitchUIMode(MAIN_MODE))
			{
				gHUD.ToggleMouse();

				// Set mouse position to center so it doesn't jar our view
				gEngfuncs.pfnSetMousePos(gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
				
				sInEditMode = false;
			}
		}
	}
}

uint32 AvHParticleEditorHandler::GetEditIndex()
{
	return sEditIndex;
}

bool AvHParticleEditorHandler::GetInEditMode()
{
	return sInEditMode;
}

void AvHParticleEditorHandler::SetEditIndex(uint32 inIndex)
{
	bool theReloadValues = false;
	if(sEditIndex != inIndex)
	{
		theReloadValues = true;
	}
	
	sEditIndex = inIndex;

	if(theReloadValues)
	{
		// Init handlers in case we are in edit mode currently
		InitHandlers();
	}
}

void AvHParticleEditorHandler::Setup()
{
	gSizeHandler.Setup();
	gScaleHandler.Setup();
	gGenerationRateHandler.Setup();
	gParticleLifetimeHandler.Setup();
	gParticleSystemLifetimeHandler.Setup();
	gMaxParticlesHandler.Setup();
	gDrawModeHandler.Setup();
	gGenVelToggleHandler.Setup();
	gGenVelShapeHandler.Setup();
	gGenVelParamNumHandler.Setup();
	gGenVelParamsHandler.Setup();
}

void AvHParticleEditorHandler::cursorMoved(int x,int y,Panel* panel)
{
}

void AvHParticleEditorHandler::mousePressed(MouseCode code,Panel* panel)
{
}

void AvHParticleEditorHandler::mouseReleased(MouseCode code,Panel* panel)
{
}

void AvHParticleEditorHandler::mouseWheeled(int delta,Panel* panel)
{
}




// Generic slider handler
AvHSliderHandler::AvHSliderHandler(const string& inSliderName, const string& inLabelName)
{
	this->mSliderName = inSliderName;
	this->mLabelName = inLabelName;
}

void AvHSliderHandler::Init()
{
	AvHParticleTemplate* theTemplate = GetEdittedParticleTemplate();
	if(theTemplate)
	{
		this->InitFromTemplate(theTemplate);
	}
}

void AvHSliderHandler::intChanged(int /*inValue*/, Panel* inPanel)
{
	AvHParticleTemplate* theTemplate = GetEdittedParticleTemplate();
	if(theTemplate)
	{
		int theNewValue;
		if(this->GetValue(theNewValue))
		{
			this->ChangeTemplateFromValue(theTemplate, theNewValue);
			
			AvHParticleSystemManager::Instance()->ReloadFromTemplates();

			this->RecomputeDependencies(theTemplate);

			this->SetText(theNewValue);
		}
	}
}

void AvHSliderHandler::SetText(int inValue)
{
	Label* theLabel;
	if(gHUD.GetManager().GetVGUIComponentNamed(this->mLabelName, theLabel))
	{
		string theText = this->GetTextFromValue(inValue);
		
		theLabel->setText(theText.c_str());
	}
}

bool AvHSliderHandler::Setup()
{
	bool theSuccess = false;
	
	Slider2* theSlider;
	if(gHUD.GetManager().GetVGUIComponentNamed(this->mSliderName, theSlider))
	{
		theSlider->addIntChangeSignal(this);
		theSuccess = true;
	}
	return theSuccess;
}

string AvHSliderHandler::GetSliderDebugInfo() const
{
	string theSliderDebugInfo("no slider");
	
	// Look up slider
	Slider2* theSlider;
	if(gHUD.GetManager().GetVGUIComponentNamed(this->mSliderName, theSlider))
	{
		// Build string using min, max, slider window
		int theMin, theMax;
		theSlider->getRange(theMin, theMax);

		int theValue = theSlider->getValue();
		//int theRangeWindow = theSlider->getRangeWindow();

		char theInfo[128];
		sprintf(theInfo, "%d %d %d", theMin, theMax, theValue);
		theSliderDebugInfo = string(theInfo);

	}
	
	// return it
	return theSliderDebugInfo;
}

bool AvHSliderHandler::GetValue(int& outValue)
{
	bool theSuccess = false;

	//Slider* theSlider;
	Slider2* theSlider;
	if(gHUD.GetManager().GetVGUIComponentNamed(this->mSliderName, theSlider))
	{
		outValue = theSlider->getValue();
		theSuccess = true;
	}

	return theSuccess;
}

void AvHSliderHandler::SetValue(int inValue)
{
	//SliderPlus* theSlider;
	Slider2* theSlider;
	if(gHUD.GetManager().GetVGUIComponentNamed(this->mSliderName, theSlider))
	{
		theSlider->setValue(inValue);
	}
}



// Particle size
void AvHSizeHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	float theSize = inValue/(float)kScalar;
	inTemplate->SetParticleSize(theSize);
}

string AvHSizeHandler::GetTextFromValue(int inValue)
{
	char theBuffer[256];
	sprintf(theBuffer, "size: %d", (inValue/kScalar));

	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHSizeHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = inTemplate->GetParticleSize()*kScalar;
	this->SetValue(theValue);
	this->SetText(theValue);
}

// Particle scale
void AvHScaleHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	float theValue = inValue/(float)kScalar;
	inTemplate->SetParticleScaling(theValue);
}

string AvHScaleHandler::GetTextFromValue(int inValue)
{
	float theValue = inValue/(float)kScalar;

	char theBuffer[256];
	sprintf(theBuffer, "scale: %.2f", theValue);
	
	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHScaleHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = (int)(inTemplate->GetParticleScaling()*kScalar);
	this->SetValue(theValue);
	this->SetText(theValue);
}

// Generation rate
void AvHGenerationRateHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	int theGenerationRate = inValue/kScalar;
	inTemplate->SetGenerationRate(theGenerationRate);
//	Slider* theSlider;
//	if(gHUD.GetManager().GetVGUIComponentNamed(kPSEScaleSlider, theSlider))
//	{
//		theSlider->setRangeWindow(true);
//	  
//		theSlider->setRangeWindow(inValue);
//	}
}

string AvHGenerationRateHandler::GetTextFromValue(int inValue)
{
	char theBuffer[256];
	sprintf(theBuffer, "gen rate: %d", (inValue/kScalar));
	
	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHGenerationRateHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = inTemplate->GetGenerationRate()*kScalar;
	this->SetValue(theValue);
	this->SetText(theValue);
}



// Particle lifetime
void AvHParticleLifetimeHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	float theValue = inValue/(float)kScalar;
	inTemplate->SetParticleLifetime(theValue);
}

string AvHParticleLifetimeHandler::GetTextFromValue(int inValue)
{
	float theValue = inValue/(float)kScalar;
	
	char theBuffer[256];
	sprintf(theBuffer, "p life: %.1f", theValue);
	
	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHParticleLifetimeHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = (int)(inTemplate->GetParticleLifetime()*kScalar);
	this->SetValue(theValue);
	this->SetText(theValue);
}


// Particle SYSTEM lifetime
void AvHParticleSystemLifetimeHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	if(this->mEditable)
	{
		float theValue = inValue/(float)kScalar;
		inTemplate->SetParticleSystemLifetime(theValue);
	}
}

string AvHParticleSystemLifetimeHandler::GetTextFromValue(int inValue)
{
	float theValue = inValue/(float)kScalar;
	
	char theBuffer[256];
	if(this->mEditable)
	{
		sprintf(theBuffer, "ps life: %.1f", theValue);
	}
	else
	{
		sprintf(theBuffer, "ps can't die");
	}
	
	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHParticleSystemLifetimeHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = (int)(inTemplate->GetParticleSystemLifetime()*kScalar);
	if(theValue > 0 || this->mEditable)
	{
		this->mEditable = true;
		this->SetValue(theValue);
		this->SetText(theValue);
	}
}


// Max particles
void AvHMaxParticlesHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	int theMaxParticles = inValue/kScalar;
	inTemplate->SetMaxParticles(theMaxParticles);
}

string AvHMaxParticlesHandler::GetTextFromValue(int inValue)
{
	char theBuffer[256];
	sprintf(theBuffer, "max parts: %d", (inValue/kScalar));
	
	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHMaxParticlesHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = inTemplate->GetMaxParticles()*kScalar;
	this->SetValue(theValue);
	this->SetText(theValue);
}


// Render mode
void AvHDrawModeHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	int theDrawMode = inValue/kScalar;
	ASSERT(theDrawMode >= 0);
	ASSERT(theDrawMode <= 5);

	inTemplate->SetRenderMode(theDrawMode);
}

string AvHDrawModeHandler::GetTextFromValue(int inValue)
{
	string theString = "render: ";
	
	switch(inValue/kScalar)
	{
	case 0:
		theString += "normal";
		break;
	case 1:
		theString += "transColor";
		break;
	case 2:
		theString += "transTexture";
		break;
	case 3:
		theString += "glow";
		break;
	case 4:
		theString += "transAlpha";
		break;
	case 5:
		theString += "transAdd";
		break;
	}
	return theString;
	//return this->GetSliderDebugInfo();
}

void AvHDrawModeHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	int theValue = inTemplate->GetRenderMode()*kScalar;
	this->SetValue(theValue);
	this->SetText(theValue);
}


// Gen/Vel toggle
void AvHGenVelToggleHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	if(inValue < kScalar)
	{
		this->mGenVelMode = true;
	}
	else
	{
		this->mGenVelMode = false;
	}

//	// Trigger change
//	gGenVelShapeHandler.InitFromTemplate(inTemplate);
//	gGenVelParamNumHandler.InitFromTemplate(inTemplate);
//	gGenVelParamsHandler.InitFromTemplate(inTemplate);
}

string AvHGenVelToggleHandler::GetTextFromValue(int inValue)
{
	string theString = "generation";

	if(inValue >= kScalar)
	{
		theString = "velocity";
	}
		
	return theString;
	//return this->GetSliderDebugInfo();
}

void AvHGenVelToggleHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	// Assume edit generation shape to start
	this->mGenVelMode = true;

	this->SetValue(1);
	this->SetText(1);
}

bool AvHGenVelToggleHandler::GetGenVelMode() const
{
	return this->mGenVelMode;
}

void AvHGenVelToggleHandler::RecomputeDependencies(AvHParticleTemplate* inReloadedTemplate)
{
	// Trigger change
	gGenVelShapeHandler.InitFromTemplate(inReloadedTemplate);
	gGenVelParamNumHandler.InitFromTemplate(inReloadedTemplate);
	gGenVelParamsHandler.InitFromTemplate(inReloadedTemplate);
}

// GenVel shape
void AvHGenVelShapeHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	if(!this->mUsingEntity)
	{
		ParticleShape theShape = PS_Point;
		switch(inValue/kScalar)
		{
		case 1:
			theShape = PS_Point;
			break;
		case 2:
			theShape = PS_Box;
			break;
		case 3:
			theShape = PS_Sphere;
			break;
		case 4:
			theShape = PS_Blob;
			break;
		default:
			ASSERT(false);
			break;
			
		}
		if(gGenVelToggleHandler.GetGenVelMode())
		{
			inTemplate->SetGenerationShape(theShape);
		}
		else
		{
			inTemplate->SetStartingVelocityShape(theShape);
		}
	}
}

string AvHGenVelShapeHandler::GetTextFromValue(int inValue)
{
	string theShape;
	if(this->mUsingEntity)
	{
		theShape = "Entity";
	}
	else
	{
		switch(inValue/kScalar)
		{
		default:
		case 1:
			theShape = "Point";
			break;
		case 2:
			theShape = "Box";
			break;
		case 3:
			theShape = "Sphere";
			break;
		case 4:
			theShape = "Blob";
			break;
		}			
	}
	
	return theShape;
	//return this->GetSliderDebugInfo();
}

bool AvHGenVelShapeHandler::GetUsingEntity() const
{
	return this->mUsingEntity;
}

void AvHGenVelShapeHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	bool theIsInGenMode = gGenVelToggleHandler.GetGenVelMode();

	int theGenerationEntityIndex = inTemplate->GetGenerationEntityIndex();
	if((theGenerationEntityIndex == -1) || !theIsInGenMode)
	{
		this->mUsingEntity = false;

		ShapeType theShape = inTemplate->GetGenerationShape();
		if(!theIsInGenMode)
		{
			theShape = inTemplate->GetStartingVelocityShape();
		}

		int theShapeIndex = 0;
	
		switch((int)(theShape))
		{
		case 0:
			theShapeIndex = 0;
			break;
		case 4:
			theShapeIndex = 2;
			break;
		case 5:
			theShapeIndex = 3;
			break;
		case 8:
			theShapeIndex = 4;
			break;
		default:
			ASSERT(false);
			break;
		}

		int theValue = theShapeIndex*kScalar;
		this->SetValue(theValue);
		this->SetText(theValue);
		
		gGenVelParamsHandler.InitFromTemplate(inTemplate);
	}
	else
	{
		this->mUsingEntity = true;
	}
}


// Param num
void AvHGenVelParamNumHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	this->mCurrentParamNum = (inValue/kScalar);
	//gGenVelParamsHandler.InitFromTemplate(inTemplate);
}

int	AvHGenVelParamNumHandler::GetCurrentParamNum() const
{
	return this->mCurrentParamNum;
}

string AvHGenVelParamNumHandler::GetTextFromValue(int inValue)
{
	char theBuffer[256];
	if(!gGenVelShapeHandler.GetUsingEntity())
	{
		sprintf(theBuffer, "param: %d", (inValue/kScalar));
	}
	else
	{
		sprintf(theBuffer, "<na>");
	}
	
	return string(theBuffer);	
	//return this->GetSliderDebugInfo();
}

void AvHGenVelParamNumHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	//	this->mCurrentParamNum = 1;
}


void AvHGenVelParamNumHandler::RecomputeDependencies(AvHParticleTemplate* inReloadedTemplate) 
{
	gGenVelParamsHandler.InitFromTemplate(inReloadedTemplate);
}

// GenVel params
void AvHGenVelParamsHandler::ChangeTemplateFromValue(AvHParticleTemplate* inTemplate, int inValue)
{
	int theValue = inValue/kScalar;
	int theParamNum = gGenVelParamNumHandler.GetCurrentParamNum();
	
	if(theParamNum >= 1 && theParamNum <= 8)
	{
		this->mCurrentParams[theParamNum-1] = theValue;
		if(gGenVelToggleHandler.GetGenVelMode())
		{
			inTemplate->SetGenerationParams(this->mCurrentParams);
		}
		else
		{
			inTemplate->SetStartingVelocityParams(this->mCurrentParams);
		}
	}
}

string AvHGenVelParamsHandler::GetTextFromValue(int inValue)
{
	string theParamString = "<na>";
	if(!gGenVelShapeHandler.GetUsingEntity())
	{
		theParamString = "params:";
		for(int i=0; i < 8; i++)
		{
			theParamString += " ";
			theParamString += MakeStringFromInt(this->mCurrentParams[i]);
		}
	}	
	return theParamString;
	//return this->GetSliderDebugInfo();
}

void AvHGenVelParamsHandler::InitFromTemplate(const AvHParticleTemplate* inTemplate)
{
	inTemplate->GetGenerationParams(this->mCurrentParams);
	if(!gGenVelToggleHandler.GetGenVelMode())
	{
		inTemplate->GetStartingVelocityParams(this->mCurrentParams);
	}

	int theCurrentParamNum = gGenVelParamNumHandler.GetCurrentParamNum();
	int theValue = this->mCurrentParams[theCurrentParamNum-1]*kScalar;

	this->SetValue(theValue);
	this->SetText(theValue);
}

