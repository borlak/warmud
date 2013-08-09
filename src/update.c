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
#include <string.h>
#include <time.h>
#include "merc.h"



/*
 * Local functions.
 */
int	hit_gain	args( ( CHAR_DATA *ch ) );
int	mana_gain	args( ( CHAR_DATA *ch ) );
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );




void gain_exp( CHAR_DATA *ch, int gain )
{
	char buf[MAX_INPUT_LENGTH];

	if ( IS_NPC(ch) || ch->level >= LEVEL_HERO || (ch->level == 1 && gain < 0) )
		return;

	ch->exp += gain;

	if( ch->exp >= ch->level+1 )
	{
		send_to_char( "You raise a level!!\n\r  ", ch );
		ch->level += 1;
		ch->exp    = 0;
		sprintf(buf,"%s has achieved level %d!", ch->name, ch->level );
		do_echo( ch, buf );
	}
	else if( ch->exp <= -(ch->level) )
	{
		send_to_char( "You lose a level!\n\r  ", ch );
		ch->level -= 1;
		ch->exp    = 0;
		sprintf(buf,"%s has dropped down to level %d!", ch->name, ch->level );
		do_echo( ch, buf );
	}
	save_char_obj(ch);

	return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
	int gain;

	switch ( ch->position )
	{
	case POS_SLEEPING:	gain = get_curr_con(ch)/3;	break;
	case POS_RESTING:	gain = get_curr_con(ch)/4;	break;
	default:		gain = get_curr_con(ch)/6;	break;
	}
	if ( IS_AFFECTED(ch, AFF_POISON) )
		gain = -2;
	return UMIN( ch->max_hit - ch->hit, gain );
}



int mana_gain( CHAR_DATA *ch )
{
	int gain;

	switch ( ch->position )
	{
	case POS_SLEEPING:	gain = 0;			break;
	case POS_RESTING:	gain = get_curr_int(ch)/5;	break;
	default:		gain = get_curr_int(ch)/9;	break;
	}
	if ( IS_AFFECTED( ch, AFF_POISON ) )
		gain = 0;

	return UMIN( ch->max_mana - ch->mana, gain);
}


/* heartbeat healing */
void heal_update( void )
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch;

	for( d = descriptor_list;d;d = d->next )
	{
		if( d->connected > CON_PLAYING
		||  (ch = d->character) == NULL
		||  IS_AFFECTED(ch,AFF_BLACKMANTLE) )
			continue;

		if( ch->position >= POS_STUNNED )
		{
			if( ch->hit  < ch->max_hit
			&& !IS_SET(class_table[ch->class].flags, CLS_NO_REGEN_HP) )
				ch->hit  += hit_gain(ch);

			if( ch->mana < ch->max_mana
			&& !IS_SET(class_table[ch->class].flags, CLS_NO_REGEN_MANA) )
				ch->mana += mana_gain(ch);
		}
	}
}


/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;

	if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM) )
	    continue;


	/* That's all for sleeping / busy monster */
	if ( ch->position != POS_STANDING )
	    continue;

	/* Scavenge */
	if ( IS_SET(ch->act, ACT_SCAVENGER)
	&&   ch->in_room->contents != NULL
	&&   number_bits( 2 ) == 0 )
	{
	    OBJ_DATA *obj;
	    OBJ_DATA *obj_best;
	    int max;

	    max         = 1;
	    obj_best    = 0;
	    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	    {
		if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max )
		{
		    obj_best    = obj;
		    max         = obj->cost;
		}
	    }

	    if ( obj_best )
	    {
		obj_from_room( obj_best );
		obj_to_char( obj_best, ch );
		act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
	    }
	}

	/* Wander */
	if ( !IS_SET(ch->act, ACT_SENTINEL)
	&& ( door = number_bits( 5 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
	&& ( !IS_SET(ch->act, ACT_STAY_AREA)
	||   pexit->to_room->area == ch->in_room->area ) )
	{
	    move_char( ch, door );
	}

	/* Flee */
	if ( ch->hit < ch->max_hit / 2
	&& ( door = number_bits( 3 ) ) <= 5
	&& ( pexit = ch->in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   !IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) )
	{
	    CHAR_DATA *rch;
	    bool found;

	    found = FALSE;
	    for ( rch  = pexit->to_room->people;
		  rch != NULL;
		  rch  = rch->next_in_room )
	    {
		if ( !IS_NPC(rch) )
		{
		    found = TRUE;
		    break;
		}
	    }
	    if ( !found )
		move_char( ch, door );
	}

    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky.\n\r" );
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}



/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{   
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	ch_next = ch->next;

	if ( ch->position == POS_STUNNED )
	    update_pos( ch );

	if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
	{
	    OBJ_DATA *obj;

	    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
	    &&   obj->item_type == ITEM_LIGHT
	    &&   obj->value[2] > 0 )
	    {
		if ( --obj->value[2] == 0 && ch->in_room != NULL )
		{
		    --ch->in_room->light;
		    act( "$p goes out.", ch, obj, NULL, TO_ROOM );
		    act( "$p goes out.", ch, obj, NULL, TO_CHAR );
		    extract_obj( obj );
		}
	    }

	    if( ch->pcdata->timer > 0 )
	    {
		    if( --ch->pcdata->timer == 0 && !IN_WAR(ch) )
			    send_to_char("You may join the war now.\n\r", ch );
	    }
	}

	/* simulate a wait state for mobs for specs */
	if( IS_NPC(ch) && ch->wait > 0 )
		ch->wait--;

	for ( paf = ch->affected; paf && ch; paf = paf_next )
	{
	    paf_next	= paf->next;
	    if ( paf->duration > 0 )
		paf->duration--;
	    else if ( paf->duration < 0 )
		;
	    else
	    {
		if ( paf_next == NULL
		||   paf_next->type != paf->type
		||   paf_next->duration > 0 )
		{
		    if ( paf->type > 0 && skill_table[paf->type].msg_off )
		    {
			send_to_char( skill_table[paf->type].msg_off, ch );
			send_to_char( "\n\r", ch );
		    }
		}	  
		affect_remove( ch, paf );
	    }
	}

	/* summoned creatures go poof once animate wears off */
	if( IS_NPC(ch) && IS_SET(ch->act, ACT_SERVANT) && !IS_AFFECTED(ch,AFF_SUMMONED) )
	{
		act("&+c$n's spirit leaves $m and $e falls to the ground..dead&N",
			ch, NULL, NULL, TO_ROOM );
		raw_kill(ch,ch);
		continue;
	}

	/*
	 * Careful with the damages here,
	 *   MUST NOT refer to ch after damage taken,
	 *   as it may be lethal damage (on NPC).
	 */
	if ( IS_AFFECTED(ch, AFF_POISON) )
	{
	    act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You shiver and suffer.\n\r", ch );
	    damage( ch, ch, dice(5,5), gsn_poison );
	}
	else if ( ch->position == POS_INCAP )
	{
	    damage( ch, ch, 1, TYPE_UNDEFINED );
	}
	else if ( ch->position == POS_MORTAL )
	{
	    damage( ch, ch, 2, TYPE_UNDEFINED );
	}
    }
    return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
	CHAR_DATA *rch;
	char *message;

	obj_next = obj->next;

	if ( obj->timer <= 0 || --obj->timer > 0 )
	    continue;

	switch ( obj->item_type )
	{
	default:              message = "$p vanishes.";         break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
	case ITEM_FOOD:       message = "$p decomposes.";	break;
	}

	if ( obj->carried_by != NULL )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL )
	{
	    act( message, rch, obj, NULL, TO_ROOM );
	    act( message, rch, obj, NULL, TO_CHAR );
	}

	extract_obj( obj );
    }

    return;
}




SPEC_OBJ_DATA *activate_specs( SPEC_OBJ_DATA *spec )
{
	SPEC_OBJ_DATA	*spec_next;
	SPEC_OBJ_DATA	*spec_prev;
	SPEC_OBJ_DATA	*beginning = spec;

	for( spec_prev = spec; spec; spec = spec_next )
	{
		spec_next = spec->next;

		/* specs with timer of -1 go on forever... */
		if( spec->timer != -1 && --spec->timer == 0
		&&  (*spec->spec_fun)(spec, NULL, NULL, NULL, UPDATE_END) == TRUE)
		{
			if( spec == beginning )
				spec_prev = beginning = spec->next;
			else
				spec_prev->next	= spec->next;
			
			spec->next	= spec_obj_free;
			spec_obj_free	= spec;
			continue;
		}
		else
		{
			/* false means the spec terminated itself	*/
			/* dont want to point to null spec (spec_prev)	*/
			if( (*spec->spec_fun)(spec, NULL, NULL, NULL, UPDATE_TIMER) == FALSE )
			{
				if( beginning == spec )
					beginning = spec_next;

				continue;
			}
		}

		spec_prev = spec;
	}
	return beginning;
}


/* will add mobiles to this update in the future */
/* Current handles objects and room function updates */
void spec_update( void )
{
	OBJ_DATA	*obj;
	OBJ_DATA	*obj_next;
	OBJ_DATA	*obj_prev;
	CHAR_DATA	*ch;
	CHAR_DATA	*ch_next;
	CHAR_DATA	*ch_prev;
	ROOM_INDEX_DATA	*room;
	ROOM_INDEX_DATA	*room_next;
	ROOM_INDEX_DATA	*room_prev;

	/*****************************Rooms********************************/
	for( room_prev = room = room_spec_list; room; room = room_next )
	{
		room_next	= room->next_spec;
		room->spec_obj	= activate_specs( room->spec_obj );


		if( !room->spec_obj )
		{
			if( room == room_spec_list )
			{
				room_prev = room_spec_list = room->next_spec;
			}
			else
			{
				room_prev->next_spec = room->next_spec;
			}
			continue;
		}
		room_prev = room;
	}

	/*****************************Objects*****************************/
	for( obj_prev = obj = obj_spec_list; obj; obj = obj_next )
	{
		obj_next	= obj->next_spec;
		obj->spec_obj	= activate_specs( obj->spec_obj );

		if( !obj->spec_obj )
		{
			if( obj == obj_spec_list )
			{
				obj_prev = obj_spec_list = obj->next_spec;
			}
			else
			{
				obj_prev->next_spec = obj->next_spec;
			}
			continue;
		}
		obj_prev = obj;
	}

	/*****************************Mobiles*****************************/
	for( ch_prev = ch = mob_spec_list; ch; ch = ch_next )
	{
		ch_next		= ch->next_spec;
		ch->spec_obj	= activate_specs( ch->spec_obj );

		if( !ch->spec_obj )
		{
			if( ch == mob_spec_list )
			{
				mob_spec_list = ch->next_spec;
			}
			else
			{
				ch_prev->next_spec = ch->next_spec;
			}
			continue;
		}
		ch_prev = ch;
	}
}





/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int     pulse_war;
    static  int     pulse_heal;
    static  int     pulse_spec;

    if ( --pulse_war      <= 0 )
    {
	pulse_war	= 60 * PULSE_PER_SECOND;
	war_update	( );
    }

    if ( --pulse_spec     <= 0 )
    {
	pulse_spec	= 5 * PULSE_PER_SECOND;
	spec_update	( );
    }

    if ( --pulse_heal     <= 0 )
    {
	pulse_heal	= number_range(3*PULSE_PER_SECOND, 6*PULSE_PER_SECOND);
	heal_update	( );
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }

    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update	( );
	char_update	( );
	obj_update	( );
    }

    tail_chain( );
    return;
}
