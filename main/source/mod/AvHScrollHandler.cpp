#include "mod/AvHScrollHandler.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "VGUI_MouseCode.h"
#include "ui/ColoredPanel.h"
#include "mod/AvHScrollPanel.h"
#include "mod/AvHActionButtons.h"
#include "ui/UIUtil.h"


int AvHScrollHandler::sScrollX = 0;
int AvHScrollHandler::sScrollY = 0;
int AvHScrollHandler::sScrollZ = 0;
int AvHScrollHandler::sLastMouseX = 0;
int AvHScrollHandler::sLastMouseY = 0;
int AvHScrollHandler::sLastMouseDownX = 0;
int AvHScrollHandler::sLastMouseDownY = 0;
int AvHScrollHandler::sLastMouseUpX = 0;
int AvHScrollHandler::sLastMouseUpY = 0;
bool AvHScrollHandler::sMouseOneDown = false;
bool AvHScrollHandler::sMouseTwoDown = false;
int AvHScrollHandler::sKeyDown = 0;

AvHScrollHandler::AvHScrollHandler()
{
	sKeyDown = 0;
}

bool AvHScrollHandler::GetMouseOneDown() const
{
	return sMouseOneDown;
}

bool AvHScrollHandler::GetMouseTwoDown() const
{
	return sMouseTwoDown;
}

int	AvHScrollHandler::GetMouseX() const
{
	return sLastMouseX;
}

int	AvHScrollHandler::GetMouseY() const
{
	return sLastMouseY;
}

int AvHScrollHandler::GetXScroll() const
{
	return (sMouseOneDown | sMouseTwoDown) ? 0 : sScrollX;
}

int AvHScrollHandler::GetYScroll() const
{
	return (sMouseOneDown | sMouseTwoDown) ? 0 : sScrollY;
}

int AvHScrollHandler::GetZScroll() const
{
	return sScrollZ;
}

void AvHScrollHandler::ClearScrollHeight()
{
	sScrollZ = 0;
}

void AvHScrollHandler::KeyScrollLeft()
{
	if ( sKeyDown < 0 ) sKeyDown=0;
	sKeyDown++;
	ScrollLeft();
}

void AvHScrollHandler::KeyScrollRight()
{
	if ( sKeyDown < 0 ) sKeyDown=0;
	sKeyDown++;
	ScrollRight();
}

void AvHScrollHandler::KeyScrollUp()
{
	if ( sKeyDown < 0 ) sKeyDown=0;
	sKeyDown++;
	ScrollUp();
}

void AvHScrollHandler::KeyScrollDown()
{
	if ( sKeyDown < 0 ) sKeyDown=0;
	sKeyDown++;
	ScrollDown();
}

void AvHScrollHandler::KeyScrollUpStop()
{
	sKeyDown--;
	if ( sKeyDown < 0 ) sKeyDown=0;
	sScrollY=0;
}

void AvHScrollHandler::KeyScrollDownStop()
{
	sKeyDown--;
	if ( sKeyDown < 0 ) sKeyDown=0;
	sScrollY=0;
}

void AvHScrollHandler::KeyScrollLeftStop()
{
	sKeyDown--;
	if ( sKeyDown < 0 ) sKeyDown=0;
	sScrollX=0;
}

void AvHScrollHandler::KeyScrollRightStop()
{
	sKeyDown--;
	if ( sKeyDown < 0 ) sKeyDown=0;
	sScrollX=0;
}


void AvHScrollHandler::ScrollLeft()
{
	sScrollX = -1;
}

void AvHScrollHandler::ScrollRight()
{
	sScrollX = 1;
}

void AvHScrollHandler::ScrollUp()
{
	sScrollY = 1;
}

void AvHScrollHandler::ScrollDown()
{
	sScrollY = -1;
}

void AvHScrollHandler::ScrollHeightUp()
{
	sScrollZ = -1;
}

void AvHScrollHandler::ScrollHeightDown()
{
	sScrollZ = 1;
}

void AvHScrollHandler::StopScroll()
{
	sScrollX = 0;
	sScrollY = 0;
	sScrollZ = 0;
}

void AvHScrollHandler::cursorMoved(int x, int y, Panel* inPanel)
{
	if ( sKeyDown > 0 ) 
		return;

	char theMessage[256];
	int theRandNumber = rand() % 10;
	sprintf(theMessage, "Cursor moved, %d, %d, rand = %d", x, y, theRandNumber);

	// Uncomment this to make it scroll when you're near the edges of the screen, nice feel but hard to use with UI elements on edge
	//int kPixelScrollTolerance = ScreenWidth/25;
	int kPixelScrollTolerance = 0;

	// Get screen coordinates of mouse
	ASSERT(inPanel);
	int theX, theY;
	inPanel->getPos(theX, theY);

	sLastMouseX = theX + x;
	sLastMouseY = theY + y;

	AvHScrollPanel* theScrollPanel = dynamic_cast<AvHScrollPanel*>(inPanel);
	if(theScrollPanel)
	{
		// Only scroll when moving over colored panel (ie, not when near the edge of another component)
		AvHScrollHandler::StopScroll();
		
		if(x <= kPixelScrollTolerance)
		{
			AvHScrollHandler::ScrollLeft();
			sprintf(theMessage, "Scrolling left");
		}
		if(x >= (ScreenWidth() - 1 - kPixelScrollTolerance))
		{
			AvHScrollHandler::ScrollRight();
			sprintf(theMessage, "Scrolling right");
		}
		if(y <= kPixelScrollTolerance)
		{
			AvHScrollHandler::ScrollUp();
			sprintf(theMessage, "Scrolling up");
		}
		if(y >= (ScreenHeight() - 1 - kPixelScrollTolerance))
		{
			AvHScrollHandler::ScrollDown();
			sprintf(theMessage, "Scrolling down");
		}
		//CenterPrint(theMessage);
	}
}

void AvHScrollHandler::keyPressed(KeyCode inKeyCode, Panel* panel)
{
}

void AvHScrollHandler::mousePressed(MouseCode code, Panel* panel)
{
	// store this
	if(code == vgui::MOUSE_LEFT)
	{
		sMouseOneDown = true;
	}
	else if(code == vgui::MOUSE_RIGHT)
	{
		sMouseTwoDown = true;
	}
}

void AvHScrollHandler::mouseReleased(MouseCode code, Panel* panel)
{
	// store this
	if(code == vgui::MOUSE_LEFT)
	{
		sMouseOneDown = false;
	}
	else if(code == vgui::MOUSE_RIGHT)
	{
		sMouseTwoDown = false;
	}
}

void AvHScrollHandler::mouseWheeled(int delta, Panel* panel)
{
	if(delta > 0)
	{
		this->ScrollHeightUp();
	}
	else if(delta < 0)
	{
		this->ScrollHeightDown();
	}
}
