//
// TRACKER SCHEME RESOURCE FILE
//
// sections:
//		colors			- all the colors used by the scheme
//		basesettings	- contains settings for app to use to draw controls
//		fonts			- list of all the fonts used by app
//		borders			- description of all the borders
//
// notes:
// 		hit ctrl-alt-shift-R in the app to reload this file
//
Scheme
{
	//Name - currently overriden in code
	//{
	//	"Name"	"ClientScheme"
	//}

	//////////////////////// COLORS ///////////////////////////
	Colors
	{
		// base colors
		"BaseText"			"255 176 0 255"	// used in text windows, lists
		"BrightBaseText"	"255 176 0 255"	// brightest text
		"SelectedText"		"255 176 0 255"	// selected text
		"DimBaseText"		"255 176 0 255"	// dim base text
		"LabelDimText"		"255 176 0 164"	// used for info text
		"ControlText"		"255 176 0 255"	// used in all text controls
	//	"BrightControlText"	"255 176 0 255"	// use for selected controls
		"BrightControlText"	"255 255 255 255"	// use for selected controls
		"DisabledText1"		"80 48 0 255"	// disabled text
		"DisabledText2"		"0 0 0 0"		// overlay color for disabled text (to give that inset look)
		"DimListText"		"188 112 0 255"	// offline friends, unsubscribed games, etc.

		// background colors
		"ControlBG"			"0 0 0 0"		// background color of controls
		"ControlDarkBG"		"0 0 0 128"		// darker background color; used for background of scrollbars
		"WindowBG"			"0 0 0 200"		// background color of text edit panes (chat, text entries, etc.)
		"SelectionBG"		"192 28 0 140"	// background color of any selected text or menu item
		"SelectionBG2"		"70 12 0 0"		// selection background in window w/o focus
		"ListBG"			"0 0 0 128"		// background of scoreboard
		"ViewportBG"		"55 43 29 140"

		// titlebar colors
		"TitleText"			"255 174 0 255"
		"TitleDimText"		"255 174 0 255"
		"TitleBG"			"255 255 0 0"
		"TitleDimBG"		"255 255 0 0"
		
		// slider tick colors
		"SliderTickColor"		"127 140 127 255"
		"SliderTrackColor"	"31 31 31 255"

		// border colors
		"BorderBright"		"64 52 36 255"	// the lit side of a control
		"BorderDark"		"64 52 36 255"	// the dark/unlit side of a control
		"BorderSelection"	"188 112 0 0"	// the additional border color for displaying the default/selected button

		"team0"			"255 179 0 255"
		"team1"			"76 102 76 255"
		"team2"			"255 64 64 255"
		"team3"			"255 179 0 255"
		"team4"			"255 179 0 255"

		"HintColor"		"255 255 255 160"
		"HintBackgroundColor"	"0 0 0 128"

		//new class menu colours
		"ClassMenuLight"	"175 127 1 255"
		"ClassMenuDark"		"64 52 36 255"
		"white"				"255 255 255 255"
		"red"				"255 0 0 255"
		"nothing"			"0 0 0 0"

		"MapDescriptionText"	"255 255 255 255" // the text used in the map description window
	}

	///////////////////// BASE SETTINGS ////////////////////////
	// default settings for all panels
	// controls use these to determine their settings
	BaseSettings
	{
		"FgColor"			"ControlText"
		"BgColor"			"ControlBG"
		"LabelBgColor"		"ControlBG"
		"SubPanelBgColor"	"ControlBG"

		"DisabledFgColor1"		"DisabledText1" 
		"DisabledFgColor2"		"DisabledText2"			// set this to the BgColor if you don't want it to draw

		"TitleBarFgColor"			"TitleText"
		"TitleBarDisabledFgColor"	"TitleDimText"
		"TitleBarBgColor"			"TitleBG"
		"TitleBarDisabledBgColor"	"TitleDimBG"

		"TitleBarIcon"				"resource/icon_steam"
		"TitleBarDisabledIcon"		"resource/icon_steam_disabled"

		"TitleButtonFgColor"			"BorderBright"
		"TitleButtonBgColor"			"ControlBG"
		"TitleButtonDisabledFgColor"	"TitleDimText"
		"TitleButtonDisabledBgColor"	"TitleDimBG"

		"TextCursorColor"			"BaseText"			// color of the blinking text cursor in text entries
		"URLTextColor"				"BrightBaseText"	// color that URL's show up in chat window

		Menu
		{
			"FgColor"			"DimBaseText"
			"BgColor"			"ControlBG"
			"ArmedFgColor"		"BrightBaseText"
			"ArmedBgColor"		"SelectionBG"
			"DividerColor"		"BorderDark"
			"TextInset"			"6"
		}

		MenuButton	  // the little arrow on the side of boxes that triggers drop down menus
		{
			"ButtonArrowColor"	"DimBaseText"		// color of arrows
		   	"ButtonBgColor"		"WindowBG"			// bg color of button. same as background color of text edit panes 
			"ArmedArrowColor"	"BrightBaseText"	// color of arrow when mouse is over button
			"ArmedBgColor"		"DimBaseText"		// bg color of button when mouse is over button
		}

		Slider
		{
			"SliderFgColor"		"ControlBG"			// handle with which the slider is grabbed
			"SliderBgColor"		"ControlDarkBG"		// area behind handle
		}

		ScrollBarSlider
		{
			"BgColor"					"ControlBG"		// this isn't really used
			"ScrollBarSliderFgColor"	"ControlBG"		// handle with which the slider is grabbed
			"ScrollBarSliderBgColor"	"ControlDarkBG"	// area behind handle
			"ButtonFgColor"				"DimBaseText"	// color of arrows
		}


		// text edit windows
		"WindowFgColor"				"BaseText"		// off-white
		"WindowBgColor"				"WindowBG"		// redundant. can we get rid of WindowBgColor and just use WindowBG?
		"WindowDisabledFgColor"		"DimBaseText"
		"WindowDisabledBgColor"		"ListBG"		// background of chat conversation
		"SelectionFgColor"			"SelectedText"	// fg color of selected text
		"SelectionBgColor"			"SelectionBG"
		"ListSelectionFgColor"		"SelectedText"
		"ListBgColor"				"ListBG"		// background of server browser control, etc
		"BuddyListBgColor"			"ListBG"		// background of buddy list pane
		
		// App-specific stuff
		"ChatBgColor"				"WindowBG"

		// status selection
		"StatusSelectFgColor"		"BrightBaseText"
		"StatusSelectFgColor2"		"BrightControlText"	// this is the color of the friends status

		// checkboxes
		"CheckButtonBorder1"   		"BorderDark"		// the left checkbutton border
		"CheckButtonBorder2"   		"BorderBright"		// the right checkbutton border
		"CheckButtonCheck"			"BrightControlText"	// color of the check itself
		"CheckBgColor"				"ListBG"

		// buttons (default fg/bg colors are used if these are not set)
		"ButtonArmedFgColor"		"BrightControlText"
		"ButtonArmedBgColor"		"ClassMenuLight"
		"ButtonDepressedFgColor"	"BrightControlText"
		"ButtonDepressedBgColor"	"ClassMenuLight"

		"ButtonFgColor"				"BrightControlText"
		"ButtonBgColor"				"ClassMenuDark"
	
		// buddy buttons
		BuddyButton
		{
			"FgColor1"				"ControlText"
			"FgColor2"				"DimListText"
			"ArmedFgColor1"			"BrightBaseText"
			"ArmedFgColor2"			"BrightBaseText"
			"ArmedBgColor"			"SelectionBG"
		}

		Chat
		{
			"TextColor"				"BrightControlText"
			"SelfTextColor"			"BaseText"
			"SeperatorTextColor"	"DimBaseText"
		}

		"SectionTextColor"		"BrightControlText"	// text color for IN-GAME, ONLINE, OFFLINE sections of buddy list
		"SectionDividerColor"	"BorderDark"		// color of line that runs under section name in buddy list
	}

	//
	//////////////////////// FONTS /////////////////////////////
	//
	// describes all the fonts
	Fonts
	{
		// fonts are used in order that they are listed
		// fonts listed later in the order will only be used if they fulfill a range not already filled
		// if a font fails to load then the subsequent fonts will replace
		"Default"
		{
			"1"
			{
				"name"		"Times New Roman"
				"tall"		"15"
				"weight"	"400"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"antialias"		"1"
			}
			"2"
			{
				"name"		"Courier New"
				"tall"		"15"
				"weight"	"400"
				"range"		"0x0000 0xFFFF"	//	all
				"antialias"		"1"
			}
		}
		"DefaultUnderline"
		{
			"1"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"500"
				"underline" "1"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
			}
			"2"
			{
				"name"		"Tahoma"
				"tall"		"12"
				"weight"	"500"
				"underline" "1"
				"range"		"0x0000 0xFFFF"	//	all
			}
		}
		"DefaultSmall"
		{
			"1"
			{
				"name"		"Times New Roman"
				"tall"		"13"
				"weight"	"800"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
			}
			"2"
			{
				"name"		"Courier New"
				"tall"		"13"
				"weight"	"800"
				"range"		"0x0000 0xFFFF"	//	all
			}
		}
		"DefaultVerySmall"
		{
			"1"
			{
				"name"		"Courier New"
				"tall"		"11"
				"weight"	"0"
				"range"		"0x0000 0x017F" //	Basic Latin, Latin-1 Supplement, Latin Extended-A
				"antialiased"	"1"
			}
			"2"
			{
				"name"		"Courier"
				"tall"		"11"
				"weight"	"0"
				"range"		"0x0000 0xFFFF"	//	all
				"antialiased"	"1"
			}
		}

		// this is the symbol font
		"Marlett"
		{
			"1"
			{
				"name"		"Marlett"
				"tall"		"14"
				"weight"	"0"
				"symbol"	"1"
				"range"		"0x0000 0x007F"	//	Basic Latin
			}
		}

		"ClassMenuTitleFont"
		{
			"1"
			{
				"name"		"Times New Roman"
				"tall"		"20"
				"weight"	"500"		//bold 0 - 1000
				"range"		"0x0000 0x007F" //  all
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Times New Roman"
				"tall"		"20"
				"weight"	"500"		
				"range"		"0x0000 0xFFFF" //  all
				"antialias"	"1"
			}
		}

		"ClassMenuBoldFont"
		{
			"1"
			{
				"name"		"Times New Roman"
				"tall"		"11"
				"weight"	"800"		//bold 0 - 1000
				"range"		"0x0000 0x007F" //  all
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Times New Roman"
				"tall"		"11"
				"weight"	"800"		
				"range"		"0x0000 0xFFFF" //  all
				"antialias"	"1"
			}
		}

		"ClassMenuBodyFont"
		{
			"1"
			{
				"name"		"Times New Roman"
				"tall"		"13"
				"weight"	"500"		//bold 0 - 1000
				"range"		"0x0000 0x007F" //  all
				"antialias"	"1"
			}
			"2"
			{
				"name"		"Times New Roman"
				"tall"		"13"
				"weight"	"500"		
				"range"		"0x0000 0xFFFF" //  all
				"antialias"	"1"
			}
		}
	}

	//
	//////////////////// BORDERS //////////////////////////////
	//
	// describes all the border types
	Borders
	{
		BaseBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}
		
		TitleButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"4"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		TitleButtonDisabledBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BgColor"
					"offset" "1 0"
				}
			}
			Top
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BgColor"
					"offset" "0 0"
				}
			}
		}

		TitleButtonDepressedBorder
		{
			"inset" "1 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonBorder
		{
			"inset" "1 0 0 0"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		ScrollBarButtonDepressedBorder
		{
			"inset" "2 2 0 0"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}
		
		ButtonDepressedBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 0"
				}
			}
		}

		ButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 0"
				}
			}
		}

		ButtonBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "nothing"
					"offset" "0 0"
				}
			}
		}

		FrameBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "0 0"
				}
			}
		}

		HintBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "HintColor"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "HintColor"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "HintColor"
					"offset" "0 1"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "HintColor"
					"offset" "0 0"
				}
			}
		}


		TabBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		TabActiveBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "ControlBG"
					"offset" "6 2"
				}
			}
		}


		ToolTipBorder
		{
			"inset" "0 0 1 0"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}

		// this is the border used for default buttons (the button that gets pressed when you hit enter)
		ButtonKeyFocusBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}
			Top
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}
			Right
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}
			Bottom
			{
				"1"
				{
					"color" "BorderSelection"
					"offset" "0 0"
				}
				"2"
				{
					"color" "BorderDark"
					"offset" "1 1"
				}
			}
		}

		ButtonDepressedBorder
		{
			"inset" "2 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		ComboBoxBorder
		{
			"inset" "0 0 1 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}

		MenuBorder
		{
			"inset" "1 1 1 1"
			Left
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "1 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}
		}
		BrowserBorder
		{
			"inset" "0 0 0 1"
			Left
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 1"
				}
			}

			Right
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}

			Top
			{
				"1"
				{
					"color" "BorderDark"
					"offset" "0 0"
				}
			}

			Bottom
			{
				"1"
				{
					"color" "BorderBright"
					"offset" "0 0"
				}
			}
		}
	}
}