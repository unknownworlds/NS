// Triangle rendering, if any

#include "hud.h"
#include "cl_util.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/demo.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "APIProxy.h"
#include "Exports.h"
#include "particles/p_vector.h"
#include "particles/papi.h"
#include "mod/AvHParticleSystemManager.h"

#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_movevars.h"
#include "mod/AvHEvents.h"
#include "mod/AvHScriptManager.h"
#include "mod/AvHClientVariables.h"

#include "common/com_model.h"
#include "mod/CollisionUtil.h"


extern playermove_t *pmove;

extern vec3_t v_origin;
extern vec3_t gLastCommanderViewpoint;

// A fountain
/*
=================
Draw_Triangles

  Example routine.  Draws a sprite offset from the player origin.
  =================
*/
//void Draw_Triangles( void )
//{
//	cl_entity_t *player;
//	vec3_t org;
//	
//	// Load it up with some bogus data
//	player = gEngfuncs.GetLocalPlayer();
//	if ( !player )
//		return;
//	
//	org = player->origin;
//	
//	org.x += 0;
//	org.y += 0;
//
//	pVector theView(player->angles.x, player->angles.y, player->angles.z);
//	theView.normalize();
//	pVector theUp(0.0f, 0.0f, 1.0f);
//	pVector theBase(org.x, org.y, org.z);
//
//	// Do what the particles do.
//	ComputeParticles(theBase);
//
//	// Create a triangle, sigh
//	//gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
//	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
//
//	gEngfuncs.pTriAPI->Brightness( 1 );
//
//	if (gHUD.m_hsprCursor == 0)
//	{
//		char sz[256];
//		//sprintf( sz, "sprites/cursor.spr" );
//		//sprintf( sz, "sprites/myrain.spr" );
//		sprintf( sz, "sprites/haze2.spr" );
//		gHUD.m_hsprCursor = SPR_Load( sz );
//	}
//	
//	if ( !gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)gEngfuncs.GetSpritePointer( gHUD.m_hsprCursor ), 0 ))
//	{
//		return;
//	}
//
//	gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd  );
//	gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, .5f);
//	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
//	DrawGroupTriSplat(theView, theUp, 70, true, true, true);
//	
//	//// Create a triangle, sigh
//	//gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
//	//
//	//gEngfuncs.pTriAPI->Begin( TRI_QUADS );
//	//// Overload p->color with index into tracer palette, p->packedColor with brightness
//	//gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, 1.0 );
//	//// UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
//	//gEngfuncs.pTriAPI->Brightness( 1 );
//	//gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
//	//gEngfuncs.pTriAPI->Vertex3f( org.x, org.y, org.z );
//	//
//	//gEngfuncs.pTriAPI->Brightness( 1 );
//	//gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
//	//gEngfuncs.pTriAPI->Vertex3f( org.x, org.y + 50, org.z );
//	//
//	//gEngfuncs.pTriAPI->Brightness( 1 );
//	//gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
//	//gEngfuncs.pTriAPI->Vertex3f( org.x + 50, org.y + 50, org.z );
//	//
//	//gEngfuncs.pTriAPI->Brightness( 1 );
//	//gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
//	//gEngfuncs.pTriAPI->Vertex3f( org.x + 50, org.y, org.z );
//	
//
//	gEngfuncs.pTriAPI->End();
//	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
//}	

//#endif

void AngleMatrix (const vec3_t angles, float (*matrix)[4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[2] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);
	
	// matrix = (Z * Y) * X
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

void VectorRotate (const vec3_t in1, const float in2[3][4], vec3_t& out)
{
	out[0] = DotProduct(in1, in2[0]);
	out[1] = DotProduct(in1, in2[1]);
	out[2] = DotProduct(in1, in2[2]);
}

/*
=================
HUD_DrawNormalTriangles

  Non-transparent triangles-- add them here
  =================
*/
void CL_DLLEXPORT HUD_DrawNormalTriangles( void )
{
	RecClDrawNormalTriangles();
	
	//	pVector			theView;
	//	cl_entity_t*	thePlayer;
	//	
	//	thePlayer = gEngfuncs.GetLocalPlayer();
	//	if(thePlayer)
	//	{	
	//		pVector theView(thePlayer->angles.x, thePlayer->angles.y, thePlayer->angles.z);
	//		theView.normalize();
	//
	//		//AvHParticleSystemManager::Instance()->Draw(theView);
	//		static int theAngle = 0;
	//		//static HSPRITE theSprite = 0;
	//		DrawCircleOnGroundAtPoint(thePlayer->origin, 6, theAngle++, 100, .5f, .5f, 1.0f, .5f);
	//	}
	
	gHUD.PreRenderFrame();
	
	//gHUD.m_Spectator.DrawOverview();
	
#if defined( TEST_IT )
	//	Draw_Triangles();
#endif
}


void DrawHitBox(const OBBox& inBox)
{

    HSPRITE sprite = Safe_SPR_Load("sprites/white.spr");

    vec3_t theBoxPoint[8];

    for (int i = 0; i < 8; ++i)
    {

        vec3_t xAmount;
        vec3_t yAmount;
        vec3_t zAmount;

        if (i & 1)
        {
            VectorScale(inBox.mAxis[0],  inBox.mExtents[0], xAmount);
        }
        else
        {
            VectorScale(inBox.mAxis[0], -inBox.mExtents[0], xAmount);
        }
        
        if (i & 2)
        {
            VectorScale(inBox.mAxis[1],  inBox.mExtents[1], yAmount);
        }
        else
        {
            VectorScale(inBox.mAxis[1], -inBox.mExtents[1], yAmount);
        }
        
        if (i & 4)
        {
            VectorScale(inBox.mAxis[2],  inBox.mExtents[2], zAmount);
        }
        else
        {
            VectorScale(inBox.mAxis[2], -inBox.mExtents[2], zAmount);
        }
        
        VectorAdd(inBox.mOrigin,  xAmount, theBoxPoint[i]);
        VectorAdd(theBoxPoint[i], yAmount, theBoxPoint[i]);
        VectorAdd(theBoxPoint[i], zAmount, theBoxPoint[i]);
        
    }
    
    struct model_s* spritePtr = (struct model_s*)(gEngfuncs.GetSpritePointer(sprite));
    gEngfuncs.pTriAPI->SpriteTexture(spritePtr, 0);

    gEngfuncs.pTriAPI->Begin(TRI_LINES);

    gEngfuncs.pTriAPI->Color4f(1, 1, 1, 1);

    // Bottom.
        
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[0]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[1]);
    
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[1]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[3]);
    
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[3]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[2]);

    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[2]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[0]);

    // Top.

    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[4]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[6]);
    
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[6]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[7]);
    
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[7]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[5]);

    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[5]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[4]);

    // Sides.

    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[0]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[4]);
    
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[2]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[6]);
    
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[3]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[7]);

    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[1]);
    gEngfuncs.pTriAPI->Vertex3fv(theBoxPoint[5]);

    gEngfuncs.pTriAPI->End();

}


void DrawHitBoxes()
{

    extern int cam_thirdperson;

    for (int j = 1; j < 512; ++j)
    {

        cl_entity_t* entity = gEngfuncs.GetEntityByIndex(j);

        if (entity == gEngfuncs.GetLocalPlayer() && !cam_thirdperson)
        {
            continue;
        }

        if (entity->curstate.effects & EF_NODRAW)
        {
            continue;
        }

        if (entity != NULL && entity->model != NULL && entity->model->type == mod_studio)
        {
        
            int theEntityIndex = j;

            OBBox theBox[256];
            int   theNumBoxes;

            NS_GetHitBoxesForEntity(theEntityIndex, 256, theBox, theNumBoxes, gEngfuncs.GetClientTime());

            for (int i = 0; i < theNumBoxes; ++i)
            {
                DrawHitBox(theBox[i]);
            }
        
        }
    
    }
    
}
 


/*

  =================
  HUD_DrawTransparentTriangles
  
	Render any triangles with transparent rendermode needs here
	=================
*/
void CL_DLLEXPORT HUD_DrawTransparentTriangles( void )
{
	RecClDrawTransparentTriangles();
	
	cl_entity_t*	thePlayer;
	
	if (gHUD.GetParticlesVisible() && gHUD.GetSafeForSpriteDrawing())
	{
		// get local player
		thePlayer = gEngfuncs.GetLocalPlayer();
		if(thePlayer)
		{
			//pVector theView(thePlayer->angles.x, thePlayer->angles.y, thePlayer->angles.z);
			//pVector theView(viewangles[0], viewangles[1], viewangles[2]);
			//theView.normalize();
			
			vec3_t angles, up, right, forward;
			//		gEngfuncs.pfnAngleVectors(thePlayer->angles, forward, right, up);
			
			// This view doesn't take pitch into account.  Pitch is stored in x.  While yaw goes from -180 to 180,
			// pitch goes from -10 (looking up) to 10 (looking down).  Alter forward[YAW} (forward[0]) so 
			// the particles can billboard themselves correctly.
			
			//		float thePitch = thePlayer->angles[PITCH];
			//		float theNormAngle = thePitch/-10.0f;
			//		int theAngle = theNormAngle*90;
			//		//float theYaw = thePlayer->angles[YAW];
			//
			//		vec3_t theRotationAngles;
			//		theRotationAngles[0] = theAngle;
			//		theRotationAngles[1] = 0;
			//		theRotationAngles[2] = 0;
			//
			//		float theMatrix[3][4];
			//		AngleMatrix(theRotationAngles, theMatrix);
			//
			//		vec3_t theOut;
			//		VectorRotate(forward, theMatrix, theOut);
			//		pVector theRealView(theOut[0], theOut[1], theOut[2]);
			
			pVector theRealView;
            gEngfuncs.pfnAngleVectors(v_angles, forward, right, up);
			
			theRealView.x = forward[0];
			theRealView.y = forward[1];
			theRealView.z = forward[2];
			
			// Disable fog
			float theFogColor[3];
			theFogColor[0] = theFogColor[1] = theFogColor[2] = 0.0f;
			gEngfuncs.pTriAPI->Fog(theFogColor, 0, 0, 0);
		
			// Draw particles via tri API, without z-buffering, unless we're debugging
			//if(cl_particleinfo->value == 0)
			//{
				AvHParticleSystemManager::Instance()->Draw(theRealView);
			//}
			
			//DrawOrdersForPlayers(gHUD.GetDrawPlayerOrders());
			
			//DrawRangeIndicator();
			
			//DrawBlips(theRealView);
			
			//DrawMarineLights(theRealView);
		
			AvHScriptManager::Instance()->DrawTransparent();
			
			//DrawDebugEffects();
			
			//static int theAngle = 0;
			//DrawCircleOnGroundAtPoint(thePlayer->origin, 6, theAngle++, 100, 1.0f, 1.0f, 1.0f, .2f);
		}

        // Draw the hitboxes for all of the objects.

/*
#ifdef AVH_PLAYTEST_BUILD
        if (CVAR_GET_FLOAT("sv_cheats") != 0 && CVAR_GET_FLOAT("cl_drawhitboxes") != 0)
        {
            DrawHitBoxes();
        }
#endif
*/        
		// Assumes that the only time players don't have view models is when they are commanding, gestating, cocooned
		// or when they aren't playing.  See the end of StudioModelRenderer::StudioDrawModel().
		//if((gHUD.GetRole() == ROLE_COMMANDER) || (gHUD.GetRole() == ROLE_GESTATING) || (gHUD.GetRole() == ROLE_COCOONED) || (gHUD.GetPlayMode() != PLAYMODE_PLAYING))
		cl_entity_t* theViewEntity = gEngfuncs.GetViewModel();
		if(!theViewEntity || !theViewEntity->model)
		{
			// This should always be rendered with the commander viewpoint
//			vec3_t theCurrentOrigin = v_origin;
//		
//			if(gHUD.GetInTopDownMode())
//			{
//				v_origin = gLastCommanderViewpoint;
//			}
			
			gHUD.RenderNoZBuffering();
		
//			if(gHUD.GetInTopDownMode())
//			{
//				v_origin = theCurrentOrigin;
//			}
		}
		
		gHUD.PostRenderFrame();
	}
}