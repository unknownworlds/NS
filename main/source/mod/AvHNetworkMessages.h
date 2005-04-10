// AvHNetworkMessages
//
// This shared file contains utility calls to send and recieve 
// messages between an NS server and an NS client.  Centralizing
// message content and function calls here will allow for future
// bandwidth optimizations and security enhancements to be handled 
// at a global level without peppering the code.
//
// Both client and server code are stored here side by side so
// that revisions are more likely to be applied to both correctly.
//
// Many of the NS signals are multi-purpose; I've divided the server
// side calls by purpose where appropriate because the input changes
// depending on which version of the signal is meant to be in use.
//
// I haven't added tempentity signals because the client-side 
// interpretation of those signals is entirely black box, making
// any encryption or optimization impossible.
//
// KGP 10/01/04

#ifndef AVH_NETWORK_MESSAGES
#define AVH_NETWORK_MESSAGES

//TYPE DEFINITIONS
#include "AvHConstants.h"
#include "AvHSpecials.h"
#include "AvHSharedTypes.h"
#include "AvHParticleTemplate.h"
#include "AvHTechNode.h"
#include "AvHOrder.h"
#include "AvHTechSlotManager.h"
#include "AvHVisibleBlipList.h"
#include "AvHEntityHierarchy.h"
#include "..\engine\shake.h"
#include "..\common\weaponinfo.h"

//FUNCTION PROTOTYPES
#ifdef AVH_SERVER
	//SERVER MESSAGE TRANSMISSION
	void Net_InitializeMessages(void);
	void NetMsg_AmmoPickup( entvars_t* const pev, const int index, const int count );
	void NetMsg_AmmoX( entvars_t *pev, const int index, const int count );
	void NetMsg_Battery( entvars_t* const pev, const int armor_amount );
	void NetMsg_CurWeapon( entvars_t* const pev, const int state, const int id, const int clip );
	void NetMsg_Damage( entvars_t* const pev, const int dmg_save, const int dmg_take, const long bits, const float* origin );
	void NetMsg_Fade( entvars_t* const pev, const ScreenFade& fade );
	void NetMsg_Flashlight( entvars_t* const pev, const int is_on, const int flash_battery );
	void NetMsg_GeigerRange( entvars_t* const pev, const int range );
	void NetMsg_Health( entvars_t* const pev, const int health );
	void NetMsg_HideWeapon( entvars_t* const pev, const int hide );
	void NetMsg_HudText( entvars_t* const pev, const string& text );
	void NetMsg_HudText2( entvars_t* const pev, const string& text, const int flags );
	void NetMsg_InitHUD( entvars_t* const pev );
	void NetMsg_ItemPickup( entvars_t* const pev, const string& item_name );
	void NetMsg_MOTD( entvars_t* const pev, const bool is_finished, const string& MOTD );
	void NetMsg_ResetHUD( entvars_t* const pev );
	void NetMsg_SayText( entvars_t* const pev, const int entity_index, const string& text, const string& location );
	void NetMsg_ServerName( entvars_t* const pev, const string& name );
	void NetMsg_SetFOV( entvars_t* const pev, const int fov );
	void NetMsg_Shake( entvars_t* const pev, const ScreenShake& shake );
	void NetMsg_ShowGameTitle( entvars_t* const pev );
	void NetMsg_ShowMenu( entvars_t* const pev, const int valid_slots, const int display_time, const int flags, const string& content );
	void NetMsg_StatusText( entvars_t* const pev, const int location, const string& content );
	void NetMsg_StatusValue( entvars_t* const pev, const int location, const int state );
	void NetMsg_TeamInfo( entvars_t* const pev, const int player_index, const string& team_id );
	void NetMsg_TeamNames( entvars_t* const pev, const vector<string>& team_names );
	void NetMsg_TeamScore( entvars_t* const pev, const string& team_name, const int score, const int deaths );
	void NetMsg_TextMsg( entvars_t* const pev, const int destination, const vector<string>& message );
	void NetMsg_Train( entvars_t* const pev, const int state );
	void NetMsg_WeaponList( entvars_t* const pev, const WeaponList& weapon );
	void NetMsg_WeapPickup( entvars_t* const pev , const int weapon_id );

	void NetMsg_AlienInfo_Upgrades( entvars_t* const pev, const AvHAlienUpgradeListType& upgrades );
	void NetMsg_AlienInfo_Hives( entvars_t* const pev, const HiveInfoListType& hives, const HiveInfoListType& client_hives );
	void NetMsg_BalanceVarChangesPending( entvars_t* const pev, const bool pending );
	void NetMsg_BalanceVarInsertFloat( entvars_t* const pev, const string& name, const float data );
	void NetMsg_BalanceVarInsertInt( entvars_t* const pev, const string& name, const int data );
	void NetMsg_BalanceVarInsertString( entvars_t* const pev, const string& name, const string& data );
	void NetMsg_BalanceVarRemove( entvars_t* const pev, const string& name );
	void NetMsg_BalanceVarClear( entvars_t* const pev );
	void NetMsg_BlipList( entvars_t* const pev, const bool friendly_blips, const AvHVisibleBlipList& list );
	void NetMsg_BuildMiniMap_Initialize( entvars_t* const pev, const string& name, const int num_samples, const int width, const int height );
	void NetMsg_BuildMiniMap_Update( entvars_t* const pev, const int num_samples, const uint8* const samples );
	void NetMsg_BuildMiniMap_Complete( entvars_t* const pev );
	void NetMsg_ClientScripts( entvars_t* const pev, const StringList& script_names );
	void NetMsg_DebugCSP( entvars_t* const pev, const weapon_data_t& weapon_data, const float next_attack );
	void NetMsg_EditPS( entvars_t* const pev, const int particle_index );
	void NetMsg_Fog( entvars_t* const pev, const bool enabled, const int R, const int G, const int B, const float start, const float end );
	void NetMsg_GameStatus_State( entvars_t* const pev, const int status_code, const int map_mode );
	void NetMsg_GameStatus_UnspentLevels( entvars_t* const pev, const int status_code, const int map_mode, const int unspent_levels );
	void NetMsg_ListPS( entvars_t* const pev, const string& system_name );
	void NetMsg_PlayHUDNotification( entvars_t* const pev, const int flags, const int sound, const float location_x, const float location_y );
	void NetMsg_ProgressBar( entvars_t* const pev, const int entity_number, const int progress );
	void NetMsg_ServerVar( entvars_t* const pev, const string& name, const string& value );
	void NetMsg_SetGammaRamp( entvars_t* const pev, const float gamma );
	void NetMsg_SetOrder( entvars_t* const pev, const AvHOrder& order );
	void NetMsg_SetParticleTemplate( entvars_t* const pev, const AvHParticleTemplate& particle_template );
	void NetMsg_SetRequest( entvars_t* pev, const int request_type, const int request_count );
	void NetMsg_SetSelect( entvars_t* const pev, Selection& selection );
	void NetMsg_SetSoundNames( entvars_t* pev, const bool reset, const string& sound_name );
	void NetMsg_SetTechNode( entvars_t* pev, const AvHTechNode* node );
	void NetMsg_SetTechSlots( entvars_t* pev, const AvHTechSlots& tech_slots );
	void NetMsg_SetTopDown_Position( entvars_t* const pev, const bool is_top_down, const float* const position );
	void NetMsg_SetTopDown_TechSlots( entvars_t* const pev, const int tech_slots );
	void NetMsg_SetupMap_Extents( entvars_t* const pev, const string& name, const float* const min_extents, const float* const max_extents, const bool draw_background );
	void NetMsg_SetupMap_Location( entvars_t* const pev, const string& name, const float* const min_extents, const float* const max_extents );
	void NetMsg_UpdateEntityHierarchy( entvars_t* const pev, const MapEntityMap& NewItems, const EntityListType& OldItems );
	void NetMsg_IssueOrder(entvars_t* const pev, const int ordertype, const int ordersource, const int ordertarget);

	//BROADCAST MESSAGE TRANSMISSION
	void NetMsg_DeathMsg( const int killer_index, const int victim_index, string& weapon_name );
	void NetMsg_GameStatus_State( const int status_code, const int map_mode );
	void NetMsg_GameStatus_Time( const int status_code, const int map_mode, const int game_time, const int timelimit, const int attacking_team_number, const bool is_reliable );
	void NetMsg_SayText( const int entity_index, const string& text, const string& location );
	void NetMsg_TeamInfo( const int player_index, const string& team_id );
	void NetMsg_TeamScore( const string& team_name, const int score, const int deaths );
	void NetMsg_TextMsg( const int destination, const vector<string>& message );
	void NetMsg_ScoreInfo( const ScoreInfo& info );
	void NetMsg_UpdateCountdown( const int countdown );

	//SPECTATOR MESSAGE TRANSMISSION
	void NetMsgSpec_TeamInfo( const int player_index, const string& team_id );
	void NetMsgSpec_TextMsg( const int destination, const vector<string>& message );
	void NetMsgSpec_SetGammaRamp( const float gamma );

#else //!AVH_SERVER

	//CLIENT MESSAGE RECEPTION - LIMIT 64
	void NetMsg_AmmoPickup( void* const buffer, const int size, int& index, int& count );
	void NetMsg_AmmoX( void* const buffer, const int size, int& index, int& count );
	void NetMsg_Battery( void* const buffer, const int size, int& armor_amount );
	void NetMsg_CurWeapon( void* const buffer, const int size, int& state, int& id, int& clip );
	void NetMsg_Damage( void* const buffer, const int size, int& dmg_save, int& dmg_take, long& bits, float* origin );
	//5
	void NetMsg_DeathMsg( void* const buffer, const int size, int& killer_index, int& victim_index, string& weapon_name );
	void NetMsg_Fade( void* const buffer, const int size, ScreenFade& fade );
	void NetMsg_Flashlight( void* const buffer, const int size, int& is_on, int& flash_battery );
	void NetMsg_GeigerRange( void* const buffer, const int size, int& range );
	void NetMsg_Health( void* const buffer, const int size, int& health );
	//10
	void NetMsg_HideWeapon( void* const buffer, const int size, int& hide );
	void NetMsg_HudText( void* const buffer, const int size, string& text );
	void NetMsg_HudText2( void* const buffer, const int size, string& text, int& flags );
	void NetMsg_InitHUD( void* const buffer, const int size );
	void NetMsg_ItemPickup( void* const buffer, const int size, string& item_name );
	//15
	void NetMsg_MOTD( void* const buffer, const int size, bool& is_finished, string& MOTD );
	void NetMsg_ResetHUD( void* const buffer, const int size );
	void NetMsg_SayText( void* const buffer, const int size, int& entity_index, string& text, string& location );
	void NetMsg_ScoreInfo( void* const buffer, const int size, ScoreInfo& info );
	void NetMsg_ServerName( void* const buffer, const int size, string& name );
	//20
	void NetMsg_SetFOV( void* const buffer, const int size, int& fov );
	void NetMsg_Shake( void* const buffer, const int size, ScreenShake& shake );
	void NetMsg_ShowGameTitle( void* const buffer, const int size );
	void NetMsg_ShowMenu( void* const buffer, const int size, int& valid_slots, int& display_time, int& flags, string& content );
	void NetMsg_StatusText( void* const buffer, const int size, int& location, string& content );
	//25
	void NetMsg_StatusValue( void* const buffer, const int size, int& location, int& state );
	void NetMsg_TeamInfo( void* const buffer, const int size, int& player_index, string& team_id );
	void NetMsg_TeamNames( void* const buffer, const int size, vector<string>& team_names );
	void NetMsg_TeamScore( void* const buffer, const int size, string& team_name, int& score, int& deaths );
	void NetMsg_TextMsg( void* const buffer, const int size, int& destination, vector<string>& message );
	//30
	void NetMsg_Train( void* const buffer, const int size, int& state );
	void NetMsg_WeaponList( void* const buffer, const int size, WeaponList& weapon );
	void NetMsg_WeapPickup( void* const buffer, const int size, int& weapon_id );
	void NetMsg_AlienInfo( void* const buffer, const int size, bool& was_hive_info, AvHAlienUpgradeListType& upgrades, HiveInfoListType& hives );
	void NetMsg_BalanceVar( void* const buffer, const int size, string& name, BalanceMessageAction& action, int& ivalue, float& fvalue, string& svalue );
	//35
	void NetMsg_BlipList( void* const buffer, const int size, bool& friendly_blips, AvHVisibleBlipList& list );
	void NetMsg_BuildMiniMap( void* const buffer, const int size, string& name, int& num_samples, int& processed_samples, int& width, int& height, uint8** map, bool& finished );
	void NetMsg_ClientScripts( void* const buffer, const int size, StringList& script_names );
	void NetMsg_DebugCSP( void* const buffer, const int size, weapon_data_t& weapon_data, float& next_attack );
	void NetMsg_EditPS( void* const buffer, const int size, int& particle_index );
	//40
	void NetMsg_Fog( void* const buffer, const int size, bool& enabled, int& R, int& G, int& B, float& start, float& end );
	void NetMsg_GameStatus( void* const buffer, const int size, int& status_code, AvHMapMode& map_mode, int& game_time, int& timelimit, int& misc_data );
	void NetMsg_ListPS( void* const buffer, const int size, string& system_name );
	void NetMsg_PlayHUDNotification( void* const buffer, const int size, int& flags, int& sound, float& location_x, float& location_y );
	void NetMsg_ProgressBar( void* const buffer, const int size, int& entity_number, int& progress );
	//45
	void NetMsg_ServerVar( void* const buffer, const int size, string& name, string& value );
	void NetMsg_SetGammaRamp( void* const buffer, const int size, float& gamma );
	void NetMsg_SetOrder( void* const buffer, const int size, AvHOrder& order );
	void NetMsg_SetParticleTemplate( void* const buffer, const int size, AvHParticleTemplate& particle_template );
	void NetMsg_SetRequest( void* const buffer, const int size, int& request_type, int& request_count );
	//50
	void NetMsg_SetSelect( void* const buffer, const int size, Selection& selection );
	void NetMsg_SetSoundNames( void* const buffer, const int size, bool& reset, string& sound_name );
	void NetMsg_SetTechNode( void* const buffer, const int size, AvHTechNode*& node );
	void NetMsg_SetTechSlots( void* const buffer, const int size, AvHTechSlots& tech_slots );
	void NetMsg_SetTopDown( void* const buffer, const int size, bool& is_menu_tech, bool& is_top_down, float* position, int& tech_slots );
	//55
	void NetMsg_SetupMap( void* const buffer, const int size, bool& is_location, string& name, float* min_extents, float* max_extents, bool& draw_background );
	void NetMsg_UpdateCountdown( void* const buffer, const int size, int& countdown );
	void NetMsg_UpdateEntityHierarchy( void* const buffer, const int size, MapEntityMap& NewItems, EntityListType& OldItems );
	void NetMsg_IssueOrder( void* const buffer, const int size, int& ordertype, int& ordersource, int& ordertarget );

#endif //AVH_SERVER

#endif