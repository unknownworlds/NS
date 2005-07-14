#ifndef UI_TAGS_H
#define UI_TAGS_H

#include <string>
using std::string;

// Default ui values here
const float UIDefaultXPos = 0.0f;
const float UIDefaultYPos = 0.0f;
const float UIDefaultWidth = .05f;
const float UIDefaultHeight = .05f;
const float UIDefaultPreferredWidth = UIDefaultWidth;
const float UIDefaultPreferredHeight = UIDefaultHeight;
const bool UIDefaultVisibility = false;
const int UIDefaultProgressBarSegments = 10;

// centered from Label enum, not worth including for dependency reasons
const int UIDefaultAlignment = 4;

const string UIDefaultPathName("ns\\");

// Here is where all recognized tags are specified.  Use this for documenting the system and when parsing tags.
const string UIRootName("rootname");
const string UIConnectorName("connectorname");
const string UITagXPos("xpos");
const string UITagYPos("ypos");
const string UITagWidth("width");
const string UITagHeight("height");
const string UITagParent("parent");
const string UITagVisible("visible");
const string UITagMinimumWidth("minimumwidth");
const string UITagMinimumHeight("minimumheight");
const string UITagEnabled("enabled");
const string UITagFGColor("fgcolor");
const string UITagBGColor("bgcolor");
const string UITagImage("image");
const string UITagAlignment("alignment");
const string UITagPreferredWidth("preferredwidth");
const string UITagPreferredHeight("preferredheight");
const string UITagText("text");
const string UITagSegments("segments");
const string UITagPathName("pathname");
const string UITagMenuAddItem("addtomenu");
const string UITagScheme("scheme");
const string UICategoryScheme("categoryscheme");
const string UINAColor("nacolor");
const string UIResearchedColor("researchedcolor");
const string UILowerValue("lowervalue");
const string UIUpperValue("uppervalue");
const string UIIsVertical("isvertical");
const string UIRangeWindow("rangewindow");
const string UIBaseSprite("basesprite");
const string UIRenderMode("rendermode");
const string UIVAlignment("valignment");
const string UIDefaultIntValue("defaultintvalue");
#endif