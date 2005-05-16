#include "AvHNetworkMessages.h"
#include "NetworkMeter.h"
#include "util/MathUtil.h" //for WrapFloat
#include "util/STLUtil.h" //for MakeBytesFromHexPairs
#include "cl_dll/parsemsg.h"
  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MESSAGE CODES - NEED TO BE INITIALIZED BEFORE CLIENT CONNECTION, OR THEY'D
// BE LOCAL STATICS INSIDE OF THE FUNCTIONS USING LAZY INSTANTIATION
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef AVH_SERVER
int	g_msgAmmoPickup = 0, g_msgAmmoX, g_msgBattery, g_msgCurWeapon, g_msgDamage,						//5
	g_msgDeathMsg, g_msgFade, g_msgFlashlight, g_msgGeigerRange, g_msgHealth,						//10
	g_msgHideWeapon, g_msgHudText, g_msgHudText2, g_msgInitHUD, g_msgItemPickup,
	g_msgMOTD, g_msgResetHUD, g_msgSayText, g_msgScoreInfo, g_msgServerName,
	g_msgSetFOV, g_msgShake, g_msgShowGameTitle, g_msgShowMenu, g_msgStatusText,
	g_msgStatusValue, g_msgTeamInfo, g_msgTeamNames, g_msgTeamScore, g_msgTextMsg,
	g_msgTrain, g_msgWeaponList, g_msgWeapPickup, g_msgAlienInfo, g_msgBalanceVar,
	g_msgBlipList, g_msgBuildMiniMap, g_msgClientScripts, g_msgDebugCSP, g_msgEditPS,
	g_msgFog, g_msgGameStatus, g_msgListPS, g_msgPlayHUDNotification, g_msgProgressBar,
	g_msgServerVar, g_msgSetGammaRamp, g_msgSetOrder, g_msgSetParticleTemplates,
	g_msgSetSelect, g_msgSetRequest, g_msgSetSoundNames, g_msgSetTechNodes, g_msgSetTechSlots,
	g_msgSetTopDown, g_msgSetupMap, g_msgUpdateCountdown, g_msgUpdateEntityHierarchy,
	g_msgProfileInfo, g_msgNexusBytes, g_msgIssueOrder;

void Net_InitializeMessages(void)
{
	if( g_msgAmmoPickup != 0 ) { return; }
	g_msgAmmoPickup = REG_USER_MSG("AmmoPickup", 2 );
	g_msgAmmoX = REG_USER_MSG("AmmoX", 2 );
    g_msgBattery = REG_USER_MSG( "Battery", 2 );
	g_msgCurWeapon = REG_USER_MSG( "CurWeapon", 3 );
	g_msgDamage = REG_USER_MSG( "Damage", 12 );
	g_msgDeathMsg = REG_USER_MSG( "DeathMsg", -1 );
	g_msgFade = REG_USER_MSG( "ScreenFade", sizeof(ScreenFade) );
	g_msgFlashlight = REG_USER_MSG( "FLashlight", 2 );
	g_msgGeigerRange = REG_USER_MSG( "Geiger", 1 );
	g_msgHealth = REG_USER_MSG( "Health", 2 );
	g_msgHideWeapon = REG_USER_MSG( "HideWeapon", 1 );
    g_msgHudText = REG_USER_MSG( "HudText", -1 );
	g_msgHudText2 = REG_USER_MSG( "HudText2", -1 );
	g_msgInitHUD = REG_USER_MSG( "InitHUD", 0 );
	g_msgItemPickup = REG_USER_MSG( "ItemPickup", -1 );
	g_msgMOTD = REG_USER_MSG( "MOTD", -1 );
	g_msgResetHUD = REG_USER_MSG( "ResetHUD", 0 );
	g_msgSayText = REG_USER_MSG( "SayText", -1 );
	g_msgScoreInfo = REG_USER_MSG( "ScoreInfo", 10 );
	g_msgServerName = REG_USER_MSG( "ServerName", -1 );
	g_msgSetFOV = REG_USER_MSG( "SetFOV", 1 );
	g_msgShake = REG_USER_MSG( "ScreenShake", 6 );
	g_msgShowGameTitle = REG_USER_MSG( "GameTitle", 1 );
	g_msgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
	g_msgStatusText = REG_USER_MSG( "StatusText", -1 );
	g_msgStatusValue = REG_USER_MSG( "StatusValue", 3 );
	g_msgTeamInfo = REG_USER_MSG( "TeamInfo", -1 );
	g_msgTeamNames = REG_USER_MSG( "TeamNames", -1 );
	g_msgTeamScore = REG_USER_MSG( "TeamScore", -1 );
	g_msgTextMsg = REG_USER_MSG( "TextMsg", -1 );
	g_msgTrain = REG_USER_MSG( "Train", 1 );
	g_msgWeaponList = REG_USER_MSG( "WeaponList", -1 );
	g_msgWeapPickup = REG_USER_MSG( "WeapPickup", 1 );
	g_msgAlienInfo = REG_USER_MSG( "AlienInfo", -1 );
	g_msgBalanceVar = REG_USER_MSG( "BalanceVar", -1 );
	g_msgBlipList = REG_USER_MSG( "BlipList", -1 );
	g_msgBuildMiniMap = REG_USER_MSG( "MiniMap", -1 );
	g_msgClientScripts = REG_USER_MSG( "ClScript", -1 );
	g_msgDebugCSP = REG_USER_MSG( "DebugCSP", 14 );
	g_msgEditPS = REG_USER_MSG( "EditPS", 2 );
	g_msgFog = REG_USER_MSG( "Fog", -1 );
	g_msgGameStatus = REG_USER_MSG( "GameStatus", -1 );
	g_msgListPS = REG_USER_MSG( "ListPS", -1 );
	g_msgPlayHUDNotification = REG_USER_MSG( "PlayHUDNot", 6 );
	g_msgProgressBar = REG_USER_MSG( "Progress", 3 );
	g_msgServerVar = REG_USER_MSG( "ServerVar", -1 );
	g_msgSetGammaRamp = REG_USER_MSG( "SetGmma", 2 );
	g_msgSetOrder = REG_USER_MSG( "SetOrder", -1 );
	g_msgSetParticleTemplates = REG_USER_MSG( "Particles", -1 );
	g_msgSetSelect = REG_USER_MSG( "SetSelect", -1 );
	g_msgSetRequest = REG_USER_MSG( "SetRequest", 2 );
	g_msgSetSoundNames = REG_USER_MSG( "SoundNames", -1 );
	g_msgSetTechNodes = REG_USER_MSG( "SetTech", 9 );
	g_msgSetTechSlots = REG_USER_MSG( "TechSlots", 1 + kNumTechSlots );
	g_msgSetTopDown = REG_USER_MSG( "SetTopDown", -1 );
	g_msgSetupMap = REG_USER_MSG( "SetupMap", -1 );
	g_msgUpdateCountdown = REG_USER_MSG( "Countdown", 1 );
	g_msgUpdateEntityHierarchy = REG_USER_MSG( "EntHier", -1 );
	g_msgProfileInfo = REG_USER_MSG( "ProfileInfo", 8 );
	g_msgNexusBytes = REG_USER_MSG( "NexusBytes", -1 );
	// tankefugl: 0000971
	g_msgIssueOrder = REG_USER_MSG( "IssueOrder", 9);
	// :tankefugl
}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MESSAGE FUNCTIONS - READ/WRITE PAIRS FOR NETWORK MESSAGES
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define END_READ() ;

#ifndef AVH_SERVER
	void NetMsg_AmmoPickup( void* const buffer, const int size, int& index, int& count )
	{
		BEGIN_READ( buffer, size );
			index = READ_BYTE();
			count = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_AmmoPickup( entvars_t* const pev, const int index, const int count )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgAmmoPickup, NULL, pev );
			WRITE_BYTE( index );
			WRITE_BYTE( count );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_AmmoX( void* const buffer, const int size, int& index, int& count )
	{
		BEGIN_READ( buffer, size );
			index = READ_BYTE();
			count = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_AmmoX( entvars_t *pev, const int index, const int count )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgAmmoX, NULL, pev );
			WRITE_BYTE( index );
			WRITE_BYTE( count );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Battery( void* const buffer, const int size, int& armor_amount )
	{
		BEGIN_READ( buffer, size );
			armor_amount = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_Battery( entvars_t* const pev, const int armor_amount )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBattery, NULL, pev );
			WRITE_SHORT( armor_amount );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_CurWeapon( void* const buffer, const int size, int& state, int& id, int& clip )
	{
		BEGIN_READ( buffer, size );
			state = READ_BYTE();
			id = READ_BYTE();
			clip = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_CurWeapon( entvars_t* const pev, const int state, const int id, const int clip )
	{
        MESSAGE_BEGIN( MSG_ONE, g_msgCurWeapon, NULL, pev );
		    WRITE_BYTE( state );
		    WRITE_BYTE( id );
		    WRITE_BYTE( clip );
	    MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Damage( void* const buffer, const int size, int& dmg_save, int& dmg_take, long& bits, float* origin )
	{
		BEGIN_READ( buffer, size );
			dmg_save = READ_BYTE();
			dmg_take = READ_BYTE();
			bits = READ_LONG();
			origin[0] = READ_COORD();
			origin[1] = READ_COORD();
			origin[2] = READ_COORD();
		END_READ();
	}
#else
	void NetMsg_Damage( entvars_t* const pev, const int dmg_save, const int dmg_take, const long bits, const float* origin )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgDamage, NULL, pev );
			WRITE_BYTE( dmg_save );
			WRITE_BYTE( dmg_take );
			WRITE_LONG( bits );
			WRITE_COORD( origin[0] );
			WRITE_COORD( origin[1] );
			WRITE_COORD( origin[2] );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_DeathMsg( void* const buffer, const int size, int& killer_index, int& victim_index, string& weapon_name )
	{
		BEGIN_READ( buffer, size );
			killer_index = READ_BYTE();
			victim_index = READ_BYTE();
			weapon_name = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_DeathMsg( const int killer_index, const int victim_index, string& weapon_name )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgDeathMsg );
			WRITE_BYTE( killer_index );
			WRITE_BYTE( victim_index );
			WRITE_STRING( weapon_name.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Fade( void* const buffer, const int size, ScreenFade& fade )
	{
		BEGIN_READ( buffer, size );
			fade.duration = READ_SHORT();
			fade.holdTime = READ_SHORT();
			fade.fadeFlags = READ_SHORT();
			fade.r = READ_BYTE();
			fade.g = READ_BYTE();
			fade.b = READ_BYTE();
			fade.a = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_Fade( entvars_t* const pev, const ScreenFade& fade )
	{
        MESSAGE_BEGIN( MSG_ONE, g_msgFade, NULL, pev );		// use the magic #1 for "one client"
			WRITE_SHORT( fade.duration );
			WRITE_SHORT( fade.holdTime );
			WRITE_SHORT( fade.fadeFlags );
			WRITE_BYTE( fade.r );
			WRITE_BYTE( fade.g );
			WRITE_BYTE( fade.b );
			WRITE_BYTE( fade.a );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Flashlight( void* const buffer, const int size, int& is_on, int& flash_battery )
	{
		BEGIN_READ( buffer, size );
			is_on = READ_BYTE();
			flash_battery = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_Flashlight( entvars_t* const pev, const int is_on, const int flash_battery )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgFlashlight, NULL, pev );
			WRITE_BYTE( is_on );
			WRITE_BYTE( flash_battery );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_GeigerRange( void* const buffer, const int size, int& range )
	{
		BEGIN_READ( buffer, size );
			range = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_GeigerRange( entvars_t* const pev, const int range )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgGeigerRange, NULL, pev );
			WRITE_BYTE( range );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Health( void* const buffer, const int size, int& health )
	{
		BEGIN_READ( buffer, size );
			health = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_Health( entvars_t* const pev, const int health )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgHealth, NULL, pev );
			WRITE_SHORT( health );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_HideWeapon( void* const buffer, const int size, int& hide )
	{
		BEGIN_READ( buffer, size );
			hide = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_HideWeapon( entvars_t* const pev, const int hide )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgHideWeapon, NULL, pev );
			WRITE_BYTE( hide );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_HudText( void* const buffer, const int size, string& text )
	{
		BEGIN_READ( buffer, size );
			text = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_HudText( entvars_t* const pev, const string& text )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgHudText, NULL, pev );
			WRITE_STRING( text.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_HudText2( void* const buffer, const int size, string& text, int& flags )
	{
		BEGIN_READ( buffer, size );
			text = READ_STRING();
			flags = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_HudText2( entvars_t* const pev, const string& text, const int flags )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgHudText2, NULL, pev );
			WRITE_STRING( text.c_str() );
			WRITE_BYTE( flags );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_InitHUD( void* const buffer, const int size )
	{
		BEGIN_READ( buffer, size );
		END_READ();
	}
#else
	void NetMsg_InitHUD( entvars_t* const pev )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgInitHUD, NULL, pev );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ItemPickup( void* const buffer, const int size, string& item_name )
	{
		BEGIN_READ( buffer, size );
			item_name = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_ItemPickup( entvars_t* const pev, const string& item_name )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgItemPickup, NULL, pev );
			WRITE_STRING( item_name.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_MOTD( void* const buffer, const int size, bool& is_finished, string& MOTD )
	{
		BEGIN_READ( buffer, size );
			is_finished = (READ_BYTE() == 1);
			MOTD = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_MOTD( entvars_t* const pev, const bool is_finished, const string& MOTD )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgMOTD, NULL, pev );
			WRITE_BYTE( is_finished ? 1 : 0);
			WRITE_STRING( MOTD.c_str() );
		MESSAGE_END();
	}
#endif 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ResetHUD( void* const buffer, const int size )
	{
		BEGIN_READ( buffer, size );
		END_READ();
	}
#else
	void NetMsg_ResetHUD( entvars_t* const pev )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgResetHUD, NULL, pev );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SayText( void* const buffer, const int size, int& entity_index, string& text, string& location )
	{
		BEGIN_READ( buffer, size );
			entity_index = READ_BYTE();
			text = READ_STRING();
			location = READ_STRING();
		END_READ();
	}
#else
	//MESSAGE TO EVERYBODY
	void NetMsg_SayText( const int entity_index, const string& text, const string& location )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgSayText, NULL );
			WRITE_BYTE( entity_index );
			WRITE_STRING( text.c_str() );
			WRITE_STRING( location.c_str() );
		MESSAGE_END();
	}

	//MESSAGE TO ONE PERSON
	void NetMsg_SayText( entvars_t* const pev, const int entity_index, const string& text, const string& location )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSayText, NULL, pev );
			WRITE_BYTE( entity_index );
			WRITE_STRING( text.c_str() );
			WRITE_STRING( location.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ScoreInfo( void* const buffer, const int size, ScoreInfo& info )
	{
		BEGIN_READ( buffer, size );
			info.player_index = READ_BYTE();
			info.score = READ_SHORT();
			info.frags = READ_SHORT();
			info.deaths = READ_SHORT();
			info.player_class = READ_BYTE();
			info.team = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_ScoreInfo( const ScoreInfo& info )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgScoreInfo );
			WRITE_BYTE( info.player_index );
			WRITE_SHORT( info.score );
			WRITE_SHORT( info.frags );
			WRITE_SHORT( info.deaths );
			WRITE_BYTE( info.player_class );
            WRITE_SHORT( info.team );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ServerName( void* const buffer, const int size, string& name )
	{
		BEGIN_READ( buffer, size );
			name = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_ServerName( entvars_t* const pev, const string& name )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgServerName, NULL, pev );
			WRITE_STRING( name.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetFOV( void* const buffer, const int size, int& fov )
	{
		BEGIN_READ( buffer, size );
			fov = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_SetFOV( entvars_t* const pev, const int fov )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetFOV, NULL, pev );
			WRITE_BYTE( fov );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Shake( void* const buffer, const int size, ScreenShake& shake )
	{
		BEGIN_READ( buffer, size );
			shake.amplitude = READ_SHORT();
			shake.duration = READ_SHORT();
			shake.frequency = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_Shake( entvars_t* const pev, const ScreenShake& shake )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgShake, NULL, pev );
			WRITE_SHORT( shake.amplitude );
			WRITE_SHORT( shake.duration );
			WRITE_SHORT( shake.frequency );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ShowGameTitle( void* const buffer, const int size )
	{
		BEGIN_READ( buffer, size );
		END_READ();
	}
#else
	void NetMsg_ShowGameTitle( entvars_t* const pev )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgShowGameTitle, NULL, pev );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ShowMenu( void* const buffer, const int size, int& valid_slots, int& display_time, int& flags, string& content )
	{
		BEGIN_READ( buffer, size );
			valid_slots = READ_SHORT();
			display_time = READ_CHAR();
			flags = READ_BYTE();
			content = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_ShowMenu( entvars_t* const pev, const int valid_slots, const int display_time, const int flags, const string& content )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgShowMenu, NULL, pev );
			WRITE_SHORT( valid_slots );
			WRITE_CHAR( display_time );
			WRITE_BYTE( flags );
			WRITE_STRING( content.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_StatusText( void* const buffer, const int size, int& location, string& content )
	{
		BEGIN_READ( buffer, size );
			location = READ_BYTE();
			content = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_StatusText( entvars_t* const pev, const int location, const string& content )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgStatusText, NULL, pev );
			WRITE_BYTE( location );
			WRITE_STRING( content.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_StatusValue( void* const buffer, const int size, int& location, int& state )
	{
		BEGIN_READ( buffer, size );
			location = READ_BYTE();
			state = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_StatusValue( entvars_t* const pev, const int location, const int state )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgStatusValue, NULL, pev );
			WRITE_BYTE( location );
			WRITE_SHORT( state );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_TeamInfo( void* const buffer, const int size, int& player_index, string& team_id )
	{
		BEGIN_READ( buffer, size );
			player_index = READ_BYTE();
			team_id = READ_STRING();
		END_READ();
	}
#else
	//MESSAGE TO EVERYBODY
	void NetMsg_TeamInfo( const int player_index, const string& team_id )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgTeamInfo );
			WRITE_BYTE( player_index );
			WRITE_STRING( team_id.c_str() );
		MESSAGE_END();
	}

	//MESSAGE TO SPECTATORS
	void NetMsgSpec_TeamInfo( const int player_index, const string& team_id )
	{
		MESSAGE_BEGIN( MSG_SPEC, g_msgTeamInfo );
			WRITE_BYTE( player_index );
			WRITE_STRING( team_id.c_str() );
		MESSAGE_END();
	}

	//MESSAGE TO ONE PERSON
	void NetMsg_TeamInfo( entvars_t* const pev, const int player_index, const string& team_id )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgTeamInfo, NULL, pev );
			WRITE_BYTE( player_index );
			WRITE_STRING( team_id.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_TeamNames( void* const buffer, const int size, StringList& team_names )
	{
		team_names.clear();
		BEGIN_READ( buffer, size );
			int num_teams = READ_BYTE();
			for( int counter = 0; counter < num_teams; counter++ )
			{ 
				string name(READ_STRING());
				team_names.push_back(name); 
			}
		END_READ();
	}
#else
	void NetMsg_TeamNames( entvars_t* const pev, const StringList& team_names )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgTeamNames, NULL, pev );
			WRITE_BYTE( team_names.size() );
			for( int counter = 0; counter < team_names.size(); counter++ )
			{
				WRITE_STRING( team_names[counter].c_str() );
			}
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_TeamScore( void* const buffer, const int size, string& team_name, int& score, int& deaths )
	{
		BEGIN_READ( buffer, size );
			team_name = READ_STRING();
			score = READ_SHORT();
			deaths = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_TeamScore( entvars_t* const pev, const string& team_name, const int score, const int deaths )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgTeamScore, NULL, pev );
			WRITE_STRING( team_name.c_str() );
			WRITE_SHORT( score );
			WRITE_SHORT( deaths );
		MESSAGE_END();
	}

	void NetMsg_TeamScore( const string& team_name, const int score, const int deaths )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgTeamScore );
			WRITE_STRING( team_name.c_str() );
			WRITE_SHORT( score );
			WRITE_SHORT( deaths );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_TextMsg( void* const buffer, const int size, int& destination, StringList& message )
	{
		message.clear();
		BEGIN_READ( buffer, size );
			destination = READ_BYTE();
			message.push_back( string( READ_STRING() ) );
			message.push_back( string( READ_STRING() ) );
			message.push_back( string( READ_STRING() ) );
			message.push_back( string( READ_STRING() ) );
			message.push_back( string( READ_STRING() ) );
		END_READ();
	}
#else
	//MESSAGE TO EVERYBODY
	void NetMsg_TextMsg( const int destination, const StringList& message )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgTextMsg );
			WRITE_BYTE( destination );
			WRITE_STRING( message[0].c_str() );
			if( message.size() > 1 )
				WRITE_STRING( message[1].c_str() );
			if( message.size() > 2 )
				WRITE_STRING( message[2].c_str() );
			if( message.size() > 3 )
				WRITE_STRING( message[3].c_str() );
			if( message.size() > 4 )
				WRITE_STRING( message[4].c_str() );
		MESSAGE_END();
	}

	//MESSAGE TO SPECTATORS
	void NetMsgSpec_TextMsg( const int destination, const StringList& message )
	{
		MESSAGE_BEGIN( MSG_SPEC, g_msgTextMsg );
			WRITE_BYTE( destination );
			WRITE_STRING( message[0].c_str() );
			if( message.size() > 1 )
				WRITE_STRING( message[1].c_str() );
			if( message.size() > 2 )
				WRITE_STRING( message[2].c_str() );
			if( message.size() > 3 )
				WRITE_STRING( message[3].c_str() );
			if( message.size() > 4 )
				WRITE_STRING( message[4].c_str() );
		MESSAGE_END();
	}

	//MESSAGE TO ONE PERSON
	void NetMsg_TextMsg( entvars_t* const pev, const int destination, const StringList& message )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgTextMsg, NULL, pev );
			WRITE_BYTE( destination );
			WRITE_STRING( message[0].c_str() );
			if( message.size() > 1 )
				WRITE_STRING( message[1].c_str() );
			if( message.size() > 2 )
				WRITE_STRING( message[2].c_str() );
			if( message.size() > 3 )
				WRITE_STRING( message[3].c_str() );
			if( message.size() > 4 )
				WRITE_STRING( message[4].c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Train( void* const buffer, const int size, int& state )
	{
		BEGIN_READ( buffer, size );
			state = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_Train( entvars_t* const pev, const int state )
	{
        MESSAGE_BEGIN( MSG_ONE, g_msgTrain, NULL, pev );
			WRITE_BYTE( state );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_WeaponList( void* const buffer, const int size, WeaponList& weapon )
	{
		BEGIN_READ( buffer, size );
			weapon.weapon_name = READ_STRING();
			weapon.ammo1_type = READ_CHAR();
			weapon.ammo1_max_amnt = READ_BYTE();
			weapon.ammo2_type = READ_CHAR();
			weapon.ammo2_max_amnt = READ_BYTE();
			weapon.bucket = READ_CHAR();
			weapon.bucket_pos = READ_CHAR();
			weapon.bit_index = READ_CHAR();
			weapon.flags = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_WeaponList( entvars_t* const pev, const WeaponList& weapon )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgWeaponList, NULL, pev );
			WRITE_STRING( weapon.weapon_name.c_str() );
			WRITE_CHAR( weapon.ammo1_type );
			WRITE_BYTE( weapon.ammo1_max_amnt );
			WRITE_CHAR( weapon.ammo2_type );
			WRITE_BYTE( weapon.ammo2_max_amnt );
			WRITE_CHAR( weapon.bucket );
			WRITE_CHAR( weapon.bucket_pos );
			WRITE_CHAR( weapon.bit_index );
			WRITE_BYTE( weapon.flags );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_WeapPickup( void* const buffer, const int size, int& weapon_id )
	{
		BEGIN_READ( buffer, size );
			weapon_id = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_WeapPickup( entvars_t* const pev , const int weapon_id )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgWeapPickup, NULL, pev );
			WRITE_BYTE( weapon_id );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum AlienInfo_ChangeFlags
{
	NO_CHANGE = 0,
	COORDS_CHANGED = 1,
	STATUS_CHANGED = 2,
	HEALTH_CHANGED = 4
};

#ifndef AVH_SERVER
	void NetMsg_AlienInfo( void* const buffer, const int size, bool& was_hive_info, AvHAlienUpgradeListType& upgrades, HiveInfoListType& hives )
	{
		BEGIN_READ( buffer, size );
			int status, type, header = READ_BYTE();
			was_hive_info = (header & 0x80) == 0;
			if( was_hive_info )
			{
				int num_hives = header;
				AlienInfo_ChangeFlags changes;
				for( int counter = 0; counter < num_hives; counter++ )
				{
					if( hives.size() <= counter )
					{ 
						AvHHiveInfo hive;
						hives.push_back( hive ); 
					}

					changes = (AlienInfo_ChangeFlags)READ_BYTE();
					if( changes & COORDS_CHANGED )
					{
						hives[counter].mPosX = READ_COORD();
						hives[counter].mPosY = READ_COORD();
						hives[counter].mPosZ = READ_COORD();
					}
					if( changes & STATUS_CHANGED )
					{
						status = READ_BYTE();
						type = (status >> 3) & 0x03;
						hives[counter].mUnderAttack = (status & 0x80) != 0;
						hives[counter].mStatus = status & 0x07;
						switch(type)
						{
						case 0: hives[counter].mTechnology = MESSAGE_NULL; break;
						case 1: hives[counter].mTechnology = ALIEN_BUILD_DEFENSE_CHAMBER; break;
						case 2: hives[counter].mTechnology = ALIEN_BUILD_SENSORY_CHAMBER; break;
						case 3: hives[counter].mTechnology = ALIEN_BUILD_MOVEMENT_CHAMBER; break;
						}
					}
					if( changes & HEALTH_CHANGED )
					{
						hives[counter].mHealthPercentage = READ_BYTE();
					}
				}
			}
			else
			{
				int num_upgrades = READ_BYTE();
				upgrades.clear();
				for( int counter = 0; counter < num_upgrades; counter++ )
				{
					AvHAlienUpgradeCategory theUpgradeCategory = AvHAlienUpgradeCategory(READ_BYTE());
					upgrades.push_back(theUpgradeCategory);
				}
			}
		END_READ();
	}
#else
	void NetMsg_AlienInfo_Upgrades( entvars_t* const pev, const AvHAlienUpgradeListType& upgrades )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgAlienInfo, NULL, pev );
			WRITE_BYTE( 0x80 ); //magic number for hive size field, upgrade info
			WRITE_BYTE( upgrades.size() );
			AvHAlienUpgradeListType::const_iterator current, end = upgrades.end();
			for( current = upgrades.begin(); current != end; ++current )
			{
				WRITE_BYTE( *current );
			}
		MESSAGE_END();
	}

	void NetMsg_AlienInfo_Hives( entvars_t* const pev, const HiveInfoListType& hives, const HiveInfoListType& client_hives )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgAlienInfo, NULL, pev );
			WRITE_BYTE( hives.size() );
			HiveInfoListType::const_iterator current, end = hives.end();
			int status, tech, index = 0;
			int change_flags = NO_CHANGE;
			for( current = hives.begin(); current != end; ++current, ++index )
			{
				//put together change bitfield
				if( client_hives.size() <= index || client_hives[index].mPosX != current->mPosX ||
					client_hives[index].mPosY != current->mPosY || client_hives[index].mPosZ != current->mPosZ )
				{ change_flags |= COORDS_CHANGED; }

				if( client_hives.size() <= index || client_hives[index].mStatus != current->mStatus ||
					client_hives[index].mUnderAttack != current->mUnderAttack || client_hives[index].mTechnology != current->mTechnology )
				{ change_flags |= STATUS_CHANGED; }

				if( client_hives.size() <= index || client_hives[index].mHealthPercentage != current->mHealthPercentage	)
				{ change_flags |= HEALTH_CHANGED; }
				WRITE_BYTE(change_flags);

				//send change data
				if( change_flags & COORDS_CHANGED )
				{
					WRITE_COORD(current->mPosX);
					WRITE_COORD(current->mPosY);
					WRITE_COORD(current->mPosZ);
				}
				if( change_flags & STATUS_CHANGED )
				{
					status = current->mStatus & 0x07;	// 3 bits
					switch( current->mTechnology )		// 2 bits
					{
					case MESSAGE_NULL: tech = 0; break;
					case ALIEN_BUILD_DEFENSE_CHAMBER: tech = 1; break;
					case ALIEN_BUILD_SENSORY_CHAMBER: tech = 2; break;
					case ALIEN_BUILD_MOVEMENT_CHAMBER: tech = 3; break;
					default: tech = 0; break;
					}
					status |= tech << 3;
					status |= current->mUnderAttack ? 0x80 : 0x00; // 1 bit
					WRITE_BYTE(status);
				}
				if( change_flags & HEALTH_CHANGED )
				{
					WRITE_BYTE(current->mHealthPercentage);
				}
			}
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//TODO: sub rapid conversion as described in Game Programming Gems...
union float_converter
{
	float f;
	long l;
};

#ifndef AVH_SERVER
	void NetMsg_BalanceVar( void* const buffer, const int size, string& name, BalanceMessageAction& action, int& ivalue, float& fvalue, string& svalue )
	{
		BEGIN_READ( buffer, size );
			action = static_cast<BalanceMessageAction>(READ_BYTE());
			switch( action )
			{
				case BALANCE_ACTION_INSERT_INT:
				{
					name = READ_STRING();
					ivalue = READ_LONG();
					break;
				}
				case BALANCE_ACTION_INSERT_FLOAT:
				{
					float_converter c;
					name = READ_STRING();
					c.l = READ_LONG();
					fvalue = c.f;
					break;
				}
				case BALANCE_ACTION_INSERT_STRING:
				{
					name = READ_STRING();
					svalue = READ_STRING();
					break;
				}
				case BALANCE_ACTION_REMOVE:
				{
					name = READ_STRING();
					break;
				}
				case BALANCE_ACTION_CLEAR:
				case BALANCE_ACTION_NOTIFY_PENDING:
				case BALANCE_ACTION_NOTIFY_FINISHED:
				{
					break;
				}
				default:
					break;
					//todo: error condition here?
			}
		END_READ();
	}
#else
	void NetMsg_BalanceVarChangesPending( entvars_t* const pev, const bool pending )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBalanceVar, NULL, pev );
			WRITE_BYTE( pending ? BALANCE_ACTION_NOTIFY_PENDING : BALANCE_ACTION_NOTIFY_FINISHED );
		MESSAGE_END();
	}

	void NetMsg_BalanceVarInsertInt( entvars_t* const pev, const string& name, const int data )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBalanceVar, NULL, pev );
			WRITE_BYTE( BALANCE_ACTION_INSERT_INT );
			WRITE_STRING( name.c_str() );
			WRITE_LONG( data );
		MESSAGE_END();
	}

	void NetMsg_BalanceVarInsertFloat( entvars_t* const pev, const string& name, const float data )
	{
		float_converter c;
		c.f = data;
		MESSAGE_BEGIN( MSG_ONE, g_msgBalanceVar, NULL, pev );
			WRITE_BYTE( BALANCE_ACTION_INSERT_FLOAT );
			WRITE_STRING( name.c_str() );
			WRITE_LONG( c.l );
		MESSAGE_END();
	}

	void NetMsg_BalanceVarInsertString( entvars_t* const pev, const string& name, const string& data )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBalanceVar, NULL, pev );
			WRITE_BYTE( BALANCE_ACTION_INSERT_STRING );
			WRITE_STRING( name.c_str() );
			WRITE_STRING( data.c_str() );
		MESSAGE_END();
	}

	void NetMsg_BalanceVarRemove( entvars_t* const pev, const string& name )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBalanceVar, NULL, pev );
			WRITE_BYTE( BALANCE_ACTION_REMOVE );
			WRITE_STRING( name.c_str() );
		MESSAGE_END();
	}

	void NetMsg_BalanceVarClear( entvars_t* const pev )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBalanceVar, NULL, pev );
			WRITE_BYTE( BALANCE_ACTION_CLEAR );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_BlipList( void* const buffer, const int size, bool& friendly_blips, AvHVisibleBlipList& list )
	{
		int blip_count;
		int8 status, info;
		float X, Y, Z;
		list.Clear();
		BEGIN_READ( buffer, size );
			int list_info = READ_BYTE();
			friendly_blips = (list_info & 0x80) != 0;
			blip_count = (list_info & 0x7F);
			for( int counter = 0; counter < blip_count; counter++ )
			{
				X = READ_COORD();
				Y = READ_COORD();
				Z = READ_COORD();
				status = READ_BYTE();
				info = friendly_blips ? READ_BYTE() : 0;
				list.AddBlip( X, Y, Z, status, info );
			}
		END_READ();
	}
#else
	void NetMsg_BlipList( entvars_t* const pev, const bool friendly_blips, const AvHVisibleBlipList& list )
	{
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, g_msgBlipList, NULL, pev );
			//pack header - 7 bits for blip count (doesn't go over 40 in practice), 1 bit for Friend or Foe
			unsigned char list_info = list.mNumBlips | (friendly_blips ? 0x80 : 0);
			WRITE_BYTE( list_info );
			//pack each blip - this could be optimized as follows once bit packer is implemented:
			// convert X, Y to integer values ranging from 0 to 2047 (11 bits each) based on map extents
			// convert Z to integer value ranging from 0 to 511 (9 bits)
			// 4 bits for status (range 0-15, 1-9 currently used)
			// 5 bits for info (range 1-32, refers to player number)
			// total is 40 bits = 5 bytes for friendly, 35 bits for foe.
			// savings would be 37.5% for friendly bytes.
			// blip precision would be equal to double large minimap precision, with worst case of 4 unit X,Y separation for MT.
			// because maps are much smaller vertically than horizontally as a rule, the worst case of 16 unit Z separation
			// will very rarely occur.
			for( int counter = 0; counter < list.mNumBlips; counter++ )
			{
				WRITE_COORD( list.mBlipPositions[counter][0] );
				WRITE_COORD( list.mBlipPositions[counter][1] );
				WRITE_COORD( list.mBlipPositions[counter][2] );
				WRITE_BYTE( list.mBlipStatus[counter] );
				if( friendly_blips ) { WRITE_BYTE( list.mBlipInfo[counter] ); }
			}
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_BuildMiniMap( void* const buffer, const int size, string& name, int& num_samples, int& processed_samples, int& width, int& height, uint8** map, bool& finished )
	{
		BEGIN_READ( buffer, size );
			switch( READ_BYTE() )
			{
				case 0:
					name = READ_STRING();
					num_samples = READ_LONG();
					processed_samples = 0;
					width = READ_LONG();
					height = READ_LONG();
					*map = new uint8[num_samples];
					finished = false;
				case 1:
				{
					int packet_samples = READ_BYTE();
					for( int counter = 0; counter < packet_samples; counter++ )
					{
						(*map)[processed_samples++] = READ_BYTE();
					}
				}
				case 2:
					finished = true;
			}
		END_READ();
	}
#else
	void NetMsg_BuildMiniMap_Initialize( entvars_t* const pev, const string& name, const int num_samples, const int width, const int height )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBuildMiniMap, NULL, pev );
			WRITE_BYTE( 0 );
			WRITE_STRING( name.c_str() );
			WRITE_LONG( num_samples );
			WRITE_LONG( width );
			WRITE_LONG( height );
		MESSAGE_END();
	}

	void NetMsg_BuildMiniMap_Update( entvars_t* const pev, const int num_samples, const uint8* const samples )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBuildMiniMap, NULL, pev );
			WRITE_BYTE( 1 );
			WRITE_BYTE( num_samples );
			for( int counter = 0; counter < num_samples; counter++ )
			{
				WRITE_BYTE( samples[counter] );
			}
		MESSAGE_END();
	}

	void NetMsg_BuildMiniMap_Complete( entvars_t* const pev )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgBuildMiniMap, NULL, pev );
			WRITE_BYTE( 2 );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ClientScripts( void* const buffer, const int size, StringList& script_names )
	{
		script_names.clear();
		BEGIN_READ( buffer, size );
			int num_scripts = READ_BYTE();
			while( script_names.size() < num_scripts )
			{
				script_names.push_back( string( READ_STRING() ) );
			}
		END_READ();
	}
#else
	void NetMsg_ClientScripts( entvars_t* const pev, const StringList& script_names )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgClientScripts, NULL, pev );
			WRITE_BYTE( script_names.size() );
			StringList::const_iterator current, end = script_names.end();
			for( current = script_names.begin(); current != end; ++current )
			{
				WRITE_STRING( current->c_str() );
			}
		MESSAGE_END();
	}
#endif
		
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_DebugCSP( void* const buffer, const int size, weapon_data_t& weapon_data, float& next_attack )
	{
		BEGIN_READ( buffer, size );
			weapon_data.m_iId = READ_LONG();
			weapon_data.m_iClip = READ_LONG();
			weapon_data.m_flNextPrimaryAttack = READ_COORD();
			weapon_data.m_flTimeWeaponIdle = READ_COORD();
			next_attack = READ_COORD();
		END_READ();
	}
#else
	void NetMsg_DebugCSP( entvars_t* const pev, const weapon_data_t& weapon_data, const float next_attack )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgDebugCSP, NULL, pev );
			WRITE_LONG( weapon_data.m_iId );
			WRITE_LONG( weapon_data.m_iClip );
			WRITE_COORD( weapon_data.m_flNextPrimaryAttack );
			WRITE_COORD( weapon_data.m_flTimeWeaponIdle );
			WRITE_COORD( next_attack );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_EditPS( void* const buffer, const int size, int& particle_index )
	{
		BEGIN_READ( buffer, size );
			particle_index = READ_SHORT();
		END_READ();
	}
#else
	void NetMsg_EditPS( entvars_t* const pev, const int particle_index )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgEditPS, NULL, pev );
			WRITE_SHORT( particle_index );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_Fog( void* const buffer, const int size, bool& enabled, int& R, int& G, int& B, float& start, float& end )
	{
		BEGIN_READ( buffer, size );
			enabled = (READ_BYTE() != 0);
			if( enabled )
			{
				R = READ_BYTE();
				G = READ_BYTE();
				B = READ_BYTE();
				start = READ_COORD();
				end = READ_COORD();
			}
		END_READ();
	}
#else
	void NetMsg_Fog( entvars_t* const pev, const bool enabled, const int R, const int G, const int B, const float start, const float end )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgFog, NULL, pev );
			WRITE_BYTE( enabled ? 1 : 0 );
			if( enabled )
			{
				WRITE_BYTE( R );
				WRITE_BYTE( G );
				WRITE_BYTE( B );
				WRITE_COORD( start );
				WRITE_COORD( end );
			}
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_GameStatus( void* const buffer, const int size, int& status_code, AvHMapMode& map_mode, int& game_time, int& timelimit, int& misc_data )
	{
		BEGIN_READ( buffer, size );
			status_code = READ_BYTE();
			map_mode = (AvHMapMode)READ_BYTE();
			switch( status_code )
			{
			case kGameStatusReset:
			case kGameStatusResetNewMap:
			case kGameStatusEnded:
				break;
			case kGameStatusGameTime:
				game_time = READ_SHORT();
				timelimit = READ_SHORT();
				misc_data = READ_BYTE();
				break;
			case kGameStatusUnspentLevels:
				misc_data = READ_BYTE();
				break;
			}
		END_READ();
	}
#else
	void NetMsg_GameStatus_State( const int status_code, const int map_mode )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgGameStatus );
			WRITE_BYTE( status_code );
			WRITE_BYTE( map_mode );
		MESSAGE_END();
	}

	void NetMsg_GameStatus_State( entvars_t* const pev, const int status_code, const int map_mode )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgGameStatus, NULL, pev );
			WRITE_BYTE( status_code );
			WRITE_BYTE( map_mode );
		MESSAGE_END();
	}

	void NetMsg_GameStatus_Time( const int status_code, const int map_mode, const int game_time, const int timelimit, const int attacking_team_number, const bool is_reliable )
	{
		int message_type = is_reliable ? MSG_ALL : MSG_BROADCAST;
		MESSAGE_BEGIN( message_type, g_msgGameStatus );
			WRITE_BYTE( status_code );
			WRITE_BYTE( map_mode );
			WRITE_SHORT( game_time );
			WRITE_SHORT( timelimit );
			WRITE_BYTE( attacking_team_number );
		MESSAGE_END();
	}

	void NetMsg_GameStatus_UnspentLevels( entvars_t* const pev, const int status_code, const int map_mode, const int unspent_levels )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgGameStatus, NULL, pev );
			WRITE_BYTE( status_code );
			WRITE_BYTE( map_mode );
			WRITE_BYTE( unspent_levels );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ListPS( void* const buffer, const int size, string& system_name )
	{
		BEGIN_READ( buffer, size );
			system_name = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_ListPS( entvars_t* const pev, const string& system_name )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgListPS, NULL, pev );
			WRITE_STRING( system_name.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_PlayHUDNotification( void* const buffer, const int size, int& flags, int& sound, float& location_x, float& location_y )
	{
		BEGIN_READ( buffer, size );
			flags = READ_BYTE();
			sound = READ_BYTE();
			location_x = READ_COORD();
			location_y = READ_COORD();
		END_READ();
	}
#else
	void NetMsg_PlayHUDNotification( entvars_t* const pev, const int flags, const int sound, const float location_x, const float location_y )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgPlayHUDNotification, NULL, pev );
			WRITE_BYTE( flags );
			WRITE_BYTE( sound );
			WRITE_COORD( location_x );
			WRITE_COORD( location_y );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ProgressBar( void* const buffer, const int size, int& entity_number, int& progress )
	{
		BEGIN_READ( buffer, size );
			entity_number = READ_SHORT();
			progress = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_ProgressBar( entvars_t* const pev, const int entity_number, const int progress )
	{
		MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, g_msgProgressBar, NULL, pev );
			WRITE_SHORT( entity_number );
			WRITE_BYTE( progress );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_ServerVar( void* const buffer, const int size, string& name, string& value )
	{
		BEGIN_READ( buffer, size );
			name = READ_STRING();
			value = READ_STRING();
		END_READ();
	}
#else
	void NetMsg_ServerVar( entvars_t* const pev, const string& name, const string& value )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgServerVar, NULL, pev );
			WRITE_STRING( name.c_str() );
			WRITE_STRING( value.c_str() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetGammaRamp( void* const buffer, const int size, float& gamma )
	{
		BEGIN_READ( buffer, size );
			gamma = READ_COORD();
		END_READ();
	}
#else
	void NetMsg_SetGammaRamp( entvars_t* const pev, const float gamma )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetGammaRamp, NULL, pev );
			WRITE_COORD( gamma );
		MESSAGE_END();
	}

	void NetMsgSpec_SetGammaRamp( const float gamma )
	{
		MESSAGE_BEGIN( MSG_SPEC, g_msgSetGammaRamp );
			WRITE_COORD( gamma );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetOrder( void* const buffer, const int size, AvHOrder& order )
	{
		EntityListType players;
		BEGIN_READ( buffer, size );
			order.SetReceiver( READ_BYTE() );
			order.SetOrderType( (AvHOrderType)READ_BYTE() );
			READ_BYTE(); //this is a redundant byte because SetOrderType automatically sets the target type as well.
			switch( order.GetOrderTargetType() )
			{
			case ORDERTARGETTYPE_LOCATION:
			{
				vec3_t location;
				location.x = READ_COORD();
				location.y = READ_COORD();
				location.z = READ_COORD();
				order.SetLocation( location );
				break;
			}
			case ORDERTARGETTYPE_TARGET:
				order.SetTargetIndex( READ_SHORT() );
				break;
			}
			order.SetUser3TargetType( (AvHUser3)READ_BYTE() );
			order.SetOrderCompleted( READ_BYTE() );
		END_READ();
	}
#else
	void NetMsg_SetOrder( entvars_t* const pev, const AvHOrder& order )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetOrder, NULL, pev );
			WRITE_BYTE( order.GetReceiver() );
			WRITE_BYTE( order.GetOrderType() );
			WRITE_BYTE( order.GetOrderTargetType() ); //this is a redundant byte because SetOrderType automatically sets the target type as well.
			switch( order.GetOrderTargetType() )
			{
			case ORDERTARGETTYPE_LOCATION:
			{
				vec3_t location;
				order.GetLocation( location );
				WRITE_COORD( location.x );
				WRITE_COORD( location.y );
				WRITE_COORD( location.z );
				break;
			}
			case ORDERTARGETTYPE_TARGET:
				WRITE_SHORT( order.GetTargetIndex() );
				break;
			}
			WRITE_BYTE( order.GetTargetUser3Type() );
			WRITE_BYTE( order.GetOrderCompleted() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetParticleTemplate( void* const buffer, const int size, AvHParticleTemplate& particle_template )
	{
		ParticleParams gen_params, vel_params;
		PSVector gravity;
		BEGIN_READ( buffer, size );
			particle_template.SetName( string( READ_STRING() ) );
			particle_template.SetMaxParticles( READ_LONG() );
			particle_template.SetParticleSize( READ_COORD() );
			particle_template.SetSprite( string( READ_STRING() ) );
			particle_template.SetParticleSystemLifetime( READ_COORD() );
			particle_template.SetParticleLifetime( READ_COORD() );
			particle_template.SetAnimationSpeed( READ_COORD() );
			particle_template.SetNumSpriteFrames( READ_BYTE() );
			particle_template.SetParticleScaling( READ_COORD() );
			particle_template.SetRenderMode( READ_BYTE() );
			particle_template.SetGenerationRate( READ_LONG() );
			particle_template.SetGenerationShape( READ_BYTE() );
			for( int counter = 0; counter < 8; counter++ ) { gen_params[counter] = READ_LONG(); }
			particle_template.SetGenerationParams( gen_params );
			particle_template.SetGenerationEntityIndex( READ_LONG() );
			particle_template.SetGenerationEntityParameter( READ_COORD() );
			particle_template.SetStartingVelocityShape( READ_BYTE() );
			for( int counter = 0; counter < 8; counter++ ) { vel_params[counter] = READ_LONG(); }
			particle_template.SetStartingVelocityParams( vel_params );
			for( int counter = 0; counter < 3; counter++ ) { gravity[counter] = READ_COORD(); }
			particle_template.SetGravity( gravity );
			particle_template.SetMaxAlpha( READ_COORD() );
			particle_template.SetParticleSystemIndexToGenerate( READ_LONG() );
			particle_template.SetFlags( READ_LONG() );
		END_READ();
	}
#else
	void NetMsg_SetParticleTemplate( entvars_t* const pev, const AvHParticleTemplate& particle_template )
	{
		ParticleParams gen_params, vel_params;
		PSVector gravity;
		MESSAGE_BEGIN( MSG_ONE, g_msgSetParticleTemplates, NULL, pev );
			WRITE_STRING( particle_template.GetName().c_str() );
			WRITE_LONG( particle_template.GetMaxParticles() );
			WRITE_COORD( particle_template.GetParticleSize() );
			WRITE_STRING( particle_template.GetSprite().c_str() );
			WRITE_COORD( particle_template.GetParticleSystemLifetime() );
			WRITE_COORD( particle_template.GetParticleLifetime() );
			WRITE_COORD( particle_template.GetAnimationSpeed() );
			WRITE_BYTE( particle_template.GetNumSpriteFrames() );
			WRITE_COORD( particle_template.GetParticleScaling() );
			WRITE_BYTE( particle_template.GetRenderMode() );
			WRITE_LONG( particle_template.GetGenerationRate() );
			WRITE_BYTE( particle_template.GetGenerationShape() );
			particle_template.GetGenerationParams( gen_params );
			for( int counter = 0; counter < 8; counter++ ) { WRITE_LONG( gen_params[counter] ); }
			WRITE_LONG( particle_template.GetGenerationEntityIndex() );
			WRITE_COORD( particle_template.GetGenerationEntityParameter() );
			WRITE_BYTE( particle_template.GetStartingVelocityShape() );
			particle_template.GetStartingVelocityParams( vel_params );
			for( int counter = 0; counter < 8; counter++ ) { WRITE_LONG( vel_params[counter] ); }
			particle_template.GetGravity( gravity );
			for( int counter = 0; counter < 3; counter++ ) { WRITE_COORD( gravity[counter] ); }
			WRITE_COORD( particle_template.GetMaxAlpha() );
			WRITE_LONG( particle_template.GetParticleSystemIndexToGenerate() );
			WRITE_LONG( particle_template.GetFlags() );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetSelect( void* const buffer, const int size, Selection& selection )
	{
		selection.selected_entities.clear();
		BEGIN_READ( buffer, size );
			selection.group_number = READ_BYTE();
			int num_entities = READ_BYTE();
			for(int counter = 0; counter < num_entities; counter++ )
			{ selection.selected_entities.push_back( READ_SHORT() ); }
			switch( selection.group_number )
			{
			case 0:
				selection.tracking_entity = (READ_BYTE() == 0) ? 0 : READ_SHORT();
				break;
			case kSelectAllHotGroup:
				break;
			default:
				selection.group_type = (AvHUser3)READ_BYTE();
				selection.group_alert = (AvHAlertType)READ_BYTE();
				break;
			}
		END_READ();
	}
#else
	void NetMsg_SetSelect( entvars_t* const pev, Selection& selection )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetSelect, NULL, pev );
			WRITE_BYTE( selection.group_number );
			WRITE_BYTE( selection.selected_entities.size() );
			EntityListType::const_iterator current, end = selection.selected_entities.end();
            for( current = selection.selected_entities.begin(); current != end; ++current )
			{ WRITE_SHORT( *current ); }
			switch( selection.group_number )
			{
			case 0:
				if( selection.tracking_entity != 0 )
				{
					WRITE_BYTE( 1 );
					WRITE_SHORT( selection.tracking_entity );
				}
				else
				{
					WRITE_BYTE( 0 );
				}
				break;
			case kSelectAllHotGroup:
				break;
			default:
				WRITE_BYTE( selection.group_type );
				WRITE_BYTE( selection.group_alert );
				break;
			}
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetRequest( void* const buffer, const int size, int& request_type, int& request_count )
	{
		BEGIN_READ( buffer, size );
			request_type = READ_BYTE();
			request_count = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_SetRequest( entvars_t* pev, const int request_type, const int request_count )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetRequest, NULL, pev );
			WRITE_BYTE( request_type );
			WRITE_BYTE( request_count );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetSoundNames( void* const buffer, const int size, bool& reset, string& sound_name )
	{
		BEGIN_READ( buffer, size );
			reset = (READ_BYTE() != 0 ) ? true : false;
			if( !reset )
			{ sound_name = READ_STRING(); }
		END_READ();
	}
#else
	void NetMsg_SetSoundNames( entvars_t* pev, const bool reset, const string& sound_name )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetSoundNames, NULL, pev );
			WRITE_BYTE( reset ? 1 : 0 );
			if( !reset )
			{ WRITE_STRING( sound_name.c_str() ); }
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetTechNode( void* const buffer, const int size, AvHTechNode*& node )
	{
		BEGIN_READ( buffer, size );
			node = new AvHTechNode( (AvHMessageID)READ_BYTE() );
			node->setTechID( (AvHTechID)READ_BYTE() );
			node->setPrereqTechID1( (AvHTechID)READ_BYTE() );
			node->setPrereqTechID2( (AvHTechID)READ_BYTE() );
			node->setCost( READ_SHORT() );
			node->setBuildTime( READ_SHORT() );
			int flags = READ_BYTE();
			node->setAllowMultiples( (flags & 0x01) != 0 );
			node->setResearchState( (flags & 0x04) != 0 );
			node->setResearchable( (flags & 0x02) != 0 );
		END_READ();
	}
#else
	void NetMsg_SetTechNode( entvars_t* pev, const AvHTechNode* node )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetTechNodes, NULL, pev );
			WRITE_BYTE( node->getMessageID() );
			WRITE_BYTE( node->getTechID() );
			WRITE_BYTE( node->getPrereqTechID1() );
			WRITE_BYTE( node->getPrereqTechID2() );
			WRITE_SHORT( node->getCost() );
			WRITE_SHORT( node->getBuildTime() );
			int flags = 0;
			if( node->getAllowMultiples() ) { flags |= 0x01; }
			if( node->getIsResearchable() ) { flags |= 0x02; }
			if( node->getIsResearched() ) { flags |= 0x04; }
			WRITE_BYTE( flags );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetTechSlots( void* const buffer, const int size, AvHTechSlots& tech_slots )
	{
		BEGIN_READ( buffer, size );
			tech_slots.mUser3 = (AvHUser3)READ_BYTE();
			for( int counter = 0; counter < kNumTechSlots; counter++ )
			{ tech_slots.mTechSlots[counter] = (AvHMessageID)READ_BYTE(); }
		END_READ();
	}
#else
	void NetMsg_SetTechSlots( entvars_t* pev, const AvHTechSlots& tech_slots )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetTechSlots, NULL, pev );
			WRITE_BYTE( tech_slots.mUser3 );
			for( int counter = 0; counter < kNumTechSlots; counter++ )
			{ WRITE_BYTE( tech_slots.mTechSlots[counter] ); }
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetTopDown( void* const buffer, const int size, bool& is_menu_tech, bool& is_top_down, float* position, int& tech_slots )
	{
		BEGIN_READ( buffer, size );
			is_menu_tech = (READ_BYTE() != 0);
			if( is_menu_tech )
			{ tech_slots = READ_LONG(); }
			else
			{
				is_top_down = (READ_BYTE() != 0);
				position[0] = READ_COORD();
				position[1] = READ_COORD();
				position[2] = READ_COORD();
			}
		END_READ();
	}
#else
	void NetMsg_SetTopDown_Position( entvars_t* const pev, const bool is_top_down, const float* const position )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetTopDown, NULL, pev );
			WRITE_BYTE( 0 );
			WRITE_BYTE( is_top_down ? 1 : 0 );
			WRITE_COORD( position[0] );
			WRITE_COORD( position[1] );
			WRITE_COORD( position[2] );
		MESSAGE_END();
	}

	void NetMsg_SetTopDown_TechSlots( entvars_t* const pev, const int tech_slots )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetTopDown, NULL, pev );
			WRITE_BYTE( 1 );
			WRITE_LONG( tech_slots );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_SetupMap( void* const buffer, const int size, bool& is_location, string& name, float* min_extents, float* max_extents, bool& draw_background )
	{
		BEGIN_READ( buffer, size );
			is_location = (READ_BYTE() != 0);
			name = READ_STRING();
			if( is_location )
			{
				max_extents[0] = READ_COORD();
				max_extents[1] = READ_COORD();
				min_extents[0] = READ_COORD();
				min_extents[1] = READ_COORD();
			}
			else
			{
				min_extents[2] = READ_COORD();
				max_extents[2] = READ_COORD();
				min_extents[0] = READ_COORD();
				min_extents[1] = READ_COORD();
				max_extents[0] = READ_COORD();
				max_extents[1] = READ_COORD();
				draw_background = (READ_BYTE() != 0);
			}
	}
#else
	void NetMsg_SetupMap_Extents( entvars_t* const pev, const string& name, const float* const min_extents, const float* const max_extents, const bool draw_background )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetupMap, NULL, pev );
			WRITE_BYTE( 0 );
			WRITE_STRING( name.c_str() );
			WRITE_COORD( min_extents[2] );
			WRITE_COORD( max_extents[2] );
			WRITE_COORD( min_extents[0] );
			WRITE_COORD( min_extents[1] );
			WRITE_COORD( max_extents[0] );
			WRITE_COORD( max_extents[1] );
			WRITE_BYTE( draw_background ? 1 : 0 );
		MESSAGE_END();
	}

	void NetMsg_SetupMap_Location( entvars_t* const pev, const string& name, const float* const min_extents, const float* const max_extents )
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgSetupMap, NULL, pev );
			WRITE_BYTE( 1 );
			WRITE_STRING( name.c_str() );
			WRITE_COORD( max_extents[0] );
			WRITE_COORD( max_extents[1] );
			WRITE_COORD( min_extents[0] );
			WRITE_COORD( min_extents[1] );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef AVH_SERVER
	void NetMsg_UpdateCountdown( void* const buffer, const int size, int& countdown )
	{
		BEGIN_READ( buffer, size );
			countdown = READ_BYTE();
		END_READ();
	}
#else
	void NetMsg_UpdateCountdown( const int countdown )
	{
		MESSAGE_BEGIN( MSG_ALL, g_msgUpdateCountdown );
			WRITE_BYTE( countdown );
		MESSAGE_END();
	}
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int	kNumStatusBits = 6;
const int	kStatusMask = 0x3F;
const int	kNumTeamBits = 2;
const int	kTeamMask = 0x03;
const int	kNumPositionCoordinateBits = 12;
const int	kPositionCoordinateMask  = 0xFFF;
const int	kPositionCoordinateOffset = 4096;
const float	kPositionCoordinateScale = 0.5f;
const int	kNumPositionBits = kNumPositionCoordinateBits*2;

const int	kNumSquadBits = 3;
const int	kSquadMask = 0x07;
const int	kNumAngleBits = 4;
const int	kAngleMask = 0x0F;
const int	kNumPlayerIndexBits = 6;
const int	kPlayerIndexMask = 0x3F;
const int	kNumIndexBits = 14;
const int	kIndexMask = 0x3FFF;
const int	kNumFlagBits = 2;
const int	kFlagMask = 0x03;
const int	kEntHierFlagPlayer		= 0x01;
const int	kEntHierFlagDeletion	= 0x02;

#ifndef AVH_SERVER
	//TODO : replace OldItems with vector<int>
	void ReadEntHier( MapEntityMap& NewItems, EntityListType& OldItems, int short_data, int long_data );
	float UnpackageCoord( const int packaged_coord );
	void NetMsg_UpdateEntityHierarchy( void* const buffer, const int size, MapEntityMap& NewItems, EntityListType& OldItems )
	{
		NewItems.clear();
		OldItems.clear();
		int amnt_read = 0;
		int short_data, long_data = 0;
		BEGIN_READ( buffer, size );
			while( amnt_read < size )
			{
				short_data = READ_SHORT();
				amnt_read += 2;
				if( (short_data & kEntHierFlagDeletion) == 0 )
				{ 
					long_data = READ_LONG();
					amnt_read += 4;
				}
				ReadEntHier( NewItems, OldItems, short_data, long_data );
			}
		END_READ();
	}

	void ReadEntHier( MapEntityMap& NewItems, EntityListType& OldItems, int short_data, int long_data )
	{
		int flags = short_data & kFlagMask;
		short_data >>= kNumFlagBits;

		if( (flags & kEntHierFlagDeletion) == kEntHierFlagDeletion )	// Deletion (player or otherwise)
		{
			OldItems.push_back( short_data & kIndexMask );
			return;
		}

		MapEntity ent;
		int index = 0;

		ent.mUser3 = (AvHUser3)(long_data & kStatusMask);
		long_data >>= kNumStatusBits;
		ent.mTeam = (AvHTeamNumber)(long_data & kTeamMask);
		long_data >>= kNumTeamBits;
		ent.mY = UnpackageCoord(long_data & kPositionCoordinateMask);
		long_data >>= kNumPositionCoordinateBits;
		ent.mX = UnpackageCoord(long_data & kPositionCoordinateMask);

		if( (flags & kEntHierFlagPlayer) == kEntHierFlagPlayer )		// Player added/changed
		{
			index = short_data & kPlayerIndexMask;
			short_data >>= kNumPlayerIndexBits;
			ent.mAngle = short_data & kAngleMask;
			short_data >>= kNumAngleBits;
			ent.mSquadNumber = short_data & kSquadMask;
		}
		else															// Other item added/changed
		{
			index = short_data & kIndexMask;
			ent.mSquadNumber = 0;
			ent.mAngle = 0;
		}

		NewItems.insert( MapEntityMap::value_type( index, ent ) );
	}

	float UnpackageCoord( const int packaged_coord )
	{
		float returnVal = packaged_coord;
		returnVal /= kPositionCoordinateScale;
		returnVal -= kPositionCoordinateOffset;
		return returnVal;
	}

#else
	void WriteEntHier( const int index, const MapEntity& ent, bool delete_flag, int& short_data, int& long_data );
	int PackageCoord( const float coord );
	void NetMsg_UpdateEntityHierarchy( entvars_t* const pev, const MapEntityMap& NewItems, const EntityListType& OldItems )
	{
		const int kMaxUpdatesPerPacket = 30;
		if( NewItems.empty() && OldItems.empty() ) { return; } //nothing to send!

		MapEntityMap::const_iterator new_current, new_end = NewItems.end();
		MapEntity temp;
		EntityListType::const_iterator old_current, old_end = OldItems.end();
		int short_data, long_data, count = 1;
		MESSAGE_BEGIN( MSG_ONE, g_msgUpdateEntityHierarchy, NULL, pev );
			for( new_current = NewItems.begin(); new_current != new_end; ++new_current, ++count )
			{
				if( count % kMaxUpdatesPerPacket == 0 )
				{
					MESSAGE_END();
					MESSAGE_BEGIN( MSG_ONE, g_msgUpdateEntityHierarchy, NULL, pev );
				}
				WriteEntHier( new_current->first, new_current->second, false, short_data, long_data );
				WRITE_SHORT(short_data);
				WRITE_LONG(long_data);
			}
			for( old_current = OldItems.begin(); old_current != old_end; ++old_current, ++count )
			{
				if( count % kMaxUpdatesPerPacket == 0 )
				{
					MESSAGE_END();
					MESSAGE_BEGIN( MSG_ONE, g_msgUpdateEntityHierarchy, NULL, pev );
				}
				WriteEntHier( *old_current, temp, true, short_data, long_data );
				WRITE_SHORT(short_data);
			}
		MESSAGE_END();
	}

	void WriteEntHier( const int index, const MapEntity& ent, bool delete_flag, int& short_data, int& long_data )
	{
		if( delete_flag )
		{
				ASSERT( (index & ~kIndexMask) == 0 );
			short_data = index;
			short_data <<= kNumFlagBits;
				ASSERT( (short_data & kFlagMask) == 0 );
			short_data |= kEntHierFlagDeletion;
			return;
		}

		long_data = PackageCoord(ent.mX);
		long_data <<= kNumPositionCoordinateBits;
			ASSERT((long_data & kPositionCoordinateMask) == 0);
		long_data |= PackageCoord(ent.mY);
		long_data <<= kNumTeamBits;
			ASSERT((long_data & kTeamMask) == 0);
			ASSERT((ent.mTeam & ~kTeamMask) == 0);
		long_data |= ent.mTeam & kTeamMask;
		long_data <<= kNumStatusBits;
			ASSERT((long_data & kStatusMask) == 0);
			ASSERT((ent.mUser3 & ~kStatusMask) == 0);
		long_data |= ent.mUser3 & kStatusMask;

		switch( ent.mUser3 )
		{
		case AVH_USER3_MARINE_PLAYER: case AVH_USER3_COMMANDER_PLAYER:
		case AVH_USER3_ALIEN_PLAYER1: case AVH_USER3_ALIEN_PLAYER2:
		case AVH_USER3_ALIEN_PLAYER3: case AVH_USER3_ALIEN_PLAYER4:
		case AVH_USER3_ALIEN_PLAYER5: case AVH_USER3_ALIEN_EMBRYO:
		case AVH_USER3_HEAVY:
		{
				ASSERT( (ent.mSquadNumber & ~kSquadMask) == 0 );
			short_data = ent.mSquadNumber;
			short_data <<= kNumAngleBits;
			int angle = WrapFloat(ent.mAngle,0,360);
			angle /= 22.5f;
				ASSERT( (short_data & kAngleMask) == 0);
				ASSERT( (angle & ~kAngleMask) == 0);
			short_data |= angle & kAngleMask;
			short_data <<= kNumPlayerIndexBits;
				ASSERT( ( short_data & kPlayerIndexMask ) == 0 );
				ASSERT( ( index & ~kPlayerIndexMask ) == 0 );
			short_data |= index & kIndexMask;
			short_data <<= kNumFlagBits;
				ASSERT( ( short_data & kFlagMask ) == 0 );
			short_data |= kEntHierFlagPlayer;
			break;
		}
		default:
				ASSERT( ( index & ~kIndexMask ) == 0 );
			short_data = index & kIndexMask;
			short_data <<= kNumFlagBits;
		}
	}

	int PackageCoord( const float coord )
	{
		float adjustedCoord = coord;
		adjustedCoord += kPositionCoordinateOffset;
		adjustedCoord *= kPositionCoordinateScale;
		int returnVal = adjustedCoord;
		ASSERT( (returnVal & ~kPositionCoordinateMask) == 0);
		returnVal &= kPositionCoordinateMask;
		return returnVal;
	}

#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// tankefugl: 0000971
#ifndef AVH_SERVER
	void NetMsg_IssueOrder( void* const buffer, const int size, int& ordertype, int& ordersource, int& ordertarget )
	{
		BEGIN_READ( buffer, size );
			ordertype = READ_BYTE();
			ordersource = READ_LONG();
			ordertarget = READ_LONG();
		END_READ();
	}
#else
	void NetMsg_IssueOrder( entvars_t* const pev, const int ordertype, const int ordersource, const int ordertarget)
	{
		MESSAGE_BEGIN( MSG_ONE, g_msgIssueOrder, NULL, pev );
			WRITE_BYTE( ordertype );
			WRITE_LONG( ordersource );
			WRITE_LONG( ordertarget );
		MESSAGE_END();
	}
#endif
// :tankefugl