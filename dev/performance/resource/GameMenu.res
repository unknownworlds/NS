"GameMenu"
{
	"1"
	{
		"label" "#GameUI_GameMenu_ResumeGame"
		"command" "ResumeGame"
		"OnlyInGame" "1"
	}
	"2"
	{
		"label" "#Menu_ReadyRoom"
		"command" "engine readyroom"
		"OnlyInGame" "1"
	}
	"3"
	{
		"label" "#GameUI_GameMenu_Disconnect"
		"command" "Disconnect"
		"OnlyInGame" "1"
	}
//	"4"
//	{
//		"label" "#GameUI_GameMenu_NewGame"
//		"command" "OpenNewGameDialog"
//		"notmulti" "1"
//	}
//	"5"
//	{
//		"label" "#GameUI_GameMenu_LoadGame"
//		"command" "OpenLoadGameDialog"
//		"notmulti" "1"
//	}
	"6"
	{
		"label" "#GameUI_GameMenu_SaveGame"
		"command" "OpenSaveGameDialog"
		"notmulti" "1"
		"OnlyInGame" "1"
	}
	"7"
	{
		"label" ""
		"command" ""
		"notmulti" "1"
	}
	"8"
	{
		"label" "#GameUI_GameMenu_FindServers"
		"command" "OpenServerBrowser"
	}
	"9"
	{
		"label" "#GameUI_GameMenu_CreateServer"
		"command" "OpenCreateMultiplayerGameDialog"
	}
//	"10"
//	{
//		"name" "LoadDemo"
//		"label" "#GameUI_GameMenu_PlayDemo"
//		"command" "OpenLoadDemoDialog"
//	}
	"11"
	{
		"label" ""
		"command" ""
	}
	"12"
	{
		"label" "#GameUI_GameMenu_ChangeGame"
		"command" "OpenChangeGameDialog"
		"notsteam" "1"
	}
	"13"
	{
		"label" "#GameUI_GameMenu_Options"
		"command" "OpenOptionsDialog"
	}
	"14"
	{
		"label" "#GameUI_GameMenu_Quit"
		"command" "Quit"
	}
}