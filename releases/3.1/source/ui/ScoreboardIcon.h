// Class for animated UPP icons shown on scoreboard, etc.

#ifndef SCOREBOARDICON_H
#define SCOREBOARDICON_H

#include "vgui.h"
#include "vgui_bitmaptga.h"
#include <vector>

class ScoreboardIcon
{
public:
	ScoreboardIcon(const int icon_number, const int icon_color);
	~ScoreboardIcon(void);

	bool updateColor(const int icon_color, const float gamma_slope);
	bool updateColor(const float gamma_slope);

	int getIconNumber(void) const;
	int getIconColor(void) const;
	vgui::Color getColor(void) const;
	vgui::BitmapTGA* getImage(const int frame_number) const;

private:
	int icon_number, icon_color;
	vgui::Color base_color, color;
	std::vector<vgui::BitmapTGA*> images;
};

#endif