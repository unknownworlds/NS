#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "common/const.h"
#include "common/entity_state.h"
#include "common/cl_entity.h"
#include "ui/UITags.h"
#include "mod/AvHTeamHierarchy.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHMiniMap.h"
#include "ui/UIUtil.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSpriteAPI.h"
#include "mod/AvHSprites.h"

using std::string;

AvHTeamHierarchy::AvHTeamHierarchy(int wide,int tall) : StaticLabel(wide, tall)
{
    mFullScreen = false;
}

void AvHTeamHierarchy::SetFullScreen(bool inFullScreen)
{
    mFullScreen = inFullScreen;
}

void AvHTeamHierarchy::GetWorldPosFromMouse(int inMouseX, int inMouseY, float& outWorldX, float& outWorldY)
{

    AvHOverviewMap::DrawInfo theDrawInfo;
    GetDrawInfo(theDrawInfo);    
    
    gHUD.GetOverviewMap().GetWorldPosFromMouse(theDrawInfo, inMouseX, inMouseY, outWorldX, outWorldY);

}

void AvHTeamHierarchy::paint()
{

    AvHOverviewMap::DrawInfo theDrawInfo;
    GetDrawInfo(theDrawInfo);
    
    AvHSpriteEnableVGUI(true);
    AvHSpriteSetVGUIOffset(theDrawInfo.mX, theDrawInfo.mY);

    gHUD.GetOverviewMap().Draw(theDrawInfo);
 
	// Evil awful hack that must be done for mouse cursors to work :(
	gHUD.ComponentJustPainted(this);

}

void AvHTeamHierarchy::paintBackground()
{
}

void AvHTeamHierarchy::GetDrawInfo(AvHOverviewMap::DrawInfo& outDrawInfo)
{

    getPos(outDrawInfo.mX, outDrawInfo.mY);
    getSize(outDrawInfo.mWidth, outDrawInfo.mHeight);
    
    AvHOverviewMap& theOverviewMap = gHUD.GetOverviewMap();
    
    AvHMapExtents theMapExtents;
    theOverviewMap.GetMapExtents(theMapExtents);

    outDrawInfo.mFullScreen = mFullScreen;

    if (this->mFullScreen)
	{

		float worldWidth  = theMapExtents.GetMaxMapX() - theMapExtents.GetMinMapX();
		float worldHeight = theMapExtents.GetMaxMapY() - theMapExtents.GetMinMapY();
	
		float xScale;
		float yScale;

		float aspect1 = worldWidth / worldHeight;
        float aspect2 = ((float)outDrawInfo.mWidth) / outDrawInfo.mHeight; 
        
        if (aspect1 > aspect2)
		{
			xScale = 1;
			yScale = 1 / aspect2;
		}
		else
		{
			xScale = aspect2;
			yScale = 1;
        }

		float centerX = (theMapExtents.GetMinMapX() + theMapExtents.GetMaxMapX()) / 2;
		float centerY = (theMapExtents.GetMinMapY() + theMapExtents.GetMaxMapY()) / 2;
        
        outDrawInfo.mViewWorldMinX = centerX - worldWidth  * xScale * 0.5;
        outDrawInfo.mViewWorldMinY = centerY - worldHeight * yScale * 0.5;
        outDrawInfo.mViewWorldMaxX = centerX + worldWidth  * xScale * 0.5;
        outDrawInfo.mViewWorldMaxY = centerY + worldHeight * yScale * 0.5;
    
    }
	else
	{

        float worldViewWidth = 800.0f;
        
        float aspectRatio = (float)(outDrawInfo.mHeight) / outDrawInfo.mWidth;
	
        float thePlayerX;
        float thePlayerY;

        theOverviewMap.GetWorldPosition(thePlayerX, thePlayerY);

        outDrawInfo.mViewWorldMinX = thePlayerX - worldViewWidth;
        outDrawInfo.mViewWorldMinY = thePlayerY - worldViewWidth * aspectRatio;
        outDrawInfo.mViewWorldMaxX = thePlayerX + worldViewWidth;
        outDrawInfo.mViewWorldMaxY = thePlayerY + worldViewWidth * aspectRatio;
    
    }

}



AvHUITeamHierarchy::AvHUITeamHierarchy(void)
{
    this->mType = "TeamHierarchy";
}

void AvHUITeamHierarchy::AllocateComponent(const TRDescription& inDesc)
{
    // Width and height (normalized screen coords)
    float theWidth = UIDefaultWidth;
    float theHeight = UIDefaultHeight;
    inDesc.GetTagValue(UITagWidth, theWidth);
    inDesc.GetTagValue(UITagHeight, theHeight);
	
    this->mHierarchyComponent = new AvHTeamHierarchy(theWidth*ScreenWidth(), theHeight*ScreenHeight());
}
                            
// Destructor automatically removes component from the engine
AvHUITeamHierarchy::~AvHUITeamHierarchy(void)
{
    delete this->mHierarchyComponent;
    this->mHierarchyComponent = NULL;
}

Panel* AvHUITeamHierarchy::GetComponentPointer(void)
{
    return this->mHierarchyComponent;
}

const string& AvHUITeamHierarchy::GetType(void) const
{
    return this->mType;
}

bool AvHUITeamHierarchy::SetClassProperties(const TRDescription& inDesc, Panel* inComponent, CSchemeManager* inSchemeManager)
{
    bool theSuccess = false;

    // read custom attributes here
    UIStaticLabel::SetClassProperties(inDesc, inComponent, inSchemeManager);

    // Get font to use
    std::string theSchemeName;
    if(inDesc.GetTagValue(UITagScheme, theSchemeName))
    {
        AvHTeamHierarchy* theHierarchy = dynamic_cast<AvHTeamHierarchy*>(inComponent);
        if(theHierarchy)
        {
            const char* theSchemeCString = theSchemeName.c_str();
            SchemeHandle_t theSchemeHandle = inSchemeManager->getSchemeHandle(theSchemeCString);
        	Font* theFont = inSchemeManager->getFont(theSchemeHandle);
            if(theFont)
            {
                theHierarchy->setFont(theFont);
            }

            theSuccess = true;
        }
    }

    return theSuccess;
}

