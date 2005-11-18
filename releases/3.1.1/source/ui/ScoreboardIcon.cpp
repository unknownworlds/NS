#include "ScoreboardIcon.h"
#include "game_shared\vgui_loadtga.h"
#include "util\STLUtil.h"
#include <algorithm>	//max

void loadImages( const int icon_number, std::vector<vgui::BitmapTGA*>& images );
vgui::Color uncompressColor( const int icon_color );
vgui::Color gammaAdjustColor( vgui::Color& color, const float gamma_slope );

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ScoreboardIcon::ScoreboardIcon( const int icon_number_, const int icon_color_ ) : icon_number( icon_number_ ), icon_color( icon_color_ )
{
	this->base_color = uncompressColor( this->icon_color );
	this->color = gammaAdjustColor( this->base_color, 1.0 );
	loadImages( this->icon_number, this->images );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ScoreboardIcon::~ScoreboardIcon(void)
{
	//free memory taken by images
	std::vector<vgui::BitmapTGA*>::iterator current, end = this->images.end();
	for( current = this->images.begin(); current != end; ++current )
	{
		delete *current;
	}
	this->images.clear();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

vgui::Color ScoreboardIcon::getColor(void) const
{
	return base_color;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int ScoreboardIcon::getIconColor(void) const
{
	return icon_color;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int ScoreboardIcon::getIconNumber(void) const
{
	return icon_number;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

vgui::BitmapTGA* ScoreboardIcon::getImage( const int frame_number ) const
{
	if( this->images.empty() ) //images not found at load time
	{ return NULL; }

	int image_number = frame_number % (int)this->images.size();
	vgui::BitmapTGA* image = const_cast<vgui::BitmapTGA*>(this->images[image_number]);
	image->setColor( this->color );
	return image;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ScoreboardIcon::updateColor( const int icon_color, const float gamma_slope )
{
	this->icon_color = icon_color;
	this->base_color = uncompressColor( this->icon_color );
	this->color = gammaAdjustColor( this->base_color, gamma_slope );
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool ScoreboardIcon::updateColor( const float gamma_slope )
{
	this->color = gammaAdjustColor( this->base_color, gamma_slope );
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

vgui::Color uncompressColor( const int icon_color )
{
	int components[4] = {0,0,0,0}; //Alpha = 0

	components[0] = ((icon_color >> 6) & 0x03) * 64; //Red
	components[1] = ((icon_color >> 6) & 0x03) * 64; //Green
	components[2] = ((icon_color >> 6) & 0x03) * 64; //Blue
	int brightness_shift = (icon_color & 0x03) * 16; //shift value

	float max = std::max(components[0], std::max(components[1], components[2]));
	if( max != 0 )
	{
		components[0] += ((int)(components[0]/max))*brightness_shift;
		components[1] += ((int)(components[1]/max))*brightness_shift;
		components[2] += ((int)(components[2]/max))*brightness_shift;
	}
	else
	{
		components[0] += brightness_shift;
		components[1] += brightness_shift;
		components[2] += brightness_shift;
	}

	return vgui::Color( components[0], components[1], components[2], components[3] );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

vgui::Color gammaAdjustColor( vgui::Color& color, const float gamma_slope )
{
	int components[4];
	color.getColor( components[0], components[1], components[2], components[3] );
	ASSERT( gamma_slope != 0 );
	vgui::Color returnVal = vgui::Color( components[0]/gamma_slope, components[1]/gamma_slope, components[2]/gamma_slope, 0 );
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void loadImages( const int icon_number, std::vector<vgui::BitmapTGA*>& images )
{
	if( icon_number == 0 ) //special case, no icon
	{ return; }
	if( icon_number > 0xFFFF ) //bogus icon number
	{ return; }

	std::string directory = "gfx/vgui/icons/";
	std::string extension = ".tga";
	std::string icon_name;

	unsigned char icon_bytes[2] = { (icon_number >> 8) & 0xFF, icon_number & 0xFF };
	MakeHexPairsFromBytes(icon_bytes,icon_name,2);
	icon_name = "0x" + icon_name;

	std::string image_name = directory + icon_name + extension;
	vgui::BitmapTGA* image = vgui_LoadTGA( image_name.c_str() );

	int frame_number = 1;
	while( image != NULL )
	{
		images.push_back( image );
		if( image == NULL )
		{ break; }
		frame_number++;
		image_name = directory + icon_name + "-" + MakeStringFromInt(frame_number) + extension;
		image = vgui_LoadTGA( image_name.c_str() );
	}
}