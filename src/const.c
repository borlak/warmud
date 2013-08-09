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
#include <stdio.h>
#include <time.h>
#include "merc.h"




/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 140, 12 },
    {  0,  0, 140, 13 }, /* 13  */
    {  0,  1, 170, 14 },
    {  1,  1, 170, 15 }, /* 15  */
    {  1,  2, 195, 16 },
    {  2,  3, 220, 22 },
    {  2,  4, 250, 25 }, /* 18  */
    {  3,  5, 400, 30 },
    {  3,  6, 500, 35 }, /* 20  */
    {  4,  7, 600, 40 },
    {  5,  7, 700, 45 },
    {  6,  8, 800, 50 },
    {  8, 10, 900, 55 },
    { 10, 12, 999, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 85 },
    { 99 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },	/* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },	/* 15 */
    { 3 },
    { 4 },
    { 5 },	/* 18 */
    { 5 },
    { 5 },	/* 20 */
    { 6 },
    { 6 },
    { 6 },
    { 6 },
    { 7 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "water",			"clear",	{  0, 1, 10 }, 10,	0,	80	},  /*  0 */
    { "beer",			"amber",	{  3, 2,  5 }, 20,	0,	80	},
    { "wine",			"rose",		{  5, 2,  5 }, 15,	30,	80	},
    { "ale",			"brown",	{  2, 2,  5 }, 15,	0,	80	},
    { "dark ale",		"dark",		{  1, 2,  5 }, 20,	10,	80	},

    { "whisky",			"golden",	{  6, 1,  4 }, 25,	10,	90	},  /*  5 */
    { "lemonade",		"pink",		{  0, 1,  8 }, 25,	1,	90	},
    { "firebreather",		"boiling",	{ 10, 0,  0 }, 25,	25,	90	},
    { "drano",			"clear",	{  3, 3,  3 }, -400,	0,	30	},
    { "slime mold juice",	"green",	{  0, 4, -8 }, -50,	-100,	50	},

    { "milk",			"white",	{  0, 3,  6 }, 40,	30,	95	},  /* 10 */
    { "tea",			"tan",		{  0, 1,  6 }, 34,	0,	95	},
    { "coffee",			"black",	{  0, 1,  6 }, 50,	20,	95	},
    { "blood",			"red",		{  0, 2, -1 }, 1000,	1000,	99	},
    { "salt water",		"clear",	{  0, 1, -2 }, -100,	-200,	30	},

    { "cola",			"cherry",	{  0, 1,  5 }, 1,	1,	0	}   /* 15 */
};


const	struct	arrow_type	arrow_table	[ARROW_MAX]	=
{
	{ "a &+ywooden&N arrow",		0			},
	{ "a &+rred&N arrow",			&gsn_curse		},
	{ "a &+ggreen&N arrow",			&gsn_poison		},
	{ "a &+Lblack&N arrow",			&gsn_blackmantle	},
	{ "a &+Wwhite&N arrow",			&gsn_blindness		},
	{ "a &+Rheart-shaped&N arrow",		&gsn_charm_person	},
	{ "a &+cfading&N arrow",		&gsn_teleport		},
	{ "a &=wWpulsing&N arrow",		&gsn_magic_missile	},
	{ "an &+Yodd&N arrow",			&gsn_feeblemind		},
	{ "a &+Rflaming&N arrow",		&gsn_faerie_fire	}
};

/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const	struct	skill_type	skill_table	[MAX_SKILL]	=
{

/*
 * Magic spells.
 */

    {
	"it",			TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT( 0),
	"",			""
    },

    {
	"acid blast",		TYPE_ACID,
	spell_acid_blast,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(70),
	"acid blast",		"!Acid Blast!"
    },

    {
	"alarm",		TYPE_NONE,
	spell_alarm,		TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(94),
	"alarm",		"!Alarm!"
    },

    {
	"animate dead",		TYPE_NONE,
	spell_animate_dead,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(85),
	"",			"You new spirit has left you."
    },

    {
	"armor",		TYPE_NONE,
	spell_armor,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_armor,		SLOT( 1),
	"tan",			"You feel less protected."
    },

    {
	"blackmantle",		TYPE_NONE,
	spell_blackmantle,	TAR_OFFENSIVE_IGNORE,		POS_STANDING,
	&gsn_blackmantle,	SLOT(86),
	"cloudy",		"The black aura shatters."
    },

    {
	"bless",		TYPE_NONE,
	spell_bless,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT( 3),
	"dark grey",		"You feel less righteous."
    },

    {
	"blindness",		TYPE_NONE,
	spell_blindness,	TAR_OFFENSIVE_IGNORE,	POS_FIGHTING,
	&gsn_blindness,		SLOT( 4),
	"pure white",		"You can see again."
    },

    {
	"burning hands",	TYPE_FIRE,
	spell_burning_hands,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 5),
	"burning hands",	"!Burning Hands!"
    },

    {
	"call lightning",	TYPE_ELECTRICITY,
	spell_call_lightning,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT( 6),
	"lightning bolt",	"!Call Lightning!"
    },

    {
	"change sex",		TYPE_NONE,
	spell_change_sex,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(82),
	"",			"Your body feels familiar again."
    },

    {
	"charm person",		TYPE_NONE,
	spell_charm_person,	TAR_OFFENSIVE_IGNORE,	POS_STANDING,
	&gsn_charm_person,	SLOT( 7),
	"",			"You feel more self-confident."
    },

    {
	"chill touch",		TYPE_COLD,
	spell_chill_touch,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT( 8),
	"chilling touch",	"You feel less cold."
    },

    {
	"colour spray",		TYPE_MYSTICAL,
	spell_colour_spray,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(10),
	"colour spray",		"!Colour Spray!"
    },

    {
	"conjure elemental",	TYPE_NONE,
	spell_conjure_elemental, TAR_IGNORE,		POS_STANDING,
	&gsn_conjure_elemental,	SLOT(87),
	"",			"You fade into nothingness"
    },

    {
	"continual light",	TYPE_NONE,
	spell_continual_light,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(57),
	"",			"!Continual Light!"
    },

    {
	"control weather",	TYPE_NONE,
	spell_control_weather,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(11),
	"",			"!Control Weather!"
    },

    {
	"create food",		TYPE_NONE,
	spell_create_food,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(12),
	"",			"!Create Food!"
    },

    {
	"create spring",	TYPE_NONE,
	spell_create_spring,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(80),
	"",			"!Create Spring!"
    },

    {
	"create water",		TYPE_NONE,
	spell_create_water,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(13),
	"",			"!Create Water!"
    },

    {
	"cure blindness",	TYPE_NONE,
	spell_cure_blindness,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(14),
	"",			"!Cure Blindness!"
    },

    {
	"cure critical",	TYPE_NONE,
	spell_cure_critical,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_critic,	SLOT(15),
	"azure",		"!Cure Critical!"
    },

    {
	"cure light",		TYPE_NONE,
	spell_cure_light,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_cure_light,	SLOT(16),
	"grey",			"!Cure Light!"
    },

    {
	"cure poison",		TYPE_NONE,
	spell_cure_poison,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_cure_poison,	SLOT(43),
	"",			"!Cure Poison!"
    },

    {
	"cure serious",		TYPE_NONE,
	spell_cure_serious,	TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(61),
	"",			"!Cure Serious!"
    },

    {
	"curse",		TYPE_NONE,
	spell_curse,		TAR_OFFENSIVE_IGNORE,	POS_STANDING,
	&gsn_curse,		SLOT(17),
	"curse",		"The curse wears off."
    },

    {
	"detect evil",		TYPE_NONE,
	spell_detect_evil,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_evil,	SLOT(18),
	"orange",		"The red in your vision disappears."
    },

    {
	"detect hidden",	TYPE_NONE,
	spell_detect_hidden,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_hidden,	SLOT(44),
	"red",			"You feel less aware of your suroundings."
    },

    {
	"detect invis",		TYPE_NONE,
	spell_detect_invis,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_invis,	SLOT(19),
	"yellow",		"You no longer see invisible objects."
    },

    {
	"detect magic",		TYPE_NONE,
	spell_detect_magic,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_detect_magic,	SLOT(20),
	"dark blue",		"The detect magic wears off."
    },

    {
	"detect poison",	TYPE_NONE,
	spell_detect_poison,	TAR_OBJ_INV,		POS_STANDING,
	&gsn_detect_poison,	SLOT(21),
	"",			"!Detect Poison!"
    },

    {
	"dispel evil",		TYPE_NONE,
	spell_dispel_evil,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_dispel_evil,	SLOT(22),
	"dispel evil",		"!Dispel Evil!"
    },

    {
	"dispel magic",		TYPE_NONE,
	spell_dispel_magic,	TAR_OFFENSIVE_IGNORE,	POS_FIGHTING,
	&gsn_dispel_magic,	SLOT(59),
	"",			"!Dispel Magic!"
    },

    {
	"earthquake",		TYPE_BLUNT,
	spell_earthquake,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(23),
	"earthquake",		"!Earthquake!"
    },

    {
	"enchant weapon",	TYPE_NONE,
	spell_enchant_weapon,	TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(24),
	"",			"!Enchant Weapon!"
    },

    {
	"energy drain",		TYPE_MYSTICAL,
	spell_energy_drain,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_energy_drain,	SLOT(25),
	"energy drain",		"!Energy Drain!"
    },

    {
	"entangling vines",	TYPE_NONE,
	spell_entangling_vines,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(91),
	"entangling vines",	"!Entangling Vines!"
    },

    {
	"faerie fire",		TYPE_NONE,
	spell_faerie_fire,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_faerie_fire,	SLOT(72),
	"faerie fire",		"The pink aura around you fades away."
    },

    {
	"feeblemind",		TYPE_NONE,
	spell_feeblemind,	TAR_OFFENSIVE_IGNORE,	POS_STANDING,
	NULL,			SLOT(84),
	"feeblemind",		"Your amnesia wears away."
    },

    {
	"faerie fog",		TYPE_NONE,
	spell_faerie_fog,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(73),
	"faerie fog",		"!Faerie Fog!"
    },

    {
	"finger of death",	TYPE_MYSTICAL,
	spell_finger_of_death,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(95),
	"finger of death",	"!Finger of Death!"
    },

    {
	"fireball",		TYPE_FIRE,
	spell_fireball,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(26),
	"fireball",		"!Fireball!"
    },

    {
	"fireshield",		TYPE_MYSTICAL,
	spell_fireshield,	TAR_CHAR_SELF,		POS_STANDING,
	&gsn_fireshield,	SLOT(88),
	"plum",			"You flaming sphere fades away"
    },

    {
	"flamestrike",		TYPE_FIRE,
	spell_flamestrike,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_flamestrike,	SLOT(65),
	"flamestrike",		"!Flamestrike!"
    },

    {
	"fly",			TYPE_NONE,
	spell_fly,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_fly,		SLOT(56),
	"green",		"You slowly float to the ground."
    },

    {
	"gate",			TYPE_NONE,
	spell_gate,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(83),
	"",			"!Gate!"
    },

    {
	"giant strength",	TYPE_NONE,
	spell_giant_strength,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_strength,		SLOT(39),
	"black",		"You feel weaker."
    },


    {
	"heal",			TYPE_NONE,
	spell_heal,		TAR_CHAR_DEFENSIVE,	POS_FIGHTING,
	&gsn_heal,		SLOT(28),
	"maroon",		"!Heal!"
    },

    {
	"identify",		TYPE_NONE,
	spell_identify,		TAR_OBJ_INV,		POS_STANDING,
	NULL,			SLOT(53),
	"",			"!Identify!"
    },

    {
	"infravision",		TYPE_NONE,
	spell_infravision,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_infravision,	SLOT(77),
	"silver",		"You no longer see in the dark."
    },

    {
	"invis",		TYPE_NONE,
	spell_invis,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_invis,		SLOT(29),
	"cyan",			"You are no longer invisible."
    },

    {
	"know alignment",	TYPE_NONE,
	spell_know_alignment,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(58),
	"",			"!Know Alignment!"
    },

    {
	"lightning bolt",	TYPE_ELECTRICITY,
	spell_lightning_bolt,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_lightning_bolt,	SLOT(30),
	"lightning bolt",	"!Lightning Bolt!"
    },

    {
	"locate object",	TYPE_NONE,
	spell_locate_object,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(31),
	"",			"!Locate Object!"
    },

    {
	"magic missile",	TYPE_MYSTICAL,
	spell_magic_missile,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_magic_missile,	SLOT(32),
	"magic missile",	"!Magic Missile!"
    },

    {
	"mass invis",		TYPE_NONE,
	spell_mass_invis,	TAR_IGNORE,		POS_STANDING,
	&gsn_mass_invis,	SLOT(69),
	"",			"!Mass Invis!"
    },

    {
	"pass door",		TYPE_NONE,
	spell_pass_door,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(74),
	"",			"You feel solid again."
    },

    {
	"poison",		TYPE_NONE,
	spell_poison,		TAR_OFFENSIVE_IGNORE,	POS_STANDING,
	&gsn_poison,		SLOT(33),
	"poison",		"You feel less sick."
    },

    {
	"protection",		TYPE_NONE,
	spell_protection,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(34),
	"",			"You feel less protected."
    },

    {
	"reflect",		TYPE_NONE,
	spell_reflect,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_reflect,		SLOT(81),
	"lime",			"!Reflect!"
    },

    {
	"remove curse",		TYPE_NONE,
	spell_remove_curse,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_remove_curse,	SLOT(35),
	"",			"!Remove Curse!"
    },

    {
	"sanctuary",		TYPE_NONE,
	spell_sanctuary,	TAR_CHAR_DEFENSIVE,	POS_STANDING,
	&gsn_sanctuary,		SLOT(36),
	"purple",		"The white aura around your body fades."
    },

    {
	"shield",		TYPE_NONE,
	spell_shield,		TAR_CHAR_DEFENSIVE,	POS_STANDING,
	NULL,			SLOT(67),
	"",			"Your force shield shimmers then fades away."
    },

    {
	"shocking grasp",	TYPE_ELECTRICITY,
	spell_shocking_grasp,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(37),
	"shocking grasp",	"!Shocking Grasp!"
    },

    {
	"sleep",		TYPE_NONE,
	spell_sleep,		TAR_OFFENSIVE_IGNORE,	POS_STANDING,
	&gsn_sleep,		SLOT(38),
	"",			"You feel less tired."
    },

    {
	"stone skin",		TYPE_NONE,
	spell_stone_skin,	TAR_CHAR_SELF,		POS_STANDING,
	NULL,			SLOT(66),
	"",			"Your skin feels soft again."
    },

    {
	"summon",		TYPE_NONE,
	spell_summon,		TAR_IGNORE,		POS_STANDING,
	&gsn_summon,		SLOT(40),
	"",			"!Summon!"
    },

    {
	"sunray",		TYPE_FIRE,
	spell_sunray,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_sunray,		SLOT(92),
	"sunray",		"!Sunray!"
    },

    {
	"teleport",		TYPE_NONE,
	spell_teleport,		TAR_CHAR_SELF,		POS_FIGHTING,
	&gsn_teleport, 		SLOT( 2),
	"",			"!Teleport!"
    },

    {
	"tornado",		TYPE_MYSTICAL,
	spell_tornado,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_tornado,		SLOT(93),
	"tornado",		"!Tornado!"
    },

    {
	"ventriloquate",	TYPE_NONE,
	spell_ventriloquate,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(41),
	"",			"!Ventriloquate!"
    },

    {
	"wall of earth",	TYPE_NONE,
	spell_wall_of_earth,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(90),
	"",			"!WallOfEarth!"
    },

    {
	"wall of fire",		TYPE_FIRE,
	spell_wall_of_fire,	TAR_IGNORE,		POS_STANDING,
	&gsn_wall_of_fire,	SLOT(89),
	"",			"!WallOfFire!"
    },

    {
	"weaken",		TYPE_NONE,
	spell_weaken,		TAR_OFFENSIVE_IGNORE,	POS_FIGHTING,
	NULL,			SLOT(68),
	"spell",		"You feel stronger."
    },

    {
	"word of recall",	TYPE_NONE,
	spell_word_of_recall,	TAR_CHAR_SELF,		POS_RESTING,
	&gsn_word_of_recall,	SLOT(42),
	"",			"!Word of Recall!"
    },

/*
 * Dragon breath
 */
    {
	"acid breath",		TYPE_ACID,
	spell_acid_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(200),	
	"blast of acid",	"!Acid Breath!"
    },

    {
	"fire breath",		TYPE_FIRE,
	spell_fire_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(201),
	"blast of flame",	"!Fire Breath!"
    },

    {
	"frost breath",		TYPE_COLD,
	spell_frost_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(202),
	"blast of frost",	"!Frost Breath!"
    },

    {
	"gas breath",		TYPE_MYSTICAL,
	spell_gas_breath,	TAR_IGNORE,		POS_FIGHTING,
	NULL,			SLOT(203),
	"blast of gas",		"!Gas Breath!"
    },

    {
	"lightning breath",	TYPE_ELECTRICITY,
	spell_lightning_breath,	TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	NULL,			SLOT(204),
	"blast of lightning",	"!Lightning Breath!"
    },

/*
 * Fighter and thief skills.
 */

    {
	"archery",		TYPE_PIERCE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_archery,		SLOT( 0),
	"archery",		"!Archery!"
    },

    {
	"awareness",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_DEAD,
	&gsn_awareness,		SLOT( 0),
	"awareness",		"!Awareness!"
    },

    {
	"backstab",		TYPE_PIERCE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_backstab,		SLOT( 0),
	"backstab",		"!Backstab!"
    },

    {
	"bash",			TYPE_BLUNT,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_bash,		SLOT( 0),
	"bash",			"!Bash!"
    },

    {
	"charge",		TYPE_SLASH,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_STANDING,
	&gsn_charge,		SLOT( 0),		
	"charge",		"!Charge!"
    },

    {
	"disarm",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_disarm,		SLOT( 0),
	"",			"!Disarm!"
    },

    {
	"dodge",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_dodge,		SLOT( 0),
	"",			"!Dodge!"
    },

    {
	"enhanced damage",	TYPE_MYSTICAL,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_enhanced_damage,	SLOT( 0),
	"",			"!Enhanced Damage!"
    },

    {
	"hide",			TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_RESTING,
	&gsn_hide,		SLOT( 0),
	"",			"!Hide!"
    },

    {
	"kick",			TYPE_BLUNT,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_kick,		SLOT( 0),
	"kick",			"!Kick!"
    },

    {
	"parry",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_parry,		SLOT( 0),
	"",			"!Parry!"
    },

    {
	"peek",			TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_peek,		SLOT( 0),
	"",			"!Peek!"
    },

    {
	"pick lock",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_pick_lock,		SLOT( 0),
	"",			"!Pick!"
    },

    {
	"quivering palm",	TYPE_MYSTICAL,
	spell_null,		TAR_CHAR_OFFENSIVE,	POS_FIGHTING,
	&gsn_quivering_palm,	SLOT( 0),		
	"quivering palm",	"!Quivering Palm!"
    },

    {
	"rescue",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_rescue,		SLOT( 0),
	"",			"!Rescue!"
    },

    {
	"ride",			TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_ride,		SLOT( 0),
	"",			"!Ride!"
    },

    {
	"second attack",	TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_second_attack,	SLOT( 0),
	"",			"!Second Attack!"
    },

    {
	"sneak",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_sneak,		SLOT( 0),
	"",			NULL
    },

    {
	"steal",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_steal,		SLOT( 0),
	"",			"!Steal!"
    },

    {
	"third attack",		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_FIGHTING,
	&gsn_third_attack,	SLOT( 0),
	"",			"!Third Attack!"
    },

    {   
        "track" ,		TYPE_NONE,
	spell_null,		TAR_IGNORE,		POS_STANDING,
	&gsn_track,		SLOT( 0),
	"",			"!Track!"
    }

};
