/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



bool	check_social	args( ( CHAR_DATA *ch, char *command,
			    char *argument ) );



/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2



/*
 * Log-all switch.
 */
bool				fLogAll		= FALSE;



/*
 * Command table.
 */
const	struct	cmd_type	cmd_table	[] =
{
    /*
     * Common movement commands.
     */
    { "north",		do_north,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NORMAL },
    { "who",		do_who,		POS_DEAD,	 0,  LOG_NORMAL	}, /* most command command :P */
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NORMAL },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NORMAL },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "buy",		do_buy,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "look",		do_look,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "order",		do_order,	POS_RESTING,	 0,  LOG_ALWAYS	},
    { "rest",		do_rest,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "tell",		do_tell,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "users",		do_users,	POS_DEAD,        0,  LOG_NORMAL	},
    { "wield",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "wizhelp",	do_wizhelp,	POS_DEAD,      900,  LOG_NORMAL	},

    /*
     * Informational commands.
     */
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "finger",		do_finger,	POS_DEAD,	 0,  LOG_NORMAL },
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "report",		do_report,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "scan",		do_scan,	POS_STANDING,	 0,  LOG_NORMAL },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "track",		do_track,	POS_STANDING,	 0,  LOG_NORMAL },
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL	},

    /*
     * Configuration commands.
     */
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "password",	do_password,	POS_DEAD,	 0,  LOG_NEVER	},
    { "title",		do_title,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL	},

    /*
     * Communication commands.
     */
    { "answer",		do_answer,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "auction",	do_auction,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "chat",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { ".",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { ",",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "gtell",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { ";",		do_gtell,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "music",		do_music,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "note",		do_note,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "pose",		do_pose,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "question",	do_question,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "reply",		do_reply,	POS_RESTING,	 0,  LOG_NORMAL },
    { "say",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "'",		do_say,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "shout",		do_shout,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "yell",		do_yell,	POS_RESTING,	 0,  LOG_NORMAL	},

    /*
     * Object manipulation commands.
     */
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "hold",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "list",		do_list,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "sell",		do_sell,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL	},
    { "sacrifice",	do_sacrifice,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "value",		do_value,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL	},

    /*
     * Combat commands.
     */
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "bash",		do_bash,	POS_FIGHTING,    0,  LOG_NORMAL },
    { "charge",		do_charge,	POS_STANDING,    0,  LOG_NORMAL },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "flee",		do_flee,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "palm",		do_palm,	POS_FIGHTING,    0,  LOG_NORMAL },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "shoot",		do_shoot,	POS_STANDING,    0,  LOG_NORMAL },

    /*
     * Miscellaneous commands.
     */
    { "color",		do_color,	POS_DEAD,	 0,  LOG_NORMAL },
    { "consent",	do_consent,	POS_DEAD,	 0,  LOG_NORMAL },
    { "dismount",	do_dismount,	POS_STANDING,    0,  LOG_NORMAL },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "ld",		do_linkdead,	POS_DEAD,	 0,  LOG_NORMAL },
    { "linkdead",	do_linkdead,	POS_DEAD,	 0,  LOG_NORMAL },
    { "mount",		do_mount,	POS_STANDING,    0,  LOG_NORMAL },
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL	},
    { "quit",		do_quit,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "recall",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "/",		do_recall,	POS_FIGHTING,	 0,  LOG_NORMAL	},
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "save",		do_save,	POS_DEAD,	 0,  LOG_NORMAL	},
    { "skills",		do_skills,	POS_DEAD,	 0,  LOG_NORMAL },
    { "practice",	do_skills,	POS_DEAD,	 0,  LOG_NORMAL },
    { "ride",		do_mount,	POS_STANDING,	 0,  LOG_NORMAL },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL	},
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL	},
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL	},
    { "where",		do_where,	POS_RESTING,	 0,  LOG_NORMAL	},

    /* War Commands */
    { "cview",		do_cview,	POS_DEAD,	 0,  LOG_NORMAL },
    { "changeclass",	do_changeclass,	POS_STANDING,	 0,  LOG_NORMAL },
    { "flags",		do_flags,	POS_DEAD,	 0,  LOG_NORMAL },
    { "joinwar",	do_join,	POS_DEAD,	 0,  LOG_NORMAL },
    { "noplay",		do_noplay,	POS_DEAD,	 0,  LOG_NORMAL },
    { "random",		do_random,	POS_DEAD,	 0,  LOG_NORMAL },
    { "survey",		do_survey,	POS_DEAD,	 0,  LOG_NORMAL },
    { "vote",		do_vote,	POS_DEAD,	 0,  LOG_NORMAL },


    /*
     * Immortal commands.
     */
    { "advance",	do_advance,	POS_DEAD,     1000,  LOG_ALWAYS	},
    { "trust",		do_trust,	POS_DEAD,     1000,  LOG_ALWAYS },

    { "material",	do_material,	POS_DEAD,      990,  LOG_NORMAL },
    { "mset",		do_mset,	POS_DEAD,      990,  LOG_ALWAYS	},
    { "reboo",		do_reboo,	POS_DEAD,      990,  LOG_NORMAL	},
    { "reboot",		do_reboot,	POS_DEAD,      990,  LOG_ALWAYS	},
    { "shutdow",	do_shutdow,	POS_DEAD,      990,  LOG_NORMAL	},
    { "shutdown",	do_shutdown,	POS_DEAD,      990,  LOG_ALWAYS	},
    { "sic",		do_sic,		POS_DEAD,      990,  LOG_ALWAYS },
    { "specs",		do_specs,	POS_DEAD,      990,  LOG_ALWAYS },
    { "wizlock",	do_wizlock,	POS_DEAD,      990,  LOG_ALWAYS	},
    { "world",		do_world,	POS_DEAD,      990,  LOG_ALWAYS },
    { "wset",		do_wset,	POS_DEAD,      990,  LOG_ALWAYS },
   
    { "allow",		do_allow,	POS_DEAD,      980,  LOG_ALWAYS	},
    { "ban",		do_ban,		POS_DEAD,      980,  LOG_ALWAYS	},
    { "cset",		do_cset,	POS_DEAD,      980,  LOG_ALWAYS },
    { "deny",		do_deny,	POS_DEAD,      980,  LOG_ALWAYS	},
    { "disconnect",	do_disconnect,	POS_DEAD,      980,  LOG_ALWAYS	},
    { "freeze",		do_freeze,	POS_DEAD,      980,  LOG_ALWAYS	},
    { "force",		do_force,	POS_DEAD,      980,  LOG_ALWAYS	},
    { "mload",		do_mload,	POS_DEAD,      980,  LOG_ALWAYS	},
    { "randeq",		do_randeq,	POS_DEAD,      980,  LOG_ALWAYS },

    { "noemote",	do_noemote,	POS_DEAD,      970,  LOG_NORMAL	},
    { "notell",		do_notell,	POS_DEAD,      970,  LOG_NORMAL	},
    { "oload",		do_oload,	POS_DEAD,      970,  LOG_ALWAYS	},
    { "oset",		do_oset,	POS_DEAD,      970,  LOG_ALWAYS	},
    { "purge",		do_purge,	POS_DEAD,      970,  LOG_NORMAL	},
    { "restore",	do_restore,	POS_DEAD,      970,  LOG_ALWAYS	},
    { "rset",		do_rset,	POS_DEAD,      970,  LOG_ALWAYS	},
    { "silence",	do_silence,	POS_DEAD,      970,  LOG_NORMAL },
    { "sla",		do_sla,		POS_DEAD,      970,  LOG_NORMAL	},
    { "slay",		do_slay,	POS_DEAD,      970,  LOG_ALWAYS	},
    { "transfer",	do_transfer,	POS_DEAD,      970,  LOG_ALWAYS	},

    { "at",		do_at,		POS_DEAD,      960,  LOG_NORMAL	},
    { "bamfin",		do_bamfin,	POS_DEAD,      960,  LOG_NORMAL	},
    { "bamfout",	do_bamfout,	POS_DEAD,      960,  LOG_NORMAL	},
    { "echo",		do_echo,	POS_DEAD,      960,  LOG_ALWAYS	},
    { "goto",		do_goto,	POS_DEAD,      960,  LOG_NORMAL	},
    { "gossip",		do_chat,	POS_SLEEPING,	 0,  LOG_NORMAL },
    { "holylight",	do_holylight,	POS_DEAD,      960,  LOG_NORMAL	},
    { "invis",		do_invis,	POS_DEAD,      960,  LOG_NORMAL	},
    { "log",		do_log,		POS_DEAD,      960,  LOG_ALWAYS	},
    { "memory",		do_memory,	POS_DEAD,      960,  LOG_NORMAL	},
    { "mfind",		do_mfind,	POS_DEAD,      960,  LOG_NORMAL },
    { "mstat",		do_mstat,	POS_DEAD,      960,  LOG_NORMAL	},
    { "mwhere",		do_mwhere,	POS_DEAD,      960,  LOG_NORMAL },
    { "ofind",		do_ofind,	POS_DEAD,      960,  LOG_NORMAL },
    { "ostat",		do_ostat,	POS_DEAD,      960,  LOG_NORMAL	},
    { "peace",		do_peace,	POS_DEAD,      960,  LOG_NORMAL	},
    { "recho",		do_recho,	POS_DEAD,      960,  LOG_ALWAYS	},
    { "return",		do_return,	POS_DEAD,      960,  LOG_NORMAL	},
    { "rstat",		do_rstat,	POS_DEAD,      960,  LOG_NORMAL	},
    { "slookup",	do_slookup,	POS_DEAD,      960,  LOG_NORMAL },
    { "snoop",		do_snoop,	POS_DEAD,      960,  LOG_NORMAL	},
    { "switch",		do_switch,	POS_DEAD,      960,  LOG_ALWAYS	},

    { "immtalk",	do_immtalk,	POS_DEAD,      900,  LOG_NORMAL	},
    { ":",		do_immtalk,	POS_DEAD,      900,  LOG_NORMAL	},
    { "sset",		do_sset,	POS_DEAD,      900,  LOG_NORMAL },

    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL	}
};



/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * No hiding.
     */
    REMOVE_BIT( ch->affected_by, AFF_HIDE );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace(*argument) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	&&   !str_prefix( command, cmd_table[cmd].name )
	&&   cmd_table[cmd].level <= trust )
	{
	    found = TRUE;
	    break;
	}
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	    send_to_char( "No.\n\r", ch );
	return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are DEAD.\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r", ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r", ch );
	    break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r", ch);
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r", ch);
	    break;

	}
	return;
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if ( *arg == '\0' )
	return FALSE;

    for ( ; *arg != '\0'; arg++ )
    {
	if ( !isdigit(*arg) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}
