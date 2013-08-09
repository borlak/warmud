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



char *	const	where_name	[] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on body>      ",
    "<worn on head>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<held>              ",
    "<quiver>            "
};



/*
 * Local functions.
 */
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
				    bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
				    bool fShort, bool fShowNothing ) );
void	show_char_to_char_0	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char_1	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	show_char_to_char	args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool	check_blind		args( ( CHAR_DATA *ch ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ( fShort )
    {
	if ( obj->short_descr != NULL )
	    strcat( buf, obj->short_descr );
    }
    else
    {
	if ( obj->description != NULL )
	    strcat( buf, obj->description );
    }

    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "&+L...invis&N"     );
    if ( IS_AFFECTED(ch, AFF_DETECT_EVIL)
         && IS_OBJ_STAT(obj, ITEM_EVIL)   )   strcat( buf, "&+r...red aura&N"  );
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "&+B...magical&N"   );
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, "&+W...glowing&N"   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "&+y...humming&N"   );

    update_specs( ch->in_room, (void*)buf, NULL, NULL, UPDATE_OBJ, UPDATE_LOOK );

    return buf;
}



/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
	return;

    /*
     * Alloc space for output lines.
     */
    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
	count++;
    prgpstrShow	= alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow	= 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    { 
	if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
	{
	    pstrShow = format_obj_to_char( obj, ch, fShort );
	    fCombine = FALSE;

	    if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    {
		/*
		 * Look for duplicates, case sensitive.
		 * Matches tend to be near end so run loop backwords.
		 */
		for ( iShow = nShow - 1; iShow >= 0; iShow-- )
		{
		    if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
		    {
			prgnShow[iShow]++;
			fCombine = TRUE;
			break;
		    }
		}
	    }

	    /*
	     * Couldn't combine, or didn't want to.
	     */
	    if ( !fCombine )
	    {
		prgpstrShow [nShow] = str_dup( pstrShow );
		prgnShow    [nShow] = 1;
		nShow++;
	    }
	}
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	{
	    if ( prgnShow[iShow] != 1 )
	    {
		sprintf( buf, "(%2d) ", prgnShow[iShow] );
		send_to_char( buf, ch );
	    }
	    else
	    {
		send_to_char( "     ", ch );
	    }
	}
	send_to_char( prgpstrShow[iShow], ch );
	send_to_char( "\n\r", ch );
	free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
	if ( IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE) )
	    send_to_char( "     ", ch );
	send_to_char( "Nothing.\n\r", ch );
    }

    /*
     * Clean up.
     */
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}


void add_char_aff_to_buf( CHAR_DATA *ch, CHAR_DATA *victim, char *buf )
{
    static char * const he_she	[] = { "It",  "He",  "She" };
    static char * const his_her	[] = { "its",  "his", "her" };

    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) 
	sprintf(buf+strlen(buf),"&+c...%s is invisible&N\n\r",			he_she[victim->sex] );

    if ( IS_AFFECTED(victim, AFF_HIDE)        )
	sprintf(buf+strlen(buf),"&+L...%s is hiding in the shadows&N\n\r",	he_she[victim->sex] );

    if ( IS_AFFECTED(victim, AFF_CHARM)       )
	sprintf(buf+strlen(buf),"&+B...%s has a blank stare on %s face&N\n\r",	he_she[victim->sex],
										his_her[victim->sex] );
    if( IS_AFFECTED(victim, AFF_FIRESHIELD)   )
	sprintf(buf+strlen(buf),"&+R...%s is encased in a flaming sphere&N\n\r", he_she[victim->sex] );

    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   )
	sprintf(buf+strlen(buf),"&+c...%s is wavering before your eyes&N\n\r",	he_she[victim->sex] );

    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
	sprintf(buf+strlen(buf),"&+R...%s is surrounded by a pink aura&N\n\r",	he_she[victim->sex] );

    if ( IS_EVIL(victim)
    &&   IS_AFFECTED(ch, AFF_DETECT_EVIL)     )
	sprintf(buf+strlen(buf),"&+r...%s is surrounded by a red aura&N\n\r",	he_she[victim->sex] );

    if ( IS_AFFECTED(victim, AFF_SANCTUARY)   )
	sprintf(buf+strlen(buf),"&+W...%s is surrounded by a white aura&N\n\r",	he_she[victim->sex] );

    if( IS_AFFECTED(victim, AFF_BLACKMANTLE)  )
	sprintf(buf+strlen(buf),"&+L...%s is surrounded by a black aura&N\n\r", he_she[victim->sex] );

    update_specs( ch->in_room, (void*)ch, NULL, NULL, UPDATE_MOB, UPDATE_LOOK );
}


void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    CHAR_DATA *mount;
    char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if( IN_WAR(victim) && !IS_IMMORTAL(victim) && !IS_NPC(victim) )
	    strcat(buf,"&+R(IN_WAR)&N");

    if ( victim->position == POS_STANDING && victim->long_descr[0] != '\0' )
    {
	strcat( buf, victim->long_descr );
	add_char_aff_to_buf( ch, victim, buf );
	send_to_char( buf, ch );
	return;
    }

    strcat( buf, PERS( victim, ch ) );

    if( (mount = find_mount(victim)) != NULL )
    {
	    sprintf(buf+strlen(buf)," is riding %s", PERS( mount, ch ) );
	    if( ch->position == POS_FIGHTING )
	    {
		    strcat(buf," and fighting " );
		    if( victim->fighting == NULL )
			    strcat( buf, "thin air??" );
		    else if ( victim->fighting == ch )
			    strcat( buf, "YOU!" );
		    else if( victim->in_room == victim->fighting->in_room )
		    {
			    strcat( buf, PERS( victim->fighting, ch ) );
			    strcat( buf, "." );
		    }
		    else
			    strcat( buf, "someone who left?" );
	    }
	    else
		    strcat(buf, "." );
    }
    else if( victim->mounted_by != NULL )
    {
	    /* victim should show in room from whos mounting him */
	    return;
    }
    else
    {
	    switch ( victim->position )
	    {
	    case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
	    case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
	    case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
	    case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
	    case POS_SLEEPING: strcat( buf, " is sleeping here." );      break;
	    case POS_RESTING:  strcat( buf, " is resting here." );       break;
	    case POS_STANDING: strcat( buf, " is here." );               break;
	    case POS_FIGHTING:
		strcat( buf, " is here, fighting " );
		if ( victim->fighting == NULL )
		    strcat( buf, "thin air??" );
		else if ( victim->fighting == ch )
		    strcat( buf, "YOU!" );
		else if ( victim->in_room == victim->fighting->in_room )
		{
		    strcat( buf, PERS( victim->fighting, ch ) );
		    strcat( buf, "." );
		}
		else
		    strcat( buf, "somone who left??" );
		break;
	    }
    }
    strcat( buf, "\n\r" );
    buf[0] = UPPER(buf[0]);
    add_char_aff_to_buf( ch, victim, buf );
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int iWear;
    int percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
	act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
	act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }

    if ( victim->description[0] != '\0' )
    {
	send_to_char( victim->description, ch );
    }
    else
    {
	act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    if ( victim->max_hit > 0 )
	percent = ( 100 * victim->hit ) / victim->max_hit;
    else
	percent = -1;

    strcpy( buf, PERS(victim, ch) );

         if ( percent >= 100 ) strcat( buf, " is in perfect health.\n\r"  );
    else if ( percent >=  90 ) strcat( buf, " is slightly scratched.\n\r" );
    else if ( percent >=  80 ) strcat( buf, " has a few bruises.\n\r"     );
    else if ( percent >=  70 ) strcat( buf, " has some cuts.\n\r"         );
    else if ( percent >=  60 ) strcat( buf, " has several wounds.\n\r"    );
    else if ( percent >=  50 ) strcat( buf, " has many nasty wounds.\n\r" );
    else if ( percent >=  40 ) strcat( buf, " is bleeding freely.\n\r"    );
    else if ( percent >=  30 ) strcat( buf, " is covered in blood.\n\r"   );
    else if ( percent >=  20 ) strcat( buf, " is leaking guts.\n\r"       );
    else if ( percent >=  10 ) strcat( buf, " is almost dead.\n\r"        );
    else                       strcat( buf, " is DYING.\n\r"              );

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
	&&   can_see_obj( ch, obj ) )
	{
	    if ( !found )
	    {
		send_to_char( "\n\r", ch );
		act( "$N is using:", ch, NULL, victim, TO_CHAR );
		found = TRUE;
	    }
	    send_to_char( where_name[iWear], ch );
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
    }

    if ( victim != ch
    &&   number_percent() <= GET_PRAC_SKILL(ch,gsn_peek) )
    {
	send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
	show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch == ch )
	    continue;

	if ( !IS_NPC(rch)
	&&   IS_SET(rch->act, PLR_WIZINVIS)
	&&   get_trust( ch ) < get_trust( rch ) )
	    continue;

	if ( can_see( ch, rch ) )
	{
	    show_char_to_char_0( rch, ch );
	}
	else if ( room_is_dark( ch->in_room )
	&&        IS_AFFECTED(rch, AFF_INFRARED ) )
	{
	    send_to_char( "&+rYou see glowing red eyes watching YOU!&N\n\r", ch );
	}
    }

    return;
} 



bool check_blind( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) )
    {
	send_to_char( "You can't see a thing!\n\r", ch );
	return FALSE;
    }

    return TRUE;
}



void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;

    if ( ch->desc == NULL )
	return;

    if ( ch->position < POS_SLEEPING )
    {
	send_to_char( "You can't see anything but stars!\n\r", ch );
	return;
    }

    if ( ch->position == POS_SLEEPING )
    {
	send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
	return;
    }

    if ( !check_blind( ch ) )
	return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room ) )
    {
	send_to_char( "&+LIt is pitch black ... &N\n\r", ch );
	show_char_to_char( ch->in_room->people, ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
	/* 'look' or 'look auto' */
	send_to_char( "&+c", ch );
	send_to_char( ch->in_room->name, ch );
	send_to_char( "&N\n\r", ch );

	do_exits( ch, "auto" );

	if ( arg1[0] == '\0'
	|| ( !IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF) ) )
	    send_to_char( ch->in_room->description, ch );

	update_specs( ch->in_room, (void*)ch, NULL, NULL, UPDATE_ROOM, UPDATE_LOOK );

	show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
	show_char_to_char( ch->in_room->people,   ch );
	return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp( arg1, "in" ) )
    {
	/* 'look in' */
	if ( arg2[0] == '\0' )
	{
	    send_to_char( "Look in what?\n\r", ch );
	    return;
	}

	if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not see that here.\n\r", ch );
	    return;
	}

	switch ( obj->item_type )
	{
	default:
	    send_to_char( "That is not a container.\n\r", ch );
	    break;

	case ITEM_DRINK_CON:
	    if ( obj->value[1] <= 0 )
	    {
		send_to_char( "It is empty.\n\r", ch );
		break;
	    }

	    sprintf( buf, "It's %s full of a %s liquid.\n\r",
		obj->value[1] <     obj->value[0] / 4
		    ? "less than" :
		obj->value[1] < 3 * obj->value[0] / 4
		    ? "about"     : "more than",
		liq_table[obj->value[2]].liq_color
		);

	    send_to_char( buf, ch );
	    break;

	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    {
		send_to_char( "It is closed.\n\r", ch );
		break;
	    }

	    act( "$p contains:", ch, obj, NULL, TO_CHAR );
	    show_list_to_char( obj->contains, ch, TRUE, TRUE );
	    break;
	}
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
    {
	show_char_to_char_1( victim, ch );
	return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    return;
	}
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
	if ( can_see_obj( ch, obj ) )
	{
	    pdesc = get_extra_descr( arg1, obj->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }

	    pdesc = get_extra_descr( arg1, obj->pIndexData->extra_descr );
	    if ( pdesc != NULL )
	    {
		send_to_char( pdesc, ch );
		return;
	    }
	}

	if ( is_name( arg1, obj->name ) )
	{
	    send_to_char( obj->description, ch );
	    return;
	}
    }

    pdesc = get_extra_descr( arg1, ch->in_room->extra_descr );
    if ( pdesc != NULL )
    {
	send_to_char( pdesc, ch );
	return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
	send_to_char( "You do not see that here.\n\r", ch );
	return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	send_to_char( "Nothing special there.\n\r", ch );
	return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
	send_to_char( pexit->description, ch );
    else
	send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
	else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
	{
	    act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
	}
    }

    return;
}



void do_flags( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *flag;
	CHAR_DATA *vch = NULL;
	ROOM_INDEX_DATA *room = NULL;
	OBJ_DATA *in_obj = NULL;

	for( flag = object_list; flag; flag = flag->next )
	{
		if( is_name(flag->name,"flag") )
		{
			if( (vch = flag->carried_by) != NULL )
			{
				sprintf(buf,"%s&+m is carried by [&+M%s&N&+m] at [&+M%s&N&+m]&N\n\r",
					flag->short_descr,
					IS_NPC(vch) ? vch->short_descr : vch->name,
					vch->in_room ? vch->in_room->name : "somewhere" );
			}
			else if( (room = flag->in_room) != NULL )
			{
				sprintf(buf,"%s&+m is in the room [&+M%s&N&+m]&N\n\r",
					flag->short_descr,
					room->name );
			}
			else if( (in_obj = flag->in_obj) != NULL )
			{
				sprintf(buf,"%s&+m is inside [&+M%s&N&+m]&N\n\r",
					flag->short_descr,
					in_obj->short_descr );
			}
			else
			{
				sprintf(buf,"%s is lost! Tell Borlak.\n\r",
					flag->short_descr );
			}
			send_to_char(buf,ch);
			continue;
		}
		if( is_name(flag->name,"homebase") )
		{
			sprintf(buf,"%s &+mis at &+M[%s&N&+m]&N\n\r",
				flag->short_descr,
				flag->in_room ? flag->in_room->name : "somewhere" );
			send_to_char(buf,ch);
		}
	}
}



/* I hate these informational commads, they take
 * the most time and are the most useless!
 * :P Borlak 
 */

void do_finger( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	bool team = FALSE;
	int wins = 0;
	int losses = 0;
	int i;

	if( !argument || argument[0] == '\0' )
	{
		send_to_char("Syntax: finger <player>\n\r", ch );
		return;
	}

	if( war_type == WAR_TEAM || war_type == WAR_CAPTURE_FLAG )
		team = TRUE;

	if( (victim = get_char_world(ch,argument)) == NULL
	||  !can_see(ch,victim)
	||  IS_NPC(victim) )
	{
		send_to_char("You don't see them anywhere.\n\r", ch );
		return;
	}

	for( i = 0; i < MAX_WAR; i++ )
	{
		wins	+= victim->pcdata->wars_won[i];
		losses	+= victim->pcdata->wars_lost[i];
	}

	sprintf(buf,"\n\r%cBasic Information:\n\r", '\t' );
	send_to_char(buf,ch);

	sprintf(buf,"  Name: %-12s%c%17s: %d\n\r", victim->name, '\t', "Kills this war", victim->pcdata->kills_this_war );
	send_to_char(buf,ch);

	sprintf(buf," Level: %-12d%c%17s: %d\n\r", victim->level, '\t', "Experience", victim->exp );
	send_to_char(buf,ch);

	sprintf(buf," Kills: %-12d%c%17s: %d\n\r", victim->pcdata->kills, '\t', "Doomsdays", victim->pcdata->doomsdays );
	send_to_char(buf,ch);

	sprintf(buf,"Deaths: %-12d%c%17s: %d\n\r", victim->pcdata->deaths, '\t', "Hot Skulls",victim->pcdata->hot_skulls );
	send_to_char(buf,ch);

	sprintf(buf,"  K/D%%: %-12.2f%c%17s: %d\n\r", 
		victim->pcdata->deaths == 0 ? (float)victim->pcdata->kills : (float)((float)victim->pcdata->kills/(float)victim->pcdata->deaths),
		'\t', "Mob Deaths", victim->pcdata->mob_deaths );
	send_to_char(buf,ch);

	sprintf(buf,"  Wins: %-12d%c%17s: %d\n\r",
		wins,
		'\t', "IT Kills", victim->pcdata->it_kills );
	send_to_char(buf,ch);

	sprintf(buf,"Losses: %-12d%c%17s: %d\n\r",
		losses,
		'\t', "Borg Kills", victim->pcdata->borg_kills );
	send_to_char(buf, ch);

	sprintf(buf,"  Win%%: %-12.2f%c%17s: %d\n\r",
		losses == 0 ? 100 : (float)((float)wins/(float)losses),
		'\t', "Damage Inflicted", victim->pcdata->dam_inflicted );
	send_to_char(buf,ch);

	sprintf(buf," Lose%%: %-12.2f%c%17s: %d\n\r",
		wins == 0 ? 100 : (float)((float)losses/(float)wins),
		'\t', "Damage Received", victim->pcdata->dam_received );
	send_to_char(buf,ch);

	send_to_char("\n\r",ch);

	sprintf(buf,"%cWar Specific Variables:\n\r", '\t' );
	send_to_char(buf,ch);

	sprintf(buf,"Random: %s\n\r", victim->pcdata->random ? "Yes" : "No" );
	send_to_char(buf,ch);

	sprintf(buf," Timer: %d\n\r", victim->pcdata->timer );
	send_to_char(buf,ch);

	sprintf(buf,"  Team: %s\n\r", team ? team_name(victim->pcdata->team) : "None" );
	send_to_char(buf,ch);

	sprintf(buf,"    IT: %s\n\r", IS_SET(victim->act, PLR_IT) ? "Yes" : "No" );
	send_to_char(buf,ch);

	send_to_char("\n\r", ch );

	sprintf(buf,"%cWar Stats: Won/Lost\n\r", '\t' );
	send_to_char(buf,ch);

	sprintf(buf,"        Pit: %2d/%-2d  City: %2d/%-2d  AllVsOne: %2d/%-2d  Artifact: %2d/%-2d\n\r",
		ch->pcdata->wars_won[WAR_PIT],		ch->pcdata->wars_lost[WAR_PIT],
		ch->pcdata->wars_won[WAR_CITY],		ch->pcdata->wars_lost[WAR_CITY],
		ch->pcdata->wars_won[WAR_ALL_VS_ONE],	ch->pcdata->wars_lost[WAR_ALL_VS_ONE],
		ch->pcdata->wars_won[WAR_ARTIFACT],	ch->pcdata->wars_lost[WAR_ARTIFACT] );
	send_to_char(buf,ch);

	sprintf(buf,"       Team: %2d/%-2d  Borg: %2d/%-2d  HotSkull: %2d/%-2d   Fittest: %2d/%-2d\n\r",
		ch->pcdata->wars_won[WAR_TEAM],		ch->pcdata->wars_lost[WAR_TEAM],
		ch->pcdata->wars_won[WAR_BORG],		ch->pcdata->wars_lost[WAR_BORG],
		ch->pcdata->wars_won[WAR_HOT_SKULL],	ch->pcdata->wars_lost[WAR_HOT_SKULL],
		ch->pcdata->wars_won[WAR_FITTEST],	ch->pcdata->wars_lost[WAR_FITTEST] );
	send_to_char(buf,ch);

	sprintf(buf,"CaptureFlag: %2d/%-2d   God: %2d/%-2d\n\r",
		ch->pcdata->wars_won[WAR_CAPTURE_FLAG],	ch->pcdata->wars_lost[WAR_CAPTURE_FLAG],
		ch->pcdata->wars_won[WAR_GOD],		ch->pcdata->wars_lost[WAR_GOD] );
	send_to_char(buf,ch);

	send_to_char("\n\r", ch );
	return;
}


void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Examine what?\n\r", ch );
	return;
    }

    do_look( ch, arg );

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	switch ( obj->item_type )
	{
	default:
	    break;

	case ITEM_DRINK_CON:
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	    send_to_char( "When you look inside, you see:\n\r", ch );
	    sprintf( buf, "in %s", arg );
	    do_look( ch, buf );
	}
    }

    return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    buf[0] = '\0';
    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
	return;

    strcpy( buf, fAuto ? "&+y[Exits:" : "Obvious exits:\n\r" );

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
	if ( ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED) )
	{
	    found = TRUE;
	    if ( fAuto )
	    {
		strcat( buf, " " );
		strcat( buf, dir_name[door] );
	    }
	    else
	    {
		sprintf( buf + strlen(buf), "%-5s - %s\n\r",
		    capitalize( dir_name[door] ),
		    room_is_dark( pexit->to_room )
			?  "Too dark to tell"
			: pexit->to_room->name
		    );
	    }
	}
    }

    if ( !found )
	strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
	strcat( buf, "]&N\n\r" );

    send_to_char( buf, ch );
    return;
}


void do_skills( CHAR_DATA *ch, char *argument )
{
	char buf[200];
	int i;
	int col = 0;

	buf[0] = '\0';

	if( IS_IMMORTAL(ch) && (argument && str_cmp(argument,"mortal")) )
	{
		for( i = 0; i < MAX_SKILL; i++ )
		{
			sprintf(buf+strlen(buf),"%-15s  ", skill_table[i].name );
			
			if( ++col == 4 )
			{
				send_to_char(buf,ch);
				send_to_char("\n\r",ch);
				buf[0] = '\0';
				col = 0;
			}
		}
		if( col != 0 )
			send_to_char(buf,ch);

		send_to_char("\n\r",ch);
		return;
	}

	for( i = 0; i < MAX_SKILL; i++ )
	{
		if( class_table[ch->class].skills[i][SKILL_PRAC] <= 0 )
			continue;

		sprintf(buf+strlen(buf), "[%-15s %-2d]  ",
			skill_table[i].name,
			class_table[ch->class].skills[i][SKILL_PRAC] );

		if( ++col == 3 )
		{
			send_to_char(buf,ch);
			send_to_char("\n\r",ch);
			buf[0] = '\0';
			col = 0;
		}
	}
	if( col != 0 )
		send_to_char(buf,ch);

	send_to_char("\n\r",ch);
	return;
}


int scan_room( CHAR_DATA *ch, const ROOM_INDEX_DATA *proom, char *buf )
{
	CHAR_DATA *target = proom->people;
	int num = 0;

	while( target )
	{
		if( IS_NPC(target) || ch == target )
		{
			target = target->next_in_room;
			continue;
		}

		if( can_see(ch,target))
		{
			strcat(buf," - &+g");
			strcat(buf,target->name);
			strcat(buf,"&N\n\r");
			num++;
		}
		target = target->next_in_room;
	}
	return num;
}


void do_scan( CHAR_DATA *ch, char *argument )
{
	EXIT_DATA *pexit = NULL;
	ROOM_INDEX_DATA *proom;
	extern char * const dir_name[];
	char buf[MAX_STRING_LENGTH];
	int dir;
	int distance;
	char * const distance_name [5] =
	{
		"Right here", "Close by", "Not too far",
		"In the distant", "Far from here to the"
	};

	sprintf(buf,"Right here you see:\n\r" );
	if( scan_room(ch,ch->in_room,buf) == 0 )
		strcat(buf,"Noone.\n\r");
	send_to_char(buf,ch);

	for( dir = 0; dir < 6; dir++ )
	{
		proom = ch->in_room;

		for( distance = 1; distance < 5; distance++ )
		{
			pexit = proom->exit[dir];
			if((pexit == NULL) || (pexit->to_room == NULL)
			|| (IS_SET(pexit->exit_info, EX_CLOSED)) )
				break;

			sprintf(buf,"&+y%s&N &+Y%s&N from here you see:\n\r", distance_name[distance], dir_name[dir]);
			
			if( scan_room(ch,proom,buf) )
				send_to_char(buf,ch);

			proom = pexit->to_room;
		}
	}
	send_to_char("\n\r",ch);
	return;
}


void do_score( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;

    sprintf( buf,
	"You are %s%s, level %d, %d years old (%d hours).\n\r",
	ch->name,
	IS_NPC(ch) ? "" : ch->pcdata->title,
	ch->level,
	get_age(ch),
	(get_age(ch) - 17) * 2 );
    send_to_char( buf, ch );

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at level %d.\n\r",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }

    sprintf( buf,
	"You have %d/%d hit, %d/%d mana,\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana );
    send_to_char( buf, ch );

    sprintf( buf,
	"You are carrying %d/%d items with weight %d/%d kg.\n\r",
	ch->carry_number, can_carry_n(ch),
	ch->carry_weight, can_carry_w(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"Str: %d  Int: %d  Wis: %d  Dex: %d  Con: %d.\n\r",
	get_curr_str(ch),
	get_curr_int(ch),
	get_curr_wis(ch),
	get_curr_dex(ch),
	get_curr_con(ch) );
    send_to_char( buf, ch );

    sprintf( buf,
	"You have scored %d exp, and have %d gold coins.\n\r",
	ch->exp, ch->gold );
    send_to_char( buf, ch );

    sprintf( buf, "Wimpy set to %d hit points.\n\r", ch->wimpy );
    send_to_char( buf, ch );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]   > 10 )
	send_to_char( "You are drunk.\n\r",   ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] ==  0 )
	send_to_char( "You are thirsty.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   ==  0 )
	send_to_char( "You are hungry.\n\r",  ch );

    switch ( ch->position )
    {
    case POS_DEAD:     
	send_to_char( "You are DEAD!!\n\r",		ch );
	break;
    case POS_MORTAL:
	send_to_char( "You are mortally wounded.\n\r",	ch );
	break;
    case POS_INCAP:
	send_to_char( "You are incapacitated.\n\r",	ch );
	break;
    case POS_STUNNED:
	send_to_char( "You are stunned.\n\r",		ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You are sleeping.\n\r",		ch );
	break;
    case POS_RESTING:
	send_to_char( "You are resting.\n\r",		ch );
	break;
    case POS_STANDING:
	send_to_char( "You are standing.\n\r",		ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are fighting.\n\r",		ch );
	break;
    }

    if ( ch->level >= 25 )
    {
	sprintf( buf, "AC: %d.  ", GET_AC(ch) );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( GET_AC(ch) >=  101 ) send_to_char( "WORSE than naked!\n\r", ch );
    else if ( GET_AC(ch) >=   80 ) send_to_char( "naked.\n\r",            ch );
    else if ( GET_AC(ch) >=   60 ) send_to_char( "wearing clothes.\n\r",  ch );
    else if ( GET_AC(ch) >=   40 ) send_to_char( "slightly armored.\n\r", ch );
    else if ( GET_AC(ch) >=   20 ) send_to_char( "somewhat armored.\n\r", ch );
    else if ( GET_AC(ch) >=    0 ) send_to_char( "armored.\n\r",          ch );
    else if ( GET_AC(ch) >= - 20 ) send_to_char( "well armored.\n\r",     ch );
    else if ( GET_AC(ch) >= - 40 ) send_to_char( "strongly armored.\n\r", ch );
    else if ( GET_AC(ch) >= - 60 ) send_to_char( "heavily armored.\n\r",  ch );
    else if ( GET_AC(ch) >= - 80 ) send_to_char( "superbly armored.\n\r", ch );
    else if ( GET_AC(ch) >= -100 ) send_to_char( "divinely armored.\n\r", ch );
    else                           send_to_char( "invincible!\n\r",       ch );

    if ( ch->level >= 15 )
    {
	sprintf( buf, "Hitroll: %d  Damroll: %d.\n\r",
	    GET_HITROLL(ch), GET_DAMROLL(ch) );
	send_to_char( buf, ch );
    }
    
    if ( ch->level >= 10 )
    {
	sprintf( buf, "Alignment: %d.  ", ch->alignment );
	send_to_char( buf, ch );
    }

    send_to_char( "You are ", ch );
         if ( ch->alignment >  900 ) send_to_char( "angelic.\n\r", ch );
    else if ( ch->alignment >  700 ) send_to_char( "saintly.\n\r", ch );
    else if ( ch->alignment >  350 ) send_to_char( "good.\n\r",    ch );
    else if ( ch->alignment >  100 ) send_to_char( "kind.\n\r",    ch );
    else if ( ch->alignment > -100 ) send_to_char( "neutral.\n\r", ch );
    else if ( ch->alignment > -350 ) send_to_char( "mean.\n\r",    ch );
    else if ( ch->alignment > -700 ) send_to_char( "evil.\n\r",    ch );
    else if ( ch->alignment > -900 ) send_to_char( "demonic.\n\r", ch );
    else                             send_to_char( "satanic.\n\r", ch );
    
    if ( ch->affected != NULL )
    {
	send_to_char( "You are affected by:\n\r", ch );
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
	    sprintf( buf, "Spell: '%s'", skill_table[paf->type].name );
	    send_to_char( buf, ch );

	    if ( ch->level >= 20 )
	    {
		sprintf( buf,
		    " modifies %s by %d for %d hours",
		    affect_loc_name( paf->location ),
		    paf->modifier,
		    paf->duration );
		send_to_char( buf, ch );
	    }

	    send_to_char( ".\n\r", ch );
	}
    }

    return;
}



char *	const	day_name	[] =
{
    "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
    "the Great Gods", "the Sun"
};

char *	const	month_name	[] =
{
    "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
    "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
    "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
    "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
	"It is %d o'clock %s, Day of %s, %d%s the Month of %s.\n\rMerc started up at %s\rThe system time is %s\r",

	(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
	time_info.hour >= 12 ? "pm" : "am",
	day_name[day % 7],
	day, suf,
	month_name[time_info.month],
	str_boot_time,
	(char *) ctime( &current_time )
	);

    send_to_char( buf, ch );
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
	"cloudless",
	"cloudy",
	"rainy",
	"lit by flashes of lightning"
    };

    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "You can't see the weather indoors.\n\r", ch );
	return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
	sky_look[weather_info.sky],
	weather_info.change >= 0
	? "a warm southerly breeze blows"
	: "a cold northern gust blows"
	);
    send_to_char( buf, ch );
    return;
}



void do_help( CHAR_DATA *ch, char *argument )
{
    char argall[MAX_INPUT_LENGTH];
    char argone[MAX_INPUT_LENGTH];
    HELP_DATA *pHelp;

    if ( argument[0] == '\0' )
	argument = "summary";

    /*
     * Tricky argument handling so 'help a b' doesn't match a.
     */
    argall[0] = '\0';
    while ( argument[0] != '\0' )
    {
	argument = one_argument( argument, argone );
	if ( argall[0] != '\0' )
	    strcat( argall, " " );
	strcat( argall, argone );
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
	if ( pHelp->level > get_trust( ch ) )
	    continue;

	if ( is_name( argall, pHelp->keyword ) )
	{
	    if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
	    {
		send_to_char( pHelp->keyword, ch );
		send_to_char( "\n\r", ch );
	    }

	    /*
	     * Strip leading '.' to allow initial blanks.
	     */
	    if ( pHelp->text[0] == '.' )
		send_to_char( pHelp->text+1, ch );
	    else
		send_to_char( pHelp->text  , ch );
	    return;
	}
    }

    send_to_char( "No help on that word.\n\r", ch );
    return;
}



void do_who( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *vch;
	int players = 0;
	bool team = FALSE;
	bool it = FALSE;
	static int max = 0;

	buf[0] = '\0';

	sprintf(buf+strlen(buf),"It is a %s war.\n\r",
		get_war_name(war_type) );
	sprintf(buf+strlen(buf),"War Timer: %d.\n\r", war_timer );
	sprintf(buf+strlen(buf),"War Flags: %s\n\r", get_war_flags(war_flags) );


	if( (war_type == WAR_TEAM || war_type == WAR_CAPTURE_FLAG) )
		team = TRUE;

	for( d = descriptor_list; d; d = d->next )
	{
		if( d->connected > CON_PLAYING
		||  (vch = d->character) == NULL 
		||  (!can_see(ch,vch) && IS_HERO(vch))
		||  IS_NPC(vch) /*switched? dont know if this works*/ )
			continue;

		it = IS_SET(vch->act, PLR_IT) ? TRUE : FALSE;

		sprintf(buf+strlen(buf),"[%-4d %.2f %4s] %s%s",
			vch->level,
			(float)vch->pcdata->kills/ (float)UMAX(1,vch->pcdata->deaths),
			IS_BORG(vch) ? "borg" : vch->pcdata->who_name[0] == '\0' ? ch->pcdata->random ? "rand" : 
				class_table[vch->class].short_name : vch->pcdata->who_name,
			IS_IMMORTAL(vch) ? "&+g(IMM) &N" : team ? vch->pcdata->in_war ? team_name(vch->pcdata->team) : "" : it ? "&+Y(IT)&N" : 
				war_type == WAR_BORG ? IS_BORG(vch) ? "&+G(Borg)&N " : 
				vch->pcdata->in_war ? "&+c(IN_WAR)&N" : "" : vch->pcdata->in_war ? "&+c(IN_WAR)&N" : "",
			can_see(ch,vch) ? vch->name : "Someone" );

		if( IS_IMMORTAL(ch) )
		{
			sprintf(buf+strlen(buf),"%c&+g<%-3d %-3d>&N&+c@%s&N %s",
				strlen(vch->name) > 7 ? ' ' : '\t',
				vch->hit,
				vch->mana,
				vch->in_room ? vch->in_room->name : "Nowhere",
				IS_IMMORTAL(vch) ? IS_SET(ch->act, PLR_WIZINVIS) ? "(Wizinvis)" : "" : "" );
		}
		else
		{
			sprintf(buf+strlen(buf),"%s", can_see(ch,vch) ? vch->pcdata->title : "." );
		}

		sprintf(buf+strlen(buf),"\n\r");

		players++;
	}
	if( players > max )
		max = players;

	sprintf(buf+strlen(buf),"%d players on, max today is %d.\n\r", players, max );

	send_to_char(buf,ch);
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are carrying:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
	    continue;

	send_to_char( where_name[iWear], ch );
	if ( can_see_obj( ch, obj ) )
	{
	    send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
	    send_to_char( "\n\r", ch );
	}
	else
	{
	    send_to_char( "something.\n\r", ch );
	}
	found = TRUE;
    }

    if ( !found )
	send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Compare what to what?\n\r", ch );
	return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
    {
	for ( obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content )
	{
	    if ( obj2->wear_loc != WEAR_NONE
	    &&   can_see_obj( ch, obj2 )
	    &&   obj1->item_type == obj2->item_type
	    && ( obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
		break;
	}

	if ( obj2 == NULL )
	{
	    send_to_char( "You aren't wearing anything comparable.\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "You do not have that item.\n\r", ch );
	    return;
	}
    }
	    
    msg		= NULL;
    value1	= 0;
    value2	= 0;

    if ( obj1 == obj2 )
    {
	msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
	msg = "You can't compare $p and $P.";
    }
    else
    {
	switch ( obj1->item_type )
	{
	default:
	    msg = "You can't compare $p and $P.";
	    break;

	case ITEM_ARMOR:
	    value1 = obj1->value[0];
	    value2 = obj2->value[0];
	    break;

	case ITEM_WEAPON:
	    value1 = obj1->value[1] + obj1->value[2];
	    value2 = obj2->value[1] + obj2->value[2];
	    break;
	}
    }

    if ( msg == NULL )
    {
	     if ( value1 == value2 ) msg = "$p and $P look about the same.";
	else if ( value1  > value2 ) msg = "$p looks better than $P.";
	else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Players near you:\n\r", ch );
	found = FALSE;
	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    && ( victim = d->character ) != NULL
	    &&   !IS_NPC(victim)
	    &&   victim->in_room != NULL
	    &&   victim->in_room->area == ch->in_room->area
	    &&   can_see( ch, victim ) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
		    victim->name, victim->in_room->name );
		send_to_char( buf, ch );
	    }
	}
	if ( !found )
	    send_to_char( "None\n\r", ch );
    }
    else
    {
	if( ch->gold < 10 && !IS_IMMORTAL(ch) )
	{
		send_to_char("You don't have enough gold!\n\r", ch );
		return;
	}
	found = FALSE;
	for ( victim = char_list; victim != NULL; victim = victim->next )
	{
	    if ( victim->in_room != NULL
	    &&   is_name( arg, victim->name )
	    &&   !IS_IMMORTAL(victim) )
	    {
		found = TRUE;
		sprintf( buf, "%-28s %s\n\r",
			can_see(ch,victim) ? PERS(victim, ch) : "someone",
			victim->in_room->name );
		send_to_char( buf, ch );
		break;
	    }
	}
	if ( !found )
		act( "You couldn't find $T.", ch, NULL, arg, TO_CHAR );
	else
		ch->gold -= 10;
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Consider killing whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They're not here.\n\r", ch );
	return;
    }

    diff = victim->level - ch->level;

         if ( diff <= -10 ) msg = "You can kill $N naked and weaponless.";
    else if ( diff <=  -5 ) msg = "$N is no match for you.";
    else if ( diff <=  -2 ) msg = "$N looks like an easy kill.";
    else if ( diff <=   1 ) msg = "The perfect match!";
    else if ( diff <=   4 ) msg = "$N says 'Do you feel lucky, punk?'.";
    else if ( diff <=   9 ) msg = "$N laughs at you mercilessly.";
    else                    msg = "Death will thank you for your gift.";

    act( msg, ch, NULL, victim, TO_CHAR );
    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
	bug( "Set_title: NPC.", 0 );
	return;
    }

    if ( isalpha(title[0]) || isdigit(title[0]) )
    {
	buf[0] = ' ';
	strcpy( buf+1, title );
    }
    else
    {
	strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;

    if ( argument[0] == '\0' )
    {
	send_to_char( "Change your title to what?\n\r", ch );
	return;
    }

    if ( strlen(argument) > 50 )
	argument[50] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "Ok.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] != '\0' )
    {
	buf[0] = '\0';
	smash_tilde( argument );
	if ( argument[0] == '+' )
	{
	    if ( ch->description != NULL )
		strcat( buf, ch->description );
	    argument++;
	    while ( isspace(*argument) )
		argument++;
	}

	if ( strlen(buf) + strlen(argument) >= MAX_STRING_LENGTH - 2 )
	{
	    send_to_char( "Description too long.\n\r", ch );
	    return;
	}

	strcat( buf, argument );
	strcat( buf, "\n\r" );
	free_string( ch->description );
	ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}



void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    sprintf( buf,
	"You report: %d/%d hp %d/%d mana %d xp.\n\r",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->exp   );

    send_to_char( buf, ch );

    sprintf( buf, "$n reports: %d/%d hp %d/%d mana %d xp.",
	ch->hit,  ch->max_hit,
	ch->mana, ch->max_mana,
	ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    return;
}



/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	wimpy = ch->max_hit / 5;
    else
	wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
	send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
	return;
    }

    if ( wimpy > ch->max_hit )
    {
	send_to_char( "Such cowardice ill becomes you.\n\r", ch );
	return;
    }

    ch->wimpy	= wimpy;
    sprintf( buf, "Wimpy set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
	return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
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
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
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
	*pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: password <old> <new>.\n\r", ch );
	return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }

    if ( strlen(arg2) < 5 )
    {
	send_to_char(
	    "New password must be at least five characters long.\n\r", ch );
	return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
	if ( *p == '~' )
	{
	    send_to_char(
		"New password not acceptable, try again.\n\r", ch );
	    return;
	}
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_socials( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	int i;
	int col = 0;
	extern struct social_type *social_table;
	extern int SOCIALS;

	buf[0] = '\0';
	for( i = 0; i < SOCIALS; i++ )
	{
		if( social_table[i].name == NULL
		||  (!str_cmp(social_table[i].name,"newsocial") && !IS_HERO(ch)) )
			continue;

		sprintf(buf+strlen(buf),"%-12s ",social_table[i].name );
		if( ++col % 5 == 0 )
			strcat(buf,"\n\r");
	}
	send_to_char(buf,ch);
	send_to_char("\n\r", ch);
	return;
}



void do_spells( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int sn;
    int col;

    col = 0;
    for ( sn = 0; sn < MAX_SKILL && skill_table[sn].name != NULL; sn++ )
    {
	sprintf( buf, "%-12s", skill_table[sn].name );
	send_to_char( buf, ch );
	if ( ++col % 6 == 0 )
	    send_to_char( "\n\r", ch );
    }

    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
 
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <  LEVEL_HERO
        &&   cmd_table[cmd].level <= get_trust( ch ) )
	{
	    sprintf( buf, "%-12s", cmd_table[cmd].name );
	    send_to_char( buf, ch );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
 
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
    return;
}



void do_channels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE) )
	{
	    send_to_char( "You are silenced.\n\r", ch );
	    return;
	}

	send_to_char( "Channels:", ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_AUCTION)
	    ? " +AUCTION"
	    : " -auction",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_CHAT)
	    ? " +CHAT"
	    : " -chat",
	    ch );

#if 0
	send_to_char( !IS_SET(ch->deaf, CHANNEL_HACKER)
	    ? " +HACKER"
	    : " -hacker",
	    ch );
#endif

	if ( IS_HERO(ch) )
	{
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_IMMTALK)
		? " +IMMTALK"
		: " -immtalk",
		ch );
	    send_to_char( !IS_SET(ch->deaf, CHANNEL_WARMESSAGE)
		    ? " +WARMESSAGE"
		    : " -warmessage",
		    ch );
	}

	send_to_char( !IS_SET(ch->deaf, CHANNEL_MUSIC)
	    ? " +MUSIC"
	    : " -music",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_QUESTION)
	    ? " +QUESTION"
	    : " -question",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_SHOUT)
	    ? " +SHOUT"
	    : " -shout",
	    ch );

	send_to_char( !IS_SET(ch->deaf, CHANNEL_YELL)
	    ? " +YELL"
	    : " -yell",
	    ch );

	send_to_char( ".\n\r", ch );
    }
    else
    {
	bool fClear;
	int bit;

	     if ( arg[0] == '+' ) fClear = TRUE;
	else if ( arg[0] == '-' ) fClear = FALSE;
	else
	{
	    send_to_char( "Channels -channel or +channel?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "auction"  ) ) bit = CHANNEL_AUCTION;
        else if ( !str_cmp( arg+1, "chat"     ) ) bit = CHANNEL_CHAT;
#if 0
	else if ( !str_cmp( arg+1, "hacker"   ) ) bit = CHANNEL_HACKER;
#endif
	else if ( !str_cmp( arg+1, "immtalk"  ) ) bit = CHANNEL_IMMTALK;
	else if ( IS_HERO(ch) && !str_cmp( arg+1, "warmessage")) bit = CHANNEL_WARMESSAGE;
	else if ( !str_cmp( arg+1, "music"    ) ) bit = CHANNEL_MUSIC;
	else if ( !str_cmp( arg+1, "question" ) ) bit = CHANNEL_QUESTION;
	else if ( !str_cmp( arg+1, "shout"    ) ) bit = CHANNEL_SHOUT;
	else if ( !str_cmp( arg+1, "yell"     ) ) bit = CHANNEL_YELL;
	else
	{
	    send_to_char( "Set or clear which channel?\n\r", ch );
	    return;
	}

	if ( fClear )
	    REMOVE_BIT (ch->deaf, bit);
	else
	    SET_BIT    (ch->deaf, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}



/*
 * Contributed by Grodyn.
 */
void do_config( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    if ( IS_NPC(ch) )
	return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "[ Keyword  ] Option\n\r", ch );

	send_to_char(  IS_SET(ch->act, PLR_BLANK)
	    ? "[+BLANK    ] You have a blank line before your prompt.\n\r"
	    : "[-blank    ] You have no blank line before your prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_BRIEF)
	    ? "[+BRIEF    ] You see brief descriptions.\n\r"
	    : "[-brief    ] You see long descriptions.\n\r"
	    , ch );
         
	send_to_char(  IS_SET(ch->act, PLR_COMBINE)
	    ? "[+COMBINE  ] You see object lists in combined format.\n\r"
	    : "[-combine  ] You see object lists in single format.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_PROMPT)
	    ? "[+PROMPT   ] You have a prompt.\n\r"
	    : "[-prompt   ] You don't have a prompt.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_TELNET_GA)
	    ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r"
	    : "[-telnetga ] You don't receive a telnet GA sequence.\n\r"
	    , ch );

	send_to_char(  IS_SET(ch->act, PLR_SILENCE)
	    ? "[+SILENCE  ] You are silenced.\n\r"
	    : ""
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_EMOTE)
	    ? ""
	    : "[-emote    ] You can't emote.\n\r"
	    , ch );

	send_to_char( !IS_SET(ch->act, PLR_NO_TELL)
	    ? ""
	    : "[-tell     ] You can't use 'tell'.\n\r"
	    , ch );
    }
    else
    {
	bool fSet;
	int bit;

	     if ( arg[0] == '+' ) fSet = TRUE;
	else if ( arg[0] == '-' ) fSet = FALSE;
	else
	{
	    send_to_char( "Config -option or +option?\n\r", ch );
	    return;
	}

	     if ( !str_cmp( arg+1, "blank"    ) ) bit = PLR_BLANK;
	else if ( !str_cmp( arg+1, "brief"    ) ) bit = PLR_BRIEF;
	else if ( !str_cmp( arg+1, "combine"  ) ) bit = PLR_COMBINE;
        else if ( !str_cmp( arg+1, "prompt"   ) ) bit = PLR_PROMPT;
	else if ( !str_cmp( arg+1, "telnetga" ) ) bit = PLR_TELNET_GA;
	else
	{
	    send_to_char( "Config which option?\n\r", ch );
	    return;
	}

	if ( fSet )
	    SET_BIT    (ch->act, bit);
	else
	    REMOVE_BIT (ch->act, bit);

	send_to_char( "Ok.\n\r", ch );
    }

    return;
}
