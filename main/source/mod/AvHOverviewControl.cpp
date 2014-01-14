#include "mod/AvHOverviewControl.h"
#include "mod/AvHOverviewMap.h"
#include "mod/AvHSprites.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"

class AvHOverviewInputSignal : public InputSignal
{

public:

    AvHOverviewInputSignal(AvHOverviewControl* inControl)
    {
        mControl = inControl;
    }
    
	virtual void cursorMoved(int x,int y,Panel* panel)
    {
        mMouseX = x;
        mMouseY = y;
    }

    virtual void mousePressed(MouseCode code,Panel* panel)
    {
        mControl->HandleMouseClick(mMouseX, mMouseY);
    }
	
	virtual void mouseDoublePressed(MouseCode code,Panel* panel)
    {
        mControl->HandleMouseClick(mMouseX, mMouseY);
    }
    
    virtual void cursorEntered(Panel* panel)                     {}
    virtual void cursorExited(Panel* panel)                      {}
	virtual void mouseReleased(MouseCode code,Panel* panel)      {}
	virtual void mouseWheeled(int delta,Panel* panel)            {}
	virtual void keyPressed(KeyCode code,Panel* panel)           {}
	virtual void keyTyped(KeyCode code,Panel* panel)             {}
	virtual void keyReleased(KeyCode code,Panel* panel)          {}
	virtual void keyFocusTicked(Panel* panel)                    {}

private:

    AvHOverviewControl* mControl;

    int mMouseX;
    int mMouseY;

};

AvHOverviewControl::AvHOverviewControl()
{
    m_hsprWhite = Safe_SPR_Load(kWhiteSprite);
    addInputSignal(new AvHOverviewInputSignal(this));
}

void AvHOverviewControl::paint()
{

    AvHOverviewMap& theOverviewMap = gHUD.GetOverviewMap();

    AvHOverviewMap::DrawInfo theDrawInfo;
    GetDrawInfo(theDrawInfo);

    if (m_hsprWhite != NULL)
    {

        float bgColor[] = { 0.1, 0.1, 0.1, 1 };
        float borderColor[] = { 1, 1, 1, 1 };

        gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
        gEngfuncs.pTriAPI->CullFace(TRI_NONE);

        gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)(gEngfuncs.GetSpritePointer(m_hsprWhite)), 0);

        float gammaScale = 1.0f / gHUD.GetGammaSlope();
        
        // Draw the background.
        
	    gEngfuncs.pTriAPI->Color4f(gammaScale * bgColor[0], gammaScale * bgColor[1], gammaScale * bgColor[2], bgColor[3]);

        gEngfuncs.pTriAPI->Begin(TRI_QUADS);
    
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY, 1);
    
        gEngfuncs.pTriAPI->End();
   
        // Draw the overview map.

        theOverviewMap.Draw(theDrawInfo);

        // Draw the border on the overview map and the inset view.

        gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
        gEngfuncs.pTriAPI->CullFace(TRI_NONE);

        gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)(gEngfuncs.GetSpritePointer(m_hsprWhite)), 0);
        gEngfuncs.pTriAPI->Color4f(gammaScale * borderColor[0], gammaScale * borderColor[1], gammaScale * borderColor[2], borderColor[3]);

        gEngfuncs.pTriAPI->Begin(TRI_LINES);
    
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY + theDrawInfo.mHeight, 1);

        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY, 1);
    
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY, 1);

        gEngfuncs.pTriAPI->End();
    
    }

}

void AvHOverviewControl::HandleMouseClick(int inMouseX, int inMouseY)
{

    AvHOverviewMap& theOverviewMap = gHUD.GetOverviewMap();

    AvHOverviewMap::DrawInfo theDrawInfo;
    GetDrawInfo(theDrawInfo);
    
    float theWorldX;
    float theWorldY;
    
    theOverviewMap.GetWorldPosFromMouse(theDrawInfo, inMouseX, inMouseY, theWorldX, theWorldY);

    // Search for a player at the world position.

    int theEntity = theOverviewMap.GetEntityAtWorldPosition(theWorldX, theWorldY, 150);

    if (theEntity > 0 && theEntity <= 32)
    {
        char command[256];
        sprintf(command, "follow %d", theEntity);

        gEngfuncs.pfnClientCmd(command);
    }


}

void AvHOverviewControl::GetDrawInfo(AvHOverviewMap::DrawInfo& outDrawInfo)
{

    AvHOverviewMap& theOverviewMap = gHUD.GetOverviewMap();

    int theWidth;
    int theHeight;
    
    getSize(theWidth, theHeight);

    outDrawInfo.mX = 0;
    outDrawInfo.mY = 0;
    outDrawInfo.mWidth  = theWidth;
    outDrawInfo.mHeight = theHeight;
	outDrawInfo.mZoomScale = 1.0f;
    AvHMapExtents theMapExtents;
    theOverviewMap.GetMapExtents(theMapExtents);

    outDrawInfo.mFullScreen = true;
	outDrawInfo.mCommander = false;

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