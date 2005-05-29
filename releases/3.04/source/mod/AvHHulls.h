#ifndef AVH_HULLS_H
#define AVH_HULLS_H

// HL hull values

// IMPORTANT: If you're changing any of these, hulls.txt must be changed and all maps recompiled

// Normal HL player
const int kHLStandHullIndex = 0;
#define HULL0_MINX -16
#define HULL0_MINY -16
#define HULL0_MINZ -36

#define HULL0_MAXX 16
#define HULL0_MAXY 16
#define HULL0_MAXZ 36

// Crouched HL player, also used for small aliens
const int kHLCrouchHullIndex = 1;
#define HULL1_MINX -16
#define HULL1_MINY -16
#define HULL1_MINZ -18

#define HULL1_MAXX 16
#define HULL1_MAXY 16
#define HULL1_MAXZ 18

// Point-hull, don't ever change
const int kHLPointHullIndex = 2;
#define HULL2_MINX 0
#define HULL2_MINY 0
#define HULL2_MINZ 0

#define HULL2_MAXX 0
#define HULL2_MAXY 0
#define HULL2_MAXZ 0


// Big alien hull
const int kNSHugeHullIndex = 3;
#define HULL3_MINX -32
#define HULL3_MINY -32
#define HULL3_MINZ -54

#define HULL3_MAXX 32
#define HULL3_MAXY 32
#define HULL3_MAXZ 54

const int kMaxPlayerHullWidth = 32;

// Percentage of standing hull to set view height to (used in AvHPlayer::SetViewForRole() and PM_Duck/PM_Unduck)
const float kStandingViewHeightPercentage = .77f;

// Percentage of ducking hull to set view height to (used in AvHPlayer::SetViewForRole() and PM_Duck/PM_Unduck)
const float kDuckingViewHeightPercentage = .66f;

// Standard Half-life hulls, don't touch
#define HULL0_MIN Vector(HULL0_MINX, HULL0_MINY, HULL0_MINZ)
#define HULL0_MAX Vector(HULL0_MAXX, HULL0_MAXY, HULL0_MAXZ)

#define HULL1_MIN Vector(HULL1_MINX, HULL1_MINY, HULL1_MINZ)
#define HULL1_MAX Vector(HULL1_MAXX, HULL1_MAXY, HULL1_MAXZ)

#define HULL2_MIN Vector(HULL2_MINX, HULL2_MINY, HULL2_MINZ)
#define HULL2_MAX Vector(HULL2_MAXX, HULL2_MAXY, HULL2_MAXZ)

// Custom NS hulls
#define HULL3_MIN Vector(HULL3_MINX, HULL3_MINY, HULL3_MINZ)
#define HULL3_MAX Vector(HULL3_MAXX, HULL3_MAXY, HULL3_MAXZ)


#endif