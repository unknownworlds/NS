//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHVisibleBlipList.h $
// $Date: 2002/07/23 17:34:59 $
//
//-------------------------------------------------------------------------------
// $Log: AvHVisibleBlipList.h,v $
// Revision 1.7  2002/07/23 17:34:59  Flayra
// - Updates for new hive sight info
//
// Revision 1.6  2002/07/08 17:21:57  Flayra
// - Allow up to 64 blips, don't ASSERT if we exceed them (trying to draw all physents from shared code on client and server)
//
// Revision 1.5  2002/07/01 21:22:09  Flayra
// - Reworked sprites to be simpler and to allow more types
//
// Revision 1.4  2002/05/28 18:13:02  Flayra
// - New hive sight (different states for different targets)
//
// Revision 1.3  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_VISIBLE_BLIP_LIST_H
#define AVH_VISIBLE_BLIP_LIST_H

#include "types.h"

#ifdef AVH_CLIENT
#include "cl_dll/util_vector.h"
#include "mod/AvHBlipConstants.h"
#include <p_vector.h>
#endif

const int	kMaxBlips = 64;

class AvHVisibleBlipList
{
public:
				AvHVisibleBlipList();

	void		AddBlip(float inX, float inY, float inZ, int8 inStatus = 0, int8 inBlipInfo = 0);
	void		AddBlipList(const AvHVisibleBlipList& other);

	void		Clear();

	#ifdef AVH_CLIENT
	void		Draw(const pVector& inView, int kDefaultStatus);
				
	void		SetTimeBlipsReceived(float inTime);

	void		VidInit();
	#endif

	bool		operator==(const AvHVisibleBlipList& inList);
	bool		operator!=(const AvHVisibleBlipList& inList);
	
	int			mNumBlips;
	float		mBlipPositions[kMaxBlips][3];
	int8		mBlipStatus[kMaxBlips];
	int8		mBlipInfo[kMaxBlips];

private:
	#ifdef AVH_CLIENT
	int			mSprite[kNumBlipTypes];
	float		mTimeBlipsReceived;
	#endif

};

#endif