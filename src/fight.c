

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
bool	check_dodge	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	dam_message	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
			    int dt ) );
void	death_cry	args( ( CHAR_DATA *ch ) );
void	group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	xp_compute	args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
bool	is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	make_corpse	args( ( CHAR_DATA *ch ) );
void	one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void	raw_kill	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	trip		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );



/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    CHAR_DATA *rch;
    CHAR_DATA *rch_next;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	ch_next	= ch->next;

	if ( IS_NPC(ch)
            && ch->fighting == NULL
            && IS_AWAKE(ch)
            && ch->hunting != NULL )
	{
		hunt_victim(ch);
		continue;
	}

	if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
	    continue;

	if( check_servants(ch,victim) )
		continue;

	if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
	    multi_hit( ch, victim, TYPE_UNDEFINED );
	else
	    stop_fighting( ch, FALSE );

	if ( ( victim = ch->fighting ) == NULL )
	    continue;

	/*
	 * Fun for the whole family!
	 */
	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
	    rch_next = rch->next_in_room;

	    if ( IS_AWAKE(rch) && rch->fighting == NULL )
	    {
		/*
		 * PC's auto-assist others in their group.
		 */
		if ( !IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM) )
		{
		    if ( ( !IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM) )
		    &&   is_same_group(ch, rch) )
			multi_hit( rch, victim, TYPE_UNDEFINED );
		    continue;
		}

		/*
		 * NPC's assist NPC's of same type or 12.5% chance regardless.
		 */
		if ( IS_NPC(rch) && !IS_AFFECTED(rch, AFF_CHARM) )
		{
		    if ( rch->pIndexData == ch->pIndexData
		    ||   number_bits( 3 ) == 0 )
		    {
			CHAR_DATA *vch;
			CHAR_DATA *target;
			int number;

			target = NULL;
			number = 0;
			for ( vch = ch->in_room->people; vch; vch = vch->next )
			{
			    if ( can_see( rch, vch )
			    &&   is_same_group( vch, victim )
			    &&   number_range( 0, number ) == 0 )
			    {
				target = vch;
				number++;
			    }
			}

			if ( target != NULL )
			    multi_hit( rch, target, TYPE_UNDEFINED );
		    }
		}
	    }
	}
    }

    return;
}



/* servants should assist masters, 10% chance each round and at beggining of an attack */
bool check_servants( CHAR_DATA *attacker, CHAR_DATA *ch )
{
	CHAR_DATA *vch;

	if( ch == attacker )
		return FALSE;

	for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
	{
		if( !IS_NPC(vch)
		||  !IS_SET(vch->act, ACT_SERVANT)
		||  vch->master != ch
		||  vch->fighting
		||  number_percent() < 75 )
			continue;

		act("$n defends $N!", vch, NULL, ch, TO_ROOM );
		act("$n defends you!", vch, NULL, ch, TO_CHAR );

		if( ch->fighting )
			stop_fighting(ch,TRUE);

		multi_hit(vch,attacker,TYPE_UNDEFINED);
		set_fighting(vch,attacker);
		return TRUE;
	}
	return FALSE;
}



/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int     chance;

    if( !IS_NPC(ch) && !IS_NPC(victim) )
	    pk_time = 5;

    if( ch->fighting )
	    update_specs( ch->in_room, (void*)victim, (void*)dt, NULL, UPDATE_ALL, UPDATE_FIGHTING );

    one_hit( ch, victim, dt );
    if ( ch->fighting != victim || dt == gsn_backstab )
	return;

    chance = IS_NPC(ch) ? ch->level/2 : GET_PRAC_SKILL(ch,gsn_second_attack);
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? ch->level/3 : GET_PRAC_SKILL(ch,gsn_third_attack);
    if ( number_percent( ) < chance )
    {
	one_hit( ch, victim, dt );
	if ( ch->fighting != victim )
	    return;
    }

    chance = IS_NPC(ch) ? ch->level / 2 : 0;
    if ( number_percent( ) < chance )
	one_hit( ch, victim, dt );
    
    return;
}



/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0 = 0;
    int dam;
    int diceroll;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
	return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
    }

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
	thac0	= 15 - ch->level;
    }
    else
    {
	thac0	= class_table[ch->class].thac0 - (GET_HITROLL(ch)/2);
    }

    victim_ac = UMAX( -15, GET_AC(victim) / 10 ) - (find_mount(victim) == NULL ? 0 : 2);
    if ( !can_see( ch, victim ) )
	victim_ac -= 4;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
	;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
	/* Miss. */
	damage( ch, victim, 0, dt );
	tail_chain( );
	return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) )
    {
	dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	if ( wield != NULL )
	    dam += dam / 2;
    }
    else
    {
	if ( wield != NULL )
	    dam = number_range( wield->value[1], wield->value[2] );
	else
	    dam = number_range( 1, 4 ) +
		  IS_SET(class_table[ch->class].flags,CLS_MARTIAL_ARTIST) ? dice(5,4) : 0;
    }

    /*
     * Bonuses.
     */
    dam += GET_DAMROLL(ch);
    if( GET_PRAC_SKILL(ch,gsn_enhanced_damage) > 0 )
	dam += (int)((float)dam * IS_NPC(ch) ? 0 : (float)((float)GET_PRAC_SKILL(ch,gsn_enhanced_damage) / 100.0));
    if ( !IS_AWAKE(victim) )
	dam *= 2;
    if ( dam <= 0 )
	dam = 1;

    if( IS_BORG(ch) )
	    dam = dice(5,5)+5;

    damage( ch, victim, dam, dt );
    tail_chain( );
    return;
}



/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    if ( victim->position == POS_DEAD )
	return;

    /*
     * Stop up any residual loopholes.
     */
    if ( dam > 1000 )
    {
	bug( "Damage: %d: more than 1000 points!", dam );
	dam = 1000;
    }

    if ( victim != ch )
    {
	/*
	 * Certain attacks are forbidden.
	 * Most other attacks are returned.
	 */
	if ( is_safe( ch, victim ) )
	    return;

	if ( victim->position > POS_STUNNED )
	{
	    if ( victim->fighting == NULL )
		set_fighting( victim, ch );

	    if( victim->position == POS_STANDING )
		    victim->position = POS_FIGHTING;
	}

	if ( victim->position > POS_STUNNED )
	{
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    /*
	     * If victim is charmed, ch might attack victim's master.
	     */
	    if ( IS_NPC(ch)
	    &&   IS_NPC(victim)
	    &&   IS_AFFECTED(victim, AFF_CHARM)
	    &&   victim->master != NULL
	    &&   victim->master->in_room == ch->in_room
	    &&   number_bits( 3 ) == 0 )
	    {
		stop_fighting( ch, FALSE );
		multi_hit( ch, victim->master, TYPE_UNDEFINED );
		return;
	    }
	}

	/*
	 * More charm stuff.
	 */
	if ( victim->master == ch )
	    stop_follower( victim );

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
	{
	    affect_strip( ch, gsn_invis );
	    affect_strip( ch, gsn_mass_invis );
	    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
	    act( "&+L$n fades into existence.&N", ch, NULL, NULL, TO_ROOM );
	}

	/*
	 * Damage modifiers.
	 */
	if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
		dam /= 2;

	if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch) )
		dam -= dam / 4;

	if( victim->position == POS_RESTING )
		dam = (int)(dam*1.5);

	if( IS_SET(ch->act, PLR_CHARGING) )
		dam *= 3;

	if( IS_AFFECTED(victim, AFF_FIRESHIELD) && dt >= TYPE_UNDEFINED )
	{
		act("&+r$n's fireshield flares wildly and burns you!&N", 
			victim, NULL, ch, TO_VICT );
		act("&+r$n's fireshield flares wildly and burns $N!&N",
			victim, NULL, ch, TO_NOTVICT);
		act("&+rYour fireshield flares wildly and burns $N!&N",
			victim, NULL, ch, TO_CHAR );
		damage( victim, ch, dam, gsn_fireshield );

		if( !ch )
			return;
	}

	if ( dam < 0 )
	    dam = 0;

	update_specs( ch->in_room, (void*)ch, (void*)dam, (void*)dt, UPDATE_ALL, UPDATE_DAMAGE );

	if( !ch || !victim )
		return;

	if( dam < 0 )
		dam = 0;


	/*
	 * Check for disarm, trip, parry, and dodge.
	 */
	if ( dt >= TYPE_HIT )
	{
	    if ( check_parry( ch, victim ) )
		return;
	    if ( check_dodge( ch, victim ) )
		return;
	}

	if( IS_BORG(victim) && number_percent() > 75 )
	{
		act("A &+gdark green shield&N forms around $N as you try to attack $m.",
			ch, NULL, victim, TO_CHAR );
		act("A &+gdark green shield&N forms around you as $n tries to attack you.",
			ch, NULL, victim, TO_VICT );
		act("A &+gdark green shield&N forms around $N as $n tries to attack $M.",
			ch, NULL, victim, TO_NOTVICT );
		dam = 0;
	}
	else
		dam_message( ch, victim, dam, dt );
    }

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */

    victim->hit -= dam;

    if( !IS_NPC(ch) )
	    ch->pcdata->dam_inflicted += dam;
    if( !IS_NPC(victim) )
	    victim->pcdata->dam_received += dam;

    if ( !IS_NPC(victim)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
	victim->hit = 1;
    update_pos( victim );

    switch( victim->position )
    {
    case POS_MORTAL:
	act( "$n is mortally wounded, and will die soon, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char( 
	    "You are mortally wounded, and will die soon, if not aided.\n\r",
	    victim );
	break;

    case POS_INCAP:
	act( "$n is incapacitated and will slowly die, if not aided.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char(
	    "You are incapacitated and will slowly die, if not aided.\n\r",
	    victim );
	break;

    case POS_STUNNED:
	act( "$n is stunned, but will probably recover.",
	    victim, NULL, NULL, TO_ROOM );
	send_to_char("You are stunned, but will probably recover.\n\r",
	    victim );
	break;

    case POS_DEAD:
	act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
	send_to_char( "You have been KILLED!!\n\r\n\r", victim );
	break;

    default:
	if ( dam > victim->max_hit / 4 )
	    send_to_char( "That really did HURT!\n\r", victim );
	if ( victim->hit < victim->max_hit / 4 )
	    send_to_char( "You sure are BLEEDING!\n\r", victim );
	break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
	stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
	group_gain( ch, victim );

	if ( !IS_NPC(victim) )
	{
	    sprintf( log_buf, "%s killed by %s at %d",
		victim->name,
		(IS_NPC(ch) ? ch->short_descr : ch->name),
		victim->in_room->vnum );
	    log_string( log_buf );
	}

	raw_kill( ch, victim );
	return;
    }

    if ( victim == ch )
	return;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, "" );
	    return;
	}
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 )
    {
	if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 1 ) == 0
	&&   victim->hit < victim->max_hit / 2 )
	||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
	&&     victim->master->in_room != victim->in_room ) )
	    do_flee( victim, "" );
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait == 0 )
	do_flee( victim, "" );

    tail_chain( );
    return;
}



bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if( victim->fighting == ch )
		return FALSE;

	if( !IN_WAR(ch) )
	{
		send_to_char("You aren't in a war!\n\r", ch );
		return TRUE;
	}

	if( !IN_WAR(victim) && !IS_IMMORTAL(ch) )
	{
		send_to_char("They aren't in the war!\n\r", ch );
		return TRUE;
	}

	if( IS_SET(war_flags, WFLAG_NO_PKING) && !IS_NPC(victim) )
	{
		send_to_char("No pking yet!\n\r", ch );
		return TRUE;
	}
	
	if( IS_NPC(ch) || IS_NPC(victim) )
		return FALSE;

	return FALSE;
}



bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *parry_obj = NULL;
    int chance;

    if ( !IS_AWAKE(victim) || IS_RESTING(victim) )
	return FALSE;

    if ( (parry_obj = get_eq_char(victim, WEAR_WIELD)) == NULL )
	return FALSE;

    if( IS_NPC(victim) )
	chance = UMIN( 60, victim->level*2 );
    else
	chance = GET_PRAC_SKILL(victim,gsn_parry) / 2;

    if ( number_percent( ) >= chance )
	return FALSE;

    act( "You parry $N's attack with $p.",  victim, parry_obj, ch, TO_CHAR    );
    act( "$n parries your attack with $p.", victim, parry_obj, ch, TO_VICT    );
    act( "$n parries $N's attack with $p.", victim, parry_obj, ch, TO_NOTVICT );
    return TRUE;
}



/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;
    int percent;

    if ( !IS_AWAKE(victim) || IS_RESTING(victim) )
	return FALSE;

    if ( IS_NPC(victim) )
	/* Tuan was here.  :) */
        chance  = UMIN( 60, 2 * victim->level );
    else
	chance = GET_PRAC_SKILL(victim,gsn_dodge)/2;

    chance += get_curr_dex(victim);
    percent = number_percent();

    if ( percent >= chance )
        return FALSE;

    if( ch && victim )
    {
	    if( chance - get_curr_dex(victim) <= percent )
	    {
		    act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
		    act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
		    act( "$N dodges $n's attack.", ch, NULL, victim, TO_NOTVICT );
	    }
	    else
	    {
		    act( "You stealthily dodge $n's attack.", ch, NULL, victim, TO_VICT    );
		    act( "$N stealthily dodges your attack.", ch, NULL, victim, TO_CHAR    );
		    act( "$N stealthily dodges $n's attack.", ch, NULL, victim, TO_NOTVICT );
	    }
    }
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }

    if ( IS_NPC(victim) || victim->hit <= -11 )
    {
	victim->position = POS_DEAD;
	return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch->fighting != NULL )
    {
	bug( "Set_fighting: already fighting", 0 );
	return;
    }

    if( ch->in_room != victim->in_room )
	return;

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
	affect_strip( ch, gsn_sleep );

    ch->fighting = victim;
    if( ch->position == POS_STANDING )
	    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
	if ( fch == ch || ( fBoth && fch->fighting == ch ) )
	{
	    fch->fighting	= NULL;
	    fch->position	= POS_STANDING;
	    update_pos( fch );
	}
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    char *name;

    if ( IS_NPC(ch) )
    {
	name		= ch->short_descr;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC));
	corpse->timer	= number_range( 2, 4 );
	if ( ch->gold > 0 )
	{
	    obj_to_obj( create_money( ch->gold ), corpse );
	    ch->gold = 0;
	}
    }
    else
    {
	name		= ch->name;
	corpse		= create_object(get_obj_index(OBJ_VNUM_CORPSE_PC));
	corpse->timer	= number_range( 25, 40 );
    }

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
	obj_next = obj->next_content;
	obj_from_char( obj );
	if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
	    extract_obj( obj );
	else
	    obj_to_obj( obj, corpse );
    }

    obj_to_room( corpse, ch->in_room );
    return;

}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;

    act( "&+R$n is dead, R.I.P.", ch, NULL, NULL, TO_ROOM );

    if( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;

	if ( ( pexit = was_in_room->exit[door] ) != NULL
	&&   pexit->to_room != NULL
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *borg = NULL;
    char buf[MAX_INPUT_LENGTH];

    stop_fighting( victim, TRUE );
    death_cry( victim );
    make_corpse( victim );

    if( victim->mounted_by )
    {
	    act("As $N falls to the ground dead, $n is sent tumbling as well.",
		    victim->mounted_by, NULL, victim, TO_ROOM );
	    act("As $N falls to the ground dead, you are sent tumbling as well.",
		    victim->mounted_by, NULL, victim, TO_CHAR );
	    victim->mounted_by->position = POS_RESTING;
	    WAIT_STATE(victim->mounted_by, 12);
	    victim->mounted_by = NULL;
    }

    if( IS_BORG(victim) )
    {
		sprintf(buf,"The &+Wcollective&N suffers as %s is terminated", 
			IS_NPC(victim) ? victim->short_descr : victim->name );
		do_warmessage( NULL, buf );
		ch->pcdata->borg_kills++;

		if( IS_NPC(victim) )
		{
			borg = create_mobile( get_mob_index(MOB_VNUM_BORG) );
			char_to_room( borg, get_room_index(ROOM_VNUM_TEMPLE) );
			borg->hunting = ch;

			sprintf(buf,"%s steps out of its collective cubicle and teleports down to Midgaard.",
				borg->short_descr );
			do_warmessage( NULL, buf );
			sprintf(buf,"%s will join the &+Wcollective&N",
				IS_NPC(ch) ? ch->short_descr : ch->name );
			do_chat( borg, buf );
		}
    }

    if( !IS_NPC(victim) ) /* A PKILL! */
    {
	char buf[MAX_INPUT_LENGTH];
	sprintf(buf,"&+W%s terminated by %s!&n",
		victim->name,
		IS_NPC(ch) ? ch->short_descr : ch->name );
	do_warmessage(NULL,buf);

	if( IS_BORG(ch) )
	{
		sprintf(buf,"%s becomes part of the &+Wcollective&N", victim->name );
		do_warmessage( NULL, buf );
		SET_BIT(victim->act, PLR_BORG);
		SET_BIT(victim->affected_by, AFF_DETECT_INVIS);
		SET_BIT(victim->affected_by, AFF_DETECT_HIDDEN);
		SET_BIT(victim->affected_by, AFF_PASS_DOOR);
		SET_BIT(victim->affected_by, AFF_FLYING);
		victim->hit  = victim->max_hit  = 300;
		victim->mana = victim->max_mana = 0;
		update_pos(victim);

		/* stop borgs after player, may be more than one */
		for( borg = char_list; borg; borg = borg->next )
			if( borg->fighting == ch )
				stop_fighting( borg, TRUE );
		return;
	}

	if( !IS_NPC(ch) && ch != victim ) /* self termination reaps no rewards! */
	{
		if( IS_SET(victim->act, PLR_IT) )
		{
			AFFECT_DATA af;

			sprintf(buf,"%s is IT!", ch->name );
			ch->pcdata->it_kills++;
			do_warmessage(NULL,buf);
			SET_BIT(ch->act, PLR_IT);

			ch->hit  = ch->max_hit	= ch->max_hit * 2;
			ch->mana = ch->max_mana	= ch->max_mana * 2;

			af.type			= 0;
			af.duration		= war_timer;
			af.bitvector		= 0;
			af.modifier		= 25;
			af.location		= APPLY_INT;
			affect_to_char( ch, &af );
			af.location		= APPLY_WIS;
			af.modifier		= 25;
			affect_to_char( ch, &af );
			af.location		= APPLY_CON;
			af.modifier		= 25;
			affect_to_char( ch, &af );
			af.location		= APPLY_DEX;
			af.modifier		= 25;
			affect_to_char( ch, &af );
			af.location		= APPLY_STR;
			af.modifier		= 25;
			affect_to_char( ch, &af );
			interpret(ch,"flex");
		}

		ch->pcdata->kills++;
		ch->pcdata->kills_this_war++;

		if( !IS_SET(war_flags, WFLAG_BLOODS) )
		{
			send_to_char("You have been restored for the kill.\n\r", ch );
			ch->hit  = ch->max_hit;
			ch->mana = ch->max_mana;
		}
	}
	else if( IS_NPC(ch) )
		victim->pcdata->mob_deaths++;

	REMOVE_BIT(victim->act, PLR_IT);
	REMOVE_BIT(victim->act, PLR_BORG);
	victim->pcdata->last_war	= current_time;
	victim->pcdata->timer		= 3;
	victim->pcdata->deaths		+= 1;
	victim->pcdata->in_war		= FALSE;
	victim->max_hit			= class_table[victim->class].hp;
	victim->max_mana		= class_table[victim->class].mana;
	victim->hit			= 1;
	victim->mana			= 1;
    }

    if ( IS_NPC(victim) )
    {
	victim->pIndexData->killed++;
	kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
	extract_char( victim, TRUE );
	return;
    }

    extract_char( victim, FALSE );
    while ( victim->affected )
	affect_remove( victim, victim->affected );
    victim->affected_by	= 0;
    victim->armor	= 100;
    victim->position	= POS_RESTING;
    victim->hit		= UMAX( 1, victim->hit  );
    victim->mana	= UMAX( 1, victim->mana );
    save_char_obj( victim );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either... sure it does! this is a pkmud. borlak
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( IS_NPC(ch) || victim == ch )
	return;

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
	if ( !is_same_group( gch, ch ) )
	    continue;

	xp = xp_compute( gch, victim );
	sprintf( buf, "You receive %d experience points.\n\r", xp );
	send_to_char( buf, gch );
	gain_exp( gch, xp );
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim )
{
    int align;
    int xp;

    xp    = !IS_NPC(victim) ? victim->level : victim->exp;
    align = gch->alignment - victim->alignment;

    if ( align >  500 )
    {
	gch->alignment  = UMIN( gch->alignment + (align-500)/4,  1000 );
	xp = 5 * xp / 4;
    }
    else if ( align < -500 )
    {
	gch->alignment  = UMAX( gch->alignment + (align+500)/4, -1000 );
    }
    else
    {
	gch->alignment -= gch->alignment / 4;
	xp = 3 * xp / 4;
    }
    return xp;
}



void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    static char * const attack_table[] =
    {
	"hit",
	"slice",  "stab",  "slash", "whip", "claw",
	"blast",  "pound", "crush", "grep", "bite",
	"pierce", "suction"
    };

    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack = "attack";
    char punct;
    int prct;


    if( dt == TYPE_UNDEFINED )
	return;

    if( dt >= 0 && dt < MAX_SKILL )
    {
	    if( ch && victim && dam > 0 )
	    {
		    sprintf( buf1, "%s does %d points of %s damage to %s with %s. ",
			    ch->name,
			    dam,
		    skill_table[dt].dam_type == TYPE_NONE	? "weapon" : 
		    skill_table[dt].dam_type == TYPE_FIRE	? "fire" : 
		    skill_table[dt].dam_type == TYPE_COLD	? "cold" :
		    skill_table[dt].dam_type == TYPE_ELECTRICITY ? "electric" : 
		    skill_table[dt].dam_type == TYPE_MYSTICAL	? "mystical" : 
		    skill_table[dt].dam_type == TYPE_ACID	? "acid" :
		    skill_table[dt].dam_type == TYPE_SPELL	? "spell" : 
		    skill_table[dt].dam_type == TYPE_BLUNT	? "blunt" : 
		    skill_table[dt].dam_type == TYPE_SLASH	? "slash" :
		    skill_table[dt].dam_type == TYPE_PIERCE	? "pierce" : "unknown(bug!)",
			    victim->name,
			    skill_table[dt].name );
		    battle_log( ch, victim, buf1 );
	    }
	    return;
    }

    prct = (int)(100*((float)dam/(victim->hit == 0 ? 1.0 : (float)victim->hit)));

         if( prct <=   0 ) { vs = "miss"; vp = "misses"; }
    else if( prct <=   3 ) { vs = "scratch"; vp = "scratches"; }
    else if( prct <=   7 ) { vs = "hit"; vp = "hits"; }
    else if( prct <=  10 ) { vs = "decimate"; vp = "decimates"; }
    else if( prct <=  15 ) { vs = "powerfully smash"; vp = "powerfully smashes"; }
    else if( prct <=  20 ) { vs = "massacre"; vp = "massacres"; }
    else if( prct <=  25 ) { vs = "utterly annihilate"; vp = "utterly annihilates"; }
    else if( prct <=  30 ) { vs = "smash a mist of blood out of"; vp = "smashes a mist of blood out of"; }
    else if( prct <=  40 ) { vs = "practically rip in two"; vp = "practically rips in two"; }
    else if( prct <=  50 ) { vs = "mortally wound"; vp = "mortally wounds"; }
    else if( prct <=  99 ) { vs = "introduce death to"; vp = "introduces death to"; }
    else		   { vs = "brutally slay"; vp = "brutally slays"; }

    punct   = (dam <= 20) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
	sprintf( buf1, "$n %s $N.",  vp );
	sprintf( buf2, "You %s $N.", vs );
	sprintf( buf3, "$n %s you.", vp );
    }
    else
    {
	if ( dt >= TYPE_HIT
	&&   dt < TYPE_HIT + sizeof(attack_table)/sizeof(attack_table[0]) )
	    attack	= attack_table[dt - TYPE_HIT];
	else
	{
	    bug( "Dam_message: bad dt %d.", dt );
	    dt  = TYPE_HIT;
	    attack  = attack_table[0];
	}

	if( ch && victim )
	{
		sprintf( buf1, "$n %s $N with $s %s.",  vp, attack );
		sprintf( buf2, "You %s $N with your %s.",  vs, attack );
		sprintf( buf3, "$n %s you with $s %s.", vp, attack );
	}
    }

    if( ch && victim && dam > 0 )
    {
	    act( buf1, ch, NULL, victim, TO_NOTVICT );
	    act( buf2, ch, NULL, victim, TO_CHAR );
	    act( buf3, ch, NULL, victim, TO_VICT );

	    sprintf( buf1, "%s does %d damage to %s with %s.",
		    ch->name,
		    dam,
		    victim->name,
		    dt == TYPE_HIT ? "fists" : attack );
	    battle_log( ch, victim, buf1 );
    }
    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
	return;

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL && number_bits( 1 ) == 0 )
	return;

    act( "$n disarms you!", ch, NULL, victim, TO_VICT    );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_NPC(victim) )
	obj_to_char( obj, victim );
    else
	obj_to_room( obj, victim->in_room );

    return;
}



/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( victim->wait == 0 )
    {
	act( "$n trips you and you go down!", ch, NULL, victim, TO_VICT    );
	act( "You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR    );
	act( "$n trips $N and $N goes down!", ch, NULL, victim, TO_NOTVICT );

	WAIT_STATE( ch,     2 * PULSE_VIOLENCE );
	WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
	victim->position = POS_RESTING;
    }

    return;
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    if( check_servants(ch,victim) )
	    return;

    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int multiplier, i, dam = 0;

    one_argument( argument, arg );

    if( GET_PRAC_SKILL(ch,gsn_backstab) <= 0 )
    {
	send_to_char("You don't know how to backstab.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Backstab whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "How can you sneak up on yourself?\n\r", ch );
	return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
    ||   !IS_SET(obj->extra_flags,ITEM_STABBER) )
    {
	send_to_char( "You need to wield an appropriate stabber.\n\r", ch );
	return;
    }

    if ( victim->fighting != NULL && !IS_SET(class_table[ch->class].flags, CLS_DEADLY_BACKSTAB) )
    {
	send_to_char( "You can't backstab a fighting person.\n\r", ch );
	return;
    }

    if( find_mount(victim) != NULL )
    {
	    act("They are mounted on $N, that might not be such a good idea...",
		    ch, NULL, find_mount(victim), TO_CHAR );
	    return;
    }

    WAIT_STATE( ch, GET_BEAT_SKILL(ch, gsn_backstab) );

    /* rack up the multipleir. */
    multiplier = 4;
    if( IS_SET(class_table[ch->class].flags, CLS_DEADLY_BACKSTAB) )
	    multiplier *= 2;
    multiplier += UMIN( GET_DAMROLL(ch)/5, 3);

    /* calculate da damage */
    for( i = 0; i < multiplier; i++ )
	    dam += dice(obj->value[1],obj->value[2]) + GET_DAMROLL(ch);

    /* hit da man if he aint aware */
    if ( !IS_AWAKE(victim)
    ||  (number_percent() < GET_PRAC_SKILL(ch,gsn_backstab)
    &&   number_percent() > GET_PRAC_SKILL(victim,gsn_awareness)) )
    {
	    act("&+YSuddenly, $n stabs you in the back!&N",
		    ch, NULL, victim, TO_VICT );
	    act("&+Y$N makes some strange gurgling noises as you place $p in $S back.",
		    ch, obj, victim, TO_CHAR );
	    act("&+Y$n places $p in the back of $N, resulting in some strange gurgling noises and blood.&N",
		    ch, obj, victim, TO_NOTVICT );
	    damage( ch, victim, dam, gsn_backstab );
    }
    else
    {
	    act("&+y$n attempts to backstab you, but almost cuts $s finger instead!&N",
		    ch, NULL, victim, TO_VICT );
	    act("&+yYou attempt to backstab $N, but almost cut your finger instead!&N",
		    ch, NULL, victim, TO_CHAR );
	    act("&+y$n attempts to backstab $N, but almost cuts $s finger instead!&N",
		    ch, NULL, victim, TO_NOTVICT );
	    
	    damage( ch, victim, 0, gsn_backstab );
    }
    return;
}

void do_bash( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	OBJ_DATA *shield;

	if( !IS_NPC(ch) && (shield = get_eq_char(ch, WEAR_SHIELD)) == NULL )
	{
		send_to_char("You need a shield to bash.\n\r", ch );
		return;
	}

	if( GET_PRAC_SKILL(ch,gsn_bash) <= 0 )
	{
		send_to_char("You don't know how to bash opponents!\n\r", ch );
		return;
	}

	if( (victim = ch->fighting) == NULL
	&&  (argument && (victim = get_char_room(ch,argument)) == NULL) )
	{
		send_to_char("Bash whom?\n\r", ch );
		return;
	}

	if( !IN_WAR(ch) )
	{
		send_to_char("You aren't in a war!\n\r", ch );
		return;
	}

	WAIT_STATE(ch,GET_BEAT_SKILL(ch,gsn_bash) );

	/* pure percentage bash, good balance */
	if( number_percent() < GET_PRAC_SKILL(ch,gsn_bash) )
	{
		CHAR_DATA *mount;
		/* if dude is mounted.. ya knock em off */
		if( (mount = find_mount(victim)) != NULL )
		{
			act("&+C$n sends $N sprawling off of $S mount!&N",
				ch, NULL, victim, TO_NOTVICT );
			act("&+CYou send $N sprawling off of $S mount!&N",
				ch, NULL, victim, TO_CHAR );
			act("&+C$n sends you sprawling off of your mount!&N",
				ch, NULL, victim, TO_VICT );
			mount->mounted_by = NULL;
		}
		else
		{
			act("&+C$n sends you sprawling with a powerful bash!&N",
				ch, NULL, victim, TO_VICT );
			act("&+C$n sends $N sprawling with a powerful bash!&N",
				ch, NULL, victim, TO_NOTVICT );
			act("&+CYou send $N sprawling with a powerful bash!&N",
				ch, NULL, victim, TO_CHAR );
		}

		victim->position = POS_RESTING;
		WAIT_STATE(victim,GET_BEAT_SKILL(ch,gsn_bash)-2);
		damage( ch, victim, dice(3,3), gsn_bash );
		return;
	}

	act("&+c$n fails miserably at $s attempt to bash you and falls on $s butt!&N",
		ch, NULL, victim, TO_VICT );
	act("&+c$n fails miserably at $s attempt to bash $N and falls on $s butt!&N",
		ch, NULL, victim, TO_NOTVICT );
	act("&+cYou fail miserably at your attempt to bash $N and fall on your butt!&N",
		ch, NULL, victim, TO_CHAR );
	ch->position = POS_RESTING;
	damage( ch, victim, 0, gsn_bash );
	return;
}



void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    int attempt;
    extern const char * dir_name[];

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
	EXIT_DATA *pexit;
	int door;

	door = number_door( );
	if ( ( pexit = was_in->exit[door] ) == 0
	||   pexit->to_room == NULL
	||   IS_SET(pexit->exit_info, EX_CLOSED)
	|| ( IS_NPC(ch)
	&&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
	    continue;

	move_char( ch, door );
	if ( ( now_in = ch->in_room ) == was_in )
	    continue;

	ch->in_room = was_in;

	if( number_percent() > get_curr_dex(ch)*3 )
		act("$n flees $t!", ch, dir_name[door], NULL, TO_ROOM );
	else
		act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
	ch->in_room = now_in;

	if( ch->fighting )
	{
		char buf[30];
		sprintf(buf,"%s flees.",
			ch->name );
		battle_log( ch, ch->fighting, buf );
	}

	stop_fighting( ch, TRUE );
	return;
    }
    send_to_char("&+W!&N&+RPANIC&N&+W!&N You failed to escape!\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;

    one_argument( argument, arg );

    if( GET_PRAC_SKILL(ch,gsn_rescue) <= 0 )
    {
	send_to_char("You don't know how to rescue.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Rescue whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( victim == ch )
    {
	send_to_char( "What about fleeing instead?\n\r", ch );
	return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
	send_to_char( "Doesn't need your help!\n\r", ch );
	return;
    }

    if ( ch->fighting == victim )
    {
	send_to_char( "Too late.\n\r", ch );
	return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
	send_to_char( "That person is not fighting right now.\n\r", ch );
	return;
    }

    WAIT_STATE( ch, GET_BEAT_SKILL(ch,gsn_rescue) );

    if( number_percent() > GET_PRAC_SKILL(ch,gsn_rescue) )
    {
	send_to_char( "&+gYou fail the rescue.&N\n\r", ch );
	act("&+g$n fails $s attempt to rescue you.&N", ch, NULL, victim, TO_VICT );
	act("&+g$n fails $s attempt to rescue $N.&N",   ch, NULL, victim, TO_NOTVICT );
	return;
    }

    act( "&+GYou rescue $N!&N",  ch, NULL, victim, TO_CHAR    );
    act( "&+G$n rescues you!&N", ch, NULL, victim, TO_VICT    );
    act( "&+G$n rescues $N!&N",  ch, NULL, victim, TO_NOTVICT );

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch );
    set_fighting( fch, ch );
    return;
}



void do_palm( CHAR_DATA *ch, char *argument )
{
	SPEC_OBJ_DATA *spec;
	CHAR_DATA *victim = NULL;

	if( GET_PRAC_SKILL(ch,gsn_quivering_palm) <= 0 )
	{
		send_to_char("You don't know how to use the mystical quivering palm!\n\r", ch );
		return;
	}

	if( !IN_WAR(ch) )
	{
		send_to_char("The ancients would be very upset at you!\n\r", ch );
		return;
	}

	if( (victim = ch->fighting) == NULL
	&&  (victim = get_char_room(ch,argument)) == NULL )
	{
		send_to_char("Your opponent is not here.\n\r", ch );
		return;
	}

	if( ch == victim )
	{
		send_to_char("Oooooh, that tickles.\n\r", ch );
		return;
	}

	if( ch->pcdata->timer > 0 )
	{
		send_to_char("You are not ready to do that again yet.\n\r", ch );
		return;
	}

	if( find_mount(victim) != NULL )
	{
		victim = find_mount(victim);
		return;
	}

	WAIT_STATE( ch, GET_BEAT_SKILL(ch,gsn_quivering_palm) );

	if( number_percent() < GET_PRAC_SKILL(ch,gsn_quivering_palm) )
	{
		if( victim->mounted_by )
		{
			act("You attempt to quiver $t but hit $N instead!",
				ch, victim->mounted_by->name, victim, TO_CHAR );
			act("$n attempts to quiver $t but hits $N instead!",
				ch, victim->mounted_by->name, victim, TO_ROOM );
		}
		else
		{
			act("&+yYou strike with perfect balance and send quivering rhythms of power through $N's body.&N",
				ch, NULL, victim, TO_CHAR );
			act("&+y$n strikes with perfect balance and sends quivering rhythms of power through $N's body.&N",
				ch, NULL, victim, TO_NOTVICT );
			act("&+y$n strikes you with perfect balance and sends quivering rhythms of power through your body.&N",
				ch, NULL, victim, TO_VICT );
		}
		damage( ch, victim, dice(2,4), gsn_quivering_palm );

		spec			= add_spec( "spec_quivering_palm", NULL, victim, NULL );
		spec->timer		= number_range( get_curr_dex(ch)/2, get_curr_dex(ch) );
		spec->vch		= ch;
		ch->pcdata->timer	= 2;
		return;
	}

	ch->pcdata->timer = 2;

	send_to_char("You fail miserably at your quivering palm attempt, the ancients would not be proud.\n\r", ch );
	act("$n fails miserably at $s attempt to use the power of quivering palm.",
		ch, NULL, NULL, TO_ROOM );
	return;
}



void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim = NULL;

    if( GET_PRAC_SKILL(ch,gsn_kick) <= 0 )
    {
	send_to_char("You don't know how to kick.\n\r", ch );
	return;
    }

    if( !IN_WAR(ch) )
    {
	    send_to_char("You can't kick out of war!\n\r", ch );
	    return;
    }

    if( (victim = ch->fighting) == NULL
    &&  (victim = get_char_room(ch,argument)) == NULL )
    {
	send_to_char("Kick whom?\n\r", ch );
	return;
    }

    if( ch == victim )
    {
	    send_to_char("Ouch.\n\r", ch );
	    return;
    }

    if( find_mount(victim) != NULL )
    {
	    act("$N is mounted, I think that would be a tad too far a stretch for you.",
		    ch, NULL, victim, TO_CHAR );
	    return;
    }

    WAIT_STATE(ch, GET_BEAT_SKILL(ch,gsn_kick) );

    if( number_percent() < GET_PRAC_SKILL(ch,gsn_kick) )
    {
	switch( dice(1,3) )
	{
	case 1:
		act("&+M$n kicks you in the solar plexus.&N", ch, NULL, victim, TO_VICT );
		act("&+M$n kicks $N in the solar plexus.&N",  ch, NULL, victim, TO_NOTVICT );
		act("&+MYou kick $N in the solar plexus.&N",  ch, NULL, victim, TO_CHAR );
		break;
	case 2:
		act("&+M$n kicks you in the groin, ouch!&N",	ch, NULL, victim, TO_VICT );
		act("&+M$n kicks $N in the groin..ouch.&N",	ch, NULL, victim, TO_NOTVICT );
		act("&+MYou kick $N in the groin, good shot!&N",ch, NULL, victim, TO_CHAR );
		break;
	case 3:
		act("&+M$n's roundhouse hits you squarely in the face!&N",ch, NULL, victim, TO_VICT );
		act("&+M$n's roundhouse hits $N squarely in the face!&N", ch, NULL, victim, TO_NOTVICT );
		act("&+MYour roundhouse hits $N squarely in the face!&N", ch, NULL, victim, TO_CHAR );
		break;
	default:
		break;
	}

	damage( ch, victim, dice(5,5), gsn_kick );
	return;
    }
    else
    {
	act("&+m$n's kick misses you by a mile.&N", ch, NULL, victim, TO_VICT );
	act("&+m$n's kick misses $N by a mile.&N",  ch, NULL, victim, TO_NOTVICT );
	act("&+mYour kick misses $N by a mile.&N",  ch, NULL, victim, TO_CHAR );
	damage( ch, victim, 0, gsn_kick );
	return;
    }

    return;
}




void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if( GET_PRAC_SKILL(ch,gsn_disarm) <= 0 )
    {
	send_to_char( "You don't know how to disarm opponents.\n\r", ch );
	return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL && !IS_SET(class_table[ch->class].flags,CLS_MARTIAL_ARTIST) )
    {
	send_to_char( "You must wield a weapon to disarm.\n\r", ch );
	return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
	send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
	return;
    }

    WAIT_STATE(ch, GET_BEAT_SKILL(ch,gsn_disarm) );

    if( number_percent() < GET_PRAC_SKILL(ch,gsn_disarm) * 2 / 3 )
    {
	char buf[60];

	disarm( ch, victim );

	sprintf(buf,"%s disarms %s",
		ch->name,
		victim->name );

	if( IS_SET(class_table[ch->class].flags,CLS_MARTIAL_ARTIST)
	&&  number_percent() < GET_PRAC_SKILL(ch,gsn_disarm)/3 )
	{
		act("As you disarm $M, you break $p with a flip of your wrist.",	ch, obj, victim, TO_CHAR );
		act("As $n disarms $N, $e breaks $p with a flip of $s wrist.",		ch, obj, victim, TO_NOTVICT);
		act("As $n disarm you, $e breaks your weapon with a flip of $s wrist.", ch, obj, victim, TO_VICT );
		extract_obj(obj);

		sprintf(buf+strlen(buf)," and destroys %s's weapon!",
			victim->name );
	}
	battle_log( ch, victim, buf );

    }
    else
	send_to_char( "You failed.\n\r", ch );
    return;
}



void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}



void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }

    if ( !IS_NPC(victim) && victim->level >= ch->level )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( ch, victim );
    return;
}


/* do_shoot: Not going to make another file for this.  Its combat */

CHAR_DATA *find_char_seen( CHAR_DATA *looker, int *distance, int *direction, char *argument )
{
	ROOM_INDEX_DATA *room;
	ROOM_INDEX_DATA *orig;
	CHAR_DATA *victim = NULL;
	EXIT_DATA *exit;
	int i;

	if( !argument || argument[0] == '\0' || !looker )
		return NULL;

	orig = room = looker->in_room;

	/* look in all directions */
	for( *direction = 0; *direction < 6; (*direction) += 1 )
	{
		/* settable distance */
		for( i = 0; i < 10; i++ )
		{
			if( (exit = room->exit[*direction]) == NULL
			||   exit->to_room == NULL
			||   IS_SET(exit->exit_info, EX_CLOSED) )
				break;
			if( (room = get_room_index(exit->vnum)) == NULL )
				break;

			char_from_room(looker);
			char_to_room(looker,room);

			if( (victim = get_char_room(looker,argument)) != NULL )
				break;
		}
		if(victim)
			break;

		char_from_room(looker);
		char_to_room(looker,orig);
		room = orig;
	}

	char_from_room(looker);
	char_to_room(looker,orig);
	*distance = i;

	return victim;
}


void do_shoot( CHAR_DATA *ch, char *argument )
{
	char buf[200];
	ROOM_INDEX_DATA *room = NULL;
	ROOM_INDEX_DATA *orig = ch->in_room;
	CHAR_DATA *victim;
	OBJ_DATA *quiver;
	OBJ_DATA *bow;
	int distance = 0;
	int direction = 0;
	int dam, i;
	extern const char * dir_name[];
	extern const sh_int rev_dir[];

	if( !IN_WAR(ch) )
	{
		send_to_char("You aren't in a war!\n\r", ch );
		return;
	}

	if( (bow = get_eq_char(ch,WEAR_WIELD)) == NULL
	||  !IS_SET(bow->extra_flags,ITEM_MISSILE_WEAPON) )
	{
		send_to_char("You need to be wielding a bow capable of firing long distances!\n\r", ch );
		return;
	}

	if( (quiver = get_eq_char(ch,WEAR_QUIVER)) == NULL )
	{
		send_to_char("You don't have a quiver, how do you expect to fire a bow?\n\r", ch );
		return;
	}

	if( GET_PRAC_SKILL(ch,gsn_archery) <= 0 )
	{
		send_to_char("You have no idea how to shoot bows and arrows!\n\r", ch );
		return;
	}

	if( !argument || argument[0] == '\0' )
	{
		send_to_char("Shoot whom?\n\r", ch );
		return;
	}

	if( quiver->value[0] <= 0 )
	{
		send_to_char("Your quiver is out of arrows.\n\r", ch );
		return;
	}

	if( quiver->value[3] >= ARROW_MAX || quiver->value[3] < 0 )
	{
		send_to_char("Those don't look like arrows in your quiver.\n\r", ch );
		return;
	}

	if( (victim = get_char_room(ch,argument)) != NULL )
	{
		if( victim == ch )
		{
			send_to_char("You point the bow at yourself, try to shoot, but fail.\n\r", ch );
			return;
		}

		/* the dudes in his friggin room! */
		WAIT_STATE(ch,GET_BEAT_SKILL(ch,gsn_archery));

		act("$n points $p at you and fires!",	ch, bow, victim, TO_VICT );
		act("You point $p at $N and fire!",	ch, bow, victim, TO_CHAR );
		act("$n points $p at $N and fires!",	ch, bow, victim, TO_NOTVICT );

		if( number_percent() > GET_PRAC_SKILL(ch,gsn_archery)
		||  number_percent() < GET_PRAC_SKILL(victim,gsn_awareness)/2 )
		{
			act("$n misses $s shot.",	ch, NULL, victim, TO_ROOM );
			act("You miss your shot.",	ch, NULL, victim, TO_CHAR );
		}

		act("$n hits you, OUCH!",	ch, NULL, victim, TO_VICT );
		act("$n hits $N!",		ch, NULL, victim, TO_NOTVICT );

		/* close range, ouch */
		dam  = dice(bow->value[1]*2,bow->value[2]*2);
		damage(ch,victim,dam,gsn_archery);

		if( victim ) /* if hes still alive */
		{
			if( arrow_table[quiver->value[3]].spell != 0 )
				obj_cast_spell( *arrow_table[quiver->value[3]].spell, ch->level, ch, victim, quiver );
		}
		return;
	}
		

	if( (victim = find_char_seen(ch, &distance, &direction, argument)) == NULL )
	{
		send_to_char("You don't see them anywhere.\n\r", ch );
		return;
	}

	if( distance >= 5 )
	{
		act("You see $M, but $E is too far away to hit from here.", ch, NULL, victim, TO_CHAR );;
		return;
	}

	sprintf(buf,"You arch $p and launch %s %swards.\n\r",
		arrow_table[quiver->value[3]].name,
		dir_name[direction] );
	act( buf, ch, bow, NULL, TO_CHAR );

	sprintf(buf,"$n archs $p and launches %s %swards.",
		arrow_table[quiver->value[3]].name,
		dir_name[direction] );
	act( buf, ch, bow, NULL, TO_ROOM );

	/* shouldnt _have_ to make any checks here.. find_char_seen should of checked all
	 * the exits already
	 */
	for( room = ch->in_room, i = 0; room && i < distance; room = room->exit[direction]->to_room, i++)
	{
		char_from_room(ch);
		char_to_room(ch, room);
		
		sprintf(buf,"%s whistles into the room from the %s.\n\r%s flies %sward.",
			arrow_table[quiver->value[3]].name,
			dir_name[rev_dir[direction]],
			arrow_table[quiver->value[3]].name,
			dir_name[direction] );
		act( buf, ch, NULL, NULL, TO_ROOM );
	}

	room = room->exit[direction]->to_room;
	char_from_room(ch);
	char_to_room(ch,room);
	WAIT_STATE( ch, GET_BEAT_SKILL(ch,gsn_archery) );

	/* hit or not hit, awareness only half as effective here */
	if( number_percent() > GET_PRAC_SKILL(ch,gsn_archery)
	||  number_percent() < GET_PRAC_SKILL(victim,gsn_awareness)/2 )
	{
		/* miss */
		sprintf(buf,"%s whistles into the room and slams into the ground, narrowly missing $n.",
			arrow_table[quiver->value[3]].name );
		act( buf, victim, NULL, ch, TO_NOTVICT );

		sprintf(buf,"%s whistles into the room and slams into the ground, narrowly missing you!",
			arrow_table[quiver->value[3]].name );
		act( buf, victim, NULL, NULL, TO_CHAR );

		send_to_char("You barely missed.\n\r", ch );
		char_from_room(ch);
		char_to_room(ch,orig);
		return;
	}

	sprintf(buf,"%s whistles into the room and rips right through $n!",
		arrow_table[quiver->value[3]].name );
	act( buf, victim, NULL, ch, TO_NOTVICT );

	sprintf(buf,"%s whistles into the room and rips right through your body!",
		arrow_table[quiver->value[3]].name );
	act( buf, victim, NULL, NULL, TO_CHAR );

	dam  = dice(bow->value[1]*2,bow->value[2]*2);
	dam -= (distance)*5;
	damage(ch,victim,dam,gsn_archery);

	/* is he still alive? */
	if( victim )
	{
		if( arrow_table[quiver->value[3]].spell != 0 )
			obj_cast_spell( *arrow_table[quiver->value[3]].spell, ch->level, ch, victim, quiver );
	}

	if( number_percent() > GET_PRAC_SKILL(ch,gsn_archery) )
		send_to_char("You barely missed.\n\r", ch );
	else
		send_to_char("Looks as if you hit your target.\n\r", ch );

	char_from_room(ch);
	char_to_room(ch,orig);
	return;
}



void do_charge( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *mount;
	OBJ_DATA *lance;
	int distance	= 1;
	int direction	= 0;
	int i		= 0;
	extern const char * dir_name[];

	if( GET_PRAC_SKILL(ch,gsn_charge) <= 0 )
	{
		send_to_char("You don't know how to charge!\n\r", ch );
		return;
	}

	if( !IN_WAR(ch) )
	{
		send_to_char("You have to be in the war to charge someone.\n\r", ch );
		return;
	}

	if( !argument || argument[0] == '\0'
	|| ((victim = get_char_room(ch,argument)) == NULL 
	&& (victim = find_char_seen(ch, &distance, &direction, argument)) == NULL) )
	{
		send_to_char("Charge whom?\n\r", ch );
		return;
	}

	for( mount = ch->in_room->people; mount; mount = mount->next_in_room )
	{
		if( mount->mounted_by == ch )
			break;
	}
	if( !mount )
	{
		send_to_char("You must be mounting a creature to charge.\n\r", ch );
		return;
	}

	if( (lance = get_eq_char(ch, WEAR_WIELD)) == NULL
	||  !IS_SET(lance->extra_flags, ITEM_CHARGE_WEAPON) )
	{
		send_to_char("You must be wielding a weapon suitable for charging opponents.\n\r", ch );
		return;
	}

	/* can only attempt a charge once every 2 ticks, too powerful to flee/charge */
	if( !IS_NPC(ch) && ch->pcdata->timer > 0 )
	{
		send_to_char("You aren't ready to charge again yet.\n\r", ch );
		return;
	}

	WAIT_STATE(ch, distance*GET_BEAT_SKILL(ch,gsn_charge) );
	SET_BIT(ch->act, PLR_CHARGING);
	if( !IS_NPC(ch) )
		ch->pcdata->timer = 2;

	if( victim->in_room != ch->in_room )
	{
		send_to_char("You charge out of the room toward your prey!\n\r", ch );
		sprintf(buf,"&+C$n charges $T, riding %s!&N", IS_NPC(mount) ? mount->short_descr : mount->name );
		for( i = 0; i < distance; i++ )
		{
			act( buf, ch, NULL, dir_name[direction], TO_ROOM );
			move_char(ch,direction);
			act("&+CYou charge into [$T]!&N", ch, NULL, ch->in_room->name, TO_CHAR );
			act("&+C$n charges into the room on $N!&N", ch, NULL, mount, TO_ROOM );
		}
		act( buf, ch, NULL, dir_name[direction], TO_ROOM );
		move_char(ch,direction);
		act("&+CYou charge into [$T]!&N",		ch, NULL, ch->in_room->name,	TO_CHAR );
		act("&+C$n charges into the room riding $N!&N", ch, NULL, mount,		TO_ROOM );
	}

	act("&+c$n heads straight for $N!&N",		ch, NULL, victim,		TO_NOTVICT );
	act("&+c$n heads straight for you!&N",		ch, NULL, victim,		TO_VICT );
	act("&+cYou head stright for $N!&N",		ch, NULL, victim,		TO_CHAR );

	if( number_percent() > (GET_PRAC_SKILL(ch,gsn_charge)-distance*5) )
	{ /* heh, oops! */
		act("&+cYou miss $N by a mile and go soaring through the air!&N", ch, NULL, victim, TO_CHAR );
		act("&+c$n misses $N by a mile and goes soaring through the air!&N", ch, NULL, victim, TO_NOTVICT);
		act("&+c$n misses you by a mile and goes soaring through the air!&N", ch, NULL, victim, TO_VICT );
		mount->mounted_by	= NULL;
		ch->position		= POS_RESTING;
		multi_hit(victim,ch,TYPE_UNDEFINED);
		REMOVE_BIT( ch->act, PLR_CHARGING );
		return;
	}

	/* here we go, uwee hee hee. resting AFTER damage or else damage is * 1.5 AND * 3 */
	act("&+CYou pummel $N with your magnifiscient charge!&N", ch, NULL, victim, TO_CHAR );
	act("&+C$n pummels $N with $s magnifiscient charge!&N", ch, NULL, victim, TO_NOTVICT );
	act("&+C$n pummels you with $s magnifiscient charge!&N", ch, NULL, victim, TO_VICT );
	WAIT_STATE( victim, ch->wait/2 );
	damage( ch, victim, dice(lance->value[1], lance->value[2]), gsn_charge );
	victim->position = POS_RESTING;
	REMOVE_BIT( ch->act, PLR_CHARGING );
	return;
}





			

	
