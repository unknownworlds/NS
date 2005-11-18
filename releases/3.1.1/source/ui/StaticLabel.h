#ifndef STATICLABEL_H
#define STATICLABEL_H

#include "vgui_Label.h"

class StaticLabel : public vgui::Label
{
public:
					StaticLabel(int wide, int tall) : mOriginalWidth(wide), mOriginalHeight(tall), Label("", 0, 0, wide, tall)
					{
					}

					StaticLabel(const char* text,int x,int y,int wide,int tall) : mOriginalWidth(wide), mOriginalHeight(tall), Label(text, x, y, wide, tall)
					{
					}

			void	SetStaticSize(int inWidth, int inHeight)
					{
						this->mOriginalWidth = inWidth;
						this->mOriginalHeight = inHeight;
						this->setSize(inWidth, inHeight);
					}
	
	virtual void	setSize(int wide,int tall)
					{
						//Label::setSize(this->mOriginalWidth, this->mOriginalHeight);
                        Label::setSize(wide, tall);
					}

private:
	int				mOriginalWidth;
	int				mOriginalHeight;
};

#endif
