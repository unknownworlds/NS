#include "../util/nowarnings.h"
#include "ui/SliderPlus.h"
#include "ui/UIUtil.h"
#include "VGUI_MouseCode.h"

SliderPlus::SliderPlus(int inX, int inY,int inWidth, int inHeight, bool inIsVertical) : Panel(inX, inY, inWidth, inHeight)
{
	this->mDragging = false;
	this->mNobDragStartPos[0] = this->mNobDragStartPos[1] = 0;
	this->mDragStartPos[0] = this->mDragStartPos[1] = 0;
	this->mNobPos[0] = this->mNobPos[1] = 0;
	this->mRange[0] = 0;
	this->mRange[1] = 10;
	this->mValue = 0;
	this->mRangeWindow = 10;
	this->mIsVertical = inIsVertical;
	this->mLastMouseX = this->mLastMouseY = 0;

	// Add ourselves as our own input signal...kinda strange sounding but useful
	this->addInputSignal(this);
}

void SliderPlus::addIntChangeSignal(IntChangeSignal* inChangeSignal)
{
	// Add inChangeSignal to list
}

void SliderPlus::computeNobBox(int& outStartX, int& outStartY, int& outEndX, int& outEndY)
{
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);
	
	outStartX = this->mNobPos[0];
	outStartY = this->mNobPos[1];
	
	if(this->mIsVertical)
	{
		outEndX = outStartX + theWidth - 1;
		outEndY = outStartY + this->mRangeWindow;
	}
	else
	{
		outEndX = outStartX + this->mRangeWindow;
		outEndY = outStartY + theHeight - 1;
	}
}

void SliderPlus::fireIntChangeSignal()
{
	// Run through list of change signals, calling intChanged() on each one
}

void SliderPlus::getNobPos(int& outMin, int& outMax)
{
	// Return the upper left corner of the nob
	outMin = this->mNobPos[0];
	outMax = this->mNobPos[1];
}

void SliderPlus::getRange(int& outMinRange, int& outMaxRange)
{
	// Return the min and max range
	outMinRange = this->mRange[0];
	outMaxRange = this->mRange[1];
}

int	SliderPlus::getRangeWindow()
{
	return this->mRangeWindow;
}

int	SliderPlus::getValue()
{
	return this->mValue;
}

bool SliderPlus::hasFullRange()
{
	return false;
}

bool SliderPlus::isVertical()
{
	return this->mIsVertical;
}

void SliderPlus::paintBackground()
{
	int theStartX, theStartY;
	int theWidth, theHeight;
	int theEndX, theEndY;

	//this->getPos(theStartX, theStartY);
	theStartX = theStartY = 0;
	
	this->getSize(theWidth, theHeight);
	
	Color theColor;
	int theR, theG, theB, theA;
	
	this->getBgColor(theColor);
	theColor.getColor(theR, theG, theB, theA);
	
	// Draw rectangle the size of the slider in background color
	theEndX = theStartX + theWidth;
	theEndY = theStartY + theHeight;
	
	// Draw hollow box in bg color
	vguiSimpleBox(theStartX, theStartY, theEndX-1, theEndY-1, theR, theG, theB, theA);
	
	// Draw nob in foreground color, size of range window
	this->getFgColor(theColor);
	theColor.getColor(theR, theG, theB, theA);
	
	// Compute the nob position so we can draw a box for it, all numbers are in pixels relative to component
	this->computeNobBox(theStartX, theStartY, theEndX, theEndY);
	
	// Draw hollow box in fg color
	vguiSimpleBox(theStartX, theStartY, theEndX, theEndY, theR, theG, theB, theA);
}

void SliderPlus::cursorMoved(int inX, int inY, Panel* inPanel)
{
	// Are we dragging the nub?
	if(this->mDragging)
	{
		// Look at inX, inY and set our new nob pos
		this->mNobPos[0] = this->mNobPos[1] = 0;

		if(this->mIsVertical)
		{
			this->mNobPos[1] = inY;
		}
		else
		{
			this->mNobPos[0] = inX;
		}

		// Recompute the value from it
		this->recomputeValueFromNobPos();
	}
	
	this->mLastMouseX = inX;
	this->mLastMouseY = inY;
}

void SliderPlus::mousePressed(MouseCode inCode, Panel* inPanel)
{
	int theStartX, theStartY, theEndX, theEndY;
	this->computeNobBox(theStartX, theStartY, theEndX, theEndY);
	
	// Was LMB was pressed?
	if(inCode == MOUSE_LEFT)
	{
		// Pressed while over the nob?
		if((this->mLastMouseX >= theStartX) && (this->mLastMouseX <= theEndX) && (this->mLastMouseY >= theStartY) && (this->mLastMouseY <= theEndY))
		{
			// Remember this starting point and remember that we're dragging
			this->mDragStartPos[0] = this->mLastMouseX;
			this->mDragStartPos[1] = this->mLastMouseY;
			this->mDragging = true;
		}
		// Above or below nub?  
		else
		{
			// TODO?: Remember this so we can start adjusting by big increments.  For now, just adjust once.
			// Adjust value by major increment
			//this->recomputeNobPosFromValue();
		}
	}
	// Was RMB pressed above or below the nub?  
	else if(inCode == MOUSE_RIGHT)
	{
		// TODO?: Remember this so we can start adjusting by tiny increments. For now, just adjust once.
		// Adjust value by minor increment
		//this->recomputeNobPosFromValue();
	}
}

void SliderPlus::mouseReleased(MouseCode inCode, Panel* inPanel)
{
	// Forget that we are dragging or adjusting by big or small increments.
	if(inCode == MOUSE_LEFT)
	{
		this->mDragging = false;
	}
}

void SliderPlus::mouseWheeled(int delta, Panel* panel)
{
}

void SliderPlus::recomputeNobPosFromValue()
{
	// Look at current value and range and set the nob pos from it
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	float theNormalizedNobPos = (this->mValue - this->mRange[0])/(float)(this->mRange[1]);
	if(this->mIsVertical)
	{
		this->mNobPos[0] = 0;
		this->mNobPos[1] = (int)(theNormalizedNobPos*theHeight);
	}
	else
	{
		this->mNobPos[0] = (int)(theNormalizedNobPos*theWidth);
		this->mNobPos[1] = 0;
	}
}

void SliderPlus::recomputeValueFromNobPos()
{
	// Look at nob pos and set new value
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);
	
	int theRelevantValue = this->mNobPos[0];
	int theRelevantMaxPixels = theWidth;

	if(this->mIsVertical)
	{
		theRelevantValue = this->mNobPos[1];
		theRelevantMaxPixels = theHeight;
	}

	float theNormalizedValue = (theRelevantValue/(float)theRelevantMaxPixels);
	int theNewValue = this->mRange[0] + (int)(theNormalizedValue*this->mRange[1]);
	
	this->setValue(theNewValue);
}

void SliderPlus::setRange(int inMinRange, int inMaxRange)
{
	// Set the range ("value")
	this->mRange[0] = min(inMinRange, inMaxRange);
	this->mRange[1] = max(inMinRange, inMaxRange);
}

void SliderPlus::setRangeWindow(int inRangeWindow)
{
	// Set the range window (in pixels)
	this->mRangeWindow = inRangeWindow;
}

void SliderPlus::setSize(int inWidth, int inHeight)
{
	// Call parent setSize
	Panel::setSize(inWidth, inHeight);

	// Recompute something?
}

void SliderPlus::setValue(int inValue)
{
	// Only allow setting the value within our specified range
	int theNewValue = inValue;
	theNewValue = max(theNewValue, this->mRange[0]);
	theNewValue = min(theNewValue, this->mRange[1]);

	this->mValue = theNewValue;

	this->recomputeNobPosFromValue();
}
