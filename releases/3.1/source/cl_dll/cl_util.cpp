#include "hud.h"
#include "cl_dll/cl_util.h"

float gTextR = 1;
float gTextG = 1;
float gTextB = 1;

void DrawSetTextColor(float r, float g, float b )
{
    gTextR = r;
    gTextG = g;
    gTextB = b;
    gEngfuncs.pfnDrawSetTextColor(r, g, b);    
}

int DrawConsoleString( int x, int y, const char *string )
{
	//return gEngfuncs.pfnDrawConsoleString( x, y, (char*) string );

    if (string[0] == 2)
    {
        ++string;
    }

    int result = gHUD.GetSmallFont().DrawString(x, y, string, gTextR * 255, gTextG * 255, gTextB * 255, kRenderTransAdd);
    DrawSetTextColor(1, 1, 1);

    return result;

}

void GetConsoleStringSize( const char *string, int *width, int *height )
{
	
    //gEngfuncs.pfnDrawConsoleStringLen( string, width, height );
    
    if (string[0] == 2)
    {
        ++string;
    }
    
    *width  = gHUD.GetSmallFont().GetStringWidth(string);
    *height = gHUD.GetSmallFont().GetStringHeight();

}

int ConsoleStringLen( const char *string )
{
	int _width, _height;
	GetConsoleStringSize( string, &_width, &_height );
	return _width;
}

void ConsolePrint( const char *string )
{
    // TODO Max: implement this
	gEngfuncs.pfnConsolePrint( string );
}

void CenterPrint( const char *string )
{
    // TODO Max: implement this
	gEngfuncs.pfnCenterPrint( string );
}