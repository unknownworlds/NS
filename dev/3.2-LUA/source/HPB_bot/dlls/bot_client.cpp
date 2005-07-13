//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_client.cpp
//

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"

#include "bot.h"
#include "bot_func.h"
#include "bot_client.h"
#include "bot_weapons.h"

// types of damage to ignore...
#define IGNORE_DAMAGE (DMG_CRUSH | DMG_FREEZE | DMG_FALL | DMG_SHOCK | \
                       DMG_DROWN | DMG_NERVEGAS | DMG_RADIATION | \
                       DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN | \
                       DMG_SLOWFREEZE | 0xFF000000)

extern int mod_id;
extern bot_t bots[32];

bot_weapon_t weapon_defs[MAX_WEAPONS]; // array of weapon definitions


// This message is sent when the TFC VGUI menu is displayed.
void BotClient_TFC_VGUI(void *p, int bot_index)
{
   if ((*(int *)p) == 2)  // is it a team select menu?

      bots[bot_index].start_action = MSG_TFC_TEAM_SELECT;

   else if ((*(int *)p) == 3)  // is is a class selection menu?

      bots[bot_index].start_action = MSG_TFC_CLASS_SELECT;
}


// This message is sent when the Counter-Strike VGUI menu is displayed.
void BotClient_CS_VGUI(void *p, int bot_index)
{
   if ((*(int *)p) == 2)  // is it a team select menu?

      bots[bot_index].start_action = MSG_CS_TEAM_SELECT;

   else if ((*(int *)p) == 26)  // is is a terrorist model select menu?

      bots[bot_index].start_action = MSG_CS_T_SELECT;

   else if ((*(int *)p) == 27)  // is is a counter-terrorist model select menu?

      bots[bot_index].start_action = MSG_CS_CT_SELECT;
}


// This message is sent when a menu is being displayed in Counter-Strike.
void BotClient_CS_ShowMenu(void *p, int bot_index)
{
   static int state = 0;   // current state machine state

   if (state < 3)
   {
      state++;  // ignore first 3 fields of message
      return;
   }

   if (strcmp((char *)p, "#Team_Select") == 0)  // team select menu?
   {
      bots[bot_index].start_action = MSG_CS_TEAM_SELECT;
   }
   else if (strcmp((char *)p, "#Terrorist_Select") == 0)  // T model select?
   {
      bots[bot_index].start_action = MSG_CS_T_SELECT;
   }
   else if (strcmp((char *)p, "#CT_Select") == 0)  // CT model select menu?
   {
      bots[bot_index].start_action = MSG_CS_CT_SELECT;
   }

   state = 0;  // reset state machine
}


// This message is sent when the TFC VGUI menu is displayed.
void BotClient_Gearbox_VGUI(void *p, int bot_index)
{
   if ((*(int *)p) == 2)  // is it a team select menu?

      bots[bot_index].start_action = MSG_OPFOR_TEAM_SELECT;

   else if ((*(int *)p) == 3)  // is is a class selection menu?

      bots[bot_index].start_action = MSG_OPFOR_CLASS_SELECT;
}


// This message is sent when the FrontLineForce VGUI menu is displayed.
void BotClient_FLF_VGUI(void *p, int bot_index)
{
   if ((*(int *)p) == 2)  // is it a team select menu?
      bots[bot_index].start_action = MSG_FLF_TEAM_SELECT;
   else if ((*(int *)p) == 3)  // is it a class selection menu?
      bots[bot_index].start_action = MSG_FLF_CLASS_SELECT;
   else if ((*(int *)p) == 70)  // is it a weapon selection menu?
      bots[bot_index].start_action = MSG_FLF_WEAPON_SELECT;
   else if ((*(int *)p) == 72)  // is it a submachine gun selection menu?
      bots[bot_index].start_action = MSG_FLF_SUBMACHINE_SELECT;
   else if ((*(int *)p) == 73)  // is it a shotgun selection menu?
      bots[bot_index].start_action = MSG_FLF_SHOTGUN_SELECT;
   else if ((*(int *)p) == 75)  // is it a rifle selection menu?
      bots[bot_index].start_action = MSG_FLF_RIFLE_SELECT;
   else if ((*(int *)p) == 76)  // is it a pistol selection menu?
      bots[bot_index].start_action = MSG_FLF_PISTOL_SELECT;
   else if ((*(int *)p) == 78)  // is it a heavyweapons selection menu?
      bots[bot_index].start_action = MSG_FLF_HEAVYWEAPONS_SELECT;
}


// This message is sent when a client joins the game.  All of the weapons
// are sent with the weapon ID and information about what ammo is used.
void BotClient_Valve_WeaponList(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static bot_weapon_t bot_weapon;

   if (state == 0)
   {
      state++;
      strcpy(bot_weapon.szClassname, (char *)p);
   }
   else if (state == 1)
   {
      state++;
      bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
   }
   else if (state == 2)
   {
      state++;
      bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
   }
   else if (state == 3)
   {
      state++;
      bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
   }
   else if (state == 4)
   {
      state++;
      bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
   }
   else if (state == 5)
   {
      state++;
      bot_weapon.iSlot = *(int *)p;  // slot for this weapon
   }
   else if (state == 6)
   {
      state++;
      bot_weapon.iPosition = *(int *)p;  // position in slot
   }
   else if (state == 7)
   {
      state++;
      bot_weapon.iId = *(int *)p;  // weapon ID
   }
   else if (state == 8)
   {
      state = 0;

      bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)

      // store away this weapon with it's ammo information...
      weapon_defs[bot_weapon.iId] = bot_weapon;
   }
}

void BotClient_TFC_WeaponList(void *p, int bot_index)
{
   // this is just like the Valve Weapon List message
   BotClient_Valve_WeaponList(p, bot_index);
}

void BotClient_CS_WeaponList(void *p, int bot_index)
{
   // this is just like the Valve Weapon List message
   BotClient_Valve_WeaponList(p, bot_index);
}

void BotClient_Gearbox_WeaponList(void *p, int bot_index)
{
   // this is just like the Valve Weapon List message
   BotClient_Valve_WeaponList(p, bot_index);
}

void BotClient_FLF_WeaponList(void *p, int bot_index)
{
   // this is just like the Valve Weapon List message
   BotClient_Valve_WeaponList(p, bot_index);
}


// This message is sent when a weapon is selected (either by the bot chosing
// a weapon or by the server auto assigning the bot a weapon).
void BotClient_Valve_CurrentWeapon(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int iState;
   static int iId;
   static int iClip;

   if (state == 0)
   {
      state++;
      iState = *(int *)p;  // state of the current weapon
   }
   else if (state == 1)
   {
      state++;
      iId = *(int *)p;  // weapon ID of current weapon
   }
   else if (state == 2)
   {
      state = 0;

      iClip = *(int *)p;  // ammo currently in the clip for this weapon

      if (iId <= 31)
      {
         bots[bot_index].bot_weapons |= (1<<iId);  // set this weapon bit

         if (iState == 1)
         {
            bots[bot_index].current_weapon.iId = iId;
            bots[bot_index].current_weapon.iClip = iClip;

            // update the ammo counts for this weapon...
            bots[bot_index].current_weapon.iAmmo1 =
               bots[bot_index].m_rgAmmo[weapon_defs[iId].iAmmo1];
            bots[bot_index].current_weapon.iAmmo2 =
               bots[bot_index].m_rgAmmo[weapon_defs[iId].iAmmo2];
         }
      }
   }
}

void BotClient_TFC_CurrentWeapon(void *p, int bot_index)
{
   // this is just like the Valve Current Weapon message
   BotClient_Valve_CurrentWeapon(p, bot_index);
}

void BotClient_CS_CurrentWeapon(void *p, int bot_index)
{
   // this is just like the Valve Current Weapon message
   BotClient_Valve_CurrentWeapon(p, bot_index);
}

void BotClient_Gearbox_CurrentWeapon(void *p, int bot_index)
{
   // this is just like the Valve Current Weapon message
   BotClient_Valve_CurrentWeapon(p, bot_index);
}

void BotClient_FLF_CurrentWeapon(void *p, int bot_index)
{
   // this is just like the Valve Current Weapon message
   BotClient_Valve_CurrentWeapon(p, bot_index);
}


// This message is sent whenever ammo ammounts are adjusted (up or down).
void BotClient_Valve_AmmoX(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int index;
   static int ammount;
   int ammo_index;

   if (state == 0)
   {
      state++;
      index = *(int *)p;  // ammo index (for type of ammo)
   }
   else if (state == 1)
   {
      state = 0;

      ammount = *(int *)p;  // the ammount of ammo currently available

      bots[bot_index].m_rgAmmo[index] = ammount;  // store it away

      ammo_index = bots[bot_index].current_weapon.iId;

      // update the ammo counts for this weapon...
      bots[bot_index].current_weapon.iAmmo1 =
         bots[bot_index].m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
      bots[bot_index].current_weapon.iAmmo2 =
         bots[bot_index].m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
   }
}

void BotClient_TFC_AmmoX(void *p, int bot_index)
{
   // this is just like the Valve AmmoX message
   BotClient_Valve_AmmoX(p, bot_index);
}

void BotClient_CS_AmmoX(void *p, int bot_index)
{
   // this is just like the Valve AmmoX message
   BotClient_Valve_AmmoX(p, bot_index);
}

void BotClient_Gearbox_AmmoX(void *p, int bot_index)
{
   // this is just like the Valve AmmoX message
   BotClient_Valve_AmmoX(p, bot_index);
}

void BotClient_FLF_AmmoX(void *p, int bot_index)
{
   // this is just like the Valve AmmoX message
   BotClient_Valve_AmmoX(p, bot_index);
}


// This message is sent when the bot picks up some ammo (AmmoX messages are
// also sent so this message is probably not really necessary except it
// allows the HUD to draw pictures of ammo that have been picked up.  The
// bots don't really need pictures since they don't have any eyes anyway.
void BotClient_Valve_AmmoPickup(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int index;
   static int ammount;
   int ammo_index;

   if (state == 0)
   {
      state++;
      index = *(int *)p;
   }
   else if (state == 1)
   {
      state = 0;

      ammount = *(int *)p;

      bots[bot_index].m_rgAmmo[index] = ammount;

      ammo_index = bots[bot_index].current_weapon.iId;

      // update the ammo counts for this weapon...
      bots[bot_index].current_weapon.iAmmo1 =
         bots[bot_index].m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
      bots[bot_index].current_weapon.iAmmo2 =
         bots[bot_index].m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
   }
}

void BotClient_TFC_AmmoPickup(void *p, int bot_index)
{
   // this is just like the Valve Ammo Pickup message
   BotClient_Valve_AmmoPickup(p, bot_index);
}

void BotClient_CS_AmmoPickup(void *p, int bot_index)
{
   // this is just like the Valve Ammo Pickup message
   BotClient_Valve_AmmoPickup(p, bot_index);
}

void BotClient_Gearbox_AmmoPickup(void *p, int bot_index)
{
   // this is just like the Valve Ammo Pickup message
   BotClient_Valve_AmmoPickup(p, bot_index);
}

void BotClient_FLF_AmmoPickup(void *p, int bot_index)
{
   // this is just like the Valve Ammo Pickup message
   BotClient_Valve_AmmoPickup(p, bot_index);
}


// This message gets sent when the bot picks up a weapon.
void BotClient_Valve_WeaponPickup(void *p, int bot_index)
{
   int index;

   index = *(int *)p;

   // set this weapon bit to indicate that we are carrying this weapon
   bots[bot_index].bot_weapons |= (1<<index);
}

void BotClient_TFC_WeaponPickup(void *p, int bot_index)
{
   // this is just like the Valve Weapon Pickup message
   BotClient_Valve_WeaponPickup(p, bot_index);
}

void BotClient_CS_WeaponPickup(void *p, int bot_index)
{
   // this is just like the Valve Weapon Pickup message
   BotClient_Valve_WeaponPickup(p, bot_index);
}

void BotClient_Gearbox_WeaponPickup(void *p, int bot_index)
{
   // this is just like the Valve Weapon Pickup message
   BotClient_Valve_WeaponPickup(p, bot_index);
}

void BotClient_FLF_WeaponPickup(void *p, int bot_index)
{
   // this is just like the Valve Weapon Pickup message
   BotClient_Valve_WeaponPickup(p, bot_index);
}


// This message gets sent when the bot picks up an item (like a battery
// or a healthkit)
void BotClient_Valve_ItemPickup(void *p, int bot_index)
{
}

void BotClient_TFC_ItemPickup(void *p, int bot_index)
{
   // this is just like the Valve Item Pickup message
   BotClient_Valve_ItemPickup(p, bot_index);
}

void BotClient_CS_ItemPickup(void *p, int bot_index)
{
   // this is just like the Valve Item Pickup message
   BotClient_Valve_ItemPickup(p, bot_index);
}

void BotClient_Gearbox_ItemPickup(void *p, int bot_index)
{
   // this is just like the Valve Item Pickup message
   BotClient_Valve_ItemPickup(p, bot_index);
}

void BotClient_FLF_ItemPickup(void *p, int bot_index)
{
   // this is just like the Valve Item Pickup message
   BotClient_Valve_ItemPickup(p, bot_index);
}


// This message gets sent when the bots health changes.
void BotClient_Valve_Health(void *p, int bot_index)
{
   bots[bot_index].bot_health = *(int *)p;  // health ammount
}

void BotClient_TFC_Health(void *p, int bot_index)
{
   // this is just like the Valve Health message
   BotClient_Valve_Health(p, bot_index);
}

void BotClient_CS_Health(void *p, int bot_index)
{
   // this is just like the Valve Health message
   BotClient_Valve_Health(p, bot_index);
}

void BotClient_Gearbox_Health(void *p, int bot_index)
{
   // this is just like the Valve Health message
   BotClient_Valve_Health(p, bot_index);
}

void BotClient_FLF_Health(void *p, int bot_index)
{
   // this is just like the Valve Health message
   BotClient_Valve_Health(p, bot_index);
}


// This message gets sent when the bots armor changes.
void BotClient_Valve_Battery(void *p, int bot_index)
{
   bots[bot_index].bot_armor = *(int *)p;  // armor ammount
}

void BotClient_TFC_Battery(void *p, int bot_index)
{
   // this is just like the Valve Battery message
   BotClient_Valve_Battery(p, bot_index);
}

void BotClient_CS_Battery(void *p, int bot_index)
{
   // this is just like the Valve Battery message
   BotClient_Valve_Battery(p, bot_index);
}

void BotClient_Gearbox_Battery(void *p, int bot_index)
{
   // this is just like the Valve Battery message
   BotClient_Valve_Battery(p, bot_index);
}

void BotClient_FLF_Battery(void *p, int bot_index)
{
   // this is just like the Valve Battery message
   BotClient_Valve_Battery(p, bot_index);
}


// This message gets sent when the bots are getting damaged.
void BotClient_Valve_Damage(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int damage_armor;
   static int damage_taken;
   static int damage_bits;  // type of damage being done
   static Vector damage_origin;

   if (state == 0)
   {
      state++;
      damage_armor = *(int *)p;
   }
   else if (state == 1)
   {
      state++;
      damage_taken = *(int *)p;
   }
   else if (state == 2)
   {
      state++;
      damage_bits = *(int *)p;
   }
   else if (state == 3)
   {
      state++;
      damage_origin.x = *(float *)p;
   }
   else if (state == 4)
   {
      state++;
      damage_origin.y = *(float *)p;
   }
   else if (state == 5)
   {
      state = 0;

      damage_origin.z = *(float *)p;

      if ((damage_armor > 0) || (damage_taken > 0))
      {
         // ignore certain types of damage...
         if (damage_bits & IGNORE_DAMAGE)
            return;

         // if the bot doesn't have an enemy and someone is shooting at it then
         // turn in the attacker's direction...
         if (bots[bot_index].pBotEnemy == NULL)
         {
            // face the attacker...
            Vector v_enemy = damage_origin - bots[bot_index].pEdict->v.origin;
            Vector bot_angles = UTIL_VecToAngles( v_enemy );

            bots[bot_index].pEdict->v.ideal_yaw = bot_angles.y;

            BotFixIdealYaw(bots[bot_index].pEdict);

            // stop using health or HEV stations...
            bots[bot_index].b_use_health_station = FALSE;
            bots[bot_index].b_use_HEV_station = FALSE;
            bots[bot_index].b_use_capture = FALSE;
         }
      }
   }
}

void BotClient_TFC_Damage(void *p, int bot_index)
{
   // this is just like the Valve Battery message
   BotClient_Valve_Damage(p, bot_index);
}

void BotClient_CS_Damage(void *p, int bot_index)
{
   // this is just like the Valve Battery message
	BotClient_Valve_Damage(p, bot_index);
}

void BotClient_Gearbox_Damage(void *p, int bot_index)
{
	// this is just like the Valve Battery message
	BotClient_Valve_Damage(p, bot_index);
}

void BotClient_FLF_Damage(void *p, int bot_index)
{
	// this is just like the Valve Battery message
	BotClient_Valve_Damage(p, bot_index);
}


// This message gets sent when the bots money ammount changes (for CS)
void BotClient_CS_Money(void *p, int bot_index)
{
	static int state = 0;   // current state machine state
	
	if (state == 0)
	{
		state++;
		
		bots[bot_index].bot_money = *(int *)p;  // amount of money
	}
	else
	{
		state = 0;  // ingore this field
	}
}

// This message gets sent when the bots get killed
void BotClient_Valve_DeathMsg(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int killer_index;
   static int victim_index;
   static edict_t *victim_edict;
   static int index;

   if (state == 0)
   {
      state++;
      killer_index = *(int *)p;  // ENTINDEX() of killer
   }
   else if (state == 1)
   {
      state++;
      victim_index = *(int *)p;  // ENTINDEX() of victim
   }
   else if (state == 2)
   {
      state = 0;

      victim_edict = INDEXENT(victim_index);

      index = UTIL_GetBotIndex(victim_edict);

      // is this message about a bot being killed?
      if (index != -1)
      {
         if ((killer_index == 0) || (killer_index == victim_index))
         {
            // bot killed by world (worldspawn) or bot killed self...
            bots[index].killer_edict = NULL;
         }
         else
         {
            // store edict of player that killed this bot...
            bots[index].killer_edict = INDEXENT(killer_index);
         }
      }
   }
}

void BotClient_TFC_DeathMsg(void *p, int bot_index)
{
   // this is just like the Valve DeathMsg message
   BotClient_Valve_DeathMsg(p, bot_index);
}

void BotClient_CS_DeathMsg(void *p, int bot_index)
{
   // this is just like the Valve DeathMsg message
   BotClient_Valve_DeathMsg(p, bot_index);
}

void BotClient_Gearbox_DeathMsg(void *p, int bot_index)
{
   // this is just like the Valve DeathMsg message
   BotClient_Valve_DeathMsg(p, bot_index);
}

void BotClient_FLF_DeathMsg(void *p, int bot_index)
{
   // this is just like the Valve DeathMsg message
   BotClient_Valve_DeathMsg(p, bot_index);
}


// This message gets sent when a text message is displayed
void BotClient_FLF_TextMsg(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int msg_dest = 0;

   if (state == 0)
   {
      state++;
      msg_dest = *(int *)p;  // HUD_PRINTCENTER, etc.
   }
   else if (state == 1)
   {
      state = 0;

      if (strcmp((char *)p, "You are Attacking\n") == 0)  // attacker msg
      {
         bots[bot_index].defender = 0;  // attacker
      }
      else if (strcmp((char *)p, "You are Defending\n") == 0)  // defender msg
      {
         bots[bot_index].defender = 1;  // defender
      }
   }
}


// This message gets sent when the WarmUpTime is enabled/disabled
void BotClient_FLF_WarmUp(void *p, int bot_index)
{
   bots[bot_index].warmup = *(int *)p;
}


// This message gets sent to ALL when the WarmUpTime is enabled/disabled
void BotClient_FLF_WarmUpAll(void *p, int bot_index)
{
   for (int i=0; i < 32; i++)
   {
      if (bots[i].is_used)  // count the number of bots in use
         bots[i].warmup = *(int *)p;
   }
}


// This message gets sent when the round is over
void BotClient_FLF_WinMessage(void *p, int bot_index)
{
   for (int i=0; i < 32; i++)
   {
      if (bots[i].is_used)  // count the number of bots in use
         bots[i].round_end = 1;
   }
}


// This message gets sent when a temp entity is created
void BotClient_FLF_TempEntity(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int te_type;     // TE_ message type

   if (p == NULL)  // end of message?
   {
      state = 0;
      return;
   }

   if (state == 0)
   {
      state++;
      te_type = *(int *)p;

      return;
   }

   if (te_type == TE_TEXTMESSAGE)
   {
      if (state == 16)
      {
         if (strncmp((char *)p, "Capturing ", 10) == 0)
         {
            // if bot is currently capturing, keep timer alive...
            if (bots[bot_index].b_use_capture)
               bots[bot_index].f_use_capture_time = gpGlobals->time + 2.0;
         }
      }

      state++;
   }
}


void BotClient_Valve_ScreenFade(void *p, int bot_index)
{
   static int state = 0;   // current state machine state
   static int duration;
   static int hold_time;
   static int fade_flags;
   int length;

   if (state == 0)
   {
      state++;
      duration = *(int *)p;
   }
   else if (state == 1)
   {
      state++;
      hold_time = *(int *)p;
   }
   else if (state == 2)
   {
      state++;
      fade_flags = *(int *)p;
   }
   else if (state == 6)
   {
      state = 0;

      length = (duration + hold_time) / 4096;
      bots[bot_index].blinded_time = gpGlobals->time + length - 2.0;
   }
   else
   {
      state++;
   }
}

void BotClient_TFC_ScreenFade(void *p, int bot_index)
{
   // this is just like the Valve ScreenFade message
   BotClient_Valve_ScreenFade(p, bot_index);
}

void BotClient_CS_ScreenFade(void *p, int bot_index)
{
   // this is just like the Valve ScreenFade message
   BotClient_Valve_ScreenFade(p, bot_index);
}

void BotClient_Gearbox_ScreenFade(void *p, int bot_index)
{
   // this is just like the Valve ScreenFade message
   BotClient_Valve_ScreenFade(p, bot_index);
}

void BotClient_FLF_ScreenFade(void *p, int bot_index)
{
   // this is just like the Valve ScreenFade message
   BotClient_Valve_ScreenFade(p, bot_index);
}

// AvH client functions
void BotClient_AVH_SetOrder(void *p, int bot_index)
{
	bot_t* theCurrentBot = &bots[bot_index];

	if(theCurrentBot->mOrderNumPlayers == -1)
	{
		if (theCurrentBot->mOrderState == 0)
		{
			// Read num players
			theCurrentBot->mOrderState++;
			theCurrentBot->mOrderNumPlayers = *(byte *)p;
		}
	}
	else
	{
		if(theCurrentBot->mOrderState <= theCurrentBot->mOrderNumPlayers)
		{
			// Skip by players
			theCurrentBot->mOrderState++;
		}
		else
		{
			int theRealState = theCurrentBot->mOrderState - theCurrentBot->mOrderNumPlayers - 1;
			if(theRealState == 0)
			{
				// read order
				theCurrentBot->mOrderState++;
				theCurrentBot->mOrderType = *((AvHOrderType *)p);
			}
			else if(theRealState == 1)
			{
				// read order target type
				theCurrentBot->mOrderState++;
				theCurrentBot->mOrderTargetType = *((AvHOrderTargetType *)p);
			}
			else 
			{
				if((theCurrentBot->mOrderTargetType == ORDERTARGETTYPE_LOCATION) && ((theRealState >= 2) && (theRealState <= 4)))
				{
					theCurrentBot->mOrderLocation[theRealState-2] = *((float*)p);
					theCurrentBot->mOrderState++;
				}
				else if((theCurrentBot->mOrderTargetType == ORDERTARGETTYPE_TARGET) && (theRealState == 2))
				{
					theCurrentBot->mOrderTargetIndex = *((int*)p);
					theCurrentBot->mOrderState++;
				}
				else if(	((theCurrentBot->mOrderTargetType == ORDERTARGETTYPE_LOCATION) && (theRealState == 5)) ||
							((theCurrentBot->mOrderTargetType == ORDERTARGETTYPE_TARGET) && (theRealState == 3)) ||
							(theRealState == 2)	)
				{
					theCurrentBot->mOrderCompleted = *((int*)p);
					theCurrentBot->mOrderState++;
				}
				else
				{
					// Reset
					theCurrentBot->mOrderState = 0;
					theCurrentBot->mOrderNumPlayers = -1;
					theCurrentBot->mOrderType = ORDERTYPE_UNDEFINED;
					theCurrentBot->mOrderTargetType = ORDERTARGETTYPE_UNDEFINED;
					theCurrentBot->mOrderLocation[0] = theCurrentBot->mOrderLocation[1] = theCurrentBot->mOrderLocation[2] = 0.0f;
					theCurrentBot->mOrderTargetIndex = -1;
					theCurrentBot->mOrderCompleted = false;
				}
			}
		}
	}
}



void BotClient_AVH_SetPlayMode(void *p, int bot_index)
{
	static int state = 0;   // current state machine state
	
	if (state == 0)
	{
		state++;
		
		bots[bot_index].mBotPlayMode = (AvHPlayMode)(*(byte *)p);  // amount of money
		bots[bot_index].pBotEnemy = NULL;
	}
	else
	{
		state = 0;  // ingore this field
	}
}

void BotClient_AVH_SetResources(void *p, int bot_index)
{
	static int state = 0;   // current state machine state
	
	if(state == 0)
	{
		state++;
	}
	else if(state == 1)
	{
		bots[bot_index].mResources = (*(int *)p);  // amount of money
		state += 4;
	}
	else
	{
		state = 0;  // ingore this field
	}
}



