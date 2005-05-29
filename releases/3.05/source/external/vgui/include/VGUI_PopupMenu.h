
#ifndef VGUI_POPUPMENU_H
#define VGUI_POPUPMENU_H

#include<VGUI.h>
#include<VGUI_Menu.h>

namespace vgui
{

class Panel;

class VGUIAPI PopupMenu : public Menu
{
public:
	PopupMenu(int x,int y,int wide,int tall);
	PopupMenu(int wide,int tall);
public:
	virtual void showModal(Panel* panel);
};

}

#endif