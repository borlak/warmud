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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );



/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( skill_table[sn].name == NULL )
	    break;
	if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0])
	&&   !str_prefix( name, skill_table[sn].name ) )
	    return sn;
    }

    return -1;
}



/*
 * Lookup a skill by slot number.
 * Used for object loading.
 */
int slot_lookup( int slot )
{
    extern bool fBootDb;
    int sn;

    if ( slot <= 0 )
	return -1;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
	if ( slot == skill_table[sn].slot )
	    return sn;
    }

    if ( fBootDb )
    {
	bug( "Slot_lookup: bad slot %d.", slot );
	return 0;
/*	abort( );*/
    }

    return -1;
}



/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
    char buf  [MAX_STRING_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    CHAR_DATA *rch;
    char *pName;
    int iSyl;
    int length;

    struct syl_type
    {
	char *	old;
	char *	new;
    };

    static const struct syl_type syl_table[] =
    {
	{ " ",		" "		},
	{ "ar",		"abra"		},
	{ "au",		"kada"		},
	{ "bless",	"fido"		},
	{ "blind",	"nose"		},
	{ "bur",	"mosa"		},
	{ "cu",		"judi"		},
	{ "de",		"oculo"		},
	{ "en",		"unso"		},
	{ "light",	"dies"		},
	{ "lo",		"hi"		},
	{ "mor",	"zak"		},
	{ "move",	"sido"		},
	{ "ness",	"lacri"		},
	{ "ning",	"illa"		},
	{ "per",	"duda"		},
	{ "ra",		"gru"		},
	{ "re",		"candus"	},
	{ "son",	"sabru"		},
	{ "tect",	"infra"		},
	{ "tri",	"cula"		},
	{ "ven",	"nofo"		},
	{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
	{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
	{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
	{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
	{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
	{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
	{ "y", "l" }, { "z", "k" },
	{ "", "" }
    };

    buf[0]	= '\0';
    for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
    {
	for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
	{
	    if ( !str_prefix( syl_table[iSyl].old, pName ) )
	    {
		strcat( buf, syl_table[iSyl].new );
		break;
	    }
	}

	if ( length == 0 )
	    length = 1;
    }

    sprintf( buf2, "$n utters the words, '%s'.", buf );
    sprintf( buf,  "$n utters the words, '%s'.", skill_table[sn].name );

    for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
    {
	if ( rch != ch )
	    act( ch->class==rch->class ? buf : buf2, ch, NULL, rch, TO_VICT );
    }

    return;
}



/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell( CHAR_DATA *victim )
{
    int save;

    save = 10 - victim->saving_throw * 5;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}



/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

void do_cast( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    void *vo;
    int sn;

    target_name = one_argument( argument, arg1 );
    one_argument( target_name, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Cast which what where?\n\r", ch );
	return;
    }

    if ( ( sn = skill_lookup( arg1 ) ) < 0
    ||   GET_PRAC_SKILL(ch,sn) <= 0 )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return;
    }
  
    if ( ch->position < skill_table[sn].minimum_position )
    {
	send_to_char( "You can't concentrate enough.\n\r", ch );
	return;
    }

    if( !IN_WAR(ch) )
    {
	    send_to_char("You aren't in a war!\n\r", ch );
	    return;
    }

    if( IS_BORG(ch) )
    {
	    send_to_char("You are Borg!  You can't even speak!\n\r", ch );
	    return;
    }

    /* cant let pets just cast anything */
    if( IS_NPC(ch) 
    && (IS_AFFECTED(ch, AFF_SUMMONED) || IS_AFFECTED(ch, AFF_CHARM)) )
	    return;

    /*
     * Locate targets.
     */
    victim	= NULL;
    obj		= NULL;
    vo		= NULL;
      
    switch ( skill_table[sn].target )
    {
    default:
	bug( "Do_cast: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	break;

    case TAR_OFFENSIVE_IGNORE:
	if ( arg2[0] == '\0' )
	{
	    if ( ( victim = ch->fighting ) == NULL )
	    {
		send_to_char( "Cast the spell on whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	if ( ch == victim )
	{
	    send_to_char( "You can't do that to yourself.\n\r", ch );
	    return;
	}

	vo = (void *) victim;
	break;	    

    case TAR_CHAR_OFFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    if ( ( victim = ch->fighting ) == NULL )
	    {
		send_to_char( "Cast the spell on whom?\n\r", ch );
		return;
	    }
	}
	else
	{
	    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	if ( ch == victim )
	{
	    send_to_char( "You can't do that to yourself.\n\r", ch );
	    return;
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( arg2[0] == '\0' )
	{
	    victim = ch;
	}
	else
	{
	    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	    {
		send_to_char( "They aren't here.\n\r", ch );
		return;
	    }
	}

	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	if ( arg2[0] != '\0' && !is_name( arg2, ch->name ) )
	{
	    send_to_char( "You cannot cast this spell on another.\n\r", ch );
	    return;
	}

	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "What should the spell be cast upon?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You are not carrying that.\n\r", ch );
	    return;
	}

	vo = (void *) obj;
	break;
    }
	    
    if ( ch->mana < GET_MANA_SKILL(ch,sn) )
    {
	send_to_char( "You don't have enough mana.\n\r", ch );
	return;
    }
      
    if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
	say_spell( ch, sn );
      
    WAIT_STATE(ch, GET_BEAT_SKILL(ch,sn) );
      
    if( number_percent() > GET_PRAC_SKILL(ch,sn) )
    {
	send_to_char( "You lost your concentration.\n\r", ch );
	ch->mana -= GET_MANA_SKILL(ch,sn) / 2;

	if( ch->fighting )
	{
		sprintf(arg1,"%s fails at casting %s",
			ch->name,
			skill_table[sn].name );
		battle_log( ch, ch->fighting, arg1 );
	}
    }
    else
    {
	ch->mana	-= GET_MANA_SKILL(ch,sn);
	(*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo );
    }



    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
    &&   victim != ch
    &&   victim->master != ch )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( victim == vch && victim->fighting == NULL )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/*
 * Cast spells at targets using a magical object.
 */
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
    void *vo;

    if ( sn <= 0 )
	return;

    if ( sn >= MAX_SKILL || skill_table[sn].spell_fun == 0 )
    {
	bug( "Obj_cast_spell: bad sn %d.", sn );
	return;
    }

    switch ( skill_table[sn].target )
    {
    default:
	bug( "Obj_cast_spell: bad target for sn %d.", sn );
	return;

    case TAR_IGNORE:
	vo = NULL;
	break;

    case TAR_OFFENSIVE_IGNORE:
	if ( victim == NULL )
	    victim = ch->fighting;
	if ( victim == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) victim;
	break;

    case TAR_CHAR_OFFENSIVE:
	if ( victim == NULL )
	    victim = ch->fighting;
	if ( victim == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) victim;
	break;

    case TAR_CHAR_DEFENSIVE:
	if ( victim == NULL )
	    victim = ch;
	vo = (void *) victim;
	break;

    case TAR_CHAR_SELF:
	vo = (void *) ch;
	break;

    case TAR_OBJ_INV:
	if ( obj == NULL )
	{
	    send_to_char( "You can't do that.\n\r", ch );
	    return;
	}
	vo = (void *) obj;
	break;
    }

    target_name = "";
    (*skill_table[sn].spell_fun) ( sn, level, ch, vo );

    if ( skill_table[sn].target == TAR_CHAR_OFFENSIVE
    &&   victim != ch
    &&   victim->master != ch )
    {
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
	    vch_next = vch->next_in_room;
	    if ( victim == vch && victim->fighting == NULL )
	    {
		multi_hit( victim, ch, TYPE_UNDEFINED );
		break;
	    }
	}
    }

    return;
}



/* spell to help blackmantle work more efficiently */
void heal_char( CHAR_DATA *ch, int amount )
{
	char buf[50];

	if( IS_AFFECTED(ch, AFF_BLACKMANTLE) )
	{
		act("$n's &+Lblack aura&N absorbs the healing.",
			ch, NULL, NULL, TO_ROOM );
		send_to_char("Your &+Lblack aura&N absorbs the healing.", ch );
		return;
	}

	if( ch->fighting )
	{
		sprintf(buf,"%s is healed for %d hp.", ch->name, amount );
		battle_log( ch, ch->fighting, buf );
	}

	ch->hit = UMIN( ch->max_hit, ch->hit + amount );
}


/*
 * Spell functions.
 */
void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act("&+LDark acidic fluids&N shoot from $n's hands at you!",
	    ch, NULL, victim, TO_VICT );
    act("&+LDark acidic fluids&N shoot from your hands at $N!",
	    ch, NULL, victim, TO_CHAR );
    act("&+LDark acidic fluids&N shoot from $n's hands at $N!",
	    ch, NULL, victim, TO_NOTVICT );

    damage( ch, victim, dice(get_curr_int(ch),6), sn );
    return;
}



void spell_alarm( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;

	send_to_char("An invisible eye appears in the room.\n\r", ch );

	spec		= add_spec( "spec_alarm", ch->in_room, ch, NULL );
	spec->timer	= number_fuzzy( get_curr_int(ch) );
	return;
}



void spell_animate_dead( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;
	char buf[MAX_INPUT_LENGTH];
	char arg[200];
	OBJ_DATA *corpse;
	OBJ_DATA *obj = NULL;
	OBJ_DATA *obj_next = NULL;
	CHAR_DATA *mob;
	CHAR_DATA *mobchk;
	AFFECT_DATA af;
	bool found = FALSE;
	char *color;
	int num;
	int i = 0;
	extern SPEC_FUN spec_animate_dead;

	num = number_argument(target_name,arg);

	for( corpse = ch->in_room->contents; corpse; corpse = corpse->next_content )
	{
		if( strstr(corpse->name,arg) == NULL )
			continue;

		if( ++i == num )
		{
			found = TRUE;
			break;
		}
	}

	if( !found )
	{
		send_to_char("Animate what?\n\r", ch );
		return;
	}

	if( strstr(corpse->short_descr, "undead") != NULL )
	{
		send_to_char("You can't animate once animated corpses.\n\r", ch );
		return;
	}

	/********Now we check if he can animate any more corpses*********/
	num	= get_curr_int(ch);
	i	= number_fuzzy( number_fuzzy(get_curr_int(ch)));
	found	= FALSE;

	for( mobchk = char_list; mobchk; mobchk = mobchk->next )
	{
		if( mobchk->master != ch
		||  !IS_NPC(mobchk)
		||  strstr(mobchk->name, "undead") == NULL)
			continue;

		found	 = has_spec( "spec_animate_dead", mobchk->spec_obj );
		num	-= mobchk->level;
	}

	if( get_curr_int(ch)/3 > num )
	{
		send_to_char("You are too weak to animate any more corpses.\n\r", ch );
		act("$n looks very weak and stressed.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	/*************He's legal, now lets see how powerful a corpse he gets ***********/
	mob		= create_mobile( get_mob_index(MOB_VNUM_UNDEAD) );
	mob->level	= 5;
	color		= "grey";

	switch( i )
	{
	default:
		mob->level	= 1;
		color		= "&+Lb&Nu&+Lr&Nn&+Lt&N";
		break;
	case 12:
	case 13:
	case 14:
		mob->level	= 3;
		color		= "&+Lblack&N";
		break;
	case 15:
	case 16:
	case 17:
		break;
	case 18:
	case 19:
	case 20:
		mob->level	= 7;
		color		= "&+rred&n";
		break;
	case 21:
	case 22:
	case 23:
		mob->level	= 9;
		color		= "&+Bblue&N";
		break;
	case 24:
	case 25:
		mob->level	= 11;
		color		= "&+Wwhite&N";
		break;
	}

	/* bonus for char-animated corpses */
	if( corpse->item_type == ITEM_CORPSE_PC )
		mob->level	+= dice(1,3);

	mob->hit  = mob->max_hit	= mob->level*10;

	sprintf(buf,"skeleton undead" );
	mob->name		= str_dup( buf );

	sprintf(buf,"A %s undead skeleton", color );
	mob->short_descr	= str_dup( buf );

	sprintf(buf,"A %s undead skeleton of a %s is here.\n\r", color, corpse->short_descr );
	mob->long_descr		= str_dup( buf );

	char_to_room(mob, ch->in_room);

	act("&+cYou breathe life into the $p...\n\r$N slowly creeps to $s feet.&N",
		ch, corpse, mob, TO_CHAR );
	act("&+c$n breathes life into the $p...\n\r$N slowly creeps to $s feet.&N",
		ch, corpse, mob, TO_ROOM );

	/* Move objects from corpse onto mob */
	for( obj = corpse->contains; obj; obj = obj_next )
	{
		obj_next = obj->next_content;
		obj_from_obj(obj);
		obj_to_char(obj,mob);
		do_wear(mob,obj->name);
	}

	if( !found )
	{
		spec		= add_spec( "spec_animate_dead", NULL, mob, NULL );
		spec->vch	= ch;
	}

	mob->leader = ch;
	mob->master = ch;
	extract_obj(corpse);
	
	/*******set him summoned and make em a reliable servant and blackmantel em************/
	SET_BIT(mob->act, ACT_SERVANT);
	SET_BIT(mob->act, ACT_SENTINEL);
	SET_BIT(mob->affected_by, AFF_BLACKMANTLE);
	af.type		= sn;
	af.duration	= get_curr_int(ch)/2;
	af.modifier	= 0;
	af.location	= 0;
	af.bitvector	= AFF_SUMMONED;
	affect_to_char( mob, &af );

	return;
}


void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = 10;
    af.modifier  = number_range(-(get_curr_wis(ch))*3,-10);
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "&+YYou feel Borlak protecting you.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}


void spell_blackmantle( int sn, int level, CHAR_DATA *ch, void *vo )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;

	if( victim->position == POS_FIGHTING || is_affected( victim, sn ) 
	||  saves_spell(victim) )
		return;

	af.type		= sn;
	af.duration	= get_curr_int(ch)/2;
	af.location	= 0;
	af.modifier	= 0;
	af.bitvector	= AFF_BLACKMANTLE;
	affect_to_char( victim, &af );

	send_to_char("&+LA black aura forms a shield around you.&N\n\r", victim );
	act("&+LA black aura forms a shield around $n.&N", victim, NULL, NULL, TO_ROOM );
	return;
}



void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
	return;

    af.type      = sn;
    af.duration  = 5;
    af.location  = APPLY_HITROLL;
    af.modifier  = number_range(1,get_curr_wis(ch)/3);
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = number_range(-(get_curr_wis(ch)/5),-1);
    affect_to_char( victim, &af );
    send_to_char( "&+CYou feel righteous.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_BLIND) || saves_spell( victim ) )
	return;

    af.type      = sn;
    af.location  = APPLY_HITROLL;
    af.modifier  = -4;
    af.duration  = 1;
    af.bitvector = AFF_BLIND;
    affect_to_char( victim, &af );
    send_to_char( "&+WYou are blinded!&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act("&+r$n's hands flame and fry you as he touches you!&N",
	    ch, NULL, victim, TO_VICT );
    act("&+r$n's hands flame and fry $N as $e touches $M!&N",
	    ch, NULL, victim, TO_NOTVICT );
    act("&+rYour hands flame and fry $N as you touch $M!&N",
	    ch, NULL, victim, TO_CHAR );


    damage( ch, victim, number_range(5,get_curr_int(ch)*2), sn );
    return;
}



void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You must be out of doors.\n\r", ch );
	return;
    }

    if ( weather_info.sky < SKY_RAINING )
    {
	send_to_char( "You need bad weather.\n\r", ch );
	return;
    }

    send_to_char( "Borlak's &-Blightning&N strikes your foes!\n\r", ch );
    act( "$n calls Borlak's &-Blightning&N to strike $s foes!",
	ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if ( vch != ch )
	    {
		    act("$n's &+Blightning&N strikes you with awesome force!",
			    ch, NULL, vch, TO_VICT );
		    act("Your &+Blightning&N strikes $N with awesome force!",
			    ch, NULL, vch, TO_CHAR );
		    act("$n's &+Blightning&N strikes $N with awesome force!",
			    ch, NULL, vch, TO_NOTVICT );
		    damage( ch, vch, dice(get_curr_int(ch)-5,5), sn );
	    }
	    continue;
	}

	if ( vch->in_room->area == ch->in_room->area
	&&   IS_OUTSIDE(vch)
	&&   IS_AWAKE(vch) )
	    send_to_char( "&-BLightning flashes wildly in the sky.&N\n\r", vch );
    }

    return;
}



void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = 10;
    af.location  = APPLY_SEX;

    do
    {
	af.modifier  = number_range( 0, 2 ) - victim->sex;
    } while ( af.modifier == 0 );

    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel different.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( victim == ch )
    {
	send_to_char( "You like yourself even better!\n\r", ch );
	return;
    }

    if ( IS_AFFECTED(victim, AFF_CHARM)
    ||   IS_AFFECTED(ch, AFF_CHARM)
    ||   level < victim->level
    ||   saves_spell( victim ) )
	return;

    if ( victim->master )
	stop_follower( victim );
    add_follower( victim, ch );
    af.type      = sn;
    af.duration  = number_range(2,4);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    act( "You like $n a lot.", ch, NULL, victim, TO_VICT );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( !saves_spell( victim ) && !is_affected(victim,sn) )
    {
	af.type      = sn;
	af.duration  = number_range(1,4);
	af.location  = APPLY_STR;
	af.modifier  = -1;
	af.bitvector = 0;
	affect_join( victim, &af );
    }

    act("&+B$n chills you.&N",	ch, NULL, victim, TO_VICT );
    act("&+BYou chill $N.&N",	ch, NULL, victim, TO_CHAR );
    act("&+B$n chills $N.&N",	ch, NULL, victim, TO_NOTVICT );
    damage( ch, victim, dice(get_curr_int(ch)/4,6), sn );
    return;
}



void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if( !saves_spell(victim) && !is_affected(victim,sn) )
    {
	    af.type		= sn;
	    af.duration		= 1;
	    af.location		= 0;
	    af.modifier		= 0;
	    af.bitvector	= AFF_BLIND;
	    affect_join( victim, &af );
    }

    act("You send an array of &+mc&+Mo&n&+cl&+Co&n&+gr&+Gs&N flying at $N!",
	    ch, NULL, victim, TO_CHAR);
    act("$n sends an array of &+mc&+Mo&+cl&+Co&+gr&+Gs&N flying at you!",
	    ch, NULL, victim, TO_VICT);
    act("$n sends an array of &+mc&+Mo&+cl&+Co&+gr&+Gs&N flying at $N!",
	    ch, NULL, victim, TO_NOTVICT);

    damage( ch, victim, dice(get_curr_int(ch)/2,7), sn );
    return;
}



void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *light;

    light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ) );
    obj_to_room( light, ch->in_room );
    act( "&+W$n twiddles $s thumbs and $p appears.&N",   ch, light, NULL, TO_ROOM );
    act( "&+WYou twiddle your thumbs and $p appears.&N", ch, light, NULL, TO_CHAR );
    return;
}



void spell_conjure_elemental( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;
	CHAR_DATA *mob;
	AFFECT_DATA af;
	bool is_pissed = FALSE;
	int roll = 0;

	for( mob = char_list; mob; mob = mob->next )
	{
		if( !IS_NPC(mob)
		||  strstr(mob->name,"elemental") == NULL
		||  mob->master != ch)
			continue;

		/* make getting another elemental VERY hard */
		if( number_percent() <= 3 )
			break;
		is_pissed = TRUE;
		break;
	}

	act("$n opens a rift to an outer plane and summons out an elemental!",
		ch, NULL, NULL, TO_ROOM );
	send_to_char("You open a rift up to an outer plane and summon an elemental!\n\r", ch );

	mob		= create_mobile( get_mob_index(MOB_VNUM_ELEMENTAL) );
	mob->level	= get_curr_int(ch)/4;
	spec		= new_spec_obj();

	roll = number_range(1,4) + (get_curr_int(ch) == 25 ? 1 : 0);
	switch( roll )
	{
	case 1: /* fire */
		mob->name		= str_dup( "fire elemental" );
		mob->short_descr	= str_dup( "&+ra fire elemental&N" );
		mob->long_descr		= str_dup( "&+rA fire elemental is standing here flaming.&N\n\r" );
		add_spec( "spec_elemental_fire", NULL, mob, NULL );
		SET_BIT(mob->affected_by, AFF_FIRESHIELD);
		break;
	case 2: /* earth */
		mob->name		= str_dup( "earth elemental" );
		mob->short_descr	= str_dup( "&+yan earth elemental&N" );
		mob->long_descr		= str_dup( "&+yAn earth elemental is standing here flexing.&N\n\r" );
		mob->level		+= 2;
		add_spec( "spec_elemental_earth", NULL, mob, NULL );
		break;
	case 3: /* water */
		mob->name		= str_dup( "water elemental" );
		mob->short_descr	= str_dup( "&+Ba water elemental&N" );
		mob->long_descr		= str_dup( "&+BA water elemental is standing here dripping.&N\n\r" );
		add_spec( "spec_elemental_water", NULL, mob, NULL );
		SET_BIT(mob->affected_by, AFF_SANCTUARY);
		break;
	case 4: /* air */
		mob->name		= str_dup( "air elemental" );
		mob->short_descr	= str_dup( "&+Wan air elemental&N" );
		mob->long_descr		= str_dup( "&+WAn air elemental is standing here whisping.&N\n\r" );
		add_spec( "spec_elemental_air", NULL, mob, NULL );
		SET_BIT(mob->affected_by, AFF_FLYING);
		SET_BIT(mob->affected_by, AFF_INVISIBLE);
		break;
	case 5: /* greater */
		mob->name		= str_dup( "greater elemental" );
		mob->short_descr	= str_dup( "&+Ca greater elemental&N" );
		mob->long_descr		= str_dup( "&+CA greater elemental is here.&N\n\r" );
		mob->level		= 15;
		add_spec( "spec_elemental_greater", NULL, mob, NULL );
		break;
	}


	mob->hit	= mob->max_hit = mob->level*6;
	is_pissed	= is_pissed ? 1 : saves_spell(mob);
	char_to_room( mob, ch->in_room );

	if( is_pissed )
	{
		act("$n arrives from the elemental plane, and looks VERY upset.",
			mob, NULL, NULL, TO_ROOM );
		multi_hit(mob,ch, TYPE_UNDEFINED);
		mob->hunting = ch;
		return;
	}

	act("$N steps out from the rift and bows before $n",
		ch, NULL, mob, TO_ROOM );
	act("$N steps out from the rift and bows before you.",
		ch, NULL, mob, TO_CHAR );

	mob->master	= ch;
	af.type		= sn;
	SET_BIT(mob->act, ACT_SERVANT);
	af.modifier	= 0;
	af.location	= 0;
	af.duration	= get_curr_int(ch)/2;
	af.bitvector	= AFF_SUMMONED;
	affect_to_char( mob, &af );
	return;
}



void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo )
{
    if ( !str_cmp( target_name, "better" ) )
	weather_info.change += dice( 12, 4 );
    else if ( !str_cmp( target_name, "worse" ) )
	weather_info.change -= dice( 12, 4 );
    else
	send_to_char ("Do you want it to get better or worse?\n\r", ch );

    send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *mushroom;

    mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ) );
    mushroom->value[0] = 1;
    obj_to_room( mushroom, ch->in_room );
    act( "&+y$p suddenly appears.&N", ch, mushroom, NULL, TO_ROOM );
    act( "&+y$p suddenly appears.&N", ch, mushroom, NULL, TO_CHAR );
    return;
}



void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *spring;

    spring = create_object( get_obj_index( OBJ_VNUM_SPRING ) );
    spring->timer = get_curr_wis(ch)/11;
    obj_to_room( spring, ch->in_room );
    act( "&+b$p flows from the ground.&N", ch, spring, NULL, TO_ROOM );
    act( "&+b$p flows from the ground.&N", ch, spring, NULL, TO_CHAR );
    return;
}



void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int water;

    if ( obj->item_type != ITEM_DRINK_CON )
    {
	send_to_char( "It is unable to hold water.\n\r", ch );
	return;
    }

    if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
    {
	send_to_char( "It contains some other liquid.\n\r", ch );
	return;
    }

    water = UMIN( number_range(1,3), obj->value[0] - obj->value[1] );
  
    if ( water > 0 )
    {
	obj->value[2] = LIQ_WATER;
	obj->value[1] += water;
	if ( !is_name( "water", obj->name ) )
	{
	    char buf[MAX_STRING_LENGTH];

	    sprintf( buf, "%s water", obj->name );
	    free_string( obj->name );
	    obj->name = str_dup( buf );
	}
	act( "$p is filled.", ch, obj, NULL, TO_CHAR );
    }

    return;
}



void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_blindness ) )
	return;
    affect_strip( victim, gsn_blindness );
    send_to_char( "&+BYour vision returns!&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    heal_char( victim, dice(get_curr_wis(ch)-6,4));
    update_pos( victim );
    send_to_char( "&+WYou feel better!&N\n\r", victim );
    act("&+W$n looks a little better.&N", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
   
    heal_char(ch, dice(get_curr_wis(ch)-14,4));
    update_pos( victim );
    send_to_char( "&+WYou feel better!&N\n\r", victim );
    act("&+W$n looks a little better.&N", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( is_affected( victim, gsn_poison ) )
    {
	affect_strip( victim, gsn_poison );
	act( "&+y$N looks better.&N", ch, NULL, victim, TO_NOTVICT );
	send_to_char( "&+yA warm feeling runs through your body.&N\n\r", victim );
	send_to_char( "Ok.\n\r", ch );
    }
    return;
}



void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
   
    heal_char(ch,dice(get_curr_wis(ch)-10,4));
    update_pos( victim );
    send_to_char( "&+WYou feel better!&N\n\r", victim );
    act("&+W$n looks a little better.&N", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_CURSE) || saves_spell( victim ) )
	return;
    af.type      = sn;
    af.duration  = 4;
    af.location  = APPLY_HITROLL;
    af.modifier  = number_range(-(get_curr_int(ch)/2),-3);
    af.bitvector = AFF_CURSE;
    affect_to_char( victim, &af );

    af.location  = APPLY_SAVING_SPELL;
    af.modifier  = number_range(1,4);
    affect_to_char( victim, &af );

    send_to_char( "&+rYou feel unclean.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_EVIL) )
	return;
    af.type      = sn;
    af.duration  = number_range(1,15);
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_HIDDEN) )
	return;
    af.type      = sn;
    af.duration  = number_range(1,15);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );
    send_to_char( "&+yYour awareness improves.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_INVIS) )
	return;
    af.type      = sn;
    af.duration  = number_range(1,15);
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    send_to_char( "&+BYour eyes tingle.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_DETECT_MAGIC) )
	return;
    af.type      = sn;
    af.duration  = number_range(1,15);
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_MAGIC;
    affect_to_char( victim, &af );
    send_to_char( "Your eyes tingle.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;

    if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
    {
	if ( obj->value[3] != 0 )
	    send_to_char( "You smell poisonous fumes.\n\r", ch );
	else
	    send_to_char( "It looks very delicious.\n\r", ch );
    }
    else
    {
	send_to_char( "It doesn't look poisoned.\n\r", ch );
    }

    return;
}



void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
 
    if( !IS_EVIL(victim) )
	    victim = ch;

    act("Borlak smites $N with holy power.",	ch, NULL, victim, TO_NOTVICT );
    act("Borlak smites $N with holy power.",	ch, NULL, victim, TO_CHAR );
    act("Borlak smites you with holy power.",	ch, NULL, victim, TO_VICT );

    damage( ch, victim, dice(4,get_curr_wis(ch)*2), sn );
    return;
}



void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( victim->affected_by == 0
    ||   saves_spell( victim ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    victim->affected_by = 0;
    send_to_char( "Ok.\n\r", ch );

    return;
}



void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char( "The earth &-Ytrembles&N beneath your feet!\n\r", ch );
    act( "$n makes the earth &-Ytremble and shiver&N.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
		act("$n is knocked all around the room!", vch, NULL, NULL, TO_ROOM );
		send_to_char("You are knocked all around the room!\n\r", vch );
		vch->position = POS_RESTING;
		damage( ch, vch, dice(get_curr_wis(ch)/4, 8), sn );
		continue;
	}

	if ( vch->in_room->area == ch->in_room->area )
	    send_to_char( "The earth &-Ytrembles and shivers&N.\n\r", vch );
    }

    return;
}



void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA *paf;

    if ( obj->item_type != ITEM_WEAPON
    ||   IS_OBJ_STAT(obj, ITEM_MAGIC)
    ||   obj->affected != NULL )
    {
	    send_to_char("The object is already glowing.\n\r", ch );
	    return;
    }

    if ( affect_free == NULL )
    {
	paf		= alloc_perm( sizeof(*paf) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= -1;
    paf->duration	= -1;
    paf->location	= APPLY_HITROLL;
    paf->modifier	= number_range(1,get_curr_int(ch)/3);
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( affect_free == NULL )
    {
	paf		= alloc_perm( sizeof(*paf) );
    }
    else
    {
	paf		= affect_free;
	affect_free	= affect_free->next;
    }

    paf->type		= -1;
    paf->duration	= -1;
    paf->location	= APPLY_DAMROLL;
    paf->modifier	= number_range(1,get_curr_int(ch)/3);
    paf->bitvector	= 0;
    paf->next		= obj->affected;
    obj->affected	= paf;

    if ( IS_GOOD(ch) )
    {
	SET_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
	act( "&+B$p glows blue.&N", ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_EVIL(ch) )
    {
	SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);
	act( "&+R$p glows red.&N", ch, obj, NULL, TO_CHAR );
    }
    else
    {
	SET_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
	SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);
	act( "&+Y$p glows yellow.&N", ch, obj, NULL, TO_CHAR );
    }
    return;
}



/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if ( saves_spell( victim ) )
	return;

    victim->mana	/= 2;
   
    ch->hit		+= number_range(5,(get_curr_int(ch)+get_curr_wis(ch))/2);

    damage( ch, victim, dice(get_curr_wis(ch)/4,get_curr_int(ch)/4), sn );

    return;
}


void spell_entangling_vines( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;
	CHAR_DATA *victim = (CHAR_DATA *)vo;

	if( has_spec( "spec_vines", ch->in_room->spec_obj ) )
	{
		send_to_char("The room is already covered in vines!\n\r", ch );
		return;
	}

	spec = add_spec( "spec_vines", ch->in_room, victim, NULL );

	spec->timer = number_range( get_curr_wis(ch)/2, get_curr_wis(ch) );

	act("&+YEntangling vines thrust out of the ground!&N",
		ch, NULL, NULL, TO_CHAR );
	act("&+YEntangling vines thrust out of the ground!&N",
		ch, NULL, NULL, TO_ROOM );
	return;
}


void spell_finger_of_death( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;
	CHAR_DATA *victim = (CHAR_DATA *)vo;

	if( has_spec( "spec_death", victim->spec_obj ) )
	{
		send_to_char("You failed.\n\r", ch );
		return;
	}

	spec		= add_spec( "spec_death", NULL, victim, NULL );
	spec->timer	= get_curr_int(ch);
	spec->vch	= ch;

	send_to_char("&+LYou feel doomed.&N\n\r", victim );
	act("&+L$n is doomed.&N", victim, NULL, NULL, TO_ROOM );
}



void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act("$n's &+rfireball&n smashes into your chest with deadly impact!",
	    ch, NULL, victim, TO_VICT );
    act("Your &+rfireball&n smashes into $N's chest with deadly impact!",
	    ch, NULL, victim, TO_CHAR );
    act("$n's &+rfireball&n smashes into $N's chest with deadly impact!",
	    ch, NULL, victim, TO_NOTVICT );

    damage( ch, victim, dice(get_curr_int(ch), get_curr_int(ch)/4)+5, sn );
    return;
}


void spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo )
{
	AFFECT_DATA af;

	if( is_affected(ch, sn))
		return;

	send_to_char("&+RYou are surrounded by a flaming sphere.&N\n\r", ch );
	act("&+R$n is surrounded by a flaming sphere.&N", ch, NULL, NULL, TO_ROOM );

	af.type		= sn;
	af.duration	= get_curr_int(ch)/4;
	af.location	= 0;
	af.modifier	= 0;
	af.bitvector	= AFF_FIRESHIELD;
	affect_to_char( ch, &af );
	return;
}


void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act("&+R$n calls down flames from the heavens and torches $N!&N",
	    ch, NULL, victim, TO_NOTVICT );
    act("&+RYou call down flames from the heavens and torch $N!&N",
	    ch, NULL, victim, TO_CHAR );
    act("&+R$n calls down flames from the heavens and torches you!&N",
	    ch, NULL, victim, TO_VICT );

    damage( ch, victim, dice(get_curr_wis(ch)/3,get_curr_wis(ch)/2), sn );
    return;
}



void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
	return;
    af.type      = sn;
    af.duration  = number_range(2,get_curr_wis(ch)/4);
    af.location  = APPLY_AC;
    af.modifier  = number_range(10,get_curr_wis(ch)*3);
    af.bitvector = AFF_FAERIE_FIRE;
    affect_to_char( victim, &af );
    send_to_char( "You are surrounded by a &+Rpink outline&N.\n\r", victim );
    act( "$n is surrounded by a &+Rpink outline&N.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *ich;

    act( "$n conjures a cloud of &+mpurple smoke&n.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You conjure a cloud of &+mpurple smoke&n.\n\r", ch );

    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {
	if ( !IS_NPC(ich) && IS_SET(ich->act, PLR_WIZINVIS) )
	    continue;

	if ( ich == ch || saves_spell( ich ) )
	    continue;

	affect_strip ( ich, gsn_invis			);
	affect_strip ( ich, gsn_mass_invis		);
	affect_strip ( ich, gsn_sneak			);
	REMOVE_BIT   ( ich->affected_by, AFF_HIDE	);
	REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE	);
	REMOVE_BIT   ( ich->affected_by, AFF_SNEAK	);
	act( "&+M$n is revealed!&N", ich, NULL, NULL, TO_ROOM );
	send_to_char( "&+MYou are revealed!&N\n\r", ich );
    }

    return;
}


void spell_feeblemind( int sn, int level, CHAR_DATA *ch, void *vo )
{
	CHAR_DATA *victim = (CHAR_DATA *)vo;
	AFFECT_DATA af;

	if( is_affected(victim,sn) || saves_spell(victim) || saves_spell(victim) 
	||  IS_NPC(victim) )
	{
		act("$n's mind resists the spell!", victim, NULL, NULL, TO_ROOM );
		send_to_char("Your mind resists the spell!\n\r", victim );
		return;
	}

	af.type		= sn;
	af.duration	= 1;
	af.location	= APPLY_INT;
	af.modifier	= -(class_table[ch->class].stat_int-1);
	af.bitvector	= 0;
	affect_to_char( victim, &af );

	af.location	= APPLY_WIS;
	af.modifier	= -(class_table[ch->class].stat_wis-1);
	affect_to_char( victim, &af );

	act("$n stumbles around looking dumbfounded.",
		victim, NULL, NULL, TO_ROOM );
	send_to_char("A terrible amnesia overcomes you.", victim );
	return;
}




void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_FLYING) )
	return;
    af.type      = sn;
    af.duration  = 5;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );
    send_to_char( "Your feet rise off the ground.\n\r", victim );
    act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
    return;
}



void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    return;
}



void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = number_range(2,5);
    af.location  = APPLY_STR;
    af.modifier  = number_range(1,get_curr_int(ch)/4);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel &+Gstronger&N.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}




void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    victim->hit = UMIN( victim->hit + get_curr_wis(ch)*5, victim->max_hit );
    update_pos( victim );
    send_to_char( "&+BA warm feeling fills your body.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf( buf,
	"Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",

	obj->name,
	item_type_name( obj ),
	extra_bit_name( obj->extra_flags ),
	obj->weight,
	obj->cost,
	obj->level
	);
    send_to_char( buf, ch );

    switch ( obj->item_type )
    {
    case ITEM_SCROLL: 
    case ITEM_POTION:
	sprintf( buf, "Level %d spells of:", obj->value[0] );
	send_to_char( buf, ch );

	if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[1]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[2]].name, ch );
	    send_to_char( "'", ch );
	}

	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;

    case ITEM_WAND: 
    case ITEM_STAFF: 
	sprintf( buf, "Has %d(%d) charges of level %d",
	    obj->value[1], obj->value[2], obj->value[0] );
	send_to_char( buf, ch );
      
	if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
	{
	    send_to_char( " '", ch );
	    send_to_char( skill_table[obj->value[3]].name, ch );
	    send_to_char( "'", ch );
	}

	send_to_char( ".\n\r", ch );
	break;
      
    case ITEM_WEAPON:
	sprintf( buf, "Damage is %d to %d (average %d).\n\r",
	    obj->value[1], obj->value[2],
	    ( obj->value[1] + obj->value[2] ) / 2 );
	send_to_char( buf, ch );
	break;

    case ITEM_ARMOR:
	sprintf( buf, "Armor class is %d.\n\r", obj->value[0] );
	send_to_char( buf, ch );
	break;
    }

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	{
	    sprintf( buf, "Affects %s by %d.\n\r",
		affect_loc_name( paf->location ), paf->modifier );
	    send_to_char( buf, ch );
	}
    }

    return;
}



void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_INFRARED) )
	return;
    act( "&+R$n's eyes glow red.&N\n\r", ch, NULL, NULL, TO_ROOM );
    af.type      = sn;
    af.duration  = number_range(1,15);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );
    send_to_char( "&+RYour eyes glow red.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_INVISIBLE) )
	return;

    act( "&+L$n fades out of existence.&N", victim, NULL, NULL, TO_ROOM );
    af.type      = sn;
    af.duration  = number_range(1,get_curr_int(ch)/5);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( victim, &af );
    send_to_char( "&+LYou fade out of existence.&N\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    char *msg;
    int ap;

    ap = victim->alignment;

         if ( ap >  700 ) msg = "&+W$N has an aura as white as the driven snow.&N";
    else if ( ap >  350 ) msg = "&+C$N is of excellent moral character.&N";
    else if ( ap >  100 ) msg = "&+c$N is often kind and thoughtful.&N";
    else if ( ap > -100 ) msg = "$N doesn't have a firm moral commitment.&N";
    else if ( ap > -350 ) msg = "&+r$N lies to $S friends.&N";
    else if ( ap > -700 ) msg = "&+R$N's slash DESTROYS you!&N";
    else msg = "I'd rather just not say anything at all about $N.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}



void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act("&+BAn arching bolt of lightning shoots from $n's hands and shocks you!&N",
	    ch, NULL, victim, TO_VICT );
    act("&+BAn arching bolt of lightning shoots from your hands and shocks $N!&N",
	    ch, NULL, victim, TO_CHAR );
    act("&+BAn arching bolt of lightning shoots from $n's hands and shocks $N!&N",
	    ch, NULL, victim, TO_NOTVICT );

    damage( ch, victim, dice(3,get_curr_int(ch)), sn );
    return;
}

	
void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo )
{
    char buf[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;

    found = FALSE;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
	if ( !can_see_obj( ch, obj ) || !is_name( target_name, obj->name ) )
	    continue;

	found = TRUE;

	for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
	    ;

	if ( in_obj->carried_by != NULL )
	{
	    sprintf( buf, "%s carried by %s.\n\r",
		obj->short_descr, PERS(in_obj->carried_by, ch) );
	}
	else
	{
	    sprintf( buf, "%s in %s.\n\r",
		obj->short_descr, in_obj->in_room == NULL
		    ? "somewhere" : in_obj->in_room->name );
	}

	buf[0] = UPPER(buf[0]);
	send_to_char( buf, ch );
    }

    if ( !found )
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );

    return;
}



void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int i, high;

    high = number_range(1,get_curr_int(ch)/4);

    for( i = 0; i < high && victim; i++ )
    {
	    act("&+YA magic missile flies from your fingertip towards $N!&N",
		    ch, NULL, victim, TO_CHAR );
	    act("&+YA magic missile flies from $n's fingertip towards you!&N",
		    ch, NULL, victim, TO_VICT );
	    act("&+YA magic missile flies from $n's fingertip towards $N!&N",
		    ch, NULL, victim, TO_NOTVICT );
	    damage( ch, victim, dice(2,4), sn );
    }
    return;
}



void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) || IS_AFFECTED(gch, AFF_INVISIBLE) )
	    continue;
	act( "&+L$n slowly fades out of existence.&N", gch, NULL, NULL, TO_ROOM );
	send_to_char( "&+LYou slowly fade out of existence.&N\n\r", gch );
	af.type      = sn;
	af.duration  = 1;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_INVISIBLE;
	affect_to_char( gch, &af );
    }
    send_to_char( "Ok.\n\r", ch );

    return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo )
{
    send_to_char( "That's not a spell!\n\r", ch );
    return;
}



void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
	return;
    af.type      = sn;
    af.duration  = number_range(1, get_curr_int(ch)/4) ;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( victim, &af );
    act( "&+L$n turns translucent.&N", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+LYou turn translucent.&N\n\r", victim );
    return;
}



void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( saves_spell( victim ) )
    {
	    act("$N resists the affect of the spell!", victim, NULL, NULL, TO_ROOM );
	    send_to_char("You resist the affect of the spell!\n\r", victim );
	    return;
    }
    af.type      = sn;
    af.duration  = number_range(1,get_curr_wis(ch)/3);
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = AFF_POISON;
    affect_join( victim, &af );
    send_to_char( "You feel very sick.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



void spell_protection( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_PROTECT) )
	return;
    af.type      = sn;
    af.duration  = number_range(1,get_curr_wis(ch)/3);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PROTECT;
    affect_to_char( victim, &af );

    act("&+BA blue aura surrounds $N.&N",  victim, NULL, NULL, TO_ROOM );
    send_to_char("&+BYou are surrounded by a blue aura.&N\n\r", victim );
    return;
}



void spell_reflect( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;

	if( saves_spell(ch) || has_spec( "spec_reflect", ch->spec_obj ) )
	{
		send_to_char("You failed.\n\r", ch );
		return;
	}

	spec		= add_spec( "spec_reflect", NULL, ch, NULL );
	spec->timer	= number_range( get_curr_int(ch)/2, get_curr_int(ch) );

	act("Energy flows around you and melds together to form a magical shield.",
		ch, NULL, NULL, TO_CHAR );
	act("Energy flows around &n and melds together to form a magical shield.",
		ch, NULL, NULL, TO_ROOM );
	return;
}



void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( is_affected( victim, gsn_curse ) )
    {
	affect_strip( victim, gsn_curse );
	send_to_char( "You feel better.\n\r", victim );
	if ( ch != victim )
	    send_to_char( "Ok.\n\r", ch );
    }

    return;
}



void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
	return;
    af.type      = sn;
    af.duration  = number_fuzzy( level / 8 );
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );
    act( "&+W$n is surrounded by a white aura.&N", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+WYou are surrounded by a white aura.&N\n\r", victim );
    return;
}



void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) )
	return;
    af.type      = sn;
    af.duration  = number_range(1, get_curr_int(ch)/3);
    af.location  = APPLY_AC;
    af.modifier  = number_range(-(get_curr_int(ch))*2, -10 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "You are surrounded by a force shield.\n\r", victim );
    return;
}



void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    act("&+C$n grasps $N and gives $m a maddening shock!&N",   ch, NULL, victim, TO_NOTVICT );
    act("&+C$n grasps you and gives you a maddening shock!&N", ch, NULL, victim, TO_VICT );
    act("&+CYou grasp $N and give $m a maddening shock!&N",    ch, NULL, victim, TO_CHAR );

    damage( ch, victim, number_range(10, get_curr_int(ch)*2), sn );
    return;
}



void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  
    if ( IS_AFFECTED(victim, AFF_SLEEP)
    ||   saves_spell( victim ) )
	return;

    af.type      = sn;
    af.duration  = 1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_join( victim, &af );

    if ( IS_AWAKE(victim) )
    {
	send_to_char( "All goes black...\n\r", victim );
	act( "$n wavers somewhat, then falls to the ground.", victim, NULL, NULL, TO_ROOM );
	victim->position = POS_SLEEPING;
    }

    return;
}



void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
	return;
    af.type      = sn;
    af.duration  = number_range(1,get_curr_int(ch)/5);
    af.location  = APPLY_AC;
    af.modifier  = number_range(-(get_curr_int(ch))*3, -30 );
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "&+L$n's skin turns to stone.&N", victim, NULL, NULL, TO_ROOM );
    send_to_char( "&+LYour skin turns to stone.&N\n\r", victim );
    return;
}



void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim;

    if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   victim->level >= level + 3
    ||   victim->in_room->area != ch->in_room->area
    ||   (!HAS_CONSENT(ch,victim) && saves_spell( victim )) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    if( victim->fighting )  /* for teamwork */
	    stop_fighting(victim, TRUE);

    act( "&+W$n disappears suddenly.&N", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "&+W$n arrives suddenly.&N", victim, NULL, NULL, TO_ROOM );
    act( "&+W$N has summoned you!&N", victim, NULL, ch,   TO_CHAR );
    do_look( victim, "auto" );
    return;
}



/* area damage spell with multiplier for undead */
void spell_sunray( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec = NULL;
	CHAR_DATA *victim = (CHAR_DATA *)vo;

	act("The &+Ysun&N glows &+Wbright&N with power and a powerful ray begins heading this way!",
		ch, NULL, NULL, TO_ROOM );
	send_to_char( "The &+Ysun&N glows &+Wbright&N with power and a powerful ray begins heading this way!\n\r", ch);

	spec		= add_spec( "spec_sunray", NULL, victim, NULL );
	spec->timer	= number_fuzzy(3);
	spec->vch	= ch;
	return;
}	



void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    ROOM_INDEX_DATA *pRoomIndex;
    extern top_room_vnum;

    if ( victim->in_room == NULL
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    || ( !IS_NPC(ch) && victim->fighting != NULL )
    || ( war_type == WAR_CITY
    ||   war_type == WAR_PIT )
    || ( victim != ch
    && ( saves_spell( victim ) || saves_spell( victim ) ) ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    for ( ; ; )
    {
	pRoomIndex = get_room_index( number_range( 0, top_room_vnum ) );
	if ( pRoomIndex != NULL )
	if ( !IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)
	&&   !IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) )
	    break;
    }

    act( "&+L$n slowly fades out of existence.&N", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, pRoomIndex );
    act( "&+L$n slowly fades into existence.&N", victim, NULL, NULL, TO_ROOM );
    do_look( victim, "auto" );
    return;
}



void spell_tornado( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec;

	if( ch->fighting && saves_spell(ch->fighting) )
	{
		send_to_char("You failed.\n\r", ch );
		return;
	}

	act("The wind around you begins to speed up and out if it's power a giant swirling tornado is created!",
		ch, NULL, NULL, TO_ROOM );
	act("The wind around you begins to speed up and out if it's power a giant swirling tornado is created!",
		ch, NULL, NULL, TO_CHAR );

	spec		= add_spec( "spec_tornado", ch->in_room, ch, NULL );
	spec->timer	= number_range( get_curr_wis(ch)/2, get_curr_wis(ch) );
	return;
}



void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo )
{
    char buf1[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char speaker[MAX_INPUT_LENGTH];
    CHAR_DATA *vch;

    target_name = one_argument( target_name, speaker );

    sprintf( buf1, "%s says '%s'.\n\r",              speaker, target_name );
    sprintf( buf2, "Someone makes %s say '%s'.\n\r", speaker, target_name );
    buf1[0] = UPPER(buf1[0]);

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
	if ( !is_name( speaker, vch->name ) )
	    send_to_char( saves_spell(  vch ) ? buf2 : buf1, vch );
    }

    return;
}


void spell_wall_of_earth( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec = NULL;
	bool found = FALSE;
	int dir;
	extern const char *dir_name [];
	int value = 0;

	for( dir = 0; dir < 6; dir++ )
	{
		if( !str_cmp(dir_name[dir], target_name) )
		{
			found = TRUE;
			break;
		}
	}
	if(!found)
	{
		send_to_char("Cast a wall of earth in what direction?\n\r", ch );
		return;
	}

	switch( dir )
	{
	case DIR_NORTH:	value = DIR_NORTH;	break;
	case DIR_SOUTH:	value = DIR_SOUTH;	break;
	case DIR_EAST:	value = DIR_EAST;	break;
	case DIR_WEST:	value = DIR_WEST;	break;
	default:
		send_to_char("You cannot create a wall in that direction.\n\r", ch );
		return;
	}

	for( spec = ch->in_room->spec_obj; spec; spec = spec->next )
	{
		if( spec->value[0] == value )
		{
			send_to_char("There is already a wall in that direction.\n\r", ch );
			return;
		}
	}

	
	if( (spec = add_spec("spec_wall", ch->in_room, NULL, NULL)) == NULL )
	{
		send_to_char("Null room object, tell an immortal.\n\r", ch );
		return;
	}

	spec->value[0]	= value;
	spec->timer	= number_range(5, get_curr_int(ch));
	spec->ch	= ch;

	act("&+yA giant wall of earth crawls out of the ground on the $tern wall.&N", ch, dir_name[dir], NULL, TO_ROOM );
	act("&+yA giant wall of earth crawls out of the ground on the $tern wall.&N", ch, dir_name[dir], NULL, TO_CHAR );
	return;
}



void spell_wall_of_fire( int sn, int level, CHAR_DATA *ch, void *vo )
{
	SPEC_OBJ_DATA *spec = NULL;
	bool found = FALSE;
	int dir;
	extern const char *dir_name [];
	int value = 0;

	for( dir = 0; dir < 6; dir++ )
	{
		if( !str_cmp(dir_name[dir], target_name) )
		{
			found = TRUE;
			break;
		}
	}
	if(!found)
	{
		send_to_char("Cast a wall of fire in what direction?\n\r", ch );
		return;
	}

	switch( dir )
	{
	case DIR_NORTH:	value = DIR_NORTH;	break;
	case DIR_SOUTH:	value = DIR_SOUTH;	break;
	case DIR_EAST:	value = DIR_EAST;	break;
	case DIR_WEST:	value = DIR_WEST;	break;
	default:
		send_to_char("You cannot create a wall in that direction.\n\r", ch );
		return;
	}
	

	for( spec = ch->in_room->spec_obj; spec; spec = spec->next )
	{
		if( spec->value[0] == value )
		{
			send_to_char("There is already a wall in that direction.\n\r", ch );
			return;
		}
	}

	if( (spec = add_spec("spec_wall", ch->in_room, NULL, NULL)) == NULL )
	{
		send_to_char("Null room object, tell an immortal.\n\r", ch );
		return;
	}

	spec->value[0]	= value;
	spec->value[1]	= TRUE;  /* fire */
	spec->timer	= number_range(5, get_curr_int(ch));
	spec->ch	= ch;

	act("&+RA flaming wall flares up on the $tern wall.&N", ch, dir_name[dir], NULL, TO_ROOM );
	act("&+RA flaming wall flares up on the $tern wall.&N", ch, dir_name[dir], NULL, TO_CHAR );
	return;
}


void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ) || saves_spell( victim ) )
	return;

    af.type      = sn;
    af.duration  = number_range(1,get_curr_int(ch)/5);
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You feel weaker.\n\r", victim );
    if ( ch != victim )
	send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * This is for muds that _want_ scrolls of recall.
 * Ick.
 */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo )
{
    ROOM_INDEX_DATA *location;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    || ( war_type == WAR_CITY
    ||   war_type == WAR_PIT )
    || ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL
    || ( IS_AFFECTED(ch, AFF_CURSE) ) )
    {
	send_to_char( "You fail.\n\r", ch );
	return;
    }

    if ( ch->in_room == location )
	return;

    act( "&+W$n disappears.&N", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "&+W$n appears in the room.&N", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    return;
}



/*
 * NPC spells.
 */
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose;
    OBJ_DATA *obj_next;
    int dam;
    int hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( victim ) )
    {
	for ( obj_lose = ch->carrying; obj_lose != NULL; obj_lose = obj_next )
	{
	    int iWear;

	    obj_next = obj_lose->next_content;

	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    case ITEM_ARMOR:
		if ( obj_lose->value[0] > 0 )
		{
		    act( "$p is pitted and etched!",
			victim, obj_lose, NULL, TO_CHAR );
		    if ( ( iWear = obj_lose->wear_loc ) != WEAR_NONE )
			victim->armor -= apply_ac( obj_lose, iWear );
		    obj_lose->value[0] -= 1;
		    obj_lose->cost      = 0;
		    if ( iWear != WEAR_NONE )
			victim->armor += apply_ac( obj_lose, iWear );
		}
		break;

	    case ITEM_CONTAINER:
		act( "$p fumes and dissolves!",
		    victim, obj_lose, NULL, TO_CHAR );
		extract_obj( obj_lose );
		break;
	    }
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose;
    OBJ_DATA *obj_next;
    int dam;
    int hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose != NULL;
	obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:             continue;
	    case ITEM_CONTAINER: msg = "$p ignites and burns!";   break;
	    case ITEM_POTION:    msg = "$p bubbles and boils!";   break;
	    case ITEM_SCROLL:    msg = "$p crackles and burns!";  break;
	    case ITEM_STAFF:     msg = "$p smokes and chars!";    break;
	    case ITEM_WAND:      msg = "$p sparks and sputters!"; break;
	    case ITEM_FOOD:      msg = "$p blackens and crisps!"; break;
	    case ITEM_PILL:      msg = "$p melts and drips!";     break;
	    }

	    act( msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj_lose;
    OBJ_DATA *obj_next;
    int dam;
    int hpch;

    if ( number_percent( ) < 2 * level && !saves_spell( victim ) )
    {
	for ( obj_lose = victim->carrying; obj_lose != NULL;
	obj_lose = obj_next )
	{
	    char *msg;

	    obj_next = obj_lose->next_content;
	    if ( number_bits( 2 ) != 0 )
		continue;

	    switch ( obj_lose->item_type )
	    {
	    default:            continue;
	    case ITEM_CONTAINER:
	    case ITEM_DRINK_CON:
	    case ITEM_POTION:   msg = "$p freezes and shatters!"; break;
	    }

	    act( msg, victim, obj_lose, NULL, TO_CHAR );
	    extract_obj( obj_lose );
	}
    }

    hpch = UMAX( 10, ch->hit );
    dam  = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}



void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;
    int hpch;

    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;
	if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
	{
	    hpch = UMAX( 10, ch->hit );
	    dam  = number_range( hpch/16+1, hpch/8 );
	    if ( saves_spell(  vch ) )
		dam /= 2;
	    damage( ch, vch, dam, sn );
	}
    }
    return;
}



void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    int hpch;

    hpch = UMAX( 10, ch->hit );
    dam = number_range( hpch/16+1, hpch/8 );
    if ( saves_spell( victim ) )
	dam /= 2;
    damage( ch, victim, dam, sn );
    return;
}
