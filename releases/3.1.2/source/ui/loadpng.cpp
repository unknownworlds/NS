#include "png.h"
#include "loadpng.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "MemoryInputStream.h"

//alternate read function for libpng
void png_read_vgui_stream(png_structp png_ptr, png_bytep data, png_size_t length);

//conversion util to change from libpng format to raw bitmap
void convertToUChar(png_infop png_info, png_bytepp rows, uchar* data);

BitmapPNG* LoadPNG(char const *pFilename)
{
	int length;
	uchar* data = gEngfuncs.COM_LoadFile((char*)pFilename, 5, &length);
	if( !data ) { return NULL; }

	MemoryInputStream stream(data,length);
	
	BitmapPNG* pRet = LoadPNG(stream);

	gEngfuncs.COM_FreeFile(data);
	
	return pRet;		
}

BitmapPNG* LoadPNG(uchar* data, int length)
{
	if( !data ) { return NULL; }
	MemoryInputStream stream(data,length);
	return LoadPNG(stream);
}

BitmapPNG* LoadPNG(vgui::InputStream& stream)
{
	//allocate structures for read
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return NULL;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr,
           (png_infopp)NULL, (png_infopp)NULL);
        return NULL;
    }

	//set up our alternate read function
	png_set_read_fn(png_ptr,&stream,png_read_vgui_stream);

	//read the image
	png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16,NULL);
	png_bytepp rows = png_get_rows(png_ptr, info_ptr);
	png_read_image(png_ptr,rows);

	//we now have the information in row format, it needs to be converted to a single array
	//of 32bit pixel values specified as a uchar* so that we can create the image object
	uchar* data = new uchar[info_ptr->width*info_ptr->height*4];
	convertToUChar(info_ptr,rows,data);

	//create the BitmpPNG that we are going to return
	BitmapPNG* returnVal = new BitmapPNG(info_ptr->width,info_ptr->height,data);

	//read the information after the image for completeness
	png_read_end(png_ptr,NULL);

	//free up png structures and other memory we've used
	delete[] data;
	png_destroy_read_struct(&png_ptr,&info_ptr,(png_infopp)NULL);

	return returnVal;
}

//implementation of alternate read function
void png_read_vgui_stream(png_structp png_ptr, png_bytep data, png_size_t length)
{
	vgui::InputStream* stream = static_cast<vgui::InputStream*>(png_get_io_ptr(png_ptr));
	bool success = true;
	stream->readUChar(data,(int)length,success);
}

//implementation of conversion function
void convertToUChar(png_infop png_info, png_bytepp rows, uchar* data)
{
	int offset = 0;
	int offset_amnt = png_info->width*4;
	for(size_t counter = 0; counter < png_info->height; ++counter)
	{
		memcpy(&data[offset],rows[counter],offset_amnt);
		offset += offset_amnt;
	}
}