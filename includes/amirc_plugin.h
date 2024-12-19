#ifndef AMIRC_PLUGIN_H
#define AMIRC_PLUGIN_H $Id: amirc_plugin.h,v 1.7 2011-07-21 04:41:50 piru Exp $

/*
** AmIRC Plugin definitions
** ========================
**
** (C) 1997-99 Oliver Wagner <owagner@vapor.com>
** All Rights Reserved
**
** Revision 1 (22-11-97)
** ---------------------
** - initial version
**
** Revision 2 (30-11-97)
** ---------------------
** - modified by Simone Tellini for
**   GCC/StormC-compatibility
** - added note to not store the
**   function table pointer globally.
**
** Revision 3 (30-11-97)
** ---------------------
** - now all hook functions are
**   called with a pointer to the
**   functable, to overcome
**   compiler which don't support
**   unique data segments per opener.
**
** Revision 4 (02-01-98)
** ---------------------
** - added new DCC handling functions
** - added sound function
** - added userdata field
**
** Revision 5 (03-01-98)
** ---------------------
** - added color code definitions
** - added AMIPLUGATTR_ for Alias, Ignore and Hilite list
** - added amiplug_malloc()
** - added amiplug_getstring()
** - added amirc_stringnode and amirc_ignore definitions
** - modified amiplug_get(dcc)attr() prototype to use
**   APTR for the storage pointer
** - added AMIPLUGATTR_StartupCFG, _FKeyStrings,
**   _Catalog and _MemPool
** - added DCC state definitions
**
** Revision 6 (04-01-98)
** ---------------------
** - added amiplug_queueline() function
**
** Revision 7 (06-01-98)
** ---------------------
** - added AMIPLUGDCCATR_UserData
**
** Revision 8 (08-01-98)
** ---------------------
** - added AMIPLUG_Query_CustomCTCP and related stuff
**
** Revision 9 (06-08-98)
** ---------------------
** - added amiplug_getserver() function
** - added amiplug_getexename() function
**
** Revision 10 (27-12-98)
** ---------------------
** - added amiplug_sendrxmsg() function
** - added AMIPLUG_Query_Hook_FinalSetup and related stuff
**
** Revision 11 (01-05-99)
** ----------------------
** (Done by Jamie "Entity" van den Berge)
** - AMIPLUG_Setup() can now return NULL to make AmIRC skip
**   the plugin.
** - added AMIPLUGATTR_Target, _Channels,
**   _ServerChannelModes, _ServerUserModes.
** - added AMIPLUG_Query_Hook_Input and related stuff
** - added AMIPLUG_Query_CustomRexx and related stuff
** - added amiplug_getchanattr() function and tags
** - added AMIPLUGATTR_Version
**
** Revision 12 (20-05-99)
** ----------------------
** (Done by Jamie "Entity" van den Berge)
** - added AMIPLUG_Query_Hook_Prefs and related stuff
**
** Revision 13 (16-06-99)
** ----------------------
** (Done by Jamie "Entity" van den Berge)
** - added amiplug_free() function to free memory allocated
**   with amiplug_malloc()
** - added AMIPLUG_Query_Hook_DCCChat and related stuff
** - added amiplug_senddccmsg() function
** - added amiplug_fakerawline() function
**
** Revision 14 (30-07-99)
** ----------------------
** (Done by Jamie "Entity" van den Berge)
** - added amiplug_out_infowin() function
** - added AMIPLUG_Query_Hook_Rawline2 and _NumericMsg2 tags
**   and related stuffs
** - added AMIPLUGPREFSPATH define. Please use this path for
**   any datafiles your plugin needs/creates.
** - added AMIPLUGCHANATTR_Topic
**
** Revision 15 (19-08-99)
** ----------------------
** (Done by Jamie "Entity" van den Berge)
** - added __stdargs and __asm definitions for StormC
**
** Revision 16 (27-10-04)
** ----------------------
** (Done by Nicolas "Henes" Sallin)
** - added MorphOS native PPC plugin stuff
**
** Revision 17 (18-07-11)
** ----------------------
** (Done by Harry "Piru" Sintonen)
** - added amiplug_new_adddcc_send() function
** - added amiplug_new_adddcc_recv() function
** - added amiplug_new_adddcc_chat() function
** - added DCCF_TURBO and DCCF_PASSIVE defines
**
** (24-12-20)
** ----------
** (Done by Joni Valtanen)
** - SDI conversions
*/

#include <SDI_compiler.h>

#ifndef PLFUNC
#define PLFUNC ASM
#endif

#include <exec/types.h>

#include <utility/tagitem.h>
#include <utility/hooks.h>

#ifdef __GNUC__
struct sockaddr_in;
#endif


#define AMIPLUG_TAGBASE (TAG_USER+0x87c12)

/*
** AMIPLUG_Setup() is supposed to return a static TagList
** which describes the ability and requirements of
** a plugin, or NULL if some crucial initialisation in your
** plugin failed and don't want the plugin to be loaded.
**
** Upon startup, AmIRC will scan PROGDIR:Plugins/#?.AMIPlug,
** OpenLibrary() anything it finds and call upon AMIPLUG_Setup()
** to get information.
** 
*/

#define AMIPLUG_QUERYBASE (AMIPLUG_TAGBASE+100)

#define AMIPLUG_Query_Version (AMIPLUG_QUERYBASE+0)			/* ULONG version */
#define AMIPLUG_Query_Revision (AMIPLUG_QUERYBASE+1)		/* ULONG revision */
#define AMIPLUG_Query_Copyright (AMIPLUG_QUERYBASE+2)		/* STRPTR copyright information */
#define AMIPLUG_Query_Infostring (AMIPLUG_QUERYBASE+3)		/* STRPTR generic info string */

/* ULONG
   Here you can specifiy the minimum AmIRC version
   required for this plugin. AmIRC versions below
   this one will simply skip the plugin during load.
   The version is specified as a longword with the
   high 16 bit word specifying the version, and the low
   16 bit word specifying the revision. Example:
   If the plugin requires a minimum AmIRC version of
   2.1, the value would be 0x00020001
*/
#define AMIPLUG_Query_MinAmIRCVersion (AMIPLUG_QUERYBASE+4)             /* ULONG */

/* LONG
   Plugin priority, ranging from -128 (lowest) to 127
   (highest). After AMIPLUG_Setup(), your plugin will
   always be called before all others with a priority
   lower than specified here.
*/
#define AMIPLUG_Query_Priority (AMIPLUG_QUERYBASE+17)

/* BOOL
   If this is TRUE, AmIRC will call the plugin's 
   AMIPLUG_Hook_Rawline() function for every raw line received
   from the server. What AmIRC does next after you're done
   can be influenced by returning a value composed of the EF_
   flags below. Beware: be very very sure to not eat up any
   WHOIS or NAMES or other server responses this way, otherwise
   you may mess up AmIRC's internal processing big time.
*/
#define AMIPLUG_Query_Hook_Rawline (AMIPLUG_QUERYBASE+10)

/* LONGBITS
   Using this tag instead of AMIPLUG_Query_Hook_Rawline
   will make AmIRC interpret the returncode of the hook
   differently. You can use the EF_ values below to tell
   AmIRC how to handle the line next.
*/
#define AMIPLUG_Query_Hook_Rawline2 (AMIPLUG_QUERYBASE+23)

/* BOOL
   If this is TRUE, AmIRC will call the plugin's 
   AMIPLUG_Hook_DCC() function for every update of the DCC list
   windows. The plugin can then call upon amiplug_getdccattr()
   to find out more about about the DCC connection in question.
*/
#define AMIPLUG_Query_Hook_DCC (AMIPLUG_QUERYBASE+14)

/* BOOL
   If this is TRUE, AmIRC will call the plugin's 
   AMIPLUG_Hook_NumericMsg() function for every raw line received
   from the server which is a numeric message code. If the function
   returns a non-zero value, AmIRC will *NOT* further process the
   line. Beware -- be very very sure to not eat up any WHOIS or
   NAMES or other server responses this way, otherwise you may
   mess up AmIRC's internal processing big time.
*/
#define AMIPLUG_Query_Hook_NumericMsg (AMIPLUG_QUERYBASE+12)

/* LONGBITS
   Using this tag instead of AMIPLUG_Query_Hook_NumericMsg
   will make AmIRC interpret the returncode of the hook
   differently. You can use the EF_ values below to tell
   AmIRC how to handle the line next.
*/
#define AMIPLUG_Query_Hook_NumericMsg2 (AMIPLUG_QUERYBASE+22)

/* BOOL
   If TRUE, AmIRC will call the plugin's AMIPLUG_Hook_Input()
   function everytime you hit enter in a channel- or query-
   window's input gadget. This function should return one of
   the following things:

	 String pointer:
	   Pointer to buffer containing replacement string.
	   If you don't want to modify the string, or modified
	   it in-place, return the pointer that was passed to you.

	 NULL:
	   If you don't want AmIRC or any other plugins with an
	   input hook to process the string. BEWARE: Be very sure
	   not to return NULL in all cases, for obvious reasons.

   Note that the string you pass back may get changed by other
   plugins with an input hook. When modifying in-place, do not
   go beyond the end of the string.
*/
#define AMIPLUG_Query_Hook_Input (AMIPLUG_QUERYBASE+18)

/* struct amiplug_cmd *
   This tag may appear several times. It specifies a 
   custom command which will appear to the user as a
   "normal" command. If the command parser hits one
   of the user commands, it calls upon the plugin's
   AMIPLUG_DoCommand() function.
   See below for a description of amiplug_cmd.
*/
#define AMIPLUG_Query_CustomCommand (AMIPLUG_QUERYBASE+11)

/* struct amiplug_menu *
   This tag may appear several times. It specifies a 
   custom menu item in the "Plugins" section. If the user
   selects the menu item, the plugin's AMIPLUG_DoMenu() 
   function is called.
   See below for a description of amiplug_menu
*/
#define AMIPLUG_Query_CustomMenu (AMIPLUG_QUERYBASE+13)

/* struct amiplug_ctcp *
   This tag may appear several times. It specifies a 
   custom ctcp command which will appear to the user as a
   "normal" ctcp command. If a user request one
   of the custom ctcp commands, it calls upon the plugin's
   AMIPLUG_DoCTCP() function.
   See below for a description of amiplug_ctcp.
*/
#define AMIPLUG_Query_CustomCTCP (AMIPLUG_QUERYBASE+15)

/* struct amiplug_rexxcmd *
   This tag may appear several times. It specifies a
   custom rexx command which will be added to AmIRC's
   internal rexx commands. If this command is called by
   REXX, it callsupon the plugin's AMIPLUG_DoRexx()
   function. You cannot override internal or already
   defined commands. You can get a pointer to the RexxMsg
   in the usual way by get'ing MUIA_Application_RexxMsg.
   See below for a description of amiplug_rexxcmd.
*/
#define AMIPLUG_Query_CustomRexx (AMIPLUG_QUERYBASE+19)

/* BOOL
   If this is TRUE, AmIRC will call the plugin's 
   AMIPLUG_FinalSetup() function after everything
   is set up.
*/
#define AMIPLUG_Query_Hook_FinalSetup (AMIPLUG_QUERYBASE+16)

/* BOOL
   If TRUE, AmIRC will call the _Hook_Prefs() function of
   the plugin to build and maintain a custom preferences
   page in AmIRC's Setup window. See the AMIPLUGPREFS_
   methodid's below.
*/ 
#define AMIPLUG_Query_Hook_Prefs (AMIPLUG_QUERYBASE+20)

/* BOOL
   If TRUE, AmIRC will call the _Hook_DCCChat() function
   of the plugin for each line received via a DCC CHAT
   connection. If the function returns a non-zero value,
   AmIRC will NOT further process the line. You may use
   amiplug_getdccattr() on the dccid passed to the hook
   function.
*/ 
#define AMIPLUG_Query_Hook_DCCChat (AMIPLUG_QUERYBASE+21)

/* Next available tag: 24 */


/* If your plugin needs to save/use data, please use the
   following directory for your data file(s). This way the
   plugin and amirc directories won't clutter up with all
   sorts of config/data files.
*/
#define AMIPLUGPREFSPATH "PROGDIR:Plugins/Data"




/*
** Magic 'Eat' flags, to be used as returncodes of
** AMIPLUG_Hook_Rawline and _NumericMsg, but ONLY when
** the AMIPLUG_Query_Hook_Rawline2 resp. _NumericMsg2
** tags were used to enable the hooks.
** You can specify any of these in return() ('OR' them
** together), but some effects may not be relevant in
** combination with others. returning TRUE instead of
** a special returncode eats the rawline as usual and
** is the same as returning EF_Eat)
*/
#define EF_Eat_AmIRC		(1<<1) // Don't let AmIRC handle this line
#define EF_Eat_Plugins		(1<<2) // Don't let following plugins handle this line
#define EF_Eat				(EF_Eat_AmIRC|EF_Eat_Plugins)

#define EF_NoOutput_AmIRC	(1<<8) // Don't eat line but suppress AmIRC output for it
#define EF_NoOutput_Plugins	(1<<9) // Don't eat line but suppress other PLUGIN output for it
#define EF_NoOutput			(EF_NoOutput_AmIRC|EF_NoOutput_Plugins)

#define EF_NoSound_AmIRC	(1<<10) // Suppress any sounds played by AmIRC for this line
#define EF_NoSound_Plugins	(1<<11) // Suppress any sounds played by other PLUGINS for this line
#define EF_NoSound			(EF_NoSound_AmIRC|EF_NoSound_Plugins)


/* DCC flags for new V17 API calls */

#define DCCF_TURBO      (1 << 0)  /* It is a Turbo DCC SEND without ACKs. */
#define DCCF_PASSIVE    (1 << 1)  /* It is a Passive (reverse) DCC. */


/*
** Structure describing a command.
** "name" is the command name (without trailing "/")
** "template" is the template given when the user types
**   /? command
** "minimumparms" is the minimum number of parameters
**   required for this command
** "needchannel" specifies that the first parameter
**   has to be a channel name.
*/
struct amiplug_cmd {
		struct MinNode n;
		char *name;
		char *template;
		ULONG commandid;
		ULONG minimumparms;
		ULONG needchannel;
};

/*
** This structure is handed to a custom command
** when it is called.
** The "parms" pointers point to the start of
** the first, second and third command parts;
** parms2b and parms3b point to the WSP between
** the second and third command token. Example:
** If you want to split out the first command word,
** do a "*parms2b = 0;"
*/
struct amiplug_cmdparm {
		char *cmdline;
		char *channelname;
		char *parms, *parms2, *parms3;
		char *parms2b, *parms3b;
		APTR muiapp;
};

/*
** Menu item specifier
*/
struct amiplug_menu {
		struct MinNode n;
		APTR link;
		char *label;
		ULONG commandid;
};

/*
** Custom CTCP command specifier
*/
struct amiplug_ctcp {
		struct MinNode n;
		char *name;
		ULONG commandid;
};

/*
** Custom ARexx command specifier
**
** "name" is the name of the ARexx function (should be lowercase)
** "template" is the ReadArgs() template used by AmIRC to verify
** usage and to parse the arguments, NULL is legal here. A pointer
** to the args array is passed to the AMIPLUG_DoRexx() function.
** "parameters" MUST equal the number of parameters in the template!
** Note: you cannot override internal or previously defined commands.
*/
struct amiplug_rexxcmd {
		struct MinNode n;
		char *name;
		char *template;
		ULONG parameters;
		ULONG commandid;
};


/*
** amiplug_getattr() can be used to get information
** an various internal states. It's used similiar
** to BOOPSI GetAttr(), and it returns a BOOL
** TRUE/FALSE value which designates whether
** the storage location is valid or not.
**
*/

enum {
	AMIPLUGATTR_first = 1,
	AMIPLUGATTR_IsConnected,		/* ULONG: currently connected */
	AMIPLUGATTR_CurrentNick,		/* STRPTR: Current nickname */
	AMIPLUGATTR_CurrentServer,		/* STRPTR: Current server */
	AMIPLUGATTR_CurrentServerPort,	/* ULONG: Current server port */
	AMIPLUGATTR_ConnectTime,		/* time_t: When connection was established */
	AMIPLUGATTR_AppObject,			/* APTR: MUI application object */
	AMIPLUGATTR_AliasList,			/* struct MinList* list of amirc_stringnodes */
	AMIPLUGATTR_HiliteList,			/* struct MinList* list of amirc_stringnodes */
	AMIPLUGATTR_IgnoreList,			/* struct MinList* list of amirc_ignorenodes */
	AMIPLUGATTR_StartupCFG,			/* STRPTR: name of startup cfg file */
	AMIPLUGATTR_MemPool,			/* APTR: general AmIRC memory pool */
	AMIPLUGATTR_FKeyStrings,		/* STRPTR[30]: function key strings*/
	AMIPLUGATTR_Catalog,			/* APTR: catalog in use (may be NULL!) */

	/* V11 attributes */

	AMIPLUGATTR_Target,             /* STRPTR: channel/nickname of current window */
	AMIPLUGATTR_Channels,           /* STRPTR *: NULL terminated array of channels joined */
	AMIPLUGATTR_ServerChannelModes,	/* STRPTR: Available channel modes on current server */
	AMIPLUGATTR_ServerUserModes,	/* STRPTR: Available user modes on current server */
	AMIPLUGATTR_Version,			/* ULONG: AmIRC version: 0xvvrraaaa vv = version,
									          rr = revision, aaaa = alpha rev. */

};

/*   
** amiplug_dccgetattr() can be used to get information
** about DCC connections. Returns FALSE if the DCCID
** could not be found, or the attribute was invalid.
** [SRC] shows whether the attribute is valid for
** Sends, Receives and Chats respectively.
**
*/

enum {
	AMIPLUGDCCATTR_first = 8192,
	AMIPLUGDCCATTR_Type,				/* [SRC] ULONG: 0 = Send, 1 = Receive, 2 = Chat */
	AMIPLUGDCCATTR_Nick,				/* [SRC] STPRTR: nickname */
	AMIPLUGDCCATTR_Filename,			/* [SR.] STPRTR: filename with path */
	AMIPLUGDCCATTR_State,				/* [SRC] ULONG: state */
	AMIPLUGDCCATTR_RemoteHostname,		/* [SRC] STRPTR: remote hostname */
	AMIPLUGDCCATTR_RemoteIP,			/* [SRC] ULONG: remote IP */
	AMIPLUGDCCATTR_Starttime,			/* [SR.] time_t: when transfer was started */
	AMIPLUGDCCATTR_Filelen,				/* [SR.] ULONG: total filelen */
	AMIPLUGDCCATTR_Filedone,			/* [SR.] ULONG: how much is transfered */
	AMIPLUGDCCATTR_Startoffset,			/* [SR.] ULONG: startoffset, for resumes/moves */
	AMIPLUGDCCATTR_IsMove,				/* [SR.] ULONG: whether this is a DCC MOVE */
	AMIPLUGDCCATTR_IsTurbo,				/* [.R.] ULONG: whether this is a DCC TSEND */
	AMIPLUGDCCATTR_IsStalled,			/* [SR.] ULONG: transfer stalled? */
	AMIPLUGDCCATTR_IsSecure,			/* [..C] ULONG: encrypted SCHAT? */
	AMIPLUGDCCATTR_UserData,			/* [..C] ULONG*: Pointer (!) to Userdata */
	AMIPLUGDCCATTR_IsPassive,			/* [.RC] ULONG: whether this is a Passive DCC [V17] */
};


/* [V11]
** amiplug_getchanattr() can be used to get various information
** about a channel. Returns FALSE if the channel could not be
** found or the attribute was invalid.
*/

enum {
	AMIPLUGCHANATTR_first = 4096,
	AMIPLUGCHANATTR_Modes,				/* LONGBITS: channel modes (CMF_ defines) */
	AMIPLUGCHANATTR_Keyword,			/* STRPTR: channel keyword */
	AMIPLUGCHANATTR_Userlimit,			/* ULONG: channel user limit */
	AMIPLUGCHANATTR_CurrentStatus,		/* ULONG: status on channel, 0 = nothing, 1 = op, 2 = voice */

	/* [V14] */
	AMIPLUGCHANATTR_Topic,				/* STRPTR: channel topic */
};


/*
** holds your plugin's custom preference page information
** A pointer to this structure, which is already allocated for you 
** by AmIRC, is passed to the AMIPLUG_Hook_Prefs() function.
*/
struct amiplug_prefs {
	STRPTR label;			/* list item label, defaults to plugin name */
	struct BitMap *bitmap;	/* 24x14 list icon bitmap, defaults to plugin image */
	APTR colormap;			/* bitmap's colormap, defaults to MWB palette (8 col.) */
	Object *object;			/* preferences object */
	ULONG reserved[8];		/* reserved for future expansion, don't touch */
}; // size: 48

/* [V12]
** These are the methods you are expected to handle in _Hook_Prefs()
** whenever AmIRC wants to know or do stuff with your prefs.
** The description of the method id is what AmIRC expects you to
** do or what to return when it is requested.
*/
enum {
	AMIPLUGPREFS_first = 16384,

	/* Setup stuff
	   Do whatever setup you need to do during this method. This can be eg.
	   setting default prefs, filling the prefs structure with the prefs
	   label, bitmap and colormap, etc. This method is called only once,
	   directly after AMIPLUG_Setup() (or AMIPLUG_FinalSetup() if you have
	   one).
	*/
	AMIPLUGPREFS_Setup,


	/* Cleanup stuff
	   This method is performed only once, just before AMIPLUG_Cleanup().
	*/
	AMIPLUGPREFS_Cleanup,

	/* Create prefsobject.
	   You should create your prefs object here and put that in prefs->object.
	   Don't check for success, AmIRC will do that for you and perform an
	   AMIPLUGPREFS_Dispose if anything went wrong. If you specify NULL as
	   bitmap pointer, AmIRC will use the default (plugin) image and colormap
	   for the list entry. Note: Make sure your prefs object is sizable in all
	   directions or you'll annoy your user!
	   Note: You could also set up the label, bitmap, etc, here instead of
	   _Setup, as they only need to be valid between _Create and _Dispose.
	*/
	AMIPLUGPREFS_Create,

	/* You should dispose of the prefsobject during this method.
	   You don't NEED to dispose, ie. you can let your object linger around
	   if you wish, but it's better to dispose it though. If you don't dispose
	   your stuff, then don't forget to get rid of it during the _Cleanup
	   method!
	*/
	AMIPLUGPREFS_Dispose,

	/* Take settings from prefsobject and make those the new current
	   settings. This method is performed when the user wants to let his/her
	   changes take effect.
	*/
	AMIPLUGPREFS_Use,

	/* Load preferences, and make those the new current settings.
	   Beware: Do NOT use the prefs object for this! Don't update or check
	   its status, just LOAD the prefs and let THOSE prefs take effect.
	   This method is (also) performed after AMIPLUGPREFS_Setup method!
	*/
	AMIPLUGPREFS_Load,

	/* Save preferences. You should save the current settings during
	   this method. Again, don't use the settings in the prefs object,
	   just save the currently used settings.
	*/
	AMIPLUGPREFS_Save,
};


/* ModeFlags for AMIPLUGCHANATTR_Modes */

#define CMF_T   1	/* Topic protection */
#define CMF_N   2	/* No messaging */
#define CMF_S   4	/* Secret */
#define CMF_I   8	/* Invite-Only */
#define CMF_P  16	/* Private */
#define CMF_M  32	/* Moderated */
#define CMF_L  64	/* user Limit */
#define CMF_K 128	/* Keyword protected */


/*
** Color codes for text output functions.
** These correspond to user defined settings.
*/
enum AMIRC_TEXTCOLS {
	amirc_tc_normal = 0,
	amirc_tc_highlite,
	amirc_tc_private,
	amirc_tc_server,
	amirc_tc_mode,
	amirc_tc_dcc,
	amirc_tc_local
};

/*
** Alias and Ignore lists are build of these nodes
** If you add your own, use amiplug_malloc() to
** obtain the memory, extending "string" to fit
** the string length, including (!) the zero terminator.
*/
struct amirc_stringnode {
	struct MinNode n;
	//char string[ 0 ];
	char *string;
};

/*
** Structure used for Ignorelist. This may
** be subject for change in later versions.
** If you add your own, use amiplug_malloc() to
** obtain the memory, extending "string" to fit
** the string length, including (!) the zero terminator.
*/
struct amirc_ignorenode {
	struct MinNode n;
	ULONG what;
	ULONG count;
	ULONG transient;
	//char userhost[ 0 ];
	char *userhost;
};
#define IGNF_TEXT 1
#define IGNF_PRIV 2
#define IGNF_CTCP 4

/*
** amiplug_getserver() returns a mcc_listtree object.
** tn_user of the treenodes of this object point to
** the following structure:
*/

struct amirc_server {
	char name[ 128 ];		// Internet hostname
	char comment[ 76 ];		// Some info
	char password[ 32 ];	// Optional password
	ULONG port;				// Port to connect to
	char autojoin[ 128 ];	// Channels to join
};	

/*
** DCC states
*/

/* Receive */
#define DCCRS_WAITING 0
#define DCCRS_CONNECTING 1
#define DCCRS_RECEIVING 2
#define DCCRS_COMPLETE 3
#define DCCRS_FAILED 4
#define DCCRS_NOCONNECT 5
#define DCCRS_ABORTED 6
#define DCCRS_NOSOCK 7
#define DCCRS_NOHOST 8
#define DCCRS_NOFILE 9
#define DCCRS_WRITEERROR 10

/* Chat */
#define DCCCS_WAITING 0
#define DCCCS_CONNECTING 1
#define DCCCS_CONNECTED 2
#define DCCCS_WAITINGFOR 3
#define DCCCS_CLOSED 4
#define DCCCS_NOCONNECT 5
#define DCCCS_NOSOCK 6
#define DCCCS_ABORTED 7

/* Send */
#define DCCSS_WAITINGFOR 0
#define DCCSS_ABORTED 1
#define DCCSS_SENDING 2
#define DCCSS_COMPLETE 3
#define DCCSS_FAILED 4
#define DCCSS_NOSOCK 5
#define DCCSS_NOFILE 6
#define DCCSS_READERROR 7
#define DCCSS_CALCCHECK 8


/*
** All hook functions also receives a pointer to a table of
** functions which the plugin can call to have AmIRC perform
** certain operations. *NOTE* You *MUST NOT* store this
** function pointer globally, as it will be a different
** table for every new opener of the plugin library.
** If you use SAS/C, you can use libinitr.o to have
** the compiler create a new data segment for every
** opener; this also allows you to use global
** variables in the code easily.
**
** Note that all functions have to follow _register
** calling conventions.
**
** The table also contains (hidden) context information,
** so it's necessary to pass it back to most of the functions
** you can call. Confusing? Nah :)
**
*/

struct amiplug_functable {
		int amiplug_functabversion;

		/* Send raw data to the server */
		int (PLFUNC * amiplug_sendraw)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,APTR data),
				REG(d0,ULONG len)
		 );

		/* find the user@host for a given nick */
		char * (PLFUNC *amiplug_findnickuserhost)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick)
		);

		/* like findnickuserhost, but will print a warning message
		if the userhost can't be found */
		char * (PLFUNC *amiplug_findnickuserhost_warn)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick)
		);

		/* The following functions are for outputting text to
		   any of the AmIRC windows */
		/* Default window */
		void (PLFUNC *amiplug_out_defwin)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,int color),
				REG(a1,STRPTR prefix),
				REG(a2,STRPTR txt)
		);

		/* Output to a query window, if it exists, otherwise def window */
		void (PLFUNC *amiplug_out_userwin)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,int color),
				REG(a1,STRPTR prefix),
				REG(a2,STRPTR txt),
				REG(a3,STRPTR nick)
		);

		/* Output to a channel window, if it exists, otherwise def window */
		void (PLFUNC *amiplug_out_channelwin)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,int color),
				REG(a1,STRPTR prefix),
				REG(a2,STRPTR txt),
				REG(a3,STRPTR channel)
		);

		/* Get a pointer to the MUI userlist object of a certain channel */
		APTR (PLFUNC *amiplug_getuserlv)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR channel)
		);

		/* Get some information from various AmIRC states. See
		   AMIPLUGATTR_ definitions above */
		int (PLFUNC *amiplug_getattr)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,ULONG attrid),
				REG(a1,APTR storage)
		);

		/* Execute "line" as if the user entered it in the default
		   window. Similiar to AmIRC's "SAY" Rexx command. Returns FALSE
		   if the line couldn't be executed for some reason, TRUE otherwise */
		int (PLFUNC *amiplug_say)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR line)
		);

		/* Play either a vanilla sound file or a event sound (if soundfile is NULL) */
		int (PLFUNC *amiplug_playsound)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR soundfile),
				REG(d0,ULONG event)
		);

		/* Add a DCC send request. Returns ID. method is 0 for DCC SEND,
		   1 for DCC MOVE. delay is in ticks, before the DCC offer is
		   send to the nick
		*/
		ULONG (PLFUNC *amiplug_adddcc_send)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick),
				REG(a2,STRPTR filename),
				REG(d0,ULONG method),
				REG(d1,ULONG delay)
		);

		/* Add a DCC receive request. ip and port show where
		   to connect to, "ismove" specifies whether this
		   is a DCC MOVE instead of a DCC SEND, "fileexists"
		   specifies the amount of "filename" already
		   there (locally), "isturbo" flags whether this is
		   a (PIRCH) TSEND, DCC SEND without ACKs. Returns
		   the DCC ID.
		*/
		ULONG (PLFUNC *amiplug_adddcc_recv)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick),
				REG(a2,STRPTR filename),
				REG(d0,ULONG len),
				REG(d1,ULONG ip),
				REG(d2,ULONG port),
				REG(d3,ULONG ismove),
				REG(d4,ULONG fileexists),
				REG(d5,ULONG isturbo)
		);

		/* Add a DCC CHAT request to "nick". */
		ULONG (PLFUNC *amiplug_adddcc_chat)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick),
				REG(d0,ULONG secure)
		);

		/* Get some information about a DCC connection. Returns
		   FALSE if the DCCID could not be found, or the attribute
		   was invalid. See AMIPLUGDCCATTR_ definitions above */
		int (PLFUNC *amiplug_dccgetattr)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,ULONG dccid),
				REG(d1,ULONG attrid),
				REG(a1,APTR storage)
		);

		/* Abort DCC connection */
		int (PLFUNC *amiplug_dccabort)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,ULONG dccid)
		);

		APTR userdata; /* Do with that whatever you want */

		/* Allocate some memory from AmIRC internal
		   pools. Used to manipulate Alias/Hilite/Ignorelists */
		APTR (PLFUNC *amiplug_malloc)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,ULONG size)
		);

		/* Get a string from the internal catalog */
		STRPTR (PLFUNC *amiplug_getstring)(
				REG(a0,struct amiplug_functable *ctx),
				REG(d0,ULONG stringid)
		);

		/* Queue a line into AmIRC's throttled output queue */
		void (PLFUNC *amiplug_queueline)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR line)
		);

		/* Return pointer to the server list Listtree object */
		APTR (PLFUNC *amiplug_getserver)(
				void
		);

		/* Return pointer to AmIRC exe name (including path) */
		STRPTR (PLFUNC *amiplug_getexename)(
				void
		);

		/* V10 */

		/* Send off a rexx command to REXX. Doesn't wait for reply */
		void (PLFUNC *amiplug_sendrxmsg)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR cmd)
		);

		/* V11 */

		/* Get information about a channel. Returns FALSE if channel
		   could not be found or the attribute was invalid */
		int (PLFUNC *amiplug_getchanattr)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR channel),
				REG(d0,ULONG attrid),
				REG(a2,APTR storage)
		);

		/* V13 */

		/* Free memory allocated by amiplug_malloc() */
		void (PLFUNC *amiplug_free)(
				REG(a0,APTR what)
		);

		/* Send a line via a DCC chat connection. A newline
		   character ('\n') will be appended.
		   Note: be careful when using this function on a chat
		   connection with yourself :^) */
		int (PLFUNC *amiplug_senddccmsg)(
				REG(d0,ULONG dccid),
				REG(a0,STRPTR line)
		);

		/* Push a rawline into input stream. Line is copied to a
		   private buffer and treated as if it was received from
		   the server the next time AmIRC reads lines from the
		   server. The line MUST NOT end with or contain "\r\n".
		   Returns FALSE if the push failed. Warning: Know what
		   you're doing! Be sure not to mess up AmIRC's internal
		   processing with this function.
		*/
		int (PLFUNC *amiplug_fakerawline)(
				REG(a0,STRPTR line),
				REG(d0,LONGBITS flags)
		);

		/* V14 */

		/* Output to Info window */
		void (PLFUNC *amiplug_out_infowin)(
				REG(d0,int color),
				REG(a0,STRPTR prefix),
				REG(a1,STRPTR txt)
		);

		APTR context; /* ***PRIVATE!!! DO NOT TOUCH!!!*** */

		/* V16 */

		/* PPC plugins call it to get a PPC functab.
		   Must be called only once in the setup, before anything else.
		   Never use this from 68k code !
		*/
		void (*iamppc)(
				struct amiplug_functable *ctx
		);

		/* V17 */

		/* Add a DCC send request. Returns ID. method is 0 for DCC SEND,
		   1 for DCC MOVE. delay is in ticks, before the DCC offer is
		   send to the nick. dccflags can be used to indicate turbo
		   or passive mode (SEND only).
		*/

		ULONG (PLFUNC *amiplug_new_adddcc_send)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick),
				REG(a2,STRPTR filename),
				REG(d0,ULONG method),
				REG(d1,ULONG delay),
				REG(d2,ULONG dccflags)
		);

		/* Add a DCC receive request. "sin" show where to
		   connect to (sin_family, sin_port and sin_addr must
		   be set), "ismove" specifies whether this is a DCC
		   MOVE instead of a DCC SEND, "fileexists" specifies
		   the amount of "filename" already there (locally),
		   "dccflags" can be used to indicate turbo or passive
		   mode. DCCF_TURBO means this is a (PIRCH) TSEND ,DCC
		   SEND without ACKs. DCCF_PASSIVE means this is a
		   passive DCC. If set, dcctoken must be a unique DCC
		   token to use. "len" and "fileexists" are pointers
		   to 64bit integers (upper 32 bits first, then lower
		   32 bits). Returns the DCC ID.
		*/
		ULONG (PLFUNC *amiplug_new_adddcc_recv)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick),
				REG(a2,STRPTR filename),
				REG(a3,ULONG *len),
				REG(a4,struct sockaddr_in *sin),
				REG(d0,ULONG ismove),
				REG(a5,ULONG *fileexists),
				REG(d1,ULONG dccflags),
				REG(d2,ULONG dcctoken)
		);

		/* Add a DCC CHAT request to "nick". In dccflags
		   DCCF_TURBO has no meaning. */
		ULONG (PLFUNC *amiplug_new_adddcc_chat)(
				REG(a0,struct amiplug_functable *ctx),
				REG(a1,STRPTR nick),
				REG(d0,ULONG secure),
				REG(d1,ULONG dccflags)
		);
};

#define AMIPLUG_FUNCTAB_VERSION 17


/*
** This macro MUST be the used at the top of the AMIPLUG_Setup function!
** NEVER remove it or the PPC plugin will crash 68k AmIRC!
** PPC plugins need AmIRC PPC's functab
*/

#ifdef __MORPHOS__
#define CheckAPIVersion(ctx) \
{ \
	if( (ctx->amiplug_functabversion < 15) || !ctx->iamppc )\
	{ \
		return NULL; \
	} \
	else \
	{ \
		ctx->iamppc(ctx); \
	} \
}
#else
#define CheckAPIVersion(ctx)
#endif


/*
** Flags for amiplug_fakerawline()
** (default action is to queue the line after the other queued fakes)
*/

#define FRLF_PUSH	1	// Line is inserted BEFORE the other fakes


/*
** Library defintions
*/

#ifndef NO_PROTOS
struct TagItem *AMIPLUG_Setup   ( REG(a0,struct amiplug_functable *ctx) );
void    AMIPLUG_Cleanup         ( REG(a0,struct amiplug_functable *ctx) );
int     AMIPLUG_Hook_Rawline    ( REG(a0,struct amiplug_functable *ctx), REG(a1,STRPTR line), REG(d0,ULONG len) );
int     AMIPLUG_Hook_NumericMsg ( REG(a0,struct amiplug_functable *ctx), REG(a1,STRPTR line), REG(d0,ULONG numericcode), REG(a2,STRPTR prefix), REG(a3,STRPTR dest), REG(d1,ULONG numargs), REG(a4,STRPTR *args) );
void    AMIPLUG_DoCommand       ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,struct amiplug_cmdparm *parmpacket) );
void    AMIPLUG_DoMenu          ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,APTR muiapp) );
void    AMIPLUG_Hook_DCC        ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG dccid) );
void    AMIPLUG_DoCTCP          ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,STRPTR from), REG(a2,STRPTR rest), REG(a3,STRPTR to) );
void    AMIPLUG_FinalSetup      ( REG(a0,struct amiplug_functable *ctx) );
char    *AMIPLUG_Hook_Input     ( REG(a0,struct amiplug_functable *ctx), REG(a1,STRPTR string), REG(d0,ULONG len) );
int     AMIPLUG_DoRexx          ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG commandid), REG(a1,STRPTR *args), REG(a2,APTR muiapp) );
void    AMIPLUG_Hook_Prefs      ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG methodid), REG(a1,struct amiplug_prefs *prefs) );
int     AMIPLUG_Hook_DCCChat    ( REG(a0,struct amiplug_functable *ctx), REG(d0,ULONG dccid), REG(a1,STRPTR nick), REG(a2,STRPTR line), REG(d1,ULONG len) );
#endif /* NO_PROTOS */

#endif /* AMIRC_PLUGIN_H */

