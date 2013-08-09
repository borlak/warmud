
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

#include "os.h"


/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		void fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN       fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN     fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN    fun
#endif



/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int				sh_int;
typedef int				bool;
#define unix
#else
typedef short    int			sh_int;
typedef unsigned char			bool;
#endif



/*
 * Structure types.
 */
typedef struct	affect_data		AFFECT_DATA;
typedef struct	area_data		AREA_DATA;
typedef struct	ban_data		BAN_DATA;
typedef struct	char_data		CHAR_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data		EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct	help_data		HELP_DATA;
typedef struct	kill_data		KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data		NOTE_DATA;
typedef struct	obj_data		OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data			PC_DATA;
typedef struct	reset_data		RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data		SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct	spec_obj_data		SPEC_OBJ_DATA;

/*
 * Function types.
 */
typedef	void DO_FUN		args( ( CHAR_DATA *ch, char *argument ) );
typedef int  SPEC_FUN		args( ( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update ));
typedef void SPELL_FUN		args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );


/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 1024
#define MAX_STRING_LENGTH	 4096
#define MAX_INPUT_LENGTH	  160



/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_SKILL		   101
#define MAX_CLASS		    4
#define MAX_LEVEL		   1000
#define LEVEL_HERO		   (MAX_LEVEL - 100)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 90)

#define PULSE_PER_SECOND	    4
#define PULSE_VIOLENCE		  ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (30 * PULSE_PER_SECOND)
#define PULSE_AREA		  (60 * PULSE_PER_SECOND)


/*****************War Flags*****************************/

/* Local Defines */
#define WEQUIP_FAIR	0
#define WEQUIP_NORM	1
#define WEQUIP_GOOD	2
/* War Flags */


#define WFLAG_EQ_SCATTERED	0x00000004
#define WFLAG_RANDOM_EQ		0x00000008
#define WFLAG_LOADALL		0x00000010
#define WFLAG_NO_PKING		0x00000020
#define WFLAG_NO_NUKE		0x00000040
#define WFLAG_BLOODS		0x00000080
#define WFLAG_SAME_CLASS	0x00000100
#define WFLAG_FOUNTAINS		0x00000200
#define WFLAG_HUNTERS		0x00000400
#define WFLAG_WRATH		0x00000800
#define WFLAG_NO_MOBS		0x00001000
#define WFLAG_RANDOM_CLASS	0x00002000

/* Wars */
#define MAX_WAR			10
#define WAR_DEFAULT		7 /* basic killing war */
#define WAR_PIT			0
#define WAR_CITY		1
#define WAR_ALL_VS_ONE		2
#define WAR_ARTIFACT		3
#define WAR_TEAM		4
#define WAR_CAPTURE_FLAG	5
#define WAR_BORG		6
#define WAR_FITTEST		7
#define WAR_HOT_SKULL		8
#define WAR_GOD			9

/***************************************************/




/*
 * Site ban structure.
 */
struct	ban_data
{
    BAN_DATA *	next;
    char *	name;
};



/*
 * Time and weather stuff.
 */
#define SUN_DARK		    0
#define SUN_RISE		    1
#define SUN_LIGHT		    2
#define SUN_SET			    3

#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3

struct	time_info_data
{
    int		hour;
    int		day;
    int		month;
    int		year;
};

struct	weather_data
{
    int		mmhg;
    int		change;
    int		sky;
    int		sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0
#define CON_INTRO			 1
#define CON_GET_NAME			 2
#define CON_GET_OLD_PASSWORD		 3
#define CON_CONFIRM_NEW_NAME		 4
#define CON_GET_NEW_PASSWORD		 5
#define CON_CONFIRM_NEW_PASSWORD	 6
#define CON_GET_NEW_SEX			 7
#define CON_READ_MOTD			 8


/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
    DESCRIPTOR_DATA *	next;
    DESCRIPTOR_DATA *	snoop_by;
    CHAR_DATA *		character;
    CHAR_DATA *		original;
    char *		host;
    unsigned int	descriptor;
    sh_int		connected;
    bool		fcommand;
    char		inbuf		[4 * MAX_INPUT_LENGTH];
    char		incomm		[MAX_INPUT_LENGTH];
    char		inlast		[MAX_INPUT_LENGTH];
    int			repeat;
    char *		outbuf;
    int			outsize;
    int			outtop;
};



/*
 * Attribute bonus structures.
 */
struct	str_app_type
{
    sh_int	tohit;
    sh_int	todam;
    sh_int	carry;
    sh_int	wield;
};

struct	int_app_type
{
    sh_int	learn;
};

struct	wis_app_type
{
    sh_int	practice;
};

struct	dex_app_type
{
    sh_int	defensive;
};

struct	con_app_type
{
    sh_int	hitp;
    sh_int	shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3



/*
 * Help table types.
 */
struct	help_data
{
    HELP_DATA *	next;
    sh_int	level;
    char *	keyword;
    char *	text;
};



/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct	shop_data
{
    SHOP_DATA *	next;			/* Next shop in list		*/
    sh_int	keeper;			/* Vnum of shop keeper mob	*/
    sh_int	buy_type [MAX_TRADE];	/* Item types shop will buy	*/
    sh_int	profit_buy;		/* Cost multiplier for buying	*/
    sh_int	profit_sell;		/* Cost multiplier for selling	*/
    sh_int	open_hour;		/* First opening hour		*/
    sh_int	close_hour;		/* First closing hour		*/
};


#define		SKILL_PRAC	0
#define		SKILL_BEAT	1
#define		SKILL_MANA	2
#define		CLASS_WAR	0
#define		CLASS_MAG	1
#define		CLASS_THI	2
#define		CLASS_CLE	3
#define		CLASS_SPE	4

/* Class Flags */
#define	CLS_NO_REGEN_HP		0x00000001
#define	CLS_NO_REGEN_MANA	0x00000002
#define	CLS_INFRAVISION		0x00000004
#define	CLS_SLOW		0x00000008
#define	CLS_ULTRAVISION		0x00000010
#define	CLS_NO_EQUIP		0x00000020
#define	CLS_NO_MAGIC		0x00000040
#define	CLS_DEADLY_BACKSTAB	0x00000080
#define	CLS_MARTIAL_ARTIST	0x00000100

/*
 * Per-class stuff.
 */
struct	class_type
{
	OBJ_INDEX_DATA	*obj_list[5];
	char		*name;
	char		*short_name;
	short		base;
	short		enabled;
	short		hp;
	short		mana;
	short		thac0;
	short		stat_str;
	short		stat_int;
	short		stat_wis;
	short		stat_dex;
	short		stat_con;
	int		flags;
	short		skills[MAX_SKILL][3];
};



/*
 * Data structure for notes.
 */
struct	note_data
{
    NOTE_DATA *	next;
    char *	sender;
    char *	date;
    char *	to_list;
    char *	subject;
    char *	text;
};



/*
 * An affect.
 */
struct	affect_data
{
    AFFECT_DATA *	next;
    sh_int		type;
    sh_int		duration;
    sh_int		location;
    sh_int		modifier;
    int			bitvector;
};



/*
 * A kill structure (indexed by level).
 */
struct	kill_data
{
    sh_int		number;
    sh_int		killed;
};



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD	3060
#define MOB_VNUM_BORG		2
#define MOB_VNUM_UNDEAD		3
#define MOB_VNUM_ELEMENTAL	4
#define MOB_VNUM_DRACOLICH	5

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		1		/* Auto set for mobs	*/
#define ACT_SENTINEL		2		/* Stays in one room	*/
#define ACT_SCAVENGER		4		/* Picks up objects	*/
#define ACT_AGGRESSIVE		32		/* Attacks PC's		*/
#define ACT_STAY_AREA		64		/* Won't leave area	*/
#define ACT_WIMPY		128		/* Flees when hurt	*/
#define ACT_PET			256		/* Auto set for pets	*/
#define ACT_TRAIN		512		/* Can train PC's	*/
#define ACT_SERVANT		1024
#define ACT_MOUNTABLE		2048

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		0x00000001
#define AFF_INVISIBLE		0x00000002
#define AFF_DETECT_EVIL		0x00000004
#define AFF_DETECT_INVIS	0x00000008
#define AFF_DETECT_MAGIC	0x00000010
#define AFF_DETECT_HIDDEN	0x00000020
#define AFF_HOLD		0x00000040	/* Unused	*/
#define AFF_SANCTUARY		0x00000080
#define AFF_FAERIE_FIRE		0x00000100
#define AFF_INFRARED		0x00000200
#define AFF_CURSE		0x00000400
#define AFF_FIRESHIELD		0x00000800
#define AFF_POISON		0x00001000
#define AFF_PROTECT		0x00002000
#define AFF_PARALYSIS		0x00004000	/* Unused	*/
#define AFF_SNEAK		0x00008000
#define AFF_HIDE		0x00010000
#define AFF_SLEEP		0x00020000
#define AFF_CHARM		0x00040000
#define AFF_FLYING		0x00080000
#define AFF_PASS_DOOR		0x00100000
#define AFF_BLACKMANTLE		0x00200000
#define AFF_SUMMONED		0x00400000



/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_BLOOD		     12

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22

#define OBJ_VNUM_BREAD			1103
#define OBJ_VNUM_WATER_CUP		3102
#define OBJ_VNUM_TORCH			3030
#define OBJ_VNUM_BAG			3032

#define OBJ_VNUM_ARTIFACT		13
#define OBJ_VNUM_FLAG			16
#define OBJ_VNUM_HOMEBASE		17
#define OBJ_VNUM_HOT_SKULL		18

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		1
#define ITEM_SCROLL		2
#define ITEM_WAND		3
#define ITEM_STAFF		4
#define ITEM_WEAPON		5
#define ITEM_TREASURE		8
#define ITEM_ARMOR		9
#define ITEM_POTION		10
#define ITEM_FURNITURE		12
#define ITEM_TRASH		13
#define ITEM_CONTAINER		15
#define ITEM_DRINK_CON		17
#define ITEM_KEY		18
#define ITEM_FOOD		19
#define ITEM_MONEY		20
#define ITEM_BOAT		22
#define ITEM_CORPSE_NPC		23
#define ITEM_CORPSE_PC		24
#define ITEM_FOUNTAIN		25
#define ITEM_PILL		26
#define ITEM_QUIVER		27


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		0x00000001 /* 1 */
#define ITEM_HUM		0x00000002 /* 2 */
#define ITEM_DARK		0x00000004 /* 4 */
#define ITEM_LOCK		0x00000008 /* 8 */
#define ITEM_EVIL		0x00000010 /* 16 */
#define ITEM_INVIS		0x00000020 /* 32 */
#define ITEM_MAGIC		0x00000040 /* 64 */
#define ITEM_NODROP		0x00000080 /* 128 */
#define ITEM_BLESS		0x00000100 /* 256 */
#define ITEM_ANTI_GOOD		0x00000200 /* 512 */
#define ITEM_ANTI_EVIL		0x00000400 /* 1024 */
#define ITEM_ANTI_NEUTRAL	0x00000800 /* 2048 */
#define ITEM_NOREMOVE		0x00001000 /* 4096 */
#define ITEM_INVENTORY		0x00002000 /* 8192 */
#define ITEM_MISSILE_WEAPON	0x00004000 /* 16392 */
#define ITEM_CHARGE_WEAPON	0x00008000 /* 32784 */
#define ITEM_STABBER		0x00010000 /* 65568 */

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		0x00000001 /* 1 */
#define ITEM_WEAR_FINGER	0x00000002 /* 2 */
#define ITEM_WEAR_NECK		0x00000004 /* 4 */
#define ITEM_WEAR_BODY		0x00000008 /* 8 */
#define ITEM_WEAR_HEAD		0x00000010 /* 16 */
#define ITEM_WEAR_LEGS		0x00000020 /* 32 */
#define ITEM_WEAR_FEET		0x00000040 /* 64 */
#define ITEM_WEAR_HANDS		0x00000080 /* 128 */
#define ITEM_WEAR_ARMS		0x00000100 /* 256 */
#define ITEM_WEAR_SHIELD	0x00000200 /* 512 */
#define ITEM_WEAR_ABOUT		0x00000400 /* 1024 */
#define ITEM_WEAR_WAIST		0x00000800 /* 2048 */
#define ITEM_WEAR_WRIST		0x00001000 /* 4096 */
#define ITEM_WIELD		0x00002000 /* 8192 */
#define ITEM_HOLD		0x00004000 /* 16392 */
#define ITEM_WEAR_QUIVER	0x00008000 /* 32784 */



/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13

#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24



/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO			2
#define ROOM_VNUM_CHAT			1200
#define ROOM_VNUM_TEMPLE		3001
#define ROOM_VNUM_ALTAR			3054
#define ROOM_VNUM_SCHOOL		3700
#define ROOM_VNUM_PIT			30404
#define ROOM_VNUM_SKULL			30410

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		1

#define ROOM_NO_MOB		4
#define ROOM_INDOORS		8
#define ROOM_PRIVATE		512
#define ROOM_SAFE		1024
#define ROOM_SOLITARY		2048
#define ROOM_PET_SHOP		4096
#define ROOM_NO_RECALL		8192



/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_PICKPROOF		     32



/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNUSED		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_QUIVER		     18
#define MAX_WEAR		     19



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK		      0
#define COND_FULL		      1
#define COND_THIRST		      2



/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_RESTING		      5
#define POS_FIGHTING		      6
#define POS_STANDING		      7



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		0x00000001 /*     1	Don't EVER set.	*/
#define PLR_BOUGHT_PET		0x00000002 /*     2	*/

#define PLR_STAY_AREA		0x00000008 /*     8	UNUSED */
#define PLR_AUTOLOOT		0x00000010 /*    16	UNUSED */
#define PLR_AUTOSAC             0x00000020 /*    32	UNUSED */
#define PLR_BLANK		0x00000040 /*    64	*/
#define PLR_BRIEF		0x00000080 /*   128	*/

#define PLR_COMBINE		0x00000200 /*   512	*/
#define PLR_PROMPT		0x00000400 /*  1024	*/
#define PLR_TELNET_GA		0x00000800 /*  2048	*/
#define PLR_HOLYLIGHT		0x00001000 /*  4096	*/
#define PLR_WIZINVIS		0x00002000 /*  8192	*/
#define PLR_CHARGING		0x00004000 /* 16392	*/
#define	PLR_SILENCE		0x00008000 /* 32768	*/
#define PLR_NO_EMOTE		0x00010000 /* 65536	*/

#define PLR_NO_TELL		0x00040000 /*262144	*/
#define PLR_LOG			0x00080000 /*524288	*/
#define PLR_DENY		0x00100000 /*1048576	*/
#define PLR_FREEZE		0x00200000 /*2097152	*/
#define PLR_IT			0x00400000 /*4194304	*/
#define PLR_RANDOM		0x00800000 /*8388608	*/
#define PLR_ANSI		0x01000000 /*16777216	*/
#define PLR_BORG		0x02000000 /*33554432	*/

/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION		      4	/* Obsolete	*/
#define PLR_CHAT		    256	/* Obsolete	*/
#define PLR_NO_SHOUT		 131072	/* Obsolete	*/
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_AUCTION			1
#define	CHANNEL_CHAT			2
#define	CHANNEL_HACKER			4
#define	CHANNEL_IMMTALK			8
#define	CHANNEL_MUSIC			16
#define	CHANNEL_QUESTION		32
#define	CHANNEL_SHOUT			64
#define	CHANNEL_YELL			128
#define CHANNEL_WARMESSAGE		256



/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA *	next;
    SPEC_OBJ_DATA *	spec_obj;
    SHOP_DATA *		pShop;
    char *		player_name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    sh_int		vnum;
    sh_int		count;
    sh_int		killed;
    sh_int		sex;
    sh_int		level;
    int			act;
    int			affected_by;
    sh_int		alignment;
    sh_int		hitroll;		/* Unused */
    sh_int		ac;			/* Unused */
    sh_int		hitnodice;		/* Unused */
    sh_int		hitsizedice;		/* Unused */
    sh_int		hitplus;		/* Unused */
    sh_int		damnodice;		/* Unused */
    sh_int		damsizedice;		/* Unused */
    sh_int		damplus;		/* Unused */
    int			gold;			/* Unused */
};



/*
 * One character (PC or NPC).
 */
struct char_data
{
    CHAR_DATA *		next;
    CHAR_DATA *		next_in_room;
    CHAR_DATA *		next_spec;
    CHAR_DATA *		master;
    CHAR_DATA *		leader;
    CHAR_DATA *		fighting;
    CHAR_DATA *		reply;
    CHAR_DATA *		hunting;
    CHAR_DATA *		mounted_by;
    SPEC_OBJ_DATA *	spec_obj;
    MOB_INDEX_DATA *	pIndexData;
    DESCRIPTOR_DATA *	desc;
    AFFECT_DATA *	affected;
    NOTE_DATA *		pnote;
    OBJ_DATA *		carrying;
    ROOM_INDEX_DATA *	in_room;
    ROOM_INDEX_DATA *	was_in_room;
    PC_DATA *		pcdata;
    char *		name;
    char *		short_descr;
    char *		long_descr;
    char *		description;
    sh_int		sex;
    sh_int		class;
    sh_int		race;
    sh_int		level;
    sh_int		trust;
    int			played;
    time_t		logon;
    time_t		save_time;
    sh_int		timer;
    sh_int		wait;
    sh_int		hit;
    sh_int		max_hit;
    sh_int		mana;
    sh_int		max_mana;
    int			gold;
    int			exp;
    int			act;
    int			affected_by;
    sh_int		position;
    sh_int		carry_weight;
    sh_int		carry_number;
    sh_int		saving_throw;
    sh_int		alignment;
    sh_int		hitroll;
    sh_int		damroll;
    sh_int		armor;
    sh_int		wimpy;
    sh_int		deaf;
};



/*
 * Data which only PC's have.
 */
struct pc_data
{
    PC_DATA *		next;
    char *		pwd;
    char *		bamfin;
    char *		bamfout;
    char *		title;
    char *		who_name;
    sh_int		perm_str;
    sh_int		perm_int;
    sh_int		perm_wis;
    sh_int		perm_dex;
    sh_int		perm_con;
    sh_int		mod_str;
    sh_int		mod_int;
    sh_int		mod_wis;
    sh_int		mod_dex;
    sh_int		mod_con;
    sh_int		condition	[3];
    sh_int		modifying;

    /* War Variables */
    CHAR_DATA *		consent;
    time_t		last_war;
    bool		recall;
    bool		random;
    bool		surveying;
    sh_int		timer;
    sh_int		wars_won	[MAX_WAR];
    sh_int		wars_lost	[MAX_WAR];
    sh_int		noplay;
    sh_int		in_war;
    sh_int		kills_this_war;
    sh_int		doomsdays;
    sh_int		kills;
    sh_int		deaths;
    sh_int		mob_deaths;
    sh_int		wins;
    sh_int		vote;
    sh_int		team;
    sh_int		hot_skulls;
    sh_int		it_kills;
    sh_int		borg_kills;
    int			dam_received;
    int			dam_inflicted;
};



/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		16

struct	liq_type
{
    char *	liq_name;
    char *	liq_color;
    sh_int	liq_affect[3];
    sh_int	heal_hp;
    sh_int	heal_mana;
    sh_int	percent_deduction;
};


#define ARROW_MAX	10

struct arrow_type
{
	char *		name;
	sh_int *	spell;
};



/*
 * Extra description data for a room or object.
 */
struct	extra_descr_data
{
    EXTRA_DESCR_DATA *next;	/* Next in list                     */
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA *	next;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    SPEC_OBJ_DATA *	spec_obj;
    char *		name;
    char *		short_descr;
    char *		description;
    sh_int		vnum;
    sh_int		item_type;
    int			extra_flags;
    int			wear_flags;
    sh_int		count;
    sh_int		weight;
    int			cost;			/* Unused */
    int			value	[4];
};



/*
 * One object.
 */
struct obj_data
{
    OBJ_DATA *		next;
    OBJ_DATA *		next_spec;
    OBJ_DATA *		next_content;
    OBJ_DATA *		contains;
    OBJ_DATA *		in_obj;
    CHAR_DATA *		carried_by;
    EXTRA_DESCR_DATA *	extra_descr;
    AFFECT_DATA *	affected;
    OBJ_INDEX_DATA *	pIndexData;
    ROOM_INDEX_DATA *	in_room;
    SPEC_OBJ_DATA *	spec_obj;
    char *		name;
    char *		short_descr;
    char *		description;
    sh_int		item_type;
    int			extra_flags;
    int			wear_flags;
    sh_int		wear_loc;
    sh_int		weight;
    int			cost;
    sh_int		level;
    sh_int		timer;
    int			value	[4];
};



/*
 * Exit data.
 */
struct	exit_data
{
    ROOM_INDEX_DATA *	to_room;
    sh_int		vnum;
    sh_int		exit_info;
    sh_int		key;
    char *		keyword;
    char *		description;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA *	next;
    char		command;
    sh_int		arg1;
    sh_int		arg2;
    sh_int		arg3;
};



/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA *		next;
    RESET_DATA *	reset_first;
    RESET_DATA *	reset_last;
    char *		name;
    sh_int		age;
    sh_int		nplayer;
};



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA *	next;
    ROOM_INDEX_DATA *	next_spec;
    SPEC_OBJ_DATA *	spec_obj;
    CHAR_DATA *		people;
    OBJ_DATA *		contents;
    EXTRA_DESCR_DATA *	extra_descr;
    AREA_DATA *		area;
    EXIT_DATA *		exit	[6];
    char *		name;
    char *		description;
    sh_int		vnum;
    sh_int		room_flags;
    sh_int		light;
    sh_int		sector_type;
};


/*
 * Types of damage.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 * heh, yeah beavis, whatever, they are just damage types.
 */
#define TYPE_NONE		0
#define TYPE_FIRE		1
#define TYPE_COLD		2
#define TYPE_ELECTRICITY	3
#define TYPE_ACID		4
#define TYPE_MYSTICAL		5
#define TYPE_SPELL		6
#define TYPE_BLUNT		7
#define TYPE_SLASH		8
#define TYPE_PIERCE		9
#define TYPE_UNDEFINED		999
#define TYPE_HIT		1000

/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4
#define TAR_OFFENSIVE_IGNORE	    5



/*
 * Skills include spells as a particular case.
 */
struct	skill_type
{
    char *	name;			/* Name of skill		*/
    sh_int	dam_type;		/* Type of damage		*/
    SPELL_FUN *	spell_fun;		/* Spell pointer (for spells)	*/
    sh_int	target;			/* Legal targets		*/
    sh_int	minimum_position;	/* Position for caster / user	*/
    sh_int *	pgsn;			/* Pointer to associated gsn	*/
    sh_int	slot;			/* Slot for #OBJECT loading	*/
    char *	spell_color;		/* Damage message		*/
    char *	msg_off;		/* Wear off message		*/
};

/********This is all the special function defines and structures*********
 * Some defines are for rooms, some for objs, mobs, etc. but i dont want*
 * To keep defining new ones for each kind of structure, so in the spec *
 * code use a simple switch() to do what you want with the updates      *
 ************************************************************************/

#define UPDATE_ALL		0
#define UPDATE_ROOM		1
#define UPDATE_OBJ		2
#define UPDATE_MOB		3
#define UPDATE_END		4
#define UPDATE_TIMER		5
#define UPDATE_FIGHTING		6
#define UPDATE_DAMAGE		7
#define UPDATE_ENTERED		8
#define UPDATE_EXITED		9
#define UPDATE_GIVE		10
#define UPDATE_SAY		11
#define UPDATE_LOOK		12

struct spec_obj_data
{
	SPEC_OBJ_DATA *		next;
	ROOM_INDEX_DATA *	room;
	CHAR_DATA *		ch;
	CHAR_DATA *		vch;
	OBJ_DATA *		obj;
	SPEC_FUN *		spec_fun;
	int			value[4];
	int			timer;
};



/**************************************************************************
 * Player statistics, will be many different lists of stats for different *
 * purposes.                                                              *
 * lists:  top pkiller, determine with wins and kills and deaths          *
 *         top war winner of each type of war				  *
 *         most damage done ever and for reboot                           *
 *         
 **************************************************************************

struct stat_list
{
	STAT_DATA	*top_pker[10];
	STAT_DATA	*top_war_winner[MAX_WAR];
	STAT_DATA	*top_damage[2];


struct stat_data
{
	STAT_DATA	*next;
	char *		name;		
	int		value[2];
}; */


/*
 * These are skill_lookup return values for common skills and spells.
 */
extern  sh_int	gsn_awareness;
extern	sh_int	gsn_backstab;
extern  sh_int  gsn_bash;
extern	sh_int	gsn_dodge;
extern	sh_int	gsn_hide;
extern	sh_int	gsn_peek;
extern	sh_int	gsn_pick_lock;
extern  sh_int	gsn_ride;
extern	sh_int	gsn_sneak;
extern	sh_int	gsn_steal;
extern  sh_int	gsn_track;
extern  sh_int	gsn_quivering_palm;

extern  sh_int  gsn_archery;
extern  sh_int	gsn_charge;
extern	sh_int	gsn_disarm;
extern	sh_int	gsn_enhanced_damage;
extern	sh_int	gsn_kick;
extern	sh_int	gsn_parry;
extern	sh_int	gsn_rescue;
extern	sh_int	gsn_second_attack;
extern	sh_int	gsn_third_attack;

extern	sh_int	gsn_blindness;
extern	sh_int	gsn_charm_person;
extern	sh_int	gsn_curse;
extern	sh_int	gsn_invis;
extern	sh_int	gsn_mass_invis;
extern	sh_int	gsn_poison;
extern	sh_int	gsn_sleep;
extern  sh_int  gsn_blackmantle;
extern  sh_int  gsn_fireshield;
extern  sh_int  gsn_conjure_elemental;
extern  sh_int	gsn_summon;
extern  sh_int	gsn_faerie_fire;
extern  sh_int	gsn_feeblemind;
extern  sh_int	gsn_sunray;
extern	sh_int	gsn_tornado;
extern	sh_int	gsn_wall_of_fire;

/* needed for staves in random.c */
extern sh_int			gsn_flamestrike;
extern sh_int			gsn_energy_drain;
extern sh_int			gsn_cure_critic;
extern sh_int			gsn_teleport;
extern sh_int			gsn_word_of_recall;
extern sh_int			gsn_fly;
extern sh_int			gsn_infravision;
extern sh_int			gsn_bless;
extern sh_int			gsn_heal;

/* needed for wands in random.c */
extern sh_int			gsn_lightning_bolt;
extern sh_int			gsn_chill_touch;
extern sh_int			gsn_dispel_evil;
extern sh_int			gsn_dispel_magic;
extern sh_int			gsn_detect_poison;
extern sh_int			gsn_magic_missile;

/* needed for potions in random.c */
extern sh_int			gsn_armor;
extern sh_int			gsn_remove_curse;
extern sh_int			gsn_cure_light;
extern sh_int			gsn_detect_evil;
extern sh_int			gsn_detect_invis;
extern sh_int			gsn_detect_magic;
extern sh_int			gsn_detect_hidden;
extern sh_int			gsn_cure_poison;
extern sh_int			gsn_sanctuary;
extern sh_int			gsn_strength;
extern sh_int			gsn_reflect;


/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))



/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IN_WAR(ch)		(IS_NPC((ch)) ? TRUE : IS_IMMORTAL((ch)) ? TRUE : (ch)->pcdata->in_war == TRUE)

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define IS_RESTING(ch)		(ch->position == POS_RESTING)
#define GET_AC(ch)		((ch)->armor				     \
				    + ( IS_AWAKE(ch)			     \
				    ? dex_app[get_curr_dex(ch)].defensive    \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+str_app[get_curr_str(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_app[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define GET_BEAT_SKILL(ch,skl)	(IS_NPC((ch)) ? 0  : IS_IMMORTAL((ch)) ? 0 : class_table[(ch)->class].skills[(skl)][SKILL_BEAT])
#define GET_PRAC_SKILL(ch,skl)  (IS_NPC((ch)) ? 90 : IS_IMMORTAL((ch)) ? 100 : class_table[(ch)->class].skills[(skl)][SKILL_PRAC])
#define GET_MANA_SKILL(ch,skl)	(IS_NPC((ch)) ? 0  : IS_IMMORTAL((ch)) ? 0 : class_table[(ch)->class].skills[(skl)][SKILL_MANA])

#define HAS_CONSENT(ch,from)	(IS_NPC((from)) ? FALSE : (from)->pcdata->consent == (ch) ? TRUE : FALSE)

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))

/* NPC macros */
#define IS_BORG(ch)		(IS_SET((ch)->act, PLR_BORG))




/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))



/*
 * Description macros.
 */
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: (ch)->name ) : "someone" )



/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
};



/*
 * Structure for a social in the socials table.
 */
struct	social_type
{
    char *	name;
    char *	char_no_arg;
    char *	others_no_arg;
    char *	char_found;
    char *	others_found;
    char *	vict_found;
    char *	char_auto;
    char *	others_auto;
};



/*
 * Global constants.
 */
extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];

extern	const	struct	cmd_type	cmd_table	[];
extern	const	struct	liq_type	liq_table	[LIQ_MAX];
extern  const   struct  arrow_type	arrow_table	[ARROW_MAX];
extern	const	struct	skill_type	skill_table	[MAX_SKILL];

/* Global Dynamic Structures */
extern	struct	class_type	*class_table;

/*
 * Global variables.
 */
extern		HELP_DATA	  *	help_first;
extern		SHOP_DATA	  *	shop_first;

extern		BAN_DATA	  *	ban_list;
extern		CHAR_DATA	  *	char_list;
extern		DESCRIPTOR_DATA   *	descriptor_list;
extern		NOTE_DATA	  *	note_list;
extern		OBJ_DATA	  *	object_list;
/* spec lists */
extern		CHAR_DATA	  *	mob_spec_list;
extern		OBJ_DATA	  *	obj_spec_list;
extern		ROOM_INDEX_DATA	  *	room_spec_list;


extern		AFFECT_DATA	  *	affect_free;
extern		BAN_DATA	  *	ban_free;
extern		CHAR_DATA	  *	char_free;
extern		DESCRIPTOR_DATA	  *	descriptor_free;
extern		EXTRA_DESCR_DATA  *	extra_descr_free;
extern		NOTE_DATA	  *	note_free;
extern		OBJ_DATA	  *	obj_free;
extern		PC_DATA		  *	pcdata_free;
extern		SPEC_OBJ_DATA	  *	spec_obj_free;

extern		char			bug_buf		[];
extern		time_t			current_time;
extern		bool			fLogAll;
extern		FILE *			fpReserve;
extern		KILL_DATA		kill_table	[];
extern		char			log_buf		[];
extern		TIME_INFO_DATA		time_info;
extern		WEATHER_DATA		weather_info;

/* war externs */
extern		int			war_type;
extern		int			war_flags;
extern		int			war_timer;
extern		int			pk_time;


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN(	do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN(	do_ban		);
DECLARE_DO_FUN( do_bash		);
DECLARE_DO_FUN(	do_brandish	);
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_changeclass	);
DECLARE_DO_FUN(	do_channels	);
DECLARE_DO_FUN(	do_chat		);
DECLARE_DO_FUN( do_charge	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN( do_color	);
DECLARE_DO_FUN(	do_commands	);
DECLARE_DO_FUN(	do_compare	);
DECLARE_DO_FUN(	do_config	);
DECLARE_DO_FUN( do_consent	);
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_cset		);
DECLARE_DO_FUN( do_cview	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN( do_dismount	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_finger	);
DECLARE_DO_FUN( do_flags	);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN( do_join		);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN( do_linkdead	);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN( do_material	);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN( do_mount	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN(	do_music	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN( do_noplay	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_notell	);
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_palm		);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN(	do_pose		);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN(	do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN( do_randeq	);
DECLARE_DO_FUN( do_random	);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recall	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN(	do_reply	);
DECLARE_DO_FUN(	do_report	);
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN(	do_sacrifice	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN( do_scan		);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN( do_shoot	);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN( do_sic		);
DECLARE_DO_FUN(	do_silence	);
DECLARE_DO_FUN( do_skills	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN(	do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_specs	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN( do_sset		);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_stats	);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN( do_survey	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN( do_track	);
DECLARE_DO_FUN(	do_train	);
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_users	);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_vote		);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN( do_warmessage	);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_wimpy	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_world	);
DECLARE_DO_FUN( do_wset		);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);



/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_alarm		);
DECLARE_SPELL_FUN(	spell_animate_dead	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_blackmantle	);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_conjure_elemental	);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_entangling_vines	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_feeblemind	);
DECLARE_SPELL_FUN(	spell_finger_of_death	);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_fireshield	);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_high_explosive	);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_protection	);
DECLARE_SPELL_FUN(	spell_reflect		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_shocking_grasp	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_sunray		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	spell_tornado		);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_wall_of_earth	);
DECLARE_SPELL_FUN(	spell_wall_of_fire	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR	""		/* Player files			*/
#define NULL_FILE	"proto.are"	/* To reserve one stream	*/
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""		/* Player files                 */
#define NULL_FILE	"nul"		/* To reserve one stream	*/
#endif

#if defined(unix)
#define PLAYER_DIR	"../player/"	/* Player files			*/
#define NULL_FILE	"/dev/null"	/* To reserve one stream	*/
#endif

#if defined WIN32
#define PLAYER_DIR	"../player/"
#define NULL_FILE	"../null_file_do_not_touch"
#endif

#define AREA_LIST	"area.lst"	/* List of areas		*/

#define BUG_FILE	"bugs.txt"      /* For 'bug' and bug( )		*/
#define IDEA_FILE	"ideas.txt"	/* For 'idea'			*/
#define TYPO_FILE	"typos.txt"     /* For 'typo'			*/
#define NOTE_FILE	"notes.txt"	/* For 'notes'			*/
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'		*/



/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SOB	SPEC_OBJ_DATA
#define SF	SPEC_FUN

/* act_comm.c */
void	add_follower	args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower	args( ( CHAR_DATA *ch ) );
void	die_follower	args( ( CHAR_DATA *ch ) );
bool	is_same_group	args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );

/* act_info.c */
void	set_title	args( ( CHAR_DATA *ch, char *title ) );

/* act_move.c */
void	move_char	args( ( CHAR_DATA *ch, int door ) );

/* act_obj.c */

/* act_wiz.c */

/* class.c */
void	read_classes	args( ( void ) );
char *	class_flag_name	args( ( int flags ) );

/* comm.c */
void	close_socket	args( ( DESCRIPTOR_DATA *dclose ) );
void	write_to_buffer	args( ( DESCRIPTOR_DATA *d, const char *txt,
			    int length ) );
void	send_to_char	args( ( const char *txt, CHAR_DATA *ch ) );
void	act		args( ( const char *format, CHAR_DATA *ch,
			    const void *arg1, const void *arg2, int type ) );

/* db.c */
void	boot_db		args( ( void ) );
CD *	create_mobile	args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object	args( ( OBJ_INDEX_DATA *pObjIndex ) );
void	clear_char	args( ( CHAR_DATA *ch ) );
void	free_char	args( ( CHAR_DATA *ch ) );
char *	get_extra_descr	args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index	args( ( int vnum ) );
OID *	get_obj_index	args( ( int vnum ) );
RID *	get_room_index	args( ( int vnum ) );
char	fread_letter	args( ( FILE *fp ) );
int	fread_number	args( ( FILE *fp ) );
char *	fread_string	args( ( FILE *fp ) );
void	fread_to_eol	args( ( FILE *fp ) );
char *	fread_word	args( ( FILE *fp ) );
void *	alloc_mem	args( ( int sMem ) );
void *	alloc_perm	args( ( int sMem ) );
void	free_mem	args( ( void *pMem, int sMem ) );
char *	str_dup		args( ( const char *str ) );
void	free_string	args( ( char *pstr ) );
int	number_fuzzy	args( ( int number ) );
int	number_range	args( ( int from, int to ) );
int	number_percent	args( ( void ) );
int	number_door	args( ( void ) );
int	number_bits	args( ( int width ) );
int	number_mm	args( ( void ) );
int	dice		args( ( int number, int size ) );
int	interpolate	args( ( int level, int value_00, int value_32 ) );
void	smash_tilde	args( ( char *str ) );
bool	str_cmp		args( ( const char *astr, const char *bstr ) );
bool	str_prefix	args( ( const char *astr, const char *bstr ) );
bool	str_infix	args( ( const char *astr, const char *bstr ) );
bool	str_suffix	args( ( const char *astr, const char *bstr ) );
char *	capitalize	args( ( const char *str ) );
void	append_file	args( ( CHAR_DATA *ch, char *file, char *str ) );
void	bug		args( ( const char *str, int param ) );
void	log_string	args( ( const char *str ) );
void	battle_log	args( ( CHAR_DATA *ch, CHAR_DATA *vch, char *str ) );
void	tail_chain	args( ( void ) );

/* fight.c */
void	violence_update	args( ( void ) );
void	multi_hit	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	damage		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	update_pos	args( ( CHAR_DATA *victim ) );
void	stop_fighting	args( ( CHAR_DATA *ch, bool fBoth ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_servants	args( ( CHAR_DATA *attacker, CHAR_DATA *ch ) );

/* handler.c */
int	get_trust	args( ( CHAR_DATA *ch ) );
int	get_age		args( ( CHAR_DATA *ch ) );
int	get_curr_str	args( ( CHAR_DATA *ch ) );
int	get_curr_int	args( ( CHAR_DATA *ch ) );
int	get_curr_wis	args( ( CHAR_DATA *ch ) );
int	get_curr_dex	args( ( CHAR_DATA *ch ) );
int	get_curr_con	args( ( CHAR_DATA *ch ) );
int	can_carry_n	args( ( CHAR_DATA *ch ) );
int	can_carry_w	args( ( CHAR_DATA *ch ) );
bool	is_name		args( ( const char *str, char *namelist ) );
void	affect_to_char	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip	args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected	args( ( CHAR_DATA *ch, int sn ) );
void	affect_join	args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room	args( ( CHAR_DATA *ch ) );
void	char_to_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char	args( ( OBJ_DATA *obj ) );
int	apply_ac	args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char	args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list	args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room	args( ( OBJ_DATA *obj ) );
void	obj_to_room	args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_to_obj	args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj	args( ( OBJ_DATA *obj ) );
void	extract_obj	args( ( OBJ_DATA *obj ) );
void	extract_char	args( ( CHAR_DATA *ch, bool fPull ) );
CD *	find_mount	args( ( CHAR_DATA *ch ) );
CD *	get_char_room	args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type	args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list	args( ( CHAR_DATA *ch, char *argument,
			    OBJ_DATA *list ) );
OD *	get_obj_carry	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	create_money	args( ( int amount ) );
int	get_obj_number	args( ( OBJ_DATA *obj ) );
int	get_obj_weight	args( ( OBJ_DATA *obj ) );
bool	room_is_dark	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private	args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *	item_type_name	args( ( OBJ_DATA *obj ) );
char *	affect_loc_name	args( ( int location ) );
char *	affect_bit_name	args( ( int vector ) );
char *	extra_bit_name	args( ( int extra_flags ) );

/* hunt.c */
void hunt_victim	args( ( CHAR_DATA *ch ) );

/* interp.c */
void	interpret	args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number	args( ( char *arg ) );
int	number_argument	args( ( char *argument, char *arg ) );
char *	one_argument	args( ( char *argument, char *arg_first ) );

/* magic.c */
int	skill_lookup	args( ( const char *name ) );
int	slot_lookup	args( ( int slot ) );
bool	saves_spell	args( ( CHAR_DATA *victim ) );
void	obj_cast_spell	args( ( int sn, int level, CHAR_DATA *ch,
				    CHAR_DATA *victim, OBJ_DATA *obj ) );
void	heal_char	args( ( CHAR_DATA *ch, int amount ) );

/* os.c */
char *crypt		args( (const char *pw, const char *salt) );
void get_time		args( (struct timeval *time) ); 

/* random.c */
OD *	load_random_object args( ( void ) );

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name ) );

/* social.c */
void	read_socials	args( ( void ) );
bool	check_social	args( ( CHAR_DATA *ch, char *argument, char *command ) );

/* specials.c */
char *	spec_name_lookup args( ( SPEC_FUN *spec ) );
bool	has_spec	args( ( char *str, SPEC_OBJ_DATA *list ) );
SF *	spec_lookup	args( ( const char *name ) );
SOB *	add_spec	args( ( const char *spec_name, ROOM_INDEX_DATA *room, CHAR_DATA *ch, OBJ_DATA *obj ) );
SOB *	new_spec_obj	args( ( void ) );

/* update.c */
void	gain_exp	args( ( CHAR_DATA *ch, int gain ) );
void	gain_condition	args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler	args( ( void ) );
int	update_specs	args( ( ROOM_INDEX_DATA *room, void *data1, void *data2, void *data3, int what, int type ) );


/* war.c */
CD *	can_war		args( ( DESCRIPTOR_DATA *d ) );
void	war_update	args( ( void ) );
void	transport_char	args( ( CHAR_DATA *ch ) );
void	equip_char_for_war args( ( CHAR_DATA *ch ) );
void	end_war		args( ( void ) );
void	reset_world	args( ( char k ) );
char *	team_name	args( ( int team ) );
char *	get_war_name	args( ( int war ) );
char *	get_war_flags	args( ( int flags ) );
int	find_number_of_players args( ( void ) );
bool	participated_this_war args( ( CHAR_DATA *ch ) );

#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef  SOB
#undef  SF
