//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot.h
//

#ifndef BOT_H
#define BOT_H

// stuff for Win32 vs. Linux builds

#ifndef __linux__

typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef void (FAR *LINK_ENTITY_FUNC)(entvars_t *);

#else

#include <dlfcn.h>
#define GetProcAddress dlsym

typedef int BOOL;

typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef void (*LINK_ENTITY_FUNC)(entvars_t *);

#endif



// define constants used to identify the MOD we are playing...

#define VALVE_DLL      1
#define TFC_DLL        2
#define CSTRIKE_DLL    3
#define GEARBOX_DLL    4
#define FRONTLINE_DLL  5
#define AVH_DLL		   6


// define some function prototypes...
BOOL ClientConnect( edict_t *pEntity, const char *pszName,
                    const char *pszAddress, char szRejectReason[ 128 ] );
void ClientPutInServer( edict_t *pEntity );
void ClientCommand( edict_t *pEntity );

void FakeClientCommand(edict_t *pBot, char *arg1, char *arg2, char *arg3);


const char *Cmd_Args( void );
const char *Cmd_Argv( int argc );
int Cmd_Argc( void );


#define LADDER_UNKNOWN  0
#define LADDER_UP       1
#define LADDER_DOWN     2

#define WANDER_LEFT  1
#define WANDER_RIGHT 2

#define BOT_PITCH_SPEED 20
#define BOT_YAW_SPEED 20

#define RESPAWN_IDLE             1
#define RESPAWN_NEED_TO_RESPAWN  2
#define RESPAWN_IS_RESPAWNING    3

// game start messages for TFC...
#define MSG_TFC_IDLE          1
#define MSG_TFC_TEAM_SELECT   2
#define MSG_TFC_CLASS_SELECT  3

// game start messages for CS...
#define MSG_CS_IDLE         1
#define MSG_CS_TEAM_SELECT  2
#define MSG_CS_CT_SELECT    3
#define MSG_CS_T_SELECT     4

// game start messages for OpFor...
#define MSG_OPFOR_IDLE          1
#define MSG_OPFOR_TEAM_SELECT   2
#define MSG_OPFOR_CLASS_SELECT  3

// game start messages for FrontLineForce...
#define MSG_FLF_IDLE            1
#define MSG_FLF_TEAM_SELECT     2
#define MSG_FLF_CLASS_SELECT    3
#define MSG_FLF_PISTOL_SELECT   4
#define MSG_FLF_WEAPON_SELECT   5
#define MSG_FLF_RIFLE_SELECT    6
#define MSG_FLF_SHOTGUN_SELECT  7
#define MSG_FLF_SUBMACHINE_SELECT   8
#define MSG_FLF_HEAVYWEAPONS_SELECT 9

// game start messages for Aliens vs. Humans...
#define MSG_AVH_IDLE            1

#define TFC_CLASS_CIVILIAN  0
#define TFC_CLASS_SCOUT     1
#define TFC_CLASS_SNIPER    2
#define TFC_CLASS_SOLDIER   3
#define TFC_CLASS_DEMOMAN   4
#define TFC_CLASS_MEDIC     5
#define TFC_CLASS_HWGUY     6
#define TFC_CLASS_PYRO      7
#define TFC_CLASS_SPY       8
#define TFC_CLASS_ENGINEER  9


#define BOT_SKIN_LEN 32
#define BOT_NAME_LEN 32

#define MAX_BOT_WHINE 100

#include "mod/AvHConstants.h"
#include "mod/AvHSpecials.h"

typedef struct
{
   int  iId;     // weapon ID
   int  iClip;   // amount of ammo in the clip
   int  iAmmo1;  // amount of ammo in primary reserve
   int  iAmmo2;  // amount of ammo in secondary reserve
} bot_current_weapon_t;


typedef struct
{
   bool is_used;
   int respawn_state;
   edict_t *pEdict;
   bool need_to_initialize;
   char name[BOT_NAME_LEN+1];
   char skin[BOT_SKIN_LEN+1];
   int bot_skill;
   int not_started;
   int start_action;
   float kick_time;
   float create_time;

// TheFatal - START
   int msecnum;
   float msecdel;
   float msecval;
// TheFatal - END

   // things from pev in CBasePlayer...
   int bot_team;
   int bot_class;
   int bot_health;
   int bot_armor;
   int bot_weapons;      // bit map of weapons the bot is carrying
   int bot_money;        // for Counter-Strike
   int primary_weapon;   // for Front Line Force
   int secondary_weapon; // for Front Line Force
   int defender;         // for Front Line Force
   int warmup;           // for Front Line Force
   float idle_angle;
   float idle_angle_time;  // for Front Line Force
   int round_end;        // round has ended (in round based games)
   float blinded_time;

   // For AvH
   AvHPlayMode mBotPlayMode;

   int mOrderState;
   int mOrderNumPlayers;
   AvHOrderType	mOrderType;
   AvHOrderTargetType mOrderTargetType;
   float mOrderLocation[3];
   int mOrderTargetIndex;
   bool mOrderCompleted;

   float mTimeOfNextTaunt;
   float mTimeOfNextAmmoRequest;
   float mTimeOfNextOrderRequest;
   float mTimeOfNextAcknowledge;
   float mTimeOfNextRandomSaying;

   AvHUser3 mGestateUser3;
   int mResources;
   // end for AvH
   
   float f_max_speed;
   float prev_speed;
   float prev_time;
   Vector v_prev_origin;

   float f_find_item;
   edict_t *pBotPickupItem;

   int ladder_dir;
   float f_start_use_ladder_time;
   float f_end_use_ladder_time;
   bool  waypoint_top_of_ladder;

   float f_wall_check_time;
   float f_wall_on_right;
   float f_wall_on_left;
   float f_dont_avoid_wall_time;
   float f_look_for_waypoint_time;
   float f_jump_time;
   float f_dont_check_stuck;

   int wander_dir;
   float f_exit_water_time;

   Vector waypoint_origin;
   float f_waypoint_time;
   int curr_waypoint_index;
   int prev_waypoint_index[5];
   float f_random_waypoint_time;
   int waypoint_goal;
   float f_waypoint_goal_time;
   bool waypoint_near_flag;
   Vector waypoint_flag_origin;
   float prev_waypoint_distance;

   edict_t *pBotEnemy;
   float f_bot_see_enemy_time;
   float f_bot_find_enemy_time;
   edict_t *pBotUser;
   float f_bot_use_time;
   float f_bot_spawn_time;
   edict_t *killer_edict;
   bool  b_bot_say_killed;
   float f_bot_say_killed;
   float f_sniper_aim_time;
   

   float f_shoot_time;
   float f_primary_charging;
   float f_secondary_charging;
   int   charging_weapon_id;
   float f_move_speed;
   float f_pause_time;
   float f_sound_update_time;
   bool  bot_has_flag;

   bool  b_see_tripmine;
   bool  b_shoot_tripmine;
   Vector v_tripmine;

   bool  b_use_health_station;
   float f_use_health_time;
   bool  b_use_HEV_station;
   float f_use_HEV_time;

   bool  b_use_button;
   float f_use_button_time;
   bool  b_lift_moving;

   bool  b_use_capture;
   float f_use_capture_time;
   edict_t *pCaptureEdict;

   bot_current_weapon_t current_weapon;  // one current weapon for each bot
   int m_rgAmmo[MAX_AMMO_SLOTS];  // total ammo amounts (1 array for each bot)

} bot_t;


//#define MAX_TEAMS 32
//#define MAX_TEAMNAME_LENGTH 16
#include "game_shared/teamconst.h"


#define MAX_FLAGS  5

typedef struct {
   bool mdl_match;
   int  team_no;
   edict_t *edict;
} FLAG_S;


// new UTIL.CPP functions...
edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius );
edict_t *UTIL_FindEntityByString( edict_t *pentStart, const char *szKeyword, const char *szValue );
edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szName );
edict_t *UTIL_FindEntityByTargetname( edict_t *pentStart, const char *szName );
void ClientPrint( edict_t *pEdict, int msg_dest, const char *msg_name);
void UTIL_SayText( const char *pText, edict_t *pEdict );
void UTIL_HostSay( edict_t *pEntity, int teamonly, char *message );
int UTIL_GetTeam(edict_t *pEntity);
int UTIL_GetClass(edict_t *pEntity);
int UTIL_GetBotIndex(edict_t *pEdict);
bot_t *UTIL_GetBotPointer(edict_t *pEdict);
bool IsAlive(edict_t *pEdict);
bool FInViewCone(Vector *pOrigin, edict_t *pEdict);
bool FVisible( const Vector &vecOrigin, edict_t *pEdict );
Vector Center(edict_t *pEdict);
Vector GetGunPosition(edict_t *pEdict);
void UTIL_SelectItem(edict_t *pEdict, char *item_name);
Vector VecBModelOrigin(edict_t *pEdict);
bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer);
void UTIL_ShowMenu( edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText );
void UTIL_BuildFileName(char *filename, char *arg1, char *arg2);


#endif // BOT_H

