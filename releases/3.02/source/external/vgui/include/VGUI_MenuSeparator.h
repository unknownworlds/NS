
#ifndef VGUI_MENUSEPARATOR_H
#define VGUI_MENUSEPARATOR_H

#include<VGUI.h>
#include<VGUI_Label.h>

namespace vgui
{

class VGUIAPI MenuSeparator : public Label
{
public:
	MenuSeparator(const char* text);
protected:
	virtual void paintBackground();
};

}

#endif