#ifndef CHAT_PANEL_H
#define CHAT_PANEL_H

#include <VGUI_Panel.h>
#include <VGUI_DefaultInputSignal.h>

#include "mod/AvHFont.h"

#include <string>

class ChatPanel : public vgui::Panel, public vgui::CDefaultInputSignal
{
public:
    ChatPanel(int x, int y, int wide, int tall);

    void CancelChat();
    void SetChatMode(std::string sChatMode);
    
    void KeyDown(int virtualKey, int scanCode);

    // Checks if a key was pushed since the chat window was opened.
    bool WasKeyPushed(int virtualKey) const;

    virtual void paint();
	virtual void paintBackground();

private:
    std::string mText;
    std::string mChatMode;

    bool        mKeyPushed[256];

};

#endif