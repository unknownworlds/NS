//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot.cpp
//

//#include "windows.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"

#include "bot.h"
#include "bot_func.h"
#include "waypoint.h"
#include "bot_weapons.h"

#include <sys/types.h>
#include <sys/stat.h>
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHMessage.h"
#include "mod/AvHCommandConstants.h"
#include "mod/AvHMessage.h"
#include "mod/AvHPlayer.h"

#ifndef __linux__
extern HINSTANCE h_Library;
#else
extern void *h_Library;
#endif


extern int mod_id;
extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern int num_waypoints;  // number of waypoints currently in use
extern int default_bot_skill;
extern edict_t *pent_info_ctfdetect;

extern int max_team_players[4];
extern int team_class_limits[4];
extern int max_teams;
extern char bot_whine[MAX_BOT_WHINE][81];
extern int whine_count;

extern int flf_bug_fix;

static FILE *fp;


#define PLAYER_SEARCH_RADIUS     40.0
#define FLF_PLAYER_SEARCH_RADIUS 60.0


bot_t bots[32];   // max of 32 bots in a game
bool b_observer_mode = FALSE;
bool b_botdontshoot = FALSE;

extern int recent_bot_whine[5];

int number_names = 0;

#define MAX_BOT_NAMES 100

#define VALVE_MAX_SKINS    10
#define GEARBOX_MAX_SKINS  20

// indicate which models are currently used for random model allocation
bool valve_skin_used[VALVE_MAX_SKINS] = {
   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

bool gearbox_skin_used[GEARBOX_MAX_SKINS] = {
   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
   FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

// store the names of the models...
char *valve_bot_skins[VALVE_MAX_SKINS] = {
   "barney", "gina", "gman", "gordon", "helmet",
   "hgrunt", "recon", "robo", "scientist", "zombie"};

char *gearbox_bot_skins[GEARBOX_MAX_SKINS] = {
   "barney", "beret", "cl_suit", "drill", "fassn", "gina", "gman",
   "gordon", "grunt", "helmet", "hgrunt", "massn", "otis", "recon",
   "recruit", "robo", "scientist", "shepard", "tower", "zombie"};

// store the player names for each of the models...
char *valve_bot_names[VALVE_MAX_SKINS] = {
   "Barney", "Gina", "G-Man", "Gordon", "Helmet",
   "H-Grunt", "Recon", "Robo", "Scientist", "Zombie"};

char *gearbox_bot_names[GEARBOX_MAX_SKINS] = {
   "Barney", "Beret", "Cl_suit", "Drill", "Fassn", "Gina", "G-Man",
   "Gordon", "Grunt", "Helmet", "H-Grunt", "Massn", "Otis", "Recon",
   "Recruit", "Robo", "Scientist", "Shepard", "Tower", "Zombie"};

char bot_names[MAX_BOT_NAMES][BOT_NAME_LEN+1];

// how often (out of 1000 times) the bot will pause, based on bot skill
float pause_frequency[5] = {4, 7, 10, 15, 20};

float pause_time[5][2] = {
   {0.2, 0.5}, {0.5, 1.0}, {0.7, 1.3}, {1.0, 1.7}, {1.2, 2.0}};


inline edict_t *CREATE_FAKE_CLIENT( const char *netname )
{
   return (*g_engfuncs.pfnCreateFakeClient)( netname );
}

inline char *GET_INFOBUFFER( edict_t *e )
{
   return (*g_engfuncs.pfnGetInfoKeyBuffer)( e );
}

inline char *GET_INFO_KEY_VALUE( char *infobuffer, char *key )
{
   return (g_engfuncs.pfnInfoKeyValue( infobuffer, key ));
}

inline void SET_CLIENT_KEY_VALUE( int clientIndex, char *infobuffer,
                                  char *key, char *value )
{
   (*g_engfuncs.pfnSetClientKeyValue)( clientIndex, infobuffer, key, value );
}


// this is the LINK_ENTITY_TO_CLASS function that creates a player (bot)
void player( entvars_t *pev )
{
   static LINK_ENTITY_FUNC otherClassName = NULL;
   if (otherClassName == NULL)
      otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, "player");
   if (otherClassName != NULL)
   {
      (*otherClassName)(pev);
   }
}

void BotProcessVoiceCommands(bot_t* pBot)
{
	if((pBot->mTimeOfNextOrderRequest != -1) && (gpGlobals->time > pBot->mTimeOfNextOrderRequest))
	{
		if(pBot->pEdict->v.impulse == 0)
		{
			pBot->pEdict->v.impulse = ORDER_REQUEST;
			pBot->mTimeOfNextOrderRequest = -1;
		}
	}
	if((pBot->mTimeOfNextAmmoRequest != -1) && (gpGlobals->time > pBot->mTimeOfNextAmmoRequest))
	{
		if(pBot->pEdict->v.impulse == 0)
		{
			pBot->pEdict->v.impulse = SAYING_5;
			pBot->mTimeOfNextAmmoRequest = -1;
		}
	}
	if((pBot->mTimeOfNextRandomSaying != -1) && (gpGlobals->time > pBot->mTimeOfNextRandomSaying))
	{
		if(pBot->pEdict->v.impulse == 0)
		{
			pBot->pEdict->v.impulse = SAYING_1 + g_engfuncs.pfnRandomLong(0, 5);
			pBot->mTimeOfNextRandomSaying = -1;
		}
	}
	if((pBot->mTimeOfNextTaunt != -1) && (gpGlobals->time > pBot->mTimeOfNextTaunt))
	{
		if(pBot->pEdict->v.impulse == 0)
		{
			pBot->pEdict->v.impulse = SAYING_3;
			pBot->mTimeOfNextTaunt = -1;
		}
	}
	if((pBot->mTimeOfNextAcknowledge != -1) && (gpGlobals->time > pBot->mTimeOfNextAcknowledge))
	{
		if(pBot->pEdict->v.impulse == 0)
		{
			pBot->pEdict->v.impulse = ORDER_ACK;
			pBot->mTimeOfNextAcknowledge = -1;
		}
	}
}

void BotSpawnInit( bot_t *pBot )
{
   pBot->v_prev_origin = Vector(9999.0, 9999.0, 9999.0);
   pBot->prev_time = gpGlobals->time;

   pBot->waypoint_origin = Vector(0, 0, 0);
   pBot->f_waypoint_time = 0.0;
   pBot->curr_waypoint_index = -1;
   pBot->prev_waypoint_index[0] = -1;
   pBot->prev_waypoint_index[1] = -1;
   pBot->prev_waypoint_index[2] = -1;
   pBot->prev_waypoint_index[3] = -1;
   pBot->prev_waypoint_index[4] = -1;

   pBot->f_random_waypoint_time = gpGlobals->time;
   pBot->waypoint_goal = -1;
   pBot->f_waypoint_goal_time = 0.0;
   pBot->waypoint_near_flag = FALSE;
   pBot->waypoint_flag_origin = Vector(0, 0, 0);
   pBot->prev_waypoint_distance = 0.0;

   pBot->msecnum = 0;
   pBot->msecdel = 0.0;
   pBot->msecval = 0.0;

   pBot->bot_health = 0;
   pBot->bot_armor = 0;
   pBot->bot_weapons = 0;
   pBot->blinded_time = 0.0;

   // Init AvH variables
   pBot->mBotPlayMode = PLAYMODE_READYROOM;

   pBot->mOrderState = 0;
   pBot->mOrderNumPlayers = -1;
   pBot->mOrderType = ORDERTYPE_UNDEFINED;
   pBot->mOrderTargetType = ORDERTARGETTYPE_UNDEFINED;
   pBot->mOrderLocation[0] = pBot->mOrderLocation[1] = pBot->mOrderLocation[2] = 0.0f;
   pBot->mOrderTargetIndex = -1;
   pBot->mOrderCompleted = false;

   pBot->mTimeOfNextAcknowledge = -1;
   pBot->mTimeOfNextAmmoRequest = -1;
   pBot->mTimeOfNextOrderRequest = -1;
   pBot->mTimeOfNextTaunt = -1;
   pBot->mTimeOfNextRandomSaying = -1;
   
   pBot->mGestateUser3 = AVH_USER3_NONE;
   pBot->mResources = 0;
   // end AvH variable init
   
   pBot->f_max_speed = CVAR_GET_FLOAT("sv_maxspeed");

   pBot->prev_speed = 0.0;  // fake "paused" since bot is NOT stuck

   pBot->f_find_item = 0.0;

   pBot->ladder_dir = LADDER_UNKNOWN;
   pBot->f_start_use_ladder_time = 0.0;
   pBot->f_end_use_ladder_time = 0.0;
   pBot->waypoint_top_of_ladder = FALSE;

   pBot->f_wall_check_time = 0.0;
   pBot->f_wall_on_right = 0.0;
   pBot->f_wall_on_left = 0.0;
   pBot->f_dont_avoid_wall_time = 0.0;
   pBot->f_look_for_waypoint_time = 0.0;
   pBot->f_jump_time = 0.0;
   pBot->f_dont_check_stuck = 0.0;

   // pick a wander direction (50% of the time to the left, 50% to the right)
   if (RANDOM_LONG(1, 100) <= 50)
      pBot->wander_dir = WANDER_LEFT;
   else
      pBot->wander_dir = WANDER_RIGHT;

   pBot->f_exit_water_time = 0.0;

   pBot->pBotEnemy = NULL;
   pBot->f_bot_see_enemy_time = gpGlobals->time;
   pBot->f_bot_find_enemy_time = gpGlobals->time;
   pBot->pBotUser = NULL;
   pBot->f_bot_use_time = 0.0;
   pBot->b_bot_say_killed = FALSE;
   pBot->f_bot_say_killed = 0.0;
   pBot->f_sniper_aim_time = 0.0;

   pBot->f_shoot_time = gpGlobals->time;
   pBot->f_primary_charging = -1.0;
   pBot->f_secondary_charging = -1.0;
   pBot->charging_weapon_id = 0;

   pBot->f_pause_time = 0.0;
   pBot->f_sound_update_time = 0.0;
   pBot->bot_has_flag = FALSE;

   pBot->b_see_tripmine = FALSE;
   pBot->b_shoot_tripmine = FALSE;
   pBot->v_tripmine = Vector(0,0,0);

   pBot->b_use_health_station = FALSE;
   pBot->f_use_health_time = 0.0;
   pBot->b_use_HEV_station = FALSE;
   pBot->f_use_HEV_time = 0.0;

   pBot->b_use_button = FALSE;
   pBot->f_use_button_time = 0;
   pBot->b_lift_moving = FALSE;

   pBot->b_use_capture = FALSE;
   pBot->f_use_capture_time = 0.0;
   pBot->pCaptureEdict = NULL;

   memset(&(pBot->current_weapon), 0, sizeof(pBot->current_weapon));
   memset(&(pBot->m_rgAmmo), 0, sizeof(pBot->m_rgAmmo));
}


void BotNameInit( void )
{
   FILE *bot_name_fp;
   char bot_name_filename[256];
   int str_index;
   char name_buffer[80];
   int length, index;

   UTIL_BuildFileName(bot_name_filename, "bot_names.txt", NULL);

   bot_name_fp = fopen(bot_name_filename, "r");

   if (bot_name_fp != NULL)
   {
      while ((number_names < MAX_BOT_NAMES) &&
             (fgets(name_buffer, 80, bot_name_fp) != NULL))
      {
         length = strlen(name_buffer);

         if (name_buffer[length-1] == '\n')
         {
            name_buffer[length-1] = 0;  // remove '\n'
            length--;
         }

         str_index = 0;
         while (str_index < length)
         {
            if ((name_buffer[str_index] < ' ') || (name_buffer[str_index] > '~') ||
                (name_buffer[str_index] == '"'))
            for (index=str_index; index < length; index++)
               name_buffer[index] = name_buffer[index+1];

            str_index++;
         }

         if (name_buffer[0] != 0)
         {
            strncpy(bot_names[number_names], name_buffer, BOT_NAME_LEN);

            number_names++;
         }
      }

      fclose(bot_name_fp);
   }
}


void BotPickName( char *name_buffer )
{
   int name_index, index;
   bool used;
   edict_t *pPlayer;
   int attempts = 0;

   // see if a name exists from a kicked bot (if so, reuse it)
   for (index=0; index < 32; index++)
   {
      if ((bots[index].is_used == FALSE) && (bots[index].name[0]))
      {
         strcpy(name_buffer, bots[index].name);

         return;
      }   
   }

   name_index = RANDOM_LONG(1, number_names) - 1;  // zero based

   // check make sure this name isn't used
   used = TRUE;

   while (used)
   {
      used = FALSE;

      for (index = 1; index <= gpGlobals->maxClients; index++)
      {
         pPlayer = INDEXENT(index);

         if (pPlayer && !pPlayer->free)
         {
            if (strcmp(bot_names[name_index], STRING(pPlayer->v.netname)) == 0)
            {
               used = TRUE;
               break;
            }
         }
      }

      if (used)
      {
         name_index++;

         if (name_index == number_names)
            name_index = 0;

         attempts++;

         if (attempts == number_names)
            used = FALSE;  // break out of loop even if already used
      }
   }

   strcpy(name_buffer, bot_names[name_index]);
}


void BotCreate( edict_t *pPlayer, const char *arg1, const char *arg2,
                const char *arg3, const char *arg4)
{
   edict_t *BotEnt;
   bot_t *pBot;
   char c_skin[BOT_SKIN_LEN+1];
   char c_name[BOT_NAME_LEN+1];
   int skill;
   int index;
   int i, j, length;
   bool found = FALSE;


   if ((mod_id == VALVE_DLL) ||
       ((mod_id == GEARBOX_DLL) && (pent_info_ctfdetect == NULL)))
   {
      int  max_skin_index;

      if (mod_id == VALVE_DLL)
         max_skin_index = VALVE_MAX_SKINS;
      else  // must be GEARBOX_DLL
         max_skin_index = GEARBOX_MAX_SKINS;

      if ((arg1 == NULL) || (*arg1 == 0))
      {
         bool *pSkinUsed;

         // pick a random skin
         if (mod_id == VALVE_DLL)
         {
            index = RANDOM_LONG(0, VALVE_MAX_SKINS-1);
            pSkinUsed = &valve_skin_used[0];
         }
         else  // must be GEARBOX_DLL
         {
            index = RANDOM_LONG(0, GEARBOX_MAX_SKINS-1);
            pSkinUsed = &gearbox_skin_used[0];
         }

         // check if this skin has already been used...
         while (pSkinUsed[index] == TRUE)
         {
            index++;

            if (index == max_skin_index)
               index = 0;
         }

         pSkinUsed[index] = TRUE;

         // check if all skins are now used...
         for (i = 0; i < max_skin_index; i++)
         {
            if (pSkinUsed[i] == FALSE)
               break;
         }

         // if all skins are used, reset used to FALSE for next selection
         if (i == max_skin_index)
         {
            for (i = 0; i < max_skin_index; i++)
               pSkinUsed[i] = FALSE;
         }

         if (mod_id == VALVE_DLL)
            strcpy( c_skin, valve_bot_skins[index] );
         else // must be GEARBOX_DLL
            strcpy( c_skin, gearbox_bot_skins[index] );
      }
      else
      {
         strncpy( c_skin, arg1, BOT_SKIN_LEN-1 );
         c_skin[BOT_SKIN_LEN] = 0;  // make sure c_skin is null terminated
      }

      for (i = 0; c_skin[i] != 0; i++)
         c_skin[i] = tolower( c_skin[i] );  // convert to all lowercase

      index = 0;

      while ((!found) && (index < max_skin_index))
      {
         if (mod_id == VALVE_DLL)
         {
            if (strcmp(c_skin, valve_bot_skins[index]) == 0)
               found = TRUE;
            else
               index++;
         }
         else // must be GEARBOX_DLL
         {
            if (strcmp(c_skin, gearbox_bot_skins[index]) == 0)
               found = TRUE;
            else
               index++;
         }
      }

      if (found == TRUE)
      {
         if ((arg2 != NULL) && (*arg2 != 0))
         {
            strncpy( c_name, arg2, BOT_SKIN_LEN-1 );
            c_name[BOT_SKIN_LEN] = 0;  // make sure c_name is null terminated
         }
         else
         {
            if (number_names > 0)
               BotPickName( c_name );
            else if (mod_id == VALVE_DLL)
               strcpy( c_name, valve_bot_names[index] );
            else // must be GEARBOX_DLL
               strcpy( c_name, gearbox_bot_names[index] );
         }
      }
      else
      {
         char dir_name[32];
         char filename[128];

         struct stat stat_str;

         GET_GAME_DIR(dir_name);

#ifndef __linux__
         sprintf(filename, "%s\\models\\player\\%s", dir_name, c_skin);
#else
         sprintf(filename, "%s/models/player/%s", dir_name, c_skin);
#endif

         if (stat(filename, &stat_str) != 0)
         {
#ifndef __linux__
            sprintf(filename, "valve\\models\\player\\%s", c_skin);
#else
            sprintf(filename, "valve/models/player/%s", c_skin);
#endif
            if (stat(filename, &stat_str) != 0)
            {
               char err_msg[80];

               sprintf( err_msg, "model \"%s\" is unknown.\n", c_skin );
               if (pPlayer)
                  ClientPrint(pPlayer, HUD_PRINTNOTIFY, err_msg );
               if (IS_DEDICATED_SERVER())
                  printf(err_msg);

               if (pPlayer)
                  ClientPrint(pPlayer, HUD_PRINTNOTIFY,
                     "use barney, gina, gman, gordon, helmet, hgrunt,\n");
               if (IS_DEDICATED_SERVER())
                  printf("use barney, gina, gman, gordon, helmet, hgrunt,\n");
               if (pPlayer)
                  ClientPrint(pPlayer, HUD_PRINTNOTIFY,
                     "    recon, robo, scientist, or zombie\n");
               if (IS_DEDICATED_SERVER())
                  printf("    recon, robo, scientist, or zombie\n");
               return;
            }
         }

         if ((arg2 != NULL) && (*arg2 != 0))
         {
            strncpy( c_name, arg2, BOT_NAME_LEN-1 );
            c_name[BOT_NAME_LEN] = 0;  // make sure c_name is null terminated
         }
         else
         {
            if (number_names > 0)
               BotPickName( c_name );
            else
            {
               // copy the name of the model to the bot's name...
               strncpy( c_name, arg1, BOT_NAME_LEN-1 );
               c_name[BOT_NAME_LEN] = 0;  // make sure c_skin is null terminated
            }
         }
      }

      skill = 0;

      if ((arg3 != NULL) && (*arg3 != 0))
         skill = atoi(arg3);
         
      if ((skill < 1) || (skill > 5))
         skill = default_bot_skill;
   }
   else
   {
      if ((arg3 != NULL) && (*arg3 != 0))
      {
         strncpy( c_name, arg3, BOT_NAME_LEN-1 );
         c_name[BOT_NAME_LEN] = 0;  // make sure c_name is null terminated
      }
      else
      {
         if (number_names > 0)
            BotPickName( c_name );
         else
            strcpy(c_name, "Bot");
      }

      skill = 0;

      if ((arg4 != NULL) && (*arg4 != 0))
         skill = atoi(arg4);
         
      if ((skill < 1) || (skill > 5))
         skill = default_bot_skill;
   }

   length = strlen(c_name);

   // remove any illegal characters from name...
   for (i = 0; i < length; i++)
   {
      if ((c_name[i] <= ' ') || (c_name[i] > '~') ||
          (c_name[i] == '"'))
      {
         for (j = i; j < length; j++)  // shuffle chars left (and null)
            c_name[j] = c_name[j+1];
         length--;
      }               
   }

   BotEnt = CREATE_FAKE_CLIENT( c_name );

   if (FNullEnt( BotEnt ))
   {
      if (pPlayer)
         ClientPrint( pPlayer, HUD_PRINTNOTIFY, "Max. Players reached.  Can't create bot!\n");
   }
   else
   {
      char ptr[128];  // allocate space for message from ClientConnect
      char *infobuffer;
      int clientIndex;
      int index;

      if (IS_DEDICATED_SERVER())
         printf("Creating bot...\n");
      else if (pPlayer)
         ClientPrint( pPlayer, HUD_PRINTNOTIFY, "Creating bot...\n");

      index = 0;
      while ((bots[index].is_used) && (index < 32))
         index++;

      if (index == 32)
      {
         ClientPrint( pPlayer, HUD_PRINTNOTIFY, "Can't create bot!\n");
         return;
      }

      // create the player entity by calling MOD's player function
      // (from LINK_ENTITY_TO_CLASS for player object)

      player( VARS(BotEnt) );

      infobuffer = GET_INFOBUFFER( BotEnt );
      clientIndex = ENTINDEX( BotEnt );


      if ((mod_id == VALVE_DLL) || (mod_id == GEARBOX_DLL))
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "model", c_skin );
      else // other mods
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "model", "gina" );

      if (mod_id == CSTRIKE_DLL)
      {
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "rate", "3500.000000");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "cl_updaterate", "20");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "cl_lw", "1");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "cl_lc", "1");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "tracker", "0");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "cl_dlmax", "128");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "lefthand", "1");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "friends", "0");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "dm", "0");
         SET_CLIENT_KEY_VALUE( clientIndex, infobuffer, "ah", "1");
      }

      ClientConnect( BotEnt, c_name, "127.0.0.1", ptr );

      // Pieter van Dijk - use instead of DispatchSpawn() - Hip Hip Hurray!
      ClientPutInServer( BotEnt );

      BotEnt->v.flags |= FL_FAKECLIENT;

      // initialize all the variables for this bot...

      pBot = &bots[index];

      pBot->is_used = TRUE;
      pBot->respawn_state = RESPAWN_IDLE;
      pBot->create_time = gpGlobals->time;
      pBot->name[0] = 0;  // name not set by server yet
      pBot->bot_money = 0;

      strcpy(pBot->skin, c_skin);

      pBot->pEdict = BotEnt;

      pBot->not_started = 1;  // hasn't joined game yet

      if (mod_id == TFC_DLL)
         pBot->start_action = MSG_TFC_IDLE;
      else if (mod_id == CSTRIKE_DLL)
         pBot->start_action = MSG_CS_IDLE;
      else if ((mod_id == GEARBOX_DLL) && (pent_info_ctfdetect != NULL))
         pBot->start_action = MSG_OPFOR_IDLE;
      else if (mod_id == FRONTLINE_DLL)
         pBot->start_action = MSG_FLF_IDLE;
      else if (mod_id == AVH_DLL)
		  pBot->start_action = MSG_AVH_IDLE;
      else
         pBot->start_action = 0;  // not needed for non-team MODs


      BotSpawnInit(pBot);

      pBot->need_to_initialize = FALSE;  // don't need to initialize yet

      BotEnt->v.idealpitch = BotEnt->v.v_angle.x;
      BotEnt->v.ideal_yaw = BotEnt->v.v_angle.y;
      BotEnt->v.pitch_speed = BOT_PITCH_SPEED;
      BotEnt->v.yaw_speed = BOT_YAW_SPEED;

      pBot->warmup = 0;  // for Front Line Force
      pBot->idle_angle = 0.0;
      pBot->idle_angle_time = 0.0;
      pBot->round_end = 0;
      pBot->defender = 0;

      pBot->bot_skill = skill - 1;  // 0 based for array indexes

      pBot->bot_team = -1;
      pBot->bot_class = -1;

      if ((mod_id == TFC_DLL) || (mod_id == CSTRIKE_DLL) ||
          ((mod_id == GEARBOX_DLL) && (pent_info_ctfdetect != NULL)) ||
          (mod_id == FRONTLINE_DLL))
      {
         if ((arg1 != NULL) && (arg1[0] != 0))
         {
            pBot->bot_team = atoi(arg1);

            if ((arg2 != NULL) && (arg2[0] != 0))
            {
               pBot->bot_class = atoi(arg2);
            }
         }
      }
   }
}


int BotInFieldOfView(bot_t *pBot, Vector dest)
{
   // find angles from source to destination...
   Vector entity_angles = UTIL_VecToAngles( dest );

   // make yaw angle 0 to 360 degrees if negative...
   if (entity_angles.y < 0)
      entity_angles.y += 360;

   // get bot's current view angle...
   float view_angle = pBot->pEdict->v.v_angle.y;

   // make view angle 0 to 360 degrees if negative...
   if (view_angle < 0)
      view_angle += 360;

   // return the absolute value of angle to destination entity
   // zero degrees means straight ahead,  45 degrees to the left or
   // 45 degrees to the right is the limit of the normal view angle

   // rsm - START angle bug fix
   int angle = abs((int)view_angle - (int)entity_angles.y);

   if (angle > 180)
      angle = 360 - angle;

   return angle;
   // rsm - END
}


bool BotEntityIsVisible( bot_t *pBot, Vector dest )
{
   TraceResult tr;

   // trace a line from bot's eyes to destination...
   UTIL_TraceLine( pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs,
                   dest, ignore_monsters,
                   pBot->pEdict->v.pContainingEntity, &tr );

   // check if line of sight to object is not blocked (i.e. visible)
   if (tr.flFraction >= 1.0)
      return TRUE;
   else
      return FALSE;
}


void BotFindItem( bot_t *pBot )
{
   edict_t *pent = NULL;
   edict_t *pPickupEntity = NULL;
   Vector pickup_origin;
   Vector entity_origin;
   float radius = 500;
   bool can_pickup;
   float min_distance;
   char item_name[40];
   TraceResult tr;
   Vector vecStart;
   Vector vecEnd;
   int angle_to_entity;
   edict_t *pEdict = pBot->pEdict;

   pBot->pBotPickupItem = NULL;

   // use a MUCH smaller search radius when waypoints are available
   if ((num_waypoints > 0) && (pBot->curr_waypoint_index != -1))
      radius = 100.0;
   else
      radius = 500.0;

   min_distance = radius + 1.0;

   while ((pent = UTIL_FindEntityInSphere( pent, pEdict->v.origin, radius )) != NULL)
   {
      can_pickup = FALSE;  // assume can't use it until known otherwise

      strcpy(item_name, STRING(pent->v.classname));

      // see if this is a "func_" type of entity (func_button, etc.)...
      if (strncmp("func_", item_name, 5) == 0)
      {
         // BModels have 0,0,0 for origin so must use VecBModelOrigin...
         entity_origin = VecBModelOrigin(pent);

         vecStart = pEdict->v.origin + pEdict->v.view_ofs;
         vecEnd = entity_origin;

         angle_to_entity = BotInFieldOfView( pBot, vecEnd - vecStart );

         // check if entity is outside field of view (+/- 45 degrees)
         if (angle_to_entity > 45)
            continue;  // skip this item if bot can't "see" it

         // check if entity is a ladder (ladders are a special case)
         // DON'T search for ladders if there are waypoints in this level...
         if ((strcmp("func_ladder", item_name) == 0) && (num_waypoints == 0))
         {
            // force ladder origin to same z coordinate as bot since
            // the VecBModelOrigin is the center of the ladder.  For
            // LONG ladders, the center MAY be hundreds of units above
            // the bot.  Fake an origin at the same level as the bot...

            entity_origin.z = pEdict->v.origin.z;
            vecEnd = entity_origin;

            // trace a line from bot's eyes to func_ladder entity...
            UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters,
                            pEdict->v.pContainingEntity, &tr);

            // check if traced all the way up to the entity (didn't hit wall)
            if (tr.flFraction >= 1.0)
            {
               // find distance to item for later use...
               float distance = (vecEnd - vecStart).Length( );

               // use the ladder about 100% of the time, if haven't
               // used a ladder in at least 5 seconds...
               if ((RANDOM_LONG(1, 100) <= 100) &&
                   ((pBot->f_end_use_ladder_time + 5.0) < gpGlobals->time))
               {
                  // if close to ladder...
                  if (distance < 100)
                  {
                     // don't avoid walls for a while
                     pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
                  }

                  can_pickup = TRUE;
               }
            }
         }
         else
         {
            // trace a line from bot's eyes to func_ entity...
            UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters,
                            pEdict->v.pContainingEntity, &tr);

            // check if traced all the way up to the entity (didn't hit wall)
            if (strcmp(item_name, STRING(tr.pHit->v.classname)) == 0)
            {
               // find distance to item for later use...
               float distance = (vecEnd - vecStart).Length( );

               // check if entity is wall mounted health charger...
               if (strcmp("func_healthcharger", item_name) == 0)
               {
                  // check if the bot can use this item and
                  // check if the recharger is ready to use (has power left)...
                  if ((pEdict->v.health < 100) && (pent->v.frame == 0))
                  {
                     // check if flag not set...
                     if (!pBot->b_use_health_station)
                     {
                        // check if close enough and facing it directly...
                        if ((distance < PLAYER_SEARCH_RADIUS) &&
                            (angle_to_entity <= 10))
                        {
                           pBot->b_use_health_station = TRUE;
                           pBot->f_use_health_time = gpGlobals->time;
                        }

                        // if close to health station...
                        if (distance < 100)
                        {
                           // don't avoid walls for a while
                           pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
                        }

                        can_pickup = TRUE;
                     }
                  }
                  else
                  {
                     // don't need or can't use this item...
                     pBot->b_use_health_station = FALSE;
                  }
               }

               // check if entity is wall mounted HEV charger...
               else if (strcmp("func_recharge", item_name) == 0)
               {
                  // check if the bot can use this item and
                  // check if the recharger is ready to use (has power left)...
                  if ((pEdict->v.armorvalue < VALVE_MAX_NORMAL_BATTERY) &&
                      (pent->v.frame == 0))
                  {
                     // check if flag not set and facing it...
                     if (!pBot->b_use_HEV_station)
                     {
                        // check if close enough and facing it directly...
                        if ((distance < PLAYER_SEARCH_RADIUS) &&
                            (angle_to_entity <= 10))
                        {
                           pBot->b_use_HEV_station = TRUE;
                           pBot->f_use_HEV_time = gpGlobals->time;
                        }

                        // if close to HEV recharger...
                        if (distance < 100)
                        {
                           // don't avoid walls for a while
                           pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
                        }

                        can_pickup = TRUE;
                     }
                  }
                  else
                  {
                     // don't need or can't use this item...
                     pBot->b_use_HEV_station = FALSE;
                  }
               }

               // check if entity is a button...
               else if (strcmp("func_button", item_name) == 0)
               {
                  // use the button about 100% of the time, if haven't
                  // used a button in at least 5 seconds...
                  if ((RANDOM_LONG(1, 100) <= 100) &&
                      ((pBot->f_use_button_time + 5) < gpGlobals->time))
                  {
                     // check if flag not set and facing it...
                     if (!pBot->b_use_button)
                     {
                        // check if close enough and facing it directly...
                        if ((distance < PLAYER_SEARCH_RADIUS) &&
                            (angle_to_entity <= 10))
                        {
                           pBot->b_use_button = TRUE;
                           pBot->b_lift_moving = FALSE;
                           pBot->f_use_button_time = gpGlobals->time;
                        }

                        // if close to button...
                        if (distance < 100)
                        {
                           // don't avoid walls for a while
                           pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
                        }

                        can_pickup = TRUE;
                     }
                  }
                  else
                  {
                     // don't need or can't use this item...
                     pBot->b_use_button = FALSE;
                  }
               }
            }
         }
      }
      else  // everything else...
      {
         entity_origin = pent->v.origin;

         vecStart = pEdict->v.origin + pEdict->v.view_ofs;
         vecEnd = entity_origin;

         // find angles from bot origin to entity...
         angle_to_entity = BotInFieldOfView( pBot, vecEnd - vecStart );

         // check if entity is outside field of view (+/- 45 degrees)
         if (angle_to_entity > 45)
            continue;  // skip this item if bot can't "see" it

         // check if line of sight to object is not blocked (i.e. visible)
         if (BotEntityIsVisible( pBot, vecEnd ))
         {
            // check if entity is a weapon...
            if (strncmp("weapon_", item_name, 7) == 0)
            {
               if (pent->v.effects & EF_NODRAW)
               {
                  // someone owns this weapon or it hasn't respawned yet
                  continue;
               }

               can_pickup = TRUE;
            }

            // check if entity is ammo...
            else if (strncmp("ammo_", item_name, 5) == 0)
            {
               // check if the item is not visible (i.e. has not respawned)
               if (pent->v.effects & EF_NODRAW)
                  continue;

               can_pickup = TRUE;
            }

            // check if entity is a battery...
            else if (strcmp("item_battery", item_name) == 0)
            {
               // check if the item is not visible (i.e. has not respawned)
               if (pent->v.effects & EF_NODRAW)
                  continue;

               // check if the bot can use this item...
               if (pEdict->v.armorvalue < VALVE_MAX_NORMAL_BATTERY)
               {
                  can_pickup = TRUE;
               }
            }

            // check if entity is a healthkit...
            else if (strcmp("item_healthkit", item_name) == 0)
            {
               // check if the item is not visible (i.e. has not respawned)
               if (pent->v.effects & EF_NODRAW)
                  continue;

               // check if the bot can use this item...
               if (pEdict->v.health < 100)
               {
                  can_pickup = TRUE;
               }
            }

            // check if entity is a packed up weapons box...
            else if (strcmp("weaponbox", item_name) == 0)
            {
               can_pickup = TRUE;
            }

            // check if entity is the spot from RPG laser
            else if (strcmp("laser_spot", item_name) == 0)
            {
            }

            // check if entity is an armed tripmine
            //else if (strcmp("monster_tripmine", item_name) == 0)
			else if (strcmp(kwsDeployedMine, item_name) == 0)
            {
               float distance = (pent->v.origin - pEdict->v.origin).Length( );

               if (pBot->b_see_tripmine)
               {
                  // see if this tripmine is closer to bot...
                  if (distance < (pBot->v_tripmine - pEdict->v.origin).Length())
                  {
                     pBot->v_tripmine = pent->v.origin;
                     pBot->b_shoot_tripmine = FALSE;

                     // see if bot is far enough to shoot the tripmine...
                     if (distance >= 375)
                     {
                        pBot->b_shoot_tripmine = TRUE;
                     }
                  }
               }
               else
               {
                  pBot->b_see_tripmine = TRUE;
                  pBot->v_tripmine = pent->v.origin;
                  pBot->b_shoot_tripmine = FALSE;

                  // see if bot is far enough to shoot the tripmine...
                  if (distance >= 375)  // 375 is damage radius
                  {
                     pBot->b_shoot_tripmine = TRUE;
                  }
               }
            }

            // check if entity is an armed satchel charge
            else if (strcmp("monster_satchel", item_name) == 0)
            {
            }

            // check if entity is a snark (squeak grenade)
            else if (strcmp("monster_snark", item_name) == 0)
            {
            }

            else if ((mod_id == FRONTLINE_DLL) && (!pBot->defender) &&
                     (strcmp("capture_point", item_name) == 0))
            {
               int team = UTIL_GetTeam(pEdict);  // skin and team must match

               if (flf_bug_fix)
                  team = 1 - team;  // BACKWARDS bug!

               // check if flag not set and point not captured...
               if ((!pBot->b_use_capture) && (pent->v.skin == team))
               {
                  float distance = (pent->v.origin - pEdict->v.origin).Length( );

                  // check if close enough and facing it directly...
                  if ((distance < FLF_PLAYER_SEARCH_RADIUS) &&
                      (angle_to_entity <= 20))
                  {
                     pBot->b_use_capture = TRUE;
                     pBot->f_use_capture_time = gpGlobals->time + 8.0;
                     pBot->pCaptureEdict = pent;
                  }

                  // if close to capture point...
                  if (distance < 160)
                  {
                     // don't avoid walls for a while
                     pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;
                  }

                  can_pickup = TRUE;
               }
            }

         }  // end if object is visible
      }  // end else not "func_" entity

      if (can_pickup) // if the bot found something it can pickup...
      {
         float distance = (entity_origin - pEdict->v.origin).Length( );

         // see if it's the closest item so far...
         if (distance < min_distance)
         {
            min_distance = distance;        // update the minimum distance
            pPickupEntity = pent;        // remember this entity
            pickup_origin = entity_origin;  // remember location of entity
         }
      }
   }  // end while loop

   if (pPickupEntity != NULL)
   {
      // let's head off toward that item...
      Vector v_item = pickup_origin - pEdict->v.origin;

      Vector bot_angles = UTIL_VecToAngles( v_item );

      pEdict->v.ideal_yaw = bot_angles.y;

      BotFixIdealYaw(pEdict);

      pBot->pBotPickupItem = pPickupEntity;  // save the item bot is trying to get
   }
}


void BotThink( bot_t *pBot )
{
   int index = 0;
   Vector v_diff;             // vector from previous to current location
   float pitch_degrees;
   float yaw_degrees;
   float moved_distance;      // length of v_diff vector (distance bot moved)
   TraceResult tr;
   bool found_waypoint;
   bool is_idle;

   edict_t *pEdict = pBot->pEdict;


   pEdict->v.flags |= FL_FAKECLIENT;

   if (pBot->name[0] == 0)  // name filled in yet?
      strcpy(pBot->name, STRING(pBot->pEdict->v.netname));


// TheFatal - START from Advanced Bot Framework (Thanks Rich!)

   // adjust the millisecond delay based on the frame rate interval...
   if (pBot->msecdel <= gpGlobals->time)
   {
      pBot->msecdel = gpGlobals->time + 0.5;
      if (pBot->msecnum > 0)
         pBot->msecval = 450.0/pBot->msecnum;
      pBot->msecnum = 0;
   }
   else
      pBot->msecnum++;

   if (pBot->msecval < 1)    // don't allow msec to be less than 1...
      pBot->msecval = 1;

   if (pBot->msecval > 100)  // ...or greater than 100
      pBot->msecval = 100;

// TheFatal - END


   pEdict->v.button = 0;
   pBot->f_move_speed = 0.0;

   if(pBot->mBotPlayMode == PLAYMODE_READYROOM)
   {
	   // Look at desired team and head to nearest start entity
	   AvHClassType theClassType = AVH_CLASS_TYPE_UNDEFINED;

	   FakeClientCommand(pEdict, kcAutoAssign, NULL, NULL);

	   // bot has now joined the game (doesn't need to be started)
	   pBot->not_started = 0;
   }

   // if the bot hasn't selected stuff to start the game yet, go do that...
   if (pBot->not_started)
   {
      BotStartGame( pBot );

	  // Do this to test server performance
	  //return;
	  
      g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle, 0.0,
                                   0, 0, pEdict->v.button, 0, pBot->msecval);

      //return;
   }
   
   if((pBot->mOrderType != ORDERTYPE_UNDEFINED) && (!pBot->mOrderCompleted))
   {
	   // Every once in a while mention our order
	   if(RANDOM_LONG(0, 200) == 0)
	   {
		   char theMessage[256];
		   sprintf(theMessage, "Order type: %d, target type: %d\n", pBot->mOrderType, pBot->mOrderTargetType);
		   //UTIL_HostSay(pBot->pEdict, 0, theMessage);
	   }
   }
   else
   {
	   // Occasionally ask for orders when not doing anything else
	   if(RANDOM_LONG(0, 1800) == 0)
	   {
		   if(pBot->mTimeOfNextOrderRequest == -1)
		   {
			   pBot->mTimeOfNextOrderRequest = gpGlobals->time;
		   }
	   }
   }

   if(RANDOM_LONG(0, 1000))
   {
	   pBot->mTimeOfNextRandomSaying = gpGlobals->time;
   }

   // If we're out of ammo, occasionally ask for more
   // Get current weapon
   int theWeaponID = pBot->current_weapon.iId;
   if(theWeaponID > 0)
   {
	   if((pBot->current_weapon.iAmmo1 != -1) && (pBot->current_weapon.iClip <= 10))
	   {
		   if(RANDOM_LONG(0, 40))
		   {
			   if(pBot->mTimeOfNextAmmoRequest == -1)
			   {
				   pBot->mTimeOfNextAmmoRequest = gpGlobals->time;
			   }
		   }
	   }
   }
   

   if ((pBot->b_bot_say_killed) && (pBot->f_bot_say_killed < gpGlobals->time))
   {
      int whine_index = 0;
      bool used;
      int i, recent_count;
      char msg[120];

      pBot->b_bot_say_killed = FALSE;
            
      recent_count = 0;

      while (recent_count < 5)
      {
         whine_index = RANDOM_LONG(0, whine_count-1);

         used = FALSE;

         for (i=0; i < 5; i++)
         {
            if (recent_bot_whine[i] == whine_index)
               used = TRUE;
         }

         if (used)
            recent_count++;
         else
            break;
      }

      for (i=4; i > 0; i--)
         recent_bot_whine[i] = recent_bot_whine[i-1];

      recent_bot_whine[0] = whine_index;

      if (strstr(bot_whine[whine_index], "%s") != NULL)  // is "%s" in whine text?
         sprintf(msg, bot_whine[whine_index], STRING(pBot->killer_edict->v.netname));
      else
         sprintf(msg, bot_whine[whine_index]);

      UTIL_HostSay(pEdict, 0, msg);
   }

   // if the bot is dead, randomly press fire to respawn...
   if ((pEdict->v.health < 1) || (pEdict->v.deadflag != DEAD_NO))
   {
      if (pBot->need_to_initialize)
      {
         BotSpawnInit(pBot);

         // did another player kill this bot AND bot whine messages loaded AND
         // has the bot been alive for at least 15 seconds AND
         if ((pBot->killer_edict != NULL) && (whine_count > 0) &&
             ((pBot->f_bot_spawn_time + 15.0) <= gpGlobals->time))
         {
            if ((RANDOM_LONG(1,100) <= 10))
            {
               pBot->b_bot_say_killed = TRUE;
               pBot->f_bot_say_killed = gpGlobals->time + 10.0 + RANDOM_FLOAT(0.0, 5.0);
            }
         }

         pBot->need_to_initialize = FALSE;
      }

      if (RANDOM_LONG(1, 100) > 50)
         pEdict->v.button = IN_ATTACK;

      g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle, pBot->f_move_speed,
                                   0, 0, pEdict->v.button, 0, pBot->msecval);

      return;
   }

   // set this for the next time the bot dies so it will initialize stuff
   if (pBot->need_to_initialize == FALSE)
   {
      pBot->need_to_initialize = TRUE;
      pBot->f_bot_spawn_time = gpGlobals->time;
   }

   is_idle = FALSE;

   if ((mod_id == FRONTLINE_DLL) && (pBot->round_end))
   {
      if (pBot->warmup)  // has warmup started (i.e. start of round?)
      {
         pBot->round_end = 0;

         BotSpawnInit(pBot);
      }

      is_idle = TRUE;

      flf_bug_fix = 0;  // BACKWARDS bug off now!
   }

   if ((mod_id == FRONTLINE_DLL) && (pBot->warmup) && (!pBot->defender))
   {
      if (pBot->curr_waypoint_index == -1)
      {
         // find the nearest visible waypoint
         int i = WaypointFindNearest(pEdict, REACHABLE_RANGE, pBot->defender);

         if (i != -1)
         {
            Vector v_direction = waypoints[i].origin - pEdict->v.origin;

            Vector bot_angles = UTIL_VecToAngles( v_direction );

            pBot->idle_angle = bot_angles.y;
         }
         else
            pBot->idle_angle = pEdict->v.v_angle.y;
      }

      is_idle = TRUE;
   }

   if (pBot->blinded_time > gpGlobals->time)
   {
      is_idle = TRUE;  // don't do anything while blinded
   }

   if(CVAR_GET_FLOAT("freezebots") > 0)
   {
        return; // Don't move.  
   }
   
   if (is_idle)
   {
      if (pBot->idle_angle_time <= gpGlobals->time)
      {
         pBot->idle_angle_time = gpGlobals->time + RANDOM_FLOAT(0.5, 2.0);

         pEdict->v.ideal_yaw = pBot->idle_angle + RANDOM_FLOAT(0.0, 40.0) - 20.0;

         BotFixIdealYaw(pEdict);
      }

      // turn towards ideal_yaw by yaw_speed degrees (slower than normal)
      BotChangeYaw( pBot, pEdict->v.yaw_speed / 2 );

      g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle, pBot->f_move_speed,
                                   0, 0, pEdict->v.button, 0, pBot->msecval);

      return;
   }
   else
   {
      pBot->idle_angle = pEdict->v.v_angle.y;
   }

   // check if time to check for player sounds (if don't already have enemy)
   if ((pBot->f_sound_update_time <= gpGlobals->time) &&
       (pBot->pBotEnemy == NULL))
   {
      int ind;
      edict_t *pPlayer;

      pBot->f_sound_update_time = gpGlobals->time + 1.0;

      for (ind = 1; ind <= gpGlobals->maxClients; ind++)
      {
         pPlayer = INDEXENT(ind);

         // is this player slot is valid and it's not this bot...
         if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEdict))
         {
            // if observer mode enabled, don't listen to this player...
            if ((b_observer_mode) && !(pPlayer->v.flags & FL_FAKECLIENT))
               continue;

            if (IsAlive(pPlayer) &&
                (FBitSet(pPlayer->v.flags, FL_CLIENT) ||
                 FBitSet(pPlayer->v.flags, FL_FAKECLIENT)))
            {
               // check for sounds being made by other players...
               if (UpdateSounds(pEdict, pPlayer))
               {
                  // don't check for sounds for another 30 seconds
                  pBot->f_sound_update_time = gpGlobals->time + 30.0;
               }
            }
         }
      }
   }

   pBot->f_move_speed = pBot->f_max_speed;  // set to max speed

   if (pBot->prev_time <= gpGlobals->time)
   {
      // see how far bot has moved since the previous position...
      v_diff = pBot->v_prev_origin - pEdict->v.origin;
      moved_distance = v_diff.Length();

      // save current position as previous
      pBot->v_prev_origin = pEdict->v.origin;
      pBot->prev_time = gpGlobals->time + 0.2;
   }
   else
   {
      moved_distance = 2.0;
   }

   // if the bot is under water, adjust pitch by pitch_speed degrees
   if ((pEdict->v.waterlevel == 2) ||
       (pEdict->v.waterlevel == 3))
   {
      // turn towards ideal_pitch by pitch_speed degrees
      pitch_degrees = BotChangePitch( pBot, pEdict->v.pitch_speed );
   }
   else
      pitch_degrees = 0.0;

   // turn towards ideal_yaw by yaw_speed degrees
   yaw_degrees = BotChangeYaw( pBot, pEdict->v.yaw_speed );

   if ((pitch_degrees >= pEdict->v.pitch_speed) ||
       (yaw_degrees >= pEdict->v.yaw_speed))
   {
      pBot->f_move_speed = 0.0;  // don't move while turning a lot
   }
   else if ((pitch_degrees >= 10) || 
            (yaw_degrees >= 10))  // turning more than 10 degrees?
   {
      pBot->f_move_speed = pBot->f_move_speed / 4;  // slow down while turning
   }
   else  // else handle movement related actions...
   {
      if (b_botdontshoot == 0)
      {
         if ((mod_id == TFC_DLL) && (pBot->bot_has_flag == TRUE))
         {
            // is it time to check whether bot should look for enemies yet?
            if (pBot->f_bot_find_enemy_time <= gpGlobals->time)
            {
               pBot->f_bot_find_enemy_time = gpGlobals->time + 5.0;

               if (RANDOM_LONG(1, 100) <= 40)
                  pBot->pBotEnemy = BotFindEnemy( pBot );
            }
         }
         else
         {
			 // Now that bots scan every entity in the world, do this less often
			 if(RANDOM_LONG(1, 100) <= 20)
			 {
				 pBot->pBotEnemy = BotFindEnemy( pBot );
			 }
         }
      }
      else
         pBot->pBotEnemy = NULL;  // clear enemy pointer (no ememy for you!)

      if (pBot->pBotEnemy != NULL)  // does an enemy exist?
      {
         BotShootAtEnemy( pBot );  // shoot at the enemy

         pBot->f_pause_time = 0;  // dont't pause if enemy exists
      }

      else if (pBot->f_pause_time > gpGlobals->time)  // is bot "paused"?
      {
         // you could make the bot look left then right, or look up
         // and down, to make it appear that the bot is hunting for
         // something (don't do anything right now)
      }

      // is bot being "used" and can still follow "user"?
      else if ((pBot->pBotUser != NULL) && BotFollowUser( pBot ))
      {
         // do nothing here!
         ;
      }

      else
      {
         // no enemy, let's just wander around...

         if ((pEdict->v.waterlevel != 2) &&  // is bot NOT under water?
             (pEdict->v.waterlevel != 3))
         {
            // reset pitch to 0 (level horizontally)
            pEdict->v.idealpitch = 0;
            pEdict->v.v_angle.x = 0;
         }

         pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)

         pEdict->v.angles.x = 0;
         pEdict->v.angles.y = pEdict->v.v_angle.y;
         pEdict->v.angles.z = 0;

         // check if bot should look for items now or not...
         if (pBot->f_find_item < gpGlobals->time)
         {
            BotFindItem( pBot );  // see if there are any visible items
         }

         // check if bot sees a tripmine...
         if (pBot->b_see_tripmine)
         {
            // check if bot can shoot the tripmine...
            if ((pBot->b_shoot_tripmine) && BotShootTripmine( pBot ))
            {
               // shot at tripmine, may or may not have hit it, clear
               // flags anyway, next BotFindItem will see it again if
               // it is still there...

               pBot->b_shoot_tripmine = FALSE;
               pBot->b_see_tripmine = FALSE;

               // pause for a while to allow tripmine to explode...
               pBot->f_pause_time = gpGlobals->time + 0.5;
            }
            else  // run away!!!
            {
               Vector tripmine_angles;

               tripmine_angles = UTIL_VecToAngles( pBot->v_tripmine - pEdict->v.origin );

               // face away from the tripmine
               pEdict->v.ideal_yaw += 180;  // rotate 180 degrees

               BotFixIdealYaw(pEdict);

               pBot->b_see_tripmine = FALSE;

               pBot->f_move_speed = 0;  // don't run while turning
            }
         }

         // check if should use wall mounted health station...
         else if (pBot->b_use_health_station)
         {
            if ((pBot->f_use_health_time + 10.0) > gpGlobals->time)
            {
               pBot->f_move_speed = 0;  // don't move while using health station

               pEdict->v.button = IN_USE;
            }
            else
            {
               // bot is stuck trying to "use" a health station...

               pBot->b_use_health_station = FALSE;

               // don't look for items for a while since the bot
               // could be stuck trying to get to an item
               pBot->f_find_item = gpGlobals->time + 0.5;
            }
         }

         // check if should use wall mounted HEV station...
         else if (pBot->b_use_HEV_station)
         {
            if ((pBot->f_use_HEV_time + 10.0) > gpGlobals->time)
            {
               pBot->f_move_speed = 0;  // don't move while using HEV station

               pEdict->v.button = IN_USE;
            }
            else
            {
               // bot is stuck trying to "use" a HEV station...

               pBot->b_use_HEV_station = FALSE;

               // don't look for items for a while since the bot
               // could be stuck trying to get to an item
               pBot->f_find_item = gpGlobals->time + 0.5;
            }
         }

         // check if should capture a point by using it...
         else if (pBot->b_use_capture)
         {
            int team = UTIL_GetTeam(pEdict);  // skin and team must match

            if (flf_bug_fix)
               team = 1 - team;  // BACKWARDS bug fix!

            // still capturing and hasn't captured yet...
            if ((pBot->f_use_capture_time > gpGlobals->time) &&
                (pBot->pCaptureEdict->v.skin == team))
            {
               pBot->f_move_speed = 0;  // don't move while capturing

               pEdict->v.button = IN_USE;
            }
            else
            {
               // bot is stuck trying to "use" a capture point...

               pBot->b_use_capture = FALSE;

               // don't look for items for a while since the bot
               // could be stuck trying to get to an item
               pBot->f_find_item = gpGlobals->time + 0.5;
            }
         }

         else if (pBot->b_use_button)
         {
            pBot->f_move_speed = 0;  // don't move while using elevator

            BotUseLift( pBot, moved_distance );
         }

         else
         {
            if (pEdict->v.waterlevel == 3)  // check if the bot is underwater...
            {
               BotUnderWater( pBot );
            }

            found_waypoint = FALSE;

            // if the bot is not trying to get to something AND
            // it is time to look for a waypoint AND
            // there are waypoints in this level...

            if ((pBot->pBotPickupItem == NULL) &&
                (pBot->f_look_for_waypoint_time <= gpGlobals->time) &&
                (num_waypoints != 0))
            {
               found_waypoint = BotHeadTowardWaypoint(pBot);
            }

            // check if the bot is on a ladder...
            if (pEdict->v.movetype == MOVETYPE_FLY)
            {
               // check if bot JUST got on the ladder...
               if ((pBot->f_end_use_ladder_time + 1.0) < gpGlobals->time)
                  pBot->f_start_use_ladder_time = gpGlobals->time;

               // go handle the ladder movement
               BotOnLadder( pBot, moved_distance );

               pBot->f_dont_avoid_wall_time = gpGlobals->time + 2.0;
               pBot->f_end_use_ladder_time = gpGlobals->time;
            }
            else
            {
               // check if the bot JUST got off the ladder...
               if ((pBot->f_end_use_ladder_time + 1.0) > gpGlobals->time)
               {
                  pBot->ladder_dir = LADDER_UNKNOWN;
               }
            }

            // if the bot isn't headed toward a waypoint...
            if (found_waypoint == FALSE)
            {
               TraceResult tr;

               // check if we should be avoiding walls
               if (pBot->f_dont_avoid_wall_time <= gpGlobals->time)
               {
                  // let's just randomly wander around
                  if (BotStuckInCorner( pBot ))
                  {
                     pEdict->v.ideal_yaw += 180;  // turn 180 degrees
            
                     BotFixIdealYaw(pEdict);
            
                     pBot->f_move_speed = 0;  // don't move while turning
                     pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
            
                     moved_distance = 2.0;  // dont use bot stuck code
                  }
                  else
                  {
                     // check if there is a wall on the left...
                     if (!BotCheckWallOnLeft( pBot ))
                     {
                        // if there was a wall on the left over 1/2 a second ago then
                        // 20% of the time randomly turn between 45 and 60 degrees
            
                        if ((pBot->f_wall_on_left != 0) &&
                            (pBot->f_wall_on_left <= gpGlobals->time - 0.5) &&
                            (RANDOM_LONG(1, 100) <= 20))
                        {
                           pEdict->v.ideal_yaw += RANDOM_LONG(45, 60);
            
                           BotFixIdealYaw(pEdict);
            
                           pBot->f_move_speed = 0;  // don't move while turning
                           pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
                        }
            
                        pBot->f_wall_on_left = 0;  // reset wall detect time
                     }
                     else if (!BotCheckWallOnRight( pBot ))
                     {
                        // if there was a wall on the right over 1/2 a second ago then
                        // 20% of the time randomly turn between 45 and 60 degrees
            
                        if ((pBot->f_wall_on_right != 0) &&
                            (pBot->f_wall_on_right <= gpGlobals->time - 0.5) &&
                            (RANDOM_LONG(1, 100) <= 20))
                        {
                           pEdict->v.ideal_yaw -= RANDOM_LONG(45, 60);
            
                           BotFixIdealYaw(pEdict);
            
                           pBot->f_move_speed = 0;  // don't move while turning
                           pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
                        }
            
                        pBot->f_wall_on_right = 0;  // reset wall detect time
                     }
                  }
               }
            
               // check if bot is about to hit a wall.  TraceResult gets returned
               if ((pBot->f_dont_avoid_wall_time <= gpGlobals->time) &&
                   BotCantMoveForward( pBot, &tr ))
               {
                  // ADD LATER
                  // need to check if bot can jump up or duck under here...
                  // ADD LATER
            
                  BotTurnAtWall( pBot, &tr );
               }
            }

            // check if bot is on a ladder and has been on a ladder for
            // more than 5 seconds...
            if ((pEdict->v.movetype == MOVETYPE_FLY) &&
                (pBot->f_start_use_ladder_time > 0.0) &&
                ((pBot->f_start_use_ladder_time + 5.0) <= gpGlobals->time))
            {
               // bot is stuck on a ladder...

               BotRandomTurn(pBot);

               // don't look for items for 2 seconds
               pBot->f_find_item = gpGlobals->time + 2.0;

               pBot->f_start_use_ladder_time = 0.0;  // reset start ladder time
            }
            
            // check if the bot hasn't moved much since the last location
            // (and NOT on a ladder since ladder stuck handled elsewhere)
            // (don't check for stuck if f_dont_check_stuck in the future)
            if ((moved_distance <= 1.0) && (pBot->prev_speed >= 1.0) &&
                (pEdict->v.movetype != MOVETYPE_FLY) &&
                (pBot->f_dont_check_stuck < gpGlobals->time))
            {
               // the bot must be stuck!

               pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0;
               pBot->f_look_for_waypoint_time = gpGlobals->time + 1.0;
            
               if (BotCanJumpUp( pBot ))  // can the bot jump onto something?
               {
                  if ((pBot->f_jump_time + 2.0) <= gpGlobals->time)
                  {
                     pBot->f_jump_time = gpGlobals->time;
                     pEdict->v.button |= IN_JUMP;  // jump up and move forward
                  }
                  else
                  {
                     // bot already tried jumping less than two seconds ago, just turn
                     BotRandomTurn(pBot);
                  }
               }
               else if (BotCanDuckUnder( pBot ))  // can the bot duck under something?
               {
                  pEdict->v.button |= IN_DUCK;  // duck down and move forward
               }
               else
               {
                  BotRandomTurn(pBot);

                  // is the bot trying to get to an item?...
                  if (pBot->pBotPickupItem != NULL)
                  {
                     // don't look for items for a while since the bot
                     // could be stuck trying to get to an item
                     pBot->f_find_item = gpGlobals->time + 0.5;
                  }
               }
            }

            // should the bot pause for a while here?
            // (don't pause on ladders or while being "used"...
            if ((RANDOM_LONG(1, 1000) <= pause_frequency[pBot->bot_skill]) &&
                (pEdict->v.movetype != MOVETYPE_FLY) &&
                (pBot->pBotUser == NULL))
            {
               // set the time that the bot will stop "pausing"
               pBot->f_pause_time = gpGlobals->time +
                  RANDOM_FLOAT(pause_time[pBot->bot_skill][0],
                               pause_time[pBot->bot_skill][1]);
            }
         }
      }
	}

   // Potentially morph up if we're an alien
   AvHUser3 theUser3 = (AvHUser3)(pBot->pEdict->v.iuser3);
   if((pBot->pBotEnemy == NULL) && (theUser3 >= AVH_USER3_ALIEN_PLAYER1) && (theUser3 <= AVH_USER3_ALIEN_PLAYER4))
   {
	   int theMaxUpgrade = (int)(AVH_USER3_ALIEN_PLAYER5 - theUser3);
	   //const UpgradeCostListType& theUpgradeCosts = GetGameRules()->GetUpgradeCosts();

	   if(RANDOM_LONG(0, 400) == 0)
	   {
		   // Pick a random upgrade
		   AvHUser3 theMinUpgrade = (AvHUser3)(theUser3 + 1);
		   int theMaxUpgrade = AVH_USER3_ALIEN_PLAYER5;
		   AvHUser3 theUpgradeUser3 = (AvHUser3)(RANDOM_LONG((int)theMinUpgrade, (int)theMaxUpgrade));
		   int thePointCost = 0;
		   AvHMessageID theMessage = MESSAGE_NULL;
		   switch(theUpgradeUser3)
		   {
		   case AVH_USER3_ALIEN_PLAYER2:
			   thePointCost = 15;
			   theMessage = ALIEN_LIFEFORM_TWO;
			   break;
		   case AVH_USER3_ALIEN_PLAYER3:
			   thePointCost = 30;
			   theMessage = ALIEN_LIFEFORM_THREE;
			   break;
		   case AVH_USER3_ALIEN_PLAYER4:
			   thePointCost = 50;
			   theMessage = ALIEN_LIFEFORM_FOUR;
			   break;
		   case AVH_USER3_ALIEN_PLAYER5:
			   thePointCost = 75;
			   theMessage = ALIEN_LIFEFORM_FIVE;
			   break;
		   }

		   //thePointCost = GetGameRules()->GetPointCostForMessageID(theMessage);

		   //if(pBot->mResources >= thePointCost) 
		   //{
			   pBot->pEdict->v.impulse = theMessage;
		   //}
	   }

	   if(RANDOM_LONG(0, 75) == 0)
	   {
		   // Pick a random upgrade to try to "buy"
		   AvHMessageID theUpgrade = (AvHMessageID)(ALIEN_EVOLUTION_ONE + RANDOM_LONG(0, kNumAlienUpgrades-1));
		   pBot->pEdict->v.impulse = theUpgrade;
	   }
   }

	// TODO: Help build a nearby team structure
	if(pBot->pBotEnemy == NULL && (theUser3 == AVH_USER3_MARINE_PLAYER))
	{
	}

   if (pBot->curr_waypoint_index != -1)  // does the bot have a waypoint?
   {
      // check if the next waypoint is a door waypoint...
      if (waypoints[pBot->curr_waypoint_index].flags & W_FL_DOOR)
      {
         pBot->f_move_speed = pBot->f_max_speed / 3;  // slow down for doors
      }

      // check if the next waypoint is a ladder waypoint...
      if (waypoints[pBot->curr_waypoint_index].flags & W_FL_LADDER)
      {
         // check if the waypoint is at the top of a ladder AND
         // the bot isn't currenly on a ladder...
         if ((pBot->waypoint_top_of_ladder) &&
             (pEdict->v.movetype != MOVETYPE_FLY))
         {
            // is the bot on "ground" above the ladder?
            if (pEdict->v.flags & FL_ONGROUND)
            {
               float waypoint_distance = (pEdict->v.origin - pBot->waypoint_origin).Length();

               if (waypoint_distance <= 20.0)  // if VERY close...
                  pBot->f_move_speed = 20.0;  // go VERY slow
               else if (waypoint_distance <= 100.0)  // if fairly close...
                  pBot->f_move_speed = 50.0;  // go fairly slow

               pBot->ladder_dir = LADDER_DOWN;
               pBot->f_dont_check_stuck = gpGlobals->time + 1.0;
            }
            else  // bot must be in mid-air, go BACKWARDS to touch ladder...
            {
               pBot->f_move_speed = -pBot->f_max_speed;
            }
         }
         else
         {
            // don't avoid walls for a while
            pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0;

            pBot->waypoint_top_of_ladder = FALSE;
         }
      }

      // check if the next waypoint is a crouch waypoint...
      if (waypoints[pBot->curr_waypoint_index].flags & W_FL_CROUCH)
         pEdict->v.button |= IN_DUCK;  // duck down while moving forward

      // check if the waypoint is a sniper waypoint AND
      // bot isn't currently aiming at an ememy...
      if ((waypoints[pBot->curr_waypoint_index].flags & W_FL_SNIPER) &&
          (pBot->pBotEnemy == NULL))
      {
         if ((mod_id != TFC_DLL) ||
             ((mod_id == TFC_DLL) && (pEdict->v.playerclass == TFC_CLASS_SNIPER)))
         {
            // check if it's time to adjust aim yet...
            if (pBot->f_sniper_aim_time <= gpGlobals->time)
            {
               int aim_index;

               aim_index = WaypointFindNearestAiming(waypoints[pBot->curr_waypoint_index].origin);

               if (aim_index != -1)
               {
                  Vector v_aim = waypoints[aim_index].origin - waypoints[pBot->curr_waypoint_index].origin;

                  Vector aim_angles = UTIL_VecToAngles( v_aim );

                  aim_angles.y += RANDOM_LONG(0, 30) - 15;

                  pEdict->v.ideal_yaw = aim_angles.y;

                  BotFixIdealYaw(pEdict);
               }

               // don't adjust aim again until after a few seconds...
               pBot->f_sniper_aim_time = gpGlobals->time + RANDOM_FLOAT(3.0, 5.0);
            }
         }
      }
   }

   // Process voice commands
   BotProcessVoiceCommands(pBot);
   
   if (pBot->f_pause_time > gpGlobals->time)  // is the bot "paused"?
      pBot->f_move_speed = 0;  // don't move while pausing

   // make the body face the same way the bot is looking
   pEdict->v.angles.y = pEdict->v.v_angle.y;

   // save the previous speed (for checking if stuck)
   pBot->prev_speed = pBot->f_move_speed;

   //CBaseEntity* theEntity = CBaseEntity::Instance(pEdict);
   //CBasePlayer* thePlayer = (CBasePlayer*)(theEntity);
   //thePlayer->PreThink();
   
   g_engfuncs.pfnRunPlayerMove( pEdict, pEdict->v.v_angle, pBot->f_move_speed,
                                0, 0, pEdict->v.button, 0, pBot->msecval);

   //thePlayer->PostThink();
   
   return;
}

