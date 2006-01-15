#include "mod/LineDrawer.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"

int vguiRound(float inFloat)
{
    return (int)(inFloat + .5f);
}

int vguiAbs(int inNum)
{
    return (inNum > -inNum ? inNum : -inNum);
}

void vguiSimpleLine(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
    //public void lineImproved(int x0, int y0, int x1, int y1, Color color)
    //{
        //int pix = color.getRGB();
        int dx = x1 - x0;
        int dy = y1 - y0;

        //raster.setPixel(pix, x0, y0);
        FillRGBA(x0, y0, 1, 1, r, g, b, a);
        //if (Math.abs(dx) > Math.abs(dy)) {          // slope < 1
        if(vguiAbs(dx) > vguiAbs(dy))
        {
            float m = (float) dy / (float) dx;      // compute slope
            float b = y0 - m*x0;
            dx = (dx < 0) ? -1 : 1;
            while (x0 != x1) 
            {
                x0 += dx;
                //raster.setPixel(pix, x0, Math.round(m*x0 + b));
                FillRGBA(x0, vguiRound(m*x0 + b), 1, 1, r, g, b, a);
            }
        } else
        if (dy != 0) 
        {                              // slope >= 1
            float m = (float) dx / (float) dy;      // compute slope
            float b = x0 - m*y0;
            dy = (dy < 0) ? -1 : 1;
            while (y0 != y1) 
            {
                y0 += dy;
                //raster.setPixel(pix, Math.round(m*y0 + b), y0);
                FillRGBA(vguiRound(m*y0 + b), y0, 1, 1, r, g, b, a);
            }
        }
    //}
}

void vguiSimpleBox(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	// Draw lines around edges of box, don't duplicate corner pixels though, looks weird where additive

	// Top
	vguiSimpleLine(x0, y0, x1, y0, r, g, b, a);
	
	// Left
	vguiSimpleLine(x0, y0+1, x0, y1, r, g, b, a);
	
	// Right
	vguiSimpleLine(x1, y0+1, x1, y1, r, g, b, a);
	
	// Bottom
	vguiSimpleLine(x0+1, y1, x1, y1, r, g, b, a);
}

//int trunc(float inX)
//{
//    return 0;
//	//return integer part of x
//}
//
//float frac(float inX)
//{
//    return 0.0f;
//	//return fractional part of x
//}
// 
//float invfrac(inX)
//{
//    return 0.0f;
//	//return 1 - (fractional part of x)
//}
//
//
//void vguiWuLine(int x1, int y1, int x2, int y2)
//{
//
//        variable declerations:
//            int variables:
//		grad, xd, yd, length,xm,ym
//		xgap, ygap, xend, yend, xf, yf
//		brigheness1, brigheness2
//
//	    integer variables:
//		x, y, ix1, ix2, iy1, iy2
//
//	    byte variables:
//		c1,c2
//
//	code starts here:
//
//	    Width and Height of the line
//	    xd = (x2-x1)
//	    yd = (y2-y1)
//	
//	   
//	    if abs(xd) > abs(yd) then			check line gradient
//	        horizontal(ish) lines
//
//
//		if x1 > x2 then				if line is back to front
//		    swap x1 and x2			then swap it round
//		    swap y1 and y2
//	            xd = (x2-x1)				and recalc xd & yd
//	            yd = (y2-y1)
//		end if
//
//	 	grad = yd/xd                             gradient of the line
//		
//
//		End Point 1
//		-----------
//
//		xend = trunc(x1+.5)                      find nearest integer X-coordinate
//		yend = y1 + grad*(xend-x1)               and corresponding Y value
//		
//		xgap = invfrac(x1+.5)                    distance i
//		
//		ix1  = int(xend)                         calc screen coordinates
//		iy1  = int(yend)
//	
//		brightness1 = invfrac(yend) * xgap       calc the intensity of the other 
//		brightness2 =    frac(yend) * xgap       end point pixel pair.
//		
//		c1 = byte(brightness1 * MaxPixelValue)	 calc pixel values
//		c2 = byte(brightness2 * MaxPixelValue)	
//
//		DrawPixel(ix1,iy1), c1			 draw the pair of pixels
//		DrawPixel(ix1,iy1+1), c2
//
//		yf = yend+grad                           calc first Y-intersection for
//                                                         main loop
//
//		End Point 2
//		-----------
//
//		xend = trunc(x2+.5)                      find nearest integer X-coordinate
//		yend = y2 + grad*(xend-x2)               and corresponding Y value
//		
//		xgap = invfrac(x2-.5)                    distance i
//		
//		ix2  = int(xend)                         calc screen coordinates
//		iy2  = int(yend)
//	
//		brightness1 = invfrac(yend) * xgap       calc the intensity of the first 
//		brightness2 =    frac(yend) * xgap       end point pixel pair.
//		
//		c1 = byte(brightness1 * MaxPixelValue)	calc pixel values
//		c2 = byte(brightness2 * MaxPixelValue)	
//
//		DrawPixel(ix2,iy2), c1			draw the pair of pixels
//		DrawPixel(ix2,iy2+1), c2
//
//
//
//		MAIN LOOP
//		---------
//
//		Loop x from (ix1+1) to (ix2-1)			main loop
//
//		    brightness1 = invfrac(yf)		        calc pixel brightnesses
//		    brightness2 =    frac(yf)
//
//  		    c1 = byte(brightness1 * MaxPixelValue)	calc pixel values
//		    c2 = byte(brightness2 * MaxPixelValue)	
//
//		    DrawPixel(x,int(yf)), c1			draw the pair of pixels
// 		    DrawPixel(x,int(yf)+1), c2
//
//	            yf = yf + grad				update the y-coordinate
//
//		end of x loop					end of loop
//
//	    else
//		vertical(ish) lines
//
//		handle the vertical(ish) lines in the
//		same way as the horizontal(ish) ones
//		but swap the roles of X and Y
//	    end if
//// end of procedure
//}


//void vguiDrawLine(int x0, int y0, int x1, int y1, vgui::Color inDrawColor) {
//    int dx, dy, temp;
//    
//    // First clip input points to ICanvas
//    if(x0 < 0) {
//        x0 = 0;
//    }
//    if(x1 < 0) {
//        x1 = 0;
//    }
//    if(x0 >= width) {
//        x0 = width - 1;
//    }
//    if(x1 >= width) {
//        x1 = width - 1;
//    }
//    if(y0 < 0) {
//        y0 = 0;
//    }
//    if(y1 < 0) {
//        y1 = 0;
//    }
//    if(y0 >= height) {
//        y0 = height - 1;
//    }
//    if(y1 >= height) {
//        y1 = height - 1;
//    }
//    
//    dx = x1-x0;
//    dy = y1-y0;
//    
//    if(y0 == y1) { // Special case horizontal lines!
//        if(dx < 0) {
//            dx = -dx; // Change to dx = -dx;
//            x0 = x1;
//        }
//        // Call asm_drawHorizontalLine
//        drawHorizontalLine(x0, y0, dx, inDrawColor);
//        return;
//    }
//    
//    if(x0 == x1) {
//        if(dy < 0) {
//            dy = -dy;
//            y0 = y1;
//        }
//        // Call asm_drawVerticalLine
//        drawVerticalLine(x0, y0, dy, inDrawColor);
//        return;
//    }
//    
//    bufferDest = (unsigned long)(buffer + y0*width + x0);
//    if(fabs(dx) > fabs(dy)) {
//        // We're going to call octant0
//        oldYFract = (float)((float)(y1 - y0)/(float)fabs(x1 - x0));
//        bufferWidth = width;
//        if(oldYFract < 0) {
//            oldYFract = -oldYFract; // Probably not needed cause sign bit shifted out
//            bufferWidth = -width;
//        }
//        oldYFract++;
//        if(x0 < x1) {
//            drawDirection = 1;
//        } else {
//            drawDirection = -1;
//        }
//        if(fabs(dx) != 0) {
//            octant0(fabs(dx));
//        }
//    } else {
//        // We're going to call octant1
//        oldXFract = (float)((float)(x1 - x0)/(float)fabs((y1 - y0)));
//        oldXFract++;
//        drawDirection = 1;
//        if(oldXFract < 0) {
//            oldXFract = -oldXFract;
//            drawDirection = -1;
//        }
//        if(y0 < y1) {
//            bufferWidth = width;
//        } else {
//            bufferWidth = -width;
//        }
//        if(fabs(dy) != 0) {
//            octant1(fabs(dy));
//        }
//    }
//}

//void vguiOctant0(int inBufferDest, int inDeltaX, vgui::Color inDrawColor)
//{
//  asm_octant0_    PROC    NEAR
//    ; Set up destination
//    mov edi,[_bufferDest]
//    mov bl,[_drawColor]


//
//    ; Set up fractional error
//    mov eax,[_oldYFract]
//    shl eax,9
//    xor ax,ax
//    mov [_yFract],eax
//;    xor eax,eax
//    xor edx,edx
//
//NOTDONEOCTANT0:
//    mov [edi],bl
//    ;inc edi
//    add edi,[_drawDirection]
//
//    ; Now just figure out whether we go to the next line or not
//    add edx,[_yFract]
//    jnc DONTADDOCTANT0
//    add edi,[_bufferWidth]
//DONTADDOCTANT0:
//    sub ecx,1
//    jnz NOTDONEOCTANT0
//    ret
//}