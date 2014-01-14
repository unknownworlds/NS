#ifndef AVHSCROLLHANDLER_H
#define AVHSCROLLHANDLER_H

#include <VGUI_InputSignal.h>

using namespace vgui;

class AvHScrollHandler : public InputSignal
{
public:
					AvHScrollHandler();

	int				GetXScroll() const;
	int				GetYScroll() const;
	int				GetZScroll() const;
	int				GetMouseX() const;
	int				GetMouseY() const;
	bool			GetMouseOneDown() const;
	bool			GetMouseTwoDown() const;
	
	static void		ClearScrollHeight();
	static void		KeyScrollLeft();
	static void		KeyScrollRight();
	static void		KeyScrollUp();
	static void		KeyScrollDown();
	static void		KeyScrollLeftStop();
	static void		KeyScrollRightStop();
	static void		KeyScrollUpStop();
	static void		KeyScrollDownStop();
	static void		ScrollLeft();
	static void		ScrollRight();
	static void		ScrollUp();
	static void		ScrollDown();
	static void		ScrollHeightUp();
	static void		ScrollHeightDown();
	static void		StopScroll();
	
	virtual void	cursorMoved(int x,int y,Panel* panel);
	virtual void	cursorEntered(Panel* panel) {}
	virtual void	cursorExited(Panel* panel) {}
	virtual void	mousePressed(MouseCode code,Panel* panel);
	virtual void	mouseDoublePressed(MouseCode code,Panel* panel) {}
	virtual void	mouseReleased(MouseCode code,Panel* panel);
	virtual void	mouseWheeled(int delta,Panel* panel);
	virtual void	keyPressed(KeyCode code,Panel* panel);
	virtual void	keyTyped(KeyCode code,Panel* panel)  {}
	virtual void	keyReleased(KeyCode code,Panel* panel)  {}
	virtual void	keyFocusTicked(Panel* panel)  {}

private:

	static int sScrollX;
	static int sScrollY;
	static int sScrollZ;
	static int sLastMouseX;
	static int sLastMouseY;
	static int sLastMouseDownX;
	static int sLastMouseDownY;
	static int sLastMouseUpX;
	static int sLastMouseUpY;
	static bool sMouseOneDown;
	static bool sMouseTwoDown;
	static int sKeyDown;
};

#endif
