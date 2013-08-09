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



char *	const	dir_name	[]		=
{
    "north", "east", "south", "west", "up", "down"
};

const	sh_int	rev_dir		[]		=
{
    2, 3, 0, 1, 5, 4
};

const	sh_int	movement_loss	[SECT_MAX]	=
{
    1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
bool	has_key		args( ( CHAR_DATA *ch, int key ) );



void move_char( CHAR_DATA *ch, int door )
{
	char buf[100];
	CHAR_DATA *fch;
	CHAR_DATA *fch_next;
	CHAR_DATA *mount;
	CHAR_DATA *mover;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;

	if ( door < 0 || door > 5 )
	{
		bug( "Do_move: bad door %d.", door );
		return;
	}

	in_room = ch->in_room;
	if ( ( pexit   = in_room->exit[door] ) == NULL
	||   ( to_room = pexit->to_room      ) == NULL )
	{
		send_to_char( "Alas, you cannot go that way.\n\r", ch );
		return;
	}

	if ( IS_SET(pexit->exit_info, EX_CLOSED)
	&&   !IS_AFFECTED(ch, AFF_PASS_DOOR) )
	{
		act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
		return;
	}

	if ((IS_AFFECTED(ch, AFF_CHARM)
	&&   ch->master != NULL
	&&   in_room == ch->master->in_room)
	||   ch->mounted_by )
	{
		send_to_char( "What?  And leave your beloved master?\n\r", ch );
		return;
	}

	if ( room_is_private( to_room ) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	if( in_room->area != to_room->area
	&&  war_type == WAR_CITY )
	{
	    send_to_char("You can't leave the area!\n\r", ch );
	    return;
	}

	/* is player mounting something? */
	for( mount = in_room->people; mount != NULL; mount = mount->next_in_room )
	{
		if( mount->mounted_by == ch )
		{
			/* check for balance */
			if( number_percent() > GET_PRAC_SKILL(ch,gsn_ride) )
			{
				act("You lose control of $N and get thrown off!",
					ch, NULL, mount, TO_CHAR );
				act("$n loses control of $N and gets thrown off!",
					ch, NULL, mount, TO_ROOM );
				mount->mounted_by = NULL;
				ch->position = POS_RESTING;
				return;
			}
			break;
		}
	}

	if( !mount )
		mover = ch;
	else
		mover = mount;

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR )
	{
		if ( !IS_AFFECTED(mover, AFF_FLYING) )
		{
			send_to_char( "You can't fly.\n\r", mover );
			act("$n tries to move $T, but cant fly.\n\r", mover, NULL, dir_name[door], TO_ROOM );
			return;
		}
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM
	||   to_room->sector_type == SECT_WATER_NOSWIM )
	{
		OBJ_DATA *obj;
		bool found;

		/*
		 * Look for a boat.
		 */
		found = FALSE;

		if( IS_AFFECTED(mover, AFF_FLYING) )
		{
			found = TRUE;
		}
		else
		{
			for ( obj = mover->carrying; obj != NULL; obj = obj->next_content )
			{
				if ( obj->item_type == ITEM_BOAT )
				{
					found = TRUE;
					break;
				}
			}
		}

		if ( !found )
		{
			send_to_char( "You need a boat to go there.\n\r", mover );
			act("$n tries to move $T, but doesn't have a boat.\n\r", ch, NULL, dir_name[door], TO_ROOM );
			return;
		}
	}

	if( update_specs(in_room, (void*)mover, (void*)door, NULL, UPDATE_ALL, UPDATE_EXITED) == FALSE )
		return;
	if( update_specs(to_room, (void*)mover, (void*)door, NULL, UPDATE_ALL, UPDATE_ENTERED) == FALSE )
		return;

	if( IS_SET(class_table[mover->class].flags, CLS_SLOW) )
		WAIT_STATE( ch, 2 );

	/* if you're riding a mob, only the mob's visibility matters */
	/* also special enter/exit messages while riding             */
	if( mount )
	{
		/* if you charge, you're always seen, and you dont see in rooms, and noone can follow you */
		if( !IS_SET(ch->act, PLR_CHARGING) )
		{
			if ( !IS_AFFECTED(mount, AFF_SNEAK)
			&& ( IS_NPC(mount) || !IS_SET(mount->act, PLR_WIZINVIS) ) )
			{
				sprintf(buf,"$n leaves $T riding %s.", IS_NPC(mount) ? mount->short_descr : mount->name );
				act( buf, ch, NULL, dir_name[door], TO_ROOM );
			}
		}

		char_from_room( ch );
		char_to_room( ch, to_room );
		char_from_room( mount );
		char_to_room( mount, to_room );

		if( !IS_SET(ch->act, PLR_CHARGING) )
		{
			if ( !IS_AFFECTED(mount, AFF_SNEAK)
			&& ( IS_NPC(mount) || !IS_SET(mount->act, PLR_WIZINVIS) ) )
				act( "$n has arrived riding $N.", ch, NULL, mount, TO_ROOM );
	
			do_look( ch, "auto" );
			for ( fch = in_room->people; fch != NULL; fch = fch_next )
			{
				fch_next = fch->next_in_room;
				if ( fch->master == mount->mounted_by && fch->position == POS_STANDING )
				{
					if( !IS_NPC(fch) && !HAS_CONSENT(ch,fch) )
					{
						if( fch->gold < 10 )
							continue;
						else
							fch->gold -= 10;
					}

					act( "You follow $N.", fch, NULL, mount->mounted_by, TO_CHAR );
					move_char( fch, door );
				}
			}
		}
		return;
	}

	if ( !IS_AFFECTED(mover, AFF_SNEAK)
	&& ( IS_NPC(mover) || !IS_SET(mover->act, PLR_WIZINVIS) ) )
		act( "$n leaves $T.", mover, NULL, dir_name[door], TO_ROOM );

	char_from_room( mover );
	char_to_room( mover, to_room );

	if ( !IS_AFFECTED(mover, AFF_SNEAK)
	&& ( IS_NPC(mover) || !IS_SET(mover->act, PLR_WIZINVIS) ) )
		act( "$n has arrived.", mover, NULL, NULL, TO_ROOM );

	do_look( mover, "auto" );

	for ( fch = in_room->people; fch != NULL; fch = fch_next )
	{
		fch_next = fch->next_in_room;
		if ( fch->master == mover && fch->position == POS_STANDING )
		{
			if( !IS_NPC(fch) && !HAS_CONSENT(ch,fch) )
			{
				if( fch->gold < 10 )
					continue;
				else
					fch->gold -= 10;
			}

			act( "You follow $N.", fch, NULL, mover, TO_CHAR );
			move_char( fch, door );
		}
	}

	return;
}



void do_north( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_NORTH );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

	 if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL
	    &&   IS_SET(pexit->exit_info, EX_ISDOOR)
	    &&   pexit->keyword != NULL
	    &&   is_name( arg, pexit->keyword ) )
		return door;
	}
	act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
	return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	send_to_char( "You can't do that.\n\r", ch );
	return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'open object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'open door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* open the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'close object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }

	SET_BIT(obj->value[1], CONT_CLOSED);
	send_to_char( "Ok.\n\r", ch );
	act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'close door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_CLOSED);
	act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "Ok.\n\r", ch );

	/* close the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;

	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
    }

    return;
}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
	if ( obj->pIndexData->vnum == key )
	    return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'lock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'lock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit	= ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }

	SET_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* lock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&&   pexit_rev->to_room == ch->in_room )
	{
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'unlock object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, obj->value[2] ) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'unlock door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !has_key( ch, pexit->key) )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* unlock the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if( GET_PRAC_SKILL(ch,gsn_pick_lock) <= 0 )
    {
	send_to_char("You don't know how to pick locks.\n\r", ch );
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }

    WAIT_STATE( ch, GET_BEAT_SKILL(ch,gsn_pick_lock) );

    if( number_percent() > GET_PRAC_SKILL(ch,gsn_pick_lock) )
    {
	send_to_char( "You failed.\n\r", ch);
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
    {
	/* 'pick object' */
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks $p.", ch, obj, NULL, TO_ROOM );
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}
    }

    return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
    switch ( ch->position )
    {
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP) )
	    { send_to_char( "You can't wake up!\n\r", ch ); return; }

	send_to_char( "You wake and stand up.\n\r", ch );
	act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_RESTING:
	send_to_char( "You stand up.\n\r", ch );
	act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;

    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    if( find_mount(ch) != NULL )
    {
	    send_to_char("You can't rest while atop a mount!\n\r", ch );
	    return;
    }
    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
	break;

    case POS_STANDING:
	send_to_char( "You rest.\n\r", ch );
	act( "$n rests.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_sleep( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mount;

    if( (mount = find_mount(ch)) != NULL )
    {
	    act("As $n falls asleep, $e falls off $N.",
		    ch, NULL, mount, TO_ROOM );
	    mount->mounted_by = NULL;
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;

    case POS_RESTING:
    case POS_STANDING: 
	send_to_char( "You sleep.\n\r", ch );
	act( "$n sleeps.", ch, NULL, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;

    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
	{ do_stand( ch, argument ); return; }

    if ( !IS_AWAKE(ch) )
	{ send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{ send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
	{ act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
	{ act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    act( "You wake $M.", ch, NULL, victim, TO_CHAR );
    act( "$n wakes you.", ch, NULL, victim, TO_VICT );
    victim->position = POS_STANDING;
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if( GET_PRAC_SKILL(ch,gsn_sneak) <= 0 )
    {
	send_to_char("You don't know how to sneak around.\n\r", ch );
	return;
    }

    send_to_char( "You attempt to move silently.\n\r", ch );
    affect_strip( ch, gsn_sneak );

    if( number_percent() <= GET_PRAC_SKILL(ch,gsn_sneak) )
    {
	af.type      = gsn_sneak;
	af.duration  = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = AFF_SNEAK;
	affect_to_char( ch, &af );
    }

    return;
}



void do_hide( CHAR_DATA *ch, char *argument )
{
    if( GET_PRAC_SKILL(ch,gsn_hide) <= 0 )
    {
	send_to_char("You don't know how to hide in shadows.\n\r", ch );
	return;
    }

    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
	REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if( number_percent() <= GET_PRAC_SKILL(ch,gsn_hide) )
	SET_BIT(ch->affected_by, AFF_HIDE);

    return;
}



/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis			);
    affect_strip ( ch, gsn_mass_invis			);
    affect_strip ( ch, gsn_sneak			);
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;

    if( IS_NPC(ch) )
	    return;

    if( IN_WAR(ch)
    && (!ch->pcdata->recall || (war_type == WAR_PIT && !IS_SET(war_flags,WFLAG_NO_PKING))) )
    {
	    send_to_char("Yeah, uh-huh.\n\r", ch );
	    return;
    }
    if( ch->fighting )
    {
	    send_to_char("You can't recall while fighting!\n\r", ch );
	    return;
    }

    act( "$n prays for transportation!", ch, 0, 0, TO_ROOM );
    ch->pcdata->recall = FALSE;

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
    {
	send_to_char( "You are completely lost.\n\r", ch );
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


void do_mount( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;

	if( !argument
	|| (victim = get_char_room(ch,argument)) == NULL )
	{
		send_to_char("Mount whom?\n\r", ch );
		return;
	}

	if( GET_PRAC_SKILL(ch,gsn_ride) <= 0 )
	{
		send_to_char("You don't know how to ride any creatures.\n\r", ch );
		return;
	}

	if( !IS_NPC(victim) )
	{
		send_to_char("Funny, very funny..\n\r", ch );
		return;
	}

	if( find_mount(ch) != NULL )
	{
		send_to_char("You are alreading mounting a creature!\n\r", ch );
		return;
	}

	if( !IS_SET(victim->act, ACT_MOUNTABLE) )
	{
		send_to_char("You can't mount that type of creature.\n\r", ch );
		return;
	}

	if( victim->mounted_by )
	{
		send_to_char("That creature is already mounted by someone!\n\r", ch );
		return;
	}

	if( !IN_WAR(ch) )
	{
		send_to_char("You only need to mount while in a war.\n\r", ch );
		return;
	}

	/* found valid creature, and is in war */
	if( number_percent() > GET_PRAC_SKILL(ch,gsn_ride) )
	{
		act("As $n tries to mount $N, $E knocks $n off with a good kick.",
			ch, NULL, victim, TO_ROOM );
		act("As you try to mount $N, $E knocks you off with a good kick.",
			ch, NULL, victim, TO_CHAR );
		return;
	}

	act("$n mounts $N skillfully.", ch, NULL, victim, TO_ROOM );
	act("You mount $N skillfully.", ch, NULL, victim, TO_CHAR );
	victim->mounted_by = ch;
	return;
}


void do_dismount( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *mount;

	if( (mount = find_mount(ch)) == NULL )
	{
		send_to_char("You aren't riding anything!\n\r", ch );
		return;
	}

	mount->mounted_by = NULL;
	act("$n dismounts $N.", ch, NULL, mount, TO_ROOM );
	act("You dismount $N.", ch, NULL, mount, TO_CHAR );
	return;
}



