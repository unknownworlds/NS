#ifndef AVHOVERVIEWCONTROL_H
#define AVHOVERVIEWCONTROL_H

#include "VGUI_Panel.h"
//#include "cl_dll/util_vector.h"
//#include "cl_dll/wrect.h"
//#include "engine/cdll_int.h"
#include "mod/AvHHud.h"

class AvHOverviewControl : public vgui::Panel
{

public:

    AvHOverviewControl();

    // Overridden from Panel.
	virtual void paint();

    void HandleMouseClick(int inX, int inY);

protected:

    void GetDrawInfo(AvHOverviewMap::DrawInfo& outDrawInfo);

private:

    HSPRITE m_hsprWhite;

};

#endif