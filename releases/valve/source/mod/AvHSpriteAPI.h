#ifndef AVHSPRITEAPI_H
#define AVHSPRITEAPI_H

enum AvHSpriteDrawMode
{
    kSpriteDrawModeFilled,
    kSpriteDrawModeBorder,
};

/**
 * Once the final coordinates are computed, they are transformed to this viewport.
 */
void AvHSpriteSetViewport(float x1, float y1, float x2, float y2);

/**
 * Clears any previously set viewport.
 */
void AvHSpriteClearViewport();


void AvHSpriteBeginFrame();
void AvHSpriteEndFrame();

/**
 * Sets the render mode used for subsequent sprite operations.
 */
void AvHSpriteSetRenderMode(int renderMode);

/**
 * Sets the rotation used for subsequence sprite operations. The rotation is
 * specified about a point in the screen (cx, cy).
 */
void AvHSpriteSetRotation(float degrees, float cx, float cy);

/**
 * Enables or disables the clipping rectangle. If the clipping rectangle is
 * disabled the sprites won't be clipping, however it will be faster.
 */
void AvHSpriteEnableClippingRect(bool enable);

/**
 * Sets the screen space rectangle which drawing operations are clipped against.
 */
void AvHSpriteSetClippingRect(float x1, float y1, float x2, float y2);

/**
 *
 */
void AvHSpriteSetDrawMode(AvHSpriteDrawMode drawMode);

/**
 * Sets the color which is multiplied by the sprite pixel values.
 */
void AvHSpriteSetColor(float r, float g, float b, float a = 1);

/**
 *
 */
void AvHSpriteSetDepthOffset(float depthOffset);

void AvHSpriteDraw(int spriteHandle, int frame, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2);
void AvHSpriteDrawTiles(int spriteHandle, int numXFrames, int numYFrames, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2);

void AvHSpriteEnableVGUI(bool enableVGUI);
void AvHSpriteSetVGUIOffset(int x, int y);

#endif