#ifndef DIRECTOR_CONST_H
#define DIRECTOR_CONST_H

// not needed anymore
//#define	SVC_HLTV			50

#define	SVC_DIRECTOR				51	// messsage for director module in proxy

// sub commands of svc_director:
#define DRC_CMD_NONE				0	// NULL director command
#define DRC_CMD_START				1	// start director mode
#define DRC_CMD_EVENT				2	// informs about director event
#define DRC_CMD_MODE				3	// switches camera modes
#define DRC_CMD_CAMERA				4	// sets camera registers
#define DRC_CMD_TIMESCALE			5	// sets time scale
#define DRC_CMD_MESSAGE				6	// send HUD centerprint
#define DRC_CMD_SOUND				7	// plays a particular sound
#define DRC_CMD_STATUS				8	// status info about broadcast
#define DRC_CMD_BANNER				9	// banner file name for HLTV gui
#define DRC_CMD_FADE				10	// send screen fade command
#define DRC_CMD_SHAKE				11	// send screen shake command
#define DRC_CMD_STUFFTEXT			12	// like the normal svc_stufftext but as director command
#define DRC_CMD_LAST				12


// HLTV_EVENT event flags
#define DRC_FLAG_PRIO_MASK			0x0F		// priorities between 0 and 15 (15 most important)
#define DRC_FLAG_SIDE				(1<<4)	// 
#define DRC_FLAG_DRAMATIC			(1<<5)	// is a dramatic scene
#define DRC_FLAG_SLOWMOTION			(1<<6)	// would look good in SloMo
#define DRC_FLAG_FACEPLAYER			(1<<7)	// player is doning something (reload/defuse bomb etc)
#define DRC_FLAG_INTRO				(1<<8)	// is a introduction scene
#define DRC_FLAG_FINAL				(1<<9)	// is a final scene
#define DRC_FLAG_NO_RANDOM			(1<<10)	// don't randomize event data

// Size of message (command length in bytes = (1+2+2+4))
#define DIRECTOR_MESSAGE_SIZE		9

// prxoy director stuff
//#define DRC_EVENT			3	// informs the dircetor about ann important game event

#define DRC_FLAG_PRIO_MASK		0x0F	//	priorities between 0 and 15 (15 most important)
#define DRC_FLAG_DRAMATIC		(1<<5)

#endif