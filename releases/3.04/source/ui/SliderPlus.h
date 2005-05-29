#ifndef SLIDER_PLUS_H
#define SLIDER_PLUS_H

#include "VGUI_Panel.h"
#include "VGUI_InputSignal.h"
using namespace vgui;

class SliderPlus : public Panel, public InputSignal
{
public:
							SliderPlus(int inX, int inY,int inWidth, int inHeight, bool inIsVertical);
public:						
	virtual void			setValue(int inValue);
	virtual int				getValue();
	virtual bool			isVertical();
	virtual void			addIntChangeSignal(IntChangeSignal* inChangeSignal);
    virtual void			setRange(int inMinRange, int inMaxRange);
	virtual void			getRange(int& outMinRange, int& outMaxRange);
	virtual int				getRangeWindow();
	virtual void			setRangeWindow(int inRangeWindow);
	virtual void			setSize(int inWidth, int inHeight);
	virtual void			getNobPos(int& outMin, int& outMax);
	virtual bool			hasFullRange();

	// Behavior
	virtual void			cursorMoved(int x,int y,Panel* panel);
	virtual void			cursorEntered(Panel* panel) {}
	virtual void			cursorExited(Panel* panel) {}
	virtual void			mousePressed(MouseCode code, Panel* panel);
	virtual void			mouseDoublePressed(MouseCode code,Panel* panel) {}
	virtual void			mouseReleased(MouseCode code,Panel* panel);
	virtual void			mouseWheeled(int delta, Panel* panel);
	virtual void			keyPressed(KeyCode code,Panel* panel) {}
	virtual void			keyTyped(KeyCode code,Panel* panel) {}
	virtual void			keyReleased(KeyCode code,Panel* panel) {}
	virtual void			keyFocusTicked(Panel* panel) {}

protected:					
    virtual void			fireIntChangeSignal();
	virtual void			paintBackground();
	
private:
	void					computeNobBox(int& outStartX, int& outStartY, int& outEndX, int& outEndY);
	virtual void			recomputeNobPosFromValue();
	virtual void			recomputeValueFromNobPos();

	bool					mDragging;
	Dar<IntChangeSignal*>	mIntChangeSignalDar;
	int						mNobDragStartPos[2];
	int						mDragStartPos[2];

	// The upper left corner of the nob, relative to the slider (so mNobPos[0] is 0 for a vertical slider, mNobPos[1] is 0 for a horizontal slider)
	int						mNobPos[2];

	int						mRange[2];
	int						mValue;

	// the number of pixels wide or high the nob is (wide for horizontal slider, heigh for vertical)
	int						mRangeWindow;

	bool					mIsVertical;

	int						mLastMouseX;
	int						mLastMouseY;
//	int						mButtonOffset;
};

#endif