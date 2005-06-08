#ifndef AVHPIEMENUHANDLER_H
#define AVHPIEMENUHANDLER_H

#include <VGUI_InputSignal.h>

using namespace vgui;
class PieNode;

class AvHPieMenuHandler : public InputSignal
{
public:
    static void ClosePieMenu(void);
    static void OpenPieMenu(void);
    static bool GetIsPieMenuOpen(void);
    static PieMenu* GetActivePieMenu(void);
    
    //static void MouseClosePieMenu(void);
    //static void MouseOpenPieMenu(void);
	
    static void NodeCancelled(void);
    static void NodeChosen(const string& inNodeName, int inMessageID);
    static void NodeActivated(const string& inNodeName);
	static string GetPieMenuControl();
	static void SetPieMenuControl(const string& inPieMenuName);

	virtual void cursorMoved(int x,int y,Panel* panel);
	virtual void cursorEntered(Panel* panel);
	virtual void cursorExited(Panel* panel);
	virtual void mousePressed(MouseCode code,Panel* panel);
	virtual void mouseDoublePressed(MouseCode code,Panel* panel);
	virtual void mouseReleased(MouseCode code,Panel* panel);
	virtual void mouseWheeled(int delta,Panel* panel);
	virtual void keyPressed(KeyCode code,Panel* panel);
	virtual void keyTyped(KeyCode code,Panel* panel);
	virtual void keyReleased(KeyCode code,Panel* panel);
	virtual void keyFocusTicked(Panel* panel);
private:
    static void InternalClosePieMenu(void);
    static PieNode*		sLastNodeHighlighted;
	static string		sPieMenuName;
	static float		sTimeLastNodeHighlighted;
	static float		sTimeMenuOpened;
    static bool         sPieMenuOpen;
};

#endif