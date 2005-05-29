#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "common/com_model.h"
#include "cl_dll/demo.h"
#include "cl_dll/r_studioint.h"
#include "common/demo_api.h"
#include "mod/AvHSpriteAPI.h"

extern vec3_t gPlaybackViewOrigin;
extern engine_studio_api_t IEngineStudio;

struct Vertex
{
    float x;
    float y;
    float u;
    float v;
};

// This is the amount that the depth is increased each time a sprite
// is rendered. This eliminates z-fighting.
const float kDepthIncrement = 0.001f;

Vector gViewOrigin;
Vector gViewXAxis;
Vector gViewYAxis;
Vector gViewZAxis;

int                 gRenderMode;
AvHSpriteDrawMode   gDrawMode;

float gTransform[2][3];

float gClipRectX1;
float gClipRectY1;
float gClipRectX2;
float gClipRectY2;

bool gClippingEnabled;

float gDepth;
float gDepthOffset;

float gColor[4];

bool gVGUIEnabled = true;
int  gVGUIOffsetX = 0;
int  gVGUIOffsetY = 0;

float gViewportXScale = 1;
float gViewportYScale = 1;
float gViewportXOffset = 0;
float gViewportYOffset = 0;

void AvHSpriteBeginFrame()
{

    // Compute the camera parameters.

    Vector origin;

	if (gEngfuncs.pDemoAPI->IsPlayingback())
	{
		VectorCopy(gPlaybackViewOrigin, origin);
	}
    else
    {
        VectorCopy(v_origin, origin);
    }

    float nearDistance = 12;
    float aspectRatio  = (float)(ScreenHeight()) / ScreenWidth();

    float w = nearDistance * tan(M_PI * gHUD.m_iFOV / 360);
    float h = nearDistance * tan(M_PI * gHUD.m_iFOV / 360) * aspectRatio;

    Vector forward;
    Vector right;
    Vector up;
    
    AngleVectors(v_angles, forward, right, up);

    gViewOrigin = origin + forward * nearDistance - right * w + up * h;
    gViewXAxis =  2 * w * right / ScreenWidth();
    gViewYAxis = -2 * h * up / ScreenHeight();
    gViewZAxis = -forward;

    gRenderMode = kRenderNormal;

    // Initialize the transform to an identity transform.

    gTransform[0][0] = 1;
    gTransform[0][1] = 0;
    gTransform[0][2] = 0;

    gTransform[1][0] = 0;
    gTransform[1][1] = 1;
    gTransform[1][2] = 0;

    // Initialize the clipping rectangle to the whole screen.
    
    gClipRectX1 = 0;
    gClipRectY1 = 0;
    gClipRectX2 = ScreenWidth();
    gClipRectY2 = ScreenHeight();
    
    gClippingEnabled = false;

    gDepth = 0;
    gDepthOffset = 0;

    gColor[0] = 1;
    gColor[1] = 1;
    gColor[2] = 1;
    gColor[3] = 1;

    gDrawMode = kSpriteDrawModeFilled;

}

void AvHSpriteEndFrame()
{
    // Restore the state to something normal, otherwise weird things happen.
    gEngfuncs.pTriAPI->Color4f(1, 1, 1, 1);

    gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
    gEngfuncs.pTriAPI->CullFace(TRI_NONE);

}

void AvHSpriteSetViewport(float x1, float y1, float x2, float y2)
{

    gViewportXScale = (x2 - x1) / ScreenWidth();
    gViewportYScale = (y2 - y1) / ScreenHeight();

    gViewportXOffset = x1;
    gViewportYOffset = y1;
    
}

void AvHSpriteClearViewport()
{
    gViewportXScale = 1;
    gViewportYScale = 1;
    gViewportXOffset = 0;
    gViewportYOffset = 0;
}

void AvHSpriteEnableClippingRect(bool enable)
{
    gClippingEnabled = enable;
}

void AvHSpriteSetClippingRect(float x1, float y1, float x2, float y2)
{
    gClipRectX1 = x1;
    gClipRectY1 = y1;
    gClipRectX2 = x2;
    gClipRectY2 = y2;
}

void AvHSpriteSetRenderMode(int renderMode)
{
    gRenderMode = renderMode;
}

void AvHSpriteSetDrawMode(AvHSpriteDrawMode drawMode)
{
    gDrawMode = drawMode;
}

void AvHSpriteSetRotation(float degrees, float cx, float cy)
{

    float radians = degrees * M_PI / 180;

    float sa = sinf(radians);
    float ca = cosf(radians);

    gTransform[0][0] = ca;
    gTransform[1][0] = sa;

    gTransform[0][1] = -sa;
    gTransform[1][1] =  ca;

    gTransform[0][2] = -ca * cx + sa * cy + cx;
    gTransform[1][2] = -sa * cx - ca * cy + cy;

}

void AvHSpriteSetColor(float r, float g, float b, float a)
{
    gColor[0] = r;
    gColor[1] = g;
    gColor[2] = b;
    gColor[3] = a;
}

void AvHSpriteSetDepthOffset(float depthOffset)
{
    gDepthOffset = depthOffset;
}

/**
 * Clips the polygon against a hyper-plane of the form a * x + b * y + d = 0.
 * The resulting polyon is stored back in the input array.
 */
void ClipPolygon(Vertex vertex[8], int& numVertices, float a, float b, float d)
{

    Vertex newVertex[8];
    int newNumVertices = 0;

    int s = numVertices - 1;
    int e = 0;
    
    // Classify the start point.

    bool sInside = a * vertex[s].x + b * vertex[s].y + d >= 0;
    
    while (e != numVertices)
    {

        bool eInside = a * vertex[e].x + b * vertex[e].y + d >= 0;

        if (sInside && eInside)
        {
            // The edge is totally inside the rectangle.
            ASSERT(newNumVertices < 8);
            newVertex[newNumVertices] = vertex[e];
            ++newNumVertices;
        }
        else if (sInside || eInside)
        {
            
            // Compute the intersection vertex.

            float dx = vertex[e].x - vertex[s].x;
            float dy = vertex[e].y - vertex[s].y;
            
            float du = vertex[e].u - vertex[s].u;
            float dv = vertex[e].v - vertex[s].v;
            
            float n = -(a * vertex[s].x + b * vertex[s].y + d);
            float d = a * dx + b * dy;
            float t = n / d;
            
            Vertex i;

            i.x = vertex[s].x + t * dx;
            i.y = vertex[s].y + t * dy;

            i.u = vertex[s].u + t * du;
            i.v = vertex[s].v + t * dv;
            
            if (sInside)
            {
                
                // The edge starts inside and goes outside.
                
                ASSERT(newNumVertices < 8);
                newVertex[newNumVertices] = i;
                ++newNumVertices;

            }
            else
            {
                
                // The edge starts outside and goes inside.
                
                ASSERT(newNumVertices < 8);
                newVertex[newNumVertices] = i;
                ++newNumVertices;
                
                ASSERT(newNumVertices < 8);
                newVertex[newNumVertices] = vertex[e];
                ++newNumVertices;
            
            }
        }

        // Advance to the next edge.

        s = e;
        e = e + 1;

        sInside = eInside;

    }

    // Store the vertices back in the input array.

    memcpy(vertex, newVertex, newNumVertices * sizeof(Vertex));
    numVertices = newNumVertices;

}


void AvHSpriteDraw(int spriteHandle, int frame, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2)
{

    gEngfuncs.pTriAPI->RenderMode(gRenderMode);
    gEngfuncs.pTriAPI->CullFace(TRI_NONE);
    
    struct model_s* spritePtr = (struct model_s*)(gEngfuncs.GetSpritePointer(spriteHandle));
    ASSERT(spritePtr);

    if (!gEngfuncs.pTriAPI->SpriteTexture(spritePtr, frame))
    {
        return;
    }

    Vertex vertex[8];

    vertex[0].x = x1;
    vertex[0].y = y1;
    vertex[0].u = u1;
    vertex[0].v = v1;

    vertex[1].x = x2;
    vertex[1].y = y1;
    vertex[1].u = u2;
    vertex[1].v = v1;

    vertex[2].x = x2;
    vertex[2].y = y2;
    vertex[2].u = u2;
    vertex[2].v = v2;

    vertex[3].x = x1;
    vertex[3].y = y2;
    vertex[3].u = u1;
    vertex[3].v = v2;

    int numVertices = 4;

    float pw = SPR_Width(spriteHandle, frame);
    float ph = SPR_Height(spriteHandle, frame);

    float uOffset = 0;
    float vOffset = 0;

    if (IEngineStudio.IsHardware() == 2)
    {

        // Direct3D addresses textures differently than OpenGL so compensate
        // for that here.

        uOffset = 0.5 / pw;
        vOffset = 0.5 / ph;
  
    }

    // Apply the transformation to the vertices.
    
    for (int i = 0; i < numVertices; ++i)
    {

        if (vertex[i].u < 0.25 / pw)
        {
            vertex[i].u = 0.25 / pw;
        }

        if (vertex[i].v < 0.25 / ph)
        {
            vertex[i].v = 0.25 / ph;
        }

        if (vertex[i].u > 1 - 0.25 / pw)
        {
            vertex[i].u = 1 - 0.25 / pw;
        }

        if (vertex[i].v > 1 - 0.25 / ph)
        {
            vertex[i].v = 1 - 0.25 / ph;
        }

        vertex[i].u += uOffset;
        vertex[i].v += vOffset;
        
        float x = vertex[i].x;
        float y = vertex[i].y;
        
        vertex[i].x = x * gTransform[0][0] + y * gTransform[0][1] + gTransform[0][2];
        vertex[i].y = x * gTransform[1][0] + y * gTransform[1][1] + gTransform[1][2];

    }

    if (gClippingEnabled)
    {
    
        // Clip the polygon to each side of the clipping rectangle. This isn't the
        // fastest way to clip a polygon against a rectangle, but it's probably the
        // simplest.

        ClipPolygon(vertex, numVertices,  1,  0, -gClipRectX1);
        ClipPolygon(vertex, numVertices, -1,  0,  gClipRectX2);
        ClipPolygon(vertex, numVertices,  0,  1, -gClipRectY1);
        ClipPolygon(vertex, numVertices,  0, -1,  gClipRectY2);

    }

    // Compensate for the overbrightening effect.

	float gammaScale = 1.0f / gHUD.GetGammaSlope();
	gEngfuncs.pTriAPI->Color4f(gammaScale * gColor[0], gammaScale * gColor[1], gammaScale * gColor[2], gColor[3]);

    // Output the vertices.

    if (gDrawMode == kSpriteDrawModeFilled)
    {
        gEngfuncs.pTriAPI->Begin(TRI_TRIANGLE_FAN);

        for (int j = 0; j < numVertices; ++j)
        {

            Vector worldPoint;
            
            if (gVGUIEnabled)
            {
                worldPoint.x = (vertex[j].x - gVGUIOffsetX) * gViewportXScale + gViewportXOffset;
                worldPoint.y = (vertex[j].y - gVGUIOffsetY) * gViewportYScale + gViewportYOffset;
                worldPoint.z = 1;
            }
            else
            {
                worldPoint = gViewOrigin +
                    gViewXAxis * vertex[j].x +
                    gViewYAxis * vertex[j].y +
                    gViewZAxis * (gDepth + gDepthOffset);
            }

            gEngfuncs.pTriAPI->TexCoord2f(vertex[j].u, vertex[j].v);
            gEngfuncs.pTriAPI->Vertex3fv((float*)&worldPoint);

        }
    
        gEngfuncs.pTriAPI->End();

    }
    else if (gDrawMode == kSpriteDrawModeBorder)
    {

        gEngfuncs.pTriAPI->Begin(TRI_LINES);

        for (int j = 0; j < numVertices; ++j)
        {

            int k = (j + 1) % numVertices;

            Vector worldPoint1;
            Vector worldPoint2;
            
            if (gVGUIEnabled)
            {

                worldPoint1.x = vertex[j].x - gVGUIOffsetX;
                worldPoint1.y = vertex[j].y - gVGUIOffsetY;
                worldPoint1.z = 1;

                worldPoint2.x = vertex[k].x - gVGUIOffsetX;
                worldPoint2.y = vertex[k].y - gVGUIOffsetY;
                worldPoint2.z = 1;
            
            }
            else
            {
                
                worldPoint1 = gViewOrigin +
                    gViewXAxis * vertex[j].x +
                    gViewYAxis * vertex[j].y +
                    gViewZAxis * (gDepth + gDepthOffset);

                worldPoint2 = gViewOrigin +
                    gViewXAxis * vertex[k].x +
                    gViewYAxis * vertex[k].y +
                    gViewZAxis * (gDepth + gDepthOffset);
            
            }

            gEngfuncs.pTriAPI->TexCoord2f(vertex[j].u, vertex[j].v);
            gEngfuncs.pTriAPI->Vertex3fv((float*)&worldPoint1);

            gEngfuncs.pTriAPI->TexCoord2f(vertex[k].u, vertex[k].v);
            gEngfuncs.pTriAPI->Vertex3fv((float*)&worldPoint2);
        
        }

        gEngfuncs.pTriAPI->End();

    }

    gDepth += kDepthIncrement; 

}

void AvHSpriteDrawTiles(int spriteHandle, int numXFrames, int numYFrames, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2)
{
    
    float dx = x2 - x1;
    float dy = y2 - y1;

    float oldDepth = gDepth;

    for (int frameY = 0; frameY < numYFrames; ++frameY)
    {
        for (int frameX = 0; frameX < numXFrames; ++frameX)
        {

            int frame = frameX + frameY * numXFrames;

            float pw = SPR_Width(spriteHandle, frame);
            float ph = SPR_Height(spriteHandle, frame);
            
            float fx1 = ((float)(frameX)) / numXFrames;
            float fy1 = ((float)(frameY)) / numYFrames;
            
            float fx2 = ((float)(frameX + 1)) / numXFrames;
            float fy2 = ((float)(frameY + 1)) / numYFrames;

            gDepth = oldDepth;

            AvHSpriteDraw(spriteHandle, frame,
                 x1 + dx * fx1, y1 + dy * fy1, x1 + dx * fx2, y1 + dy * fy2,
                 0, 0, 1, 1);

        }
    }

}

void AvHSpriteEnableVGUI(bool enableVGUI)
{
    gVGUIEnabled = enableVGUI;
}

void AvHSpriteSetVGUIOffset(int x, int y)
{
    gVGUIOffsetX = x;
    gVGUIOffsetY = y;
}
