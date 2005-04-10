//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHVisibleBlipList.cpp $
// $Date: 2002/09/25 20:52:34 $
//
//-------------------------------------------------------------------------------
// $Log: AvHVisibleBlipList.cpp,v $
// Revision 1.11  2002/09/25 20:52:34  Flayra
// - Refactoring
//
// Revision 1.10  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.9  2002/07/23 17:34:59  Flayra
// - Updates for new hive sight info
//
// Revision 1.8  2002/07/08 17:21:57  Flayra
// - Allow up to 64 blips, don't ASSERT if we exceed them (trying to draw all physents from shared code on client and server)
//
// Revision 1.7  2002/07/01 21:22:09  Flayra
// - Reworked sprites to be simpler and to allow more types
//
// Revision 1.6  2002/06/10 20:04:39  Flayra
// - Blip list wasn't being cleared properly (?)
//
// Revision 1.5  2002/05/28 18:13:02  Flayra
// - New hive sight (different states for different targets)
//
// Revision 1.4  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHVisibleBlipList.h"

#ifdef AVH_CLIENT
#include "common/triangleapi.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"

#include "cl_dll/cl_util.h"
#include "cl_dll/util_vector.h"
#include "common/renderingconst.h"
#include "common/const.h"
#include "engine/progdefs.h"
#include "engine/edict.h"
#include "pm_shared/pm_defs.h"
#include "engine/cdll_int.h"
#include "common/event_api.h"
#include "common/cl_entity.h"
#include <particledefs.h>
#include <p_vector.h>
#include <papi.h>
#include "common/usercmd.h"
#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"
#include "pm_shared/pm_movevars.h"
#include "pm_shared/pm_debug.h"

#include "mod/AvHParticleSystemManager.h"
#include "mod/AvHHudConstants.h"
#include "cl_dll/ev_hldm.h"
#include "cl_dll/hud.h"
#include "util/STLUtil.h"

void AvHVisibleBlipList::Draw(const pVector& inView, int kDefaultStatus)
{
	//bool theIsEnemy = this->GetType() == BLIP_TYPE_ENEMY;
	
	// Now draw the thing!
	for(int theCurrentBlip = 0; theCurrentBlip < this->mNumBlips; theCurrentBlip++)
	{
		int theStatus = this->mBlipStatus[theCurrentBlip];
		ASSERT(theStatus >= 0);
		ASSERT(theStatus < kNumBlipTypes);

		// This has to be reloaded too
		if(!this->mSprite[theStatus])
		{
			string theSpriteName = string(kSpriteDirectory) + string("/") + string(kBlipSprite) + MakeStringFromInt(theStatus) + string(".spr");
			this->mSprite[theStatus] = Safe_SPR_Load(theSpriteName.c_str());
		}

		int theSprite = this->mSprite[theStatus];
		if(!theSprite)
		{
			theSprite = this->mSprite[kDefaultStatus];
		}

		if(theSprite > 0)
		{
			int theNumFrames = SPR_Frames(theSprite);
			const int kFrameRate = 12;
			const float theCurrentTime = gEngfuncs.GetClientTime();
			int theFrame = (int)((theCurrentTime - this->mTimeBlipsReceived)*kFrameRate) % theNumFrames;
			
			if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(theSprite), theFrame))
			{
				//gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
				gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
			
				pVector up(0, 0, 1);
				pVector right = inView ^ up;
				right.normalize();
				pVector nup = right ^ inView;
				
				int theScale = kWorldBlipScale;
				right *= theScale/2.0f;
				nup *= theScale/2.0f;
				
				// Quad draws v0, v1, v2, v3
				pVector V0 = -(right + nup);
				pVector V1 = -(right - nup);
				pVector V2 = right + nup;
				pVector V3 = right - nup;
				
				pVector sV0 = V0;
				pVector sV1 = V1;
				pVector sV2 = V2;
				pVector sV3 = V3;
			
				gEngfuncs.pTriAPI->CullFace( TRI_NONE );
				
				gEngfuncs.pTriAPI->Begin( TRI_TRIANGLE_STRIP );
			
				gEngfuncs.pTriAPI->Brightness(1.0f);
				
					pVector p;
					p.x = this->mBlipPositions[theCurrentBlip][0];
					p.y = this->mBlipPositions[theCurrentBlip][1];
					p.z = this->mBlipPositions[theCurrentBlip][2];
					
					gEngfuncs.pTriAPI->TexCoord2f(0, 0);
					pVector ver = p + sV0;
					gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
			
					gEngfuncs.pTriAPI->TexCoord2f(0, 1);
					ver = p + sV1;
					gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
			
					gEngfuncs.pTriAPI->TexCoord2f(1, 0);
					ver = p + sV3;
					gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);

					gEngfuncs.pTriAPI->TexCoord2f(1, 1);
					ver = p + sV2;
					gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
				
				gEngfuncs.pTriAPI->End();
			
				gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
			}
		}
	}
}
#endif

#ifdef AVH_SERVER
#include "dlls/extdll.h"
#include "dlls/util.h"
#endif

AvHVisibleBlipList::AvHVisibleBlipList()
{
	this->Clear();
}

void AvHVisibleBlipList::AddBlip(float inX, float inY, float inZ, int8 inStatus, int8 inBlipInfo)
{
	//ASSERT(this->mNumBlips < kMaxBlips);
	if(this->mNumBlips < (kMaxBlips-1))
	{
		int theBlipOffset = this->mNumBlips;
		
		this->mBlipPositions[theBlipOffset][0] = inX;
		this->mBlipPositions[theBlipOffset][1] = inY;
		this->mBlipPositions[theBlipOffset][2] = inZ;
		this->mBlipStatus[theBlipOffset] = inStatus;
		this->mBlipInfo[theBlipOffset] = inBlipInfo;
		
		this->mNumBlips++;
	}
	else
	{
		#ifdef AVH_SERVER
		UTIL_LogPrintf("AvHVisibleBlipList::AddBlip(%f, %f, %f, status: %d):  Can't add blip, max limit %d reached.\n", inX, inY, inZ, inStatus, kMaxBlips);
		#endif
	}
}

void AvHVisibleBlipList::AddBlipList(const AvHVisibleBlipList& other)
{
	for(int copy_index = 0; copy_index < other.mNumBlips; copy_index++)
	{
		this->AddBlip( 
			other.mBlipPositions[copy_index][0],
			other.mBlipPositions[copy_index][1],
			other.mBlipPositions[copy_index][2],
			other.mBlipStatus[copy_index],
			other.mBlipInfo[copy_index]
		);
	}
}

void AvHVisibleBlipList::Clear()
{
	#ifdef AVH_CLIENT
	//memset(this->mSprite, 0, kNumBlipTypes*sizeof(int));
	this->mTimeBlipsReceived = -1;
	#endif

	this->mNumBlips = 0;
	
	memset(this->mBlipPositions, 0, sizeof(float)*kMaxBlips*3);
	memset(this->mBlipStatus, 0, sizeof(int8)*kMaxBlips);
	memset(this->mBlipInfo, 0, sizeof(int8)*kMaxBlips);
}

#ifdef AVH_CLIENT
void AvHVisibleBlipList::SetTimeBlipsReceived(float inTime)
{
	this->mTimeBlipsReceived = inTime;
}

void AvHVisibleBlipList::VidInit()
{
	memset(this->mSprite, 0, kNumBlipTypes*sizeof(int));
}
#endif

#define CHECK_EQUAL(x) (this->##x == inList.##x)
bool AvHVisibleBlipList::operator==(const AvHVisibleBlipList& inList)
{
	bool theAreEqual = CHECK_EQUAL(mNumBlips);
#ifdef AVH_CLIENT
	theAreEqual = theAreEqual && CHECK_EQUAL(mTimeBlipsReceived)
		&& !memcmp(this->mSprite, inList.mSprite, kNumBlipTypes*sizeof(int));
#endif

	for( int index = 0; theAreEqual && index < this->mNumBlips; ++index )
	{
		theAreEqual = CHECK_EQUAL(mBlipPositions[index][0])
			&& CHECK_EQUAL(mBlipPositions[index][1])
			&& CHECK_EQUAL(mBlipPositions[index][2])
			&& CHECK_EQUAL(mBlipStatus[index])
			&& CHECK_EQUAL(mBlipInfo[index]);
	}

	return theAreEqual;
}
#undef CHECK_EQUAL

bool AvHVisibleBlipList::operator!=(const AvHVisibleBlipList& inList)
{
	return !this->operator==(inList);
}
