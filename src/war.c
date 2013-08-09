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

/* File added by Borlak aka Michael Morrison */
/* This file is for the war system. */

/* This file has one dangerous global variable, 'players'. Do not directly
 * modify this in functions that _can_ be called during a war.
 */

#include <sys/types.h>
#if !defined WIN32
#include <sys/time.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"


/***********************Local Funcs******************************/
void	begin_war		args( ( void ) );
void	end_war			args( ( void) );
void	equip_char_for_war	args( ( CHAR_DATA *ch ) );
void	random_class		args( ( CHAR_DATA *ch ) );
void	determine_war		args( ( void ) );
void	determine_flags		args( ( void ) );
void	reset_world		args( ( char k ) );
void	nuke_world		args( ( void ) );
void	modify_flag		args( ( int flag ) );
void	transport_char		args( ( CHAR_DATA *ch ) );

bool	participated_this_war	args( ( CHAR_DATA *ch ) );

int	get_war_num		args( ( char *str ) );
int	get_war_flag_num	args( ( char *str ) );
int	calculate_load_percent	args( ( OBJ_INDEX_DATA *obj ) );
int	calculate_obj_cost	args( ( OBJ_DATA *obj ) );
int	find_number_of_players	args( ( void ) );

char *	get_war_name		args( ( int war ) );
char *  get_war_flags		args( ( int flags ) );
char *	team_name		args( ( int team ) );

CHAR_DATA *can_war		args( ( DESCRIPTOR_DATA *d ) );

/*************************Locals Vars***************************/
AREA_DATA *city_area	= NULL;

int war_votes		[MAX_WAR];
int war_votes_wiz	[MAX_WAR]; /* for Wiz influence, Resets at end of war */

int players		= 0;
int pk_time		= 0;
int num_per_team	= 0;
int num_of_teams	= 0;
int war_type		= WAR_DEFAULT;
int war_timer		= 30;
int war_flags		= 0;
int war_flags_wiz	= 0; /* more influence. also resets at end of war */
int war_equip		= WEQUIP_NORM;
int war_default_length	= 30;

time_t war_start_time	= 0;
/****************************************************************/


void war_update( void )
{
	char buf[100];

	switch( war_type )
	{
	case WAR_PIT:
		if( war_timer == 1 && IS_SET(war_flags, WFLAG_NO_PKING) )
		{
			DESCRIPTOR_DATA *d;
			CHAR_DATA *ch;
			CHAR_DATA *fch, *fch_next; /* followers */

			REMOVE_BIT(war_flags, WFLAG_NO_PKING);

			/* erm, trans all to the pit */
			for( d = descriptor_list; d != NULL; d = d->next )
			{
				if( (ch = can_war(d)) == NULL )
					continue;

				/* trans followers for necros/conjureres */
				for( fch = ch->in_room->people; fch; fch = fch_next )
				{
					fch_next = fch->next_in_room;
						if( !IS_NPC(fch) || fch == ch ) continue;
						if( IS_SET(fch->act, ACT_SERVANT) )
					{
						char_from_room(fch);
						char_to_room( ch, get_room_index(ROOM_VNUM_PIT));
					}
				}
					send_to_char("You are summoned to the PIT!\n\r",ch);

				char_from_room(ch);
				char_to_room(ch, get_room_index(ROOM_VNUM_PIT));
				do_flee(ch,"");
			}

			/* next update timer will be -1, only one must survive */
			war_timer = 7;
			do_warmessage(NULL,"You have all been transed to the PIT, fight it out!");
		}
		break;
	default:
		break;
	}

	war_timer--;

	if( war_timer == 0 )
	{
		end_war  ( );
		begin_war( );
	}

	if( IS_SET(war_flags, WFLAG_WRATH) 
	&&  find_number_of_players() > 1
	&&  war_type != WAR_PIT
	&&  --pk_time <= 0 )
	{
		DESCRIPTOR_DATA *d;
		CHAR_DATA *ch;
		CHAR_DATA *wizard = NULL;

		do_warmessage( NULL, "God's wrath and fury is unleashed upon the player killers!!" );
		for( d = descriptor_list; d != NULL; d = d->next )
		{
			if( !d->character
			||  !IS_IMMORTAL(d->character) )
				continue;
			wizard = d->character;
			break;
		}

		sprintf(buf,"%s's wrath smites you with incredible power, leaving you completely devastated.",
			wizard ? wizard->name : "God" );

		for( d = descriptor_list; d != NULL; d = d->next )
		{
			if( !(ch = d->character)
			||  !IN_WAR(ch)
			||  IS_IMMORTAL(ch) )
				continue;

			send_to_char(buf,ch);
			ch->mana -= ch->max_mana/2;
			damage( wizard ? wizard : ch, ch, ch->max_hit/2, TYPE_UNDEFINED );
		}

		pk_time = 5;
	}

	if( war_timer <= 5 && war_timer > 0 && war_type != WAR_PIT )
	{
		sprintf(buf,"%d tick%s until war ends",
			war_timer,
			war_timer != 1 ? "s" : "" );
		do_warmessage(NULL,buf);
	}

}


/************************War-system specific functions************************/


void begin_war( void )
{
	CHAR_DATA *ch;
	DESCRIPTOR_DATA *d;

	war_start_time	= current_time;
	war_timer	= war_default_length;
	city_area	= NULL;
	war_flags	= 0;
	pk_time		= 5;

	find_number_of_players	( );
	determine_war		( );
	determine_flags		( );
	reset_world		( 'A' );

	for( d = descriptor_list; d != NULL; d = d->next )
	{
		if( (ch = can_war(d)) == NULL )		
			continue;

		ch->pcdata->in_war		= FALSE;
		ch->pcdata->kills_this_war	= 0;
	}

	do_warmessage( NULL, "A new war has begun." );
}


void end_war( void )
{
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *ch = NULL;
	CHAR_DATA *ch_next = NULL;
	CHAR_DATA *winner = NULL;
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	int kills = 0;

	do_warmessage( NULL, "This war is over!" );

	for( ch = char_list; ch; ch = ch_next )
	{
		ch_next = ch->next;

		if( IS_NPC(ch)
		||  IS_IMMORTAL(ch) )
			continue;

		/* Remove flags for certain types of wars */
		REMOVE_BIT(ch->act, PLR_IT);
		REMOVE_BIT(ch->act, PLR_BORG);
		REMOVE_BIT(ch->act, PLR_CHARGING);

		/* Stop surveying so they can join the war */
		ch->pcdata->surveying = FALSE;

		while( (obj = ch->carrying) )
		{
			unequip_char(ch,obj);
			extract_obj( obj );
		}
		while( (paf = ch->affected) != NULL )
			affect_remove( ch, paf );

		if( participated_this_war(ch) )
			ch->pcdata->wars_lost[war_type]++;
		else
			continue;

		/* stop fights! */
		if( ch->fighting )
			stop_fighting( ch, TRUE );

		if( kills < ch->pcdata->kills_this_war )
		{
			kills = ch->pcdata->kills_this_war;
			winner = ch;
		}

		ch->pcdata->kills_this_war	= 0;
		ch->pcdata->in_war		= FALSE;
		ch->leader			= NULL;
		ch->master			= NULL;
	}

	if( winner )
	{
		sprintf(buf,"%s is the winner, with %d total kills this war!",
			winner->name,
			kills );
		do_warmessage( NULL, buf );
		send_to_char("You win the war!\n\r", winner );
		winner->pcdata->wars_won[war_type]++;
		winner->pcdata->wars_lost[war_type]--;
	}

	if( !IS_SET(war_flags, WFLAG_NO_NUKE) )
		do_world( NULL, "nuke" );
	else
		do_warmessage( NULL, "Your equipment has been saved" );
	
}


void determine_war( void )
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *ch	= NULL;
	OBJ_DATA *obj	= NULL;
	DESCRIPTOR_DATA	*d;
	int i;
	int hi = 0;
	int war = number_range(0,MAX_WAR-1);

	/* Add some randomness so players dont choose same wars */
	for( i = 0; i < MAX_WAR; i++ )
		war_votes[i] = number_range( 0, players > 5 ? 15 : players*2 ) + war_votes_wiz[i];

	for( d = descriptor_list; d != NULL; d = d->next )
	{
		if( (ch = can_war(d)) == NULL )
			continue;

		war_votes[ch->pcdata->vote]++;
	}

	for( i = 0; i < MAX_WAR; i++ )
	{
		if( war_votes[i] > hi )
		{
			war = i;
			hi = war_votes[i];
		}
	}

	/* reset the things */
	for( i = 0; i < MAX_WAR; i++ )
	{
		war_votes[i]		= 0;
		war_votes_wiz[i]	= 0;
	}

	war_type	= war;
	war_timer	= war_default_length;
	num_per_team	= 1;
	num_of_teams	= 2;

	sprintf(buf,"The war will be a %s war!!", get_war_name(war) );
	do_warmessage( NULL, buf );

	find_number_of_players( );

	switch( war_type )
	{
	case WAR_PIT: /* players equip for 10 mins then trans to pit for massive bloodshed */
		SET_BIT(war_flags, WFLAG_NO_PKING);
		do_warmessage(NULL, "No pkilling until time is up!" );
		war_timer	= 10;
		break;


	case WAR_CAPTURE_FLAG: /* two teams. one flag per team. other team gets flag to home base to win
			        * only war when you die you get thrown back in.
				*/
		{
		ROOM_INDEX_DATA *room;
		int start_team_0;
		int start_team_1;

		do
		{
			i = number_range(3001, 20000);
		} while( (room = get_room_index(i)) == NULL );
		start_team_0 = i;

		do
		{
			i = number_range(3001, 20000);
		} while( (room = get_room_index(i)) == NULL );
		start_team_1 = i;

		obj			= create_object( get_obj_index(OBJ_VNUM_FLAG) );
		obj->value[0]		= 0;
		obj->value[1]		= start_team_0;
		obj->value[2]		= start_team_1;
		sprintf(buf,"a %sflag", team_name(0));
		obj->short_descr	= str_dup(buf);
		sprintf(buf,"A %sflag is on the ground here, waving slightly.", team_name(0));
		obj->description	= str_dup(buf);
		obj_to_room(obj,get_room_index(start_team_0) );
		/* flag made, create homebase */
		obj			= create_object( get_obj_index(OBJ_VNUM_HOMEBASE) );
		sprintf(buf,"The %steam's homebase is here!", team_name(0) );
		obj->description	= str_dup(buf);
		sprintf(buf,"The %steam's homebase", team_name(0) );
		obj->short_descr	= str_dup(buf);
		obj_to_room(obj,get_room_index(start_team_0) );
		/* homebase made.. chat location */
		sprintf(buf,"The %sflag is at [%s]!", team_name(0), get_room_index(start_team_0)->name );
		do_warmessage( NULL, buf );

		obj			= create_object( get_obj_index(OBJ_VNUM_FLAG) );
		obj->value[0]		= 1;
		obj->value[1]		= start_team_1;
		obj->value[2]		= start_team_0;
		sprintf(buf,"a %sflag", team_name(1));
		obj->short_descr	= str_dup(buf);
		sprintf(buf,"A %sflag is on the ground here, waving slightly.", team_name(1));
		obj->description	= str_dup(buf);
		obj_to_room(obj,get_room_index(start_team_1) );
		/* flag made, create homebase */
		obj			= create_object( get_obj_index(OBJ_VNUM_HOMEBASE) );
		sprintf(buf,"The %steam's homebase is here!", team_name(1) );
		obj->description	= str_dup(buf);
		sprintf(buf,"The %steam's homebase", team_name(1) );
		obj->short_descr	= str_dup(buf);
		obj_to_room(obj,get_room_index(start_team_1) );
		sprintf(buf,"The %sflag is at [%s]!", team_name(1), get_room_index(start_team_1)->name );
		do_warmessage( NULL, buf );
		break;
		}

	case WAR_HOT_SKULL:
		{
			OBJ_DATA *obj = create_object( get_obj_index( OBJ_VNUM_HOT_SKULL) );
			obj_to_room( obj, get_room_index( number_range(30409, 30433) ) );
			do_warmessage( NULL, "The Flaming Skull flies into the arena...It will not look for a victim" );
			do_warmessage( NULL, "until there is at least two players, so join!" );
		}
		break;

	default:
		break;
	}
}

void determine_flags( void )
{
	/* Give each flag 8% of loading */
	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_EQ_SCATTERED) )
	{
		SET_BIT(       war_flags, WFLAG_EQ_SCATTERED);
		do_warmessage( NULL, "Equipment is falling from the skies!" );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_RANDOM_EQ) )
	{
		SET_BIT(       war_flags, WFLAG_RANDOM_EQ);
		do_warmessage( NULL, "Equipment has been randomized." );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_LOADALL) )
	{
		SET_BIT(       war_flags, WFLAG_LOADALL);
		do_warmessage( NULL, "Everything has loaded!" );
	}

	if( (number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_BLOODS))
	&&  war_type != WAR_PIT )
	{
		SET_BIT(       war_flags, WFLAG_BLOODS);
		do_warmessage( NULL, "Sacrifice corpses for Bloods." );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_SAME_CLASS) )
	{
		SET_BIT(	war_flags, WFLAG_SAME_CLASS);
		do_warmessage( NULL, "You are all the same class!" );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_NO_NUKE) )
	{
		SET_BIT(	war_flags, WFLAG_NO_NUKE);
		do_warmessage( NULL, "World will not be nuked at end of war" );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_FOUNTAINS) )
	{
		SET_BIT(       war_flags, WFLAG_FOUNTAINS);
		do_warmessage( NULL, "Drink up!" );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_HUNTERS) )
	{
		SET_BIT(       war_flags, WFLAG_HUNTERS);
		do_warmessage( NULL, "Beware the Elven Hunters" );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_WRATH) )
	{
		SET_BIT(       war_flags, WFLAG_WRATH);
		do_warmessage( NULL, "PK or face God's Wrath" );
	}

	if( number_percent() < 8 || IS_SET(war_flags_wiz, WFLAG_RANDOM_CLASS) )
	{
		SET_BIT(	war_flags, WFLAG_RANDOM_CLASS);
		do_warmessage( NULL, "What class are you?" );
	}

	/* reset influence */
	war_flags_wiz	= 0;
}


/*************************World related functions for war**************************/


void nuke_world( void )
{
	CHAR_DATA *mob;
	CHAR_DATA *mob_next;

	while( object_list )
		extract_obj( object_list );
	
	for( mob = char_list; mob != NULL; mob = mob_next )
	{
		mob_next = mob->next;
		if( !IS_NPC(mob) ) 
			continue;
		extract_char(mob, TRUE);
	}
}

/* function needed by reset_world */
OBJ_DATA *create_war_object( OBJ_INDEX_DATA *pObjIndex )
{
	OBJ_DATA *obj;
	if( IS_SET(war_flags, WFLAG_RANDOM_EQ) )
	{
		switch( pObjIndex->item_type )
		{
		case ITEM_KEY:
		case ITEM_BOAT:
			break;
		default:
			if( (obj = load_random_object()) == NULL )
				break;
			return obj;
		}
	}
	return create_object(pObjIndex);
}


/* steal reset_area code and modify.  Borlak */
void reset_world( char k )
{
	extern AREA_DATA *area_first;
	extern int top_room_vnum;
	AREA_DATA *area;
	RESET_DATA *reset;
	CHAR_DATA *mob;
	bool last;
	int i = 0;

	mob  = NULL;
	last = TRUE;

	if( IS_SET(war_flags, WFLAG_NO_MOBS) )
		return;

	for( area = area_first; area != NULL; area = area->next )
	{
		for( reset = area->reset_first; reset != NULL; reset = reset->next )
		{
			ROOM_INDEX_DATA *pRoomIndex;
			MOB_INDEX_DATA *pMobIndex;
			OBJ_INDEX_DATA *pObjIndex;
			OBJ_INDEX_DATA *pObjToIndex;
			EXIT_DATA *pexit;
			OBJ_DATA *obj = NULL;
			OBJ_DATA *obj_to = NULL;

			switch( reset->command )
			{
			case 'M':
				if( k != 'M' && k != 'A' )
					break;
				if( IS_SET(war_flags, WFLAG_NO_MOBS) )
					break;

				if( (pMobIndex = get_mob_index(reset->arg1)) == NULL )
				{
					bug("Reset_world: 'M': bad vnum %d.", reset->arg1 );
					break;
				}
				if( (pRoomIndex = get_room_index(reset->arg3)) == NULL )
				{
					bug("Reset_world: 'R': bad vnum %d.", reset->arg3 );
					break;
				}
				if( pMobIndex->count >= reset->arg2 )
				{
					last = FALSE;
					break;
				}

				mob = create_mobile( pMobIndex );
				
				{
					ROOM_INDEX_DATA *pRoomIndexPrev;
					pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
					if( pRoomIndexPrev != NULL
					&&  IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP) )
						SET_BIT(mob->act, ACT_PET);
				}
				
				last = TRUE;
				char_to_room( mob, pRoomIndex );
				break;

			case 'O':
				if( k != 'O' && k != 'A' )
					break;
				if( (pObjIndex = get_obj_index(reset->arg1)) == NULL )
				{
					bug("Reset_world: 'O':bad vnum %d.", reset->arg1 );
					break;
				}
				if( (pRoomIndex = get_room_index(reset->arg3)) == NULL )
				{
					bug("Reset_world: 'O': bad vnum %d.", reset->arg3 );
					break;
				}

				if( number_percent() > calculate_load_percent(pObjIndex) )
				{
					last = FALSE;
					break;
				}

				if( IS_SET(war_flags, WFLAG_EQ_SCATTERED) )
				{
					do
					{
						i = number_range( 3001, top_room_vnum );
					} while( (pRoomIndex = get_room_index(i)) == NULL );
				}

				obj		= create_war_object(pObjIndex);
				obj->cost	= calculate_obj_cost(obj);
				obj_to_room( obj, pRoomIndex );
				last		= TRUE;
				break;

			case 'P':
				if( k != 'P' && k != 'A' )
					break;
				if( (pObjIndex = get_obj_index(reset->arg1)) == NULL )
				{
					bug("Reset_world: 'P': bad vnum %d.", reset->arg1 );
					break;
				}
				if( (pObjToIndex = get_obj_index(reset->arg3)) == NULL )
				{
					bug("Reset_arae: 'Pp': bad vnum %d.", reset->arg3 );
					break;
				}
				
				if( number_percent() > calculate_load_percent(pObjIndex) )
				{
					last = FALSE;
					break;
				}

				if( number_percent() > calculate_load_percent(pObjToIndex) )
				{
					last = FALSE;
					break;
				}


				obj		= create_object( pObjIndex );
				obj->cost	= calculate_obj_cost( obj );
				obj_to		= create_war_object( pObjToIndex );
				obj->cost	= calculate_obj_cost( obj_to );
				obj_to_obj( obj, obj_to );
				last = TRUE;
				break;

			case 'G':
			case 'E':
				if( k != 'E' && k != 'A' )
					break;
				if( (pObjIndex = get_obj_index(reset->arg1)) == NULL )
				{
					bug("Reset_world: 'E' or 'G': bad vnum %d.", reset->arg1 );
					break;
				}

				if( !last )
					break;

				if( mob == NULL && !IS_SET(war_flags, WFLAG_NO_MOBS) )
				{
					bug( "Reset_world: 'E' or 'G': null mob for vnum %d.", reset->arg1 );
					last = FALSE;
					break;
				}

				if( number_percent() > calculate_load_percent(pObjIndex) )
				{
					last = FALSE;
					break;
				}

				if( mob->pIndexData->pShop != NULL )
				{
					obj = create_war_object( pObjIndex );
					SET_BIT( obj->extra_flags, ITEM_INVENTORY );
				}
				else
				{
					obj = create_war_object( pObjIndex );
				}

				if( IS_SET(war_flags, WFLAG_EQ_SCATTERED) )
				{
					do
					{
						i = number_range( 3001, top_room_vnum );
					} while( (pRoomIndex = get_room_index(i)) == NULL );
					obj_to_room(obj,pRoomIndex);

					last = TRUE;
					break;
				}

				obj_to_char( obj, mob );
				do_wear(mob,obj->name);
				last = TRUE;
				break;

			case 'D':
				if( k != 'D' && k != 'A' )
					break;
				if( (pRoomIndex = get_room_index(reset->arg1)) == NULL )
				{
					bug( "Reset_world: 'D': bad vnum %d.", reset->arg1 );
					break;
				}
				if( (pexit = pRoomIndex->exit[reset->arg2] ) == NULL )
					break;

				switch( reset->arg3 )
				{
				case 0:
					REMOVE_BIT( pexit->exit_info, EX_CLOSED );
					REMOVE_BIT( pexit->exit_info, EX_LOCKED );
					break;
				case 1:
					SET_BIT(    pexit->exit_info, EX_CLOSED );
					REMOVE_BIT( pexit->exit_info, EX_LOCKED );
					break;
				case 2:
					SET_BIT(    pexit->exit_info, EX_CLOSED );
					SET_BIT(    pexit->exit_info, EX_LOCKED );
					break;
				}

				if( war_type == WAR_CITY )
				{
					REMOVE_BIT( pexit->exit_info, EX_CLOSED );
					REMOVE_BIT( pexit->exit_info, EX_LOCKED );
				}

				last = TRUE;
				break;

			case 'R':
				if( k != 'R' && k != 'A' )
					break;
				if( (pRoomIndex = get_room_index(reset->arg1)) == NULL )
				{
					bug("Reset_world: 'R': bad vnum %d.", reset->arg1 );
					break;
				}

				{
					int d0;
					int d1;

					for( d0 = 0; d0 < reset->arg2 - 1; d0++ )
					{
						d1			= number_range( d0, reset->arg2-1 );
						pexit			= pRoomIndex->exit[d0];
						pRoomIndex->exit[d0]	= pRoomIndex->exit[d1];
						pRoomIndex->exit[d1]	= pexit;
					}
				}
				break;
			}
		}
	}
}


/* moan */
void modify_flag( int flag )
{
	CHAR_DATA *ch = NULL;
	OBJ_DATA *obj;
	bool is_set = FALSE;

	if( IS_SET(war_flags, flag) )
	{
		is_set = TRUE;
		REMOVE_BIT(war_flags,flag);
	}
	else
	{
		SET_BIT(war_flags,flag);
	}

	if( flag & WFLAG_NO_MOBS )
	{
		if( is_set )
		{
			reset_world( 'M' );
			return;
		}
		for( ch = char_list; ch != NULL; ch = ch->next )
			if( IS_NPC(ch) )
				extract_char( ch, TRUE );
		return;
	}

	if( flag & WFLAG_FOUNTAINS )
	{
		if( is_set )
		{
			for( obj = object_list; obj != NULL; obj = obj->next )
				if( obj->item_type == ITEM_FOUNTAIN )
					extract_obj( obj );
			return;
		}
		reset_world( 'O' );
		return;
	}

	if( flag & WFLAG_LOADALL )
	{
		if( is_set )
		{
			/* heheh */
			for( obj = object_list; obj != NULL; obj = obj->next )
			{
				if( obj->carried_by && !IS_NPC(obj->carried_by) )
					continue;
				extract_obj(obj);
			}
			return;
		}
		do_world(ch,"nuke");
		do_world(ch,"reset A");
		return;
	}
	return;
}



/************************MISC Char manipulation functions*************************/


/* function needed by transporting chars for a city war */
AREA_DATA *random_area( void )
{
	extern AREA_DATA *area_first;
	AREA_DATA *area;
	int areas = 1;
	int i = 1;

	for( area = area_first; area != NULL; area = area->next, areas++ )
		;

	areas	= number_range(4,areas);

	for( area = area_first; area != NULL; area = area->next )
		if( i++ == areas )
			break;

	if(!area)
	{
		bug("Random_area: had null area. i=%d",i);

		while( war_type == WAR_CITY )
			war_type = number_range(0,MAX_WAR-1);
	}

	return area;
}


void random_class( CHAR_DATA *ch )
{
	int num_class;
	int class;
	static int same = -1;
	extern int CLASSES;

	if( war_type == WAR_GOD )
	{
		do_changeclass( ch, "godling a1s2d3f4" );
		return;
	}

	if( IS_SET(war_flags, WFLAG_SAME_CLASS) )
	{
		if( same == -1 )
			same = number_range(0,CLASSES-1);

		do_changeclass(ch, class_table[same].name);
		return;
	}

	if( !ch->pcdata->random && !IS_SET(war_flags, WFLAG_RANDOM_CLASS) ) 
		return;

	for( num_class = 0; class_table[num_class].name != NULL; num_class++ )
		;

	class		= number_range(0, num_class);
	do_changeclass(ch, class_table[class].name );
}


void equip_char_for_war( CHAR_DATA *ch )
{
	OBJ_DATA *obj;
	int i = 0;

	while( (obj = ch->carrying) )
		extract_obj(obj);

	/* must use latnern first or cant see rest of equip! */
	obj		= create_object( get_obj_index( 30324 ) );
	obj_to_char( obj, ch );
	do_wear(ch,obj->name);

	/* everyone gets a canoe */
	obj		= create_object( get_obj_index( 30325 ) );
	obj_to_char( obj, ch );

	switch( class_table[ch->class].base )
	{
	case CLASS_WAR:
		obj		= create_object( get_obj_index( 30316 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30317 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30318 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30319 ) ); obj_to_char( obj, ch );
 		obj		= create_object( get_obj_index( 30320 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30321 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30322 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30323 ) ); obj_to_char( obj, ch );
		break;

	case CLASS_CLE:
		obj		= create_object( get_obj_index( 30304 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30305 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30306 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30307 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30308 ) ); obj_to_char( obj, ch );
		break;

	case CLASS_THI:
		obj		= create_object( get_obj_index( 30309 ) ); obj_to_char( obj, ch );	
		obj		= create_object( get_obj_index( 30310 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30311 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30312 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30313 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30314 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30315 ) ); obj_to_char( obj, ch );
		break;

	case CLASS_MAG:
		obj		= create_object( get_obj_index( 30300 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30301 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30302 ) ); obj_to_char( obj, ch );
		obj		= create_object( get_obj_index( 30303 ) ); obj_to_char( obj, ch );
		break;
	default:
		break;
	}

	for( i = 0; i < 5; i++ )
	{
		if( class_table[ch->class].obj_list[i] != NULL )
		{
			obj	= create_object( class_table[ch->class].obj_list[i] ); obj_to_char( obj, ch );
		}
	}


	for( obj = ch->carrying; obj; obj = obj->next_content )
	{
		switch( obj->item_type )
		{
		case ITEM_WEAPON:
			switch( war_equip )
			{
			case WEQUIP_FAIR:
				obj->value[1]--;
				obj->value[2]--;
				break;
			case WEQUIP_GOOD:
				obj->value[1]++;
				obj->value[2]++;
				break;
			default:
				break;
			}
			do_wear(ch,obj->name);
			break;
		case ITEM_ARMOR:
			switch( war_equip )
			{
			case WEQUIP_FAIR:
				obj->value[0] -= obj->value[0]/2;
				break;
			case WEQUIP_GOOD:
				obj->value[0] += obj->value[0]/2;
				break;
			default:
				break;
			}	
			do_wear(ch,obj->name);
			break;
		default:
			break;
		}
	}

	return;
}

void transport_char( CHAR_DATA *ch )
{
	ROOM_INDEX_DATA *room = NULL;
	int i;
	extern int top_room_vnum;
	bool area_specific = TRUE;

	if( !city_area && war_type == WAR_CITY )
		city_area = random_area( );

	do
	{
		switch( war_type )
		{
		case WAR_HOT_SKULL:	i = number_range( 30409, 30433 );		break;
		case WAR_CITY:		i = number_range( 3001, top_room_vnum );	break;
		case WAR_PIT:
			if( !IS_SET(war_flags, WFLAG_NO_PKING) )
				i = number_range( 30400, 30408 );
			else
				i = number_range( 3001, 20000 );
			break;
		default:		i = number_range( 3001, 20000 );		break;
		}

		room		= get_room_index(i);

		area_specific	= war_type == WAR_CITY ? room ? room->area == city_area ? 1 : 0 : 1 : 1;

	} while( room == NULL || !area_specific );

	char_from_room(ch);
	char_to_room(ch,room);
	do_look(ch,"auto");
}

/* battle logs between characters
 * Situations where you log:
 * flee
 * all spells cast in combat
 * any type of damage
 */
void battle_log( CHAR_DATA *ch, CHAR_DATA *vch, char *str )
{
	DESCRIPTOR_DATA *d = NULL;
	char buf[MAX_STRING_LENGTH];

	if( !ch || !vch || !str || str[0] == '\0' || ch == vch 
	||  IS_NPC(ch) || IS_NPC(vch) )
		return;

	sprintf( buf, "&+C[%s:%-3dh %-3dm]&Nvs&+C[%s:%-3dh %-3dm]&N: &+y%s&N\n\r",
		ch->name, ch->hit, ch->mana,
		vch->name, vch->hit, vch->mana,
		str );

	for( d = descriptor_list; d; d = d->next )
	{
		if( d->connected > CON_PLAYING
		||  d->character == NULL
		||  d->character->pcdata->surveying == FALSE
		||  (IN_WAR(d->character) && !IS_IMMORTAL(d->character)) )
			continue;

		send_to_char( buf, d->character );
	}

	return;
}


/*************************Player Commands*************************/


/* this sort of has repetitive code from determine_war(), have to re-setup
 * players if they join a war.
 */
void do_join( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA *d = NULL;
	char buf[MAX_STRING_LENGTH];

	if( IS_NPC(ch) )
		return;

	if( ch->pcdata->timer > 0 )
	{
		sprintf(buf,"%d tick%s until you can join.\n\r", ch->pcdata->timer,
			ch->pcdata->timer == 1 ? "" : "s" );
		send_to_char(buf,ch);
		return;
	}

	if( IN_WAR(ch) || ch->pcdata->surveying )
	{
		send_to_char("You mind senses a wrongness in the fabric of space.\n\r", ch );
		return;
	}

	if( (int)ch->pcdata->last_war < (int)war_start_time )
		ch->pcdata->kills_this_war = 0;

	sprintf(buf,"&+g++     %s joins the war     ++&N", ch->name );
	do_warmessage( NULL, buf );

	/* clear the char */
	ch->pcdata->mod_str	= 0;
	ch->pcdata->mod_int	= 0;
	ch->pcdata->mod_wis	= 0;
	ch->pcdata->mod_dex	= 0;
	ch->pcdata->mod_con	= 0;
	ch->armor		= 100;
	ch->hitroll		= 0;
	ch->damroll		= 0;
	ch->saving_throw	= 0;
	ch->gold		= 0;

	if( class_table[ch->class].base == CLASS_SPE )
		ch->class = 0;

	random_class(ch);
	ch->pcdata->in_war = TRUE;
	ch->pcdata->recall = TRUE;
	equip_char_for_war(ch);
	transport_char(ch);

	/* put player in team if team war */
	switch( war_type )
	{
	case WAR_TEAM:
	case WAR_CAPTURE_FLAG:
		{
			CHAR_DATA *vch = NULL;
			CHAR_DATA *leader = NULL;
			int i;
			int num;
			bool found = FALSE;
	
			for( i = 0; i < num_of_teams; i++ )
			{
				for( d = descriptor_list, num = 0; d; d = d->next )
				{
					if( !(vch = d->character)
					||  !IN_WAR(vch)
					||  IS_IMMORTAL(vch)
					||  vch->pcdata->team != i )
						continue;
					
					if( vch->leader == vch )
						leader = vch;
					num++;
				}

				if( !leader )
					leader = ch;
	
				if( num < num_per_team )
				{
					ch->pcdata->team	= i;
					ch->leader		= leader;
					found			= TRUE;
					break;
				}
			}
			if( !found )  /* all the teams are full, increment num per team */
			{
				if( num_per_team < num_of_teams || war_type == WAR_CAPTURE_FLAG )
					num_per_team++;
				else
					num_of_teams++;

				ch->pcdata->team = 0;
			}
			sprintf(buf,"You are a part of the %steam!\n\r", team_name(ch->pcdata->team) );
			send_to_char(buf, ch);
		}
		break;

	case WAR_BORG:
		{
			CHAR_DATA *borg = create_mobile( get_mob_index(MOB_VNUM_BORG) );
			char_to_room(borg, get_room_index(ROOM_VNUM_TEMPLE) );
			borg->hunting = ch;
			do_chat( borg, "You dare join the war and go against the collective?" );
		}
		break;

	case WAR_ARTIFACT:
		if( find_number_of_players() <= 1 )
		{
			OBJ_DATA *obj;
			send_to_char("You get the artifact!\n\r", ch );
			obj = create_object( get_obj_index(OBJ_VNUM_ARTIFACT) );
			obj_to_char(obj,ch);
			sprintf(buf,"%s floats down from the heavens and lands in %s's hands!",
				obj->short_descr, ch->name );
			do_warmessage( NULL, buf );
		}
		break;

	case WAR_ALL_VS_ONE:
		if( find_number_of_players() <= 1 )
		{
			AFFECT_DATA af;

			send_to_char("You're IT!  Run!\n\r", ch );
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
			sprintf(buf,"%s is it!", ch->name );
			do_warmessage( NULL, buf );
		}
		break;

	default:
		break;
	}


	if( !IS_BORG(ch) )
	{
		ch->hit			= number_range( ch->max_hit/2,  ch->max_hit	);
		ch->mana		= number_range( ch->max_mana/2, ch->max_mana	);
	}
	ch->pcdata->last_war	= current_time;

	return;
}



void do_survey( CHAR_DATA *ch, char *argument )
{
	if( IS_NPC(ch) ) return;

	if( IN_WAR(ch) && !IS_IMMORTAL(ch) )
	{
		send_to_char("You wish, punk.\n\r", ch );
		return;
	}

	if( !argument || argument[0] == '\0' )
	{
		send_to_char("Syntax: survey <on/off>\n\r", ch );
		return;
	}

	if( !str_cmp(argument,"on") )
	{
		send_to_char("You are now surveying fights.\n\r", ch );
		ch->pcdata->surveying = TRUE;
		return;
	}

	if( !str_cmp(argument,"off") )
	{
		if( !ch->pcdata->surveying )
		{
			send_to_char("You aren't surveying!\n\r", ch );
			return;
		}
		send_to_char("Surveying off, 7 ticks until you can join the war.\n\r", ch );
		ch->pcdata->surveying	= FALSE;
		ch->pcdata->timer	= 7;
		return;
	}

	do_survey(ch,"");
	return;
}



void do_noplay( CHAR_DATA *ch, char *argument )
{
	if( IS_NPC(ch) ) return;

	if( ch->pcdata->noplay )
	{
		send_to_char("You will join in the next war.\n\r", ch );
		ch->pcdata->noplay = FALSE;
		return;
	}
	send_to_char("You will not join in the next war.\n\r", ch );
	ch->pcdata->noplay = TRUE;
	return;
}


void do_random( CHAR_DATA *ch, char *argument )
{
	if( IS_NPC(ch) )
		return;

	if( ch->pcdata->random )
	{
		ch->pcdata->random = FALSE;
		send_to_char("You are no longer random.\n\r", ch );
		return;
	}
	ch->pcdata->random = TRUE;
	send_to_char("You will now be random.\n\r", ch );
	return;
}


void do_world( CHAR_DATA *ch, char *argument )
{
	char arg[20];
	argument = one_argument(argument,arg);

	if( !arg || arg[0] == '\0' )
	{
		send_to_char("Syntax: world reset <letter>\n\r",		ch );
		send_to_char("or:     world nuke\n\r",				ch );
		send_to_char("\n\r",						ch );
		send_to_char("Valid <letter>'s are: (must be capitalized)\n\r",	ch );
		send_to_char("<A>ll      - Reset everything.\n\r",		ch );
		send_to_char("<O>bjects  - Reset objects on ground.\n\r",	ch );
		send_to_char("<M>mobiles - Reset mobiles.\n\r",			ch );
		send_to_char("<R>ooms    - Randomize random room exits.\n\r",	ch );
		send_to_char("<D>oors    - Set doors back to original position.\n\r", ch );
		return;
	}

	if( !str_cmp(arg,"nuke") )
	{
		nuke_world( );
		do_warmessage(NULL,"The world has been nuked!");
		return;
	}

	if( !str_cmp(arg,"reset") )
	{
	
		if(strlen(argument) > 1 )
		{
			send_to_char("You can only reset with one letter!\n\r", ch );
			return;
		}

		switch(argument[0])
		{
		case 'A':
			do_warmessage(NULL,"World has been reset!");
			break;
		case 'O':
			do_warmessage(NULL,"Ground objects have been reset!");
			break;
		case 'M':
			do_warmessage(NULL,"Mobiles have been reset!");
			break;
		case 'R':
			do_warmessage(NULL,"Random exits have been randomized.");
			break;
		case 'D':
			do_warmessage(NULL,"Doors have been reset.");
			break;
		default:
			send_to_char("That's not a choice!\n\r", ch );
			do_world(ch,"");
			return;
		}

		reset_world(argument[0]);
		return;
	}

	do_world(ch,"");
	return;
}

void do_vote( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int war;

	if( IS_NPC(ch) ) return;

	one_argument(argument,arg);

	if( ch->pcdata->noplay )
	{
		send_to_char("Why would you want to vote?\n\r", ch );
		return;
	}

	if( (war = get_war_num(arg)) == -1 )
	{
		send_to_char("Thats not a type of war.\n\r",			ch );
		send_to_char("Choices:\n\r",					ch );
		send_to_char("  pit city fittest capture team allvsone\n\r",	ch );
		send_to_char("  artifact skull\n\r",				ch );
		return;
	}

	if( war == WAR_BORG && !IS_IMMORTAL(ch) )
	{
		send_to_char("You may not vote for a Borg war.\n\r", ch );
		return;
	}

	ch->pcdata->vote = war;

	sprintf(buf,"You vote for a %s war.\n\r", get_war_name(war) );
	send_to_char(buf,ch);
	return;
}



void do_wset( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int value;
	int i;

	argument = one_argument(argument,arg1);
	argument = one_argument(argument,arg2);

	/* would add equip, but who cares? */
	if( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char("Syntax: wset <field> <value>\n\r",	ch );
		send_to_char("and:    wset wvote <war> <value>\n\r",	ch );
		send_to_char("Field being one of:\n\r",			ch );
		send_to_char("  wtype wtimer wflag\n\r",		ch );
		return;
	}

	if( !str_cmp(arg1, "wtype") )
	{
		if( (value = get_war_num(arg2)) == -1 )
		{
			buf[0] = '\0';
			send_to_char("That's not a valid war type.\n\r", ch );
			send_to_char("Valid types are:\n\r", ch );
			for( i = 0 ; i < MAX_WAR; i++ )
				sprintf(buf+strlen(buf),"  %s%s", get_war_name(i),
					i % 4 == 0 ? "\n\r" : "" );
			send_to_char(buf, ch);
			return;
		}
		sprintf(buf,"War has been changed to a %s war!",
			get_war_name(value) );
		do_warmessage( NULL, buf );
		war_type = value;
		return;
	}

	if( !str_cmp(arg1, "wtimer") )
	{
		value = atoi(arg2);
		if( value < 0 || value > 180 )
		{
			send_to_char("Range is from 0 to 180.\n\r", ch );
			return;
		}
		war_timer = value;
		sprintf(buf,"Timer has been set to %d.", war_timer );
		do_warmessage( NULL, buf );
		return;
	}

	if( !str_cmp(arg1, "wflag") )
	{
		if( (value = get_war_flag_num(arg2)) < 0 )
		{
			send_to_char("Thats not a valid war flag.\n\r", ch );
			return;
		}

		war_flags_wiz += value;
		modify_flag( value );
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp(arg1, "wvote") )
	{
		char arg3[MAX_INPUT_LENGTH];
		int value2; /* the actual vote number */

		strcpy( arg3, argument );

		if( arg3[0] == '\0' )
		{
			send_to_char("Syntax: wset wvote <war> <number of votes>\n\r", ch );
			return;
		}

		if( (value = get_war_num(arg2)) == -1 )
		{
			/* show char the wars with wtypes default */
			do_wset(ch,"wtype blah");
			return;
		}

		value2			= atoi(arg3);	
		war_votes_wiz[value]	+= value2;

		sprintf(buf,"You add %d votes for a %s war.\n\rThese votes will be deleted after next war starts.\n\r",
			value2, get_war_name(value) );
		send_to_char(buf,ch);
		return;
	}
	/* Give default message */
	do_wset( ch, "blah" );
}


/************************Functions with return values*****************************/


char *get_war_flags( int flags )
{
	static char buf[250];

	buf[0] = '\0';

	if( flags & WFLAG_BLOODS	) strcat(buf,"[&+RBloods&N] "		);
	if( flags & WFLAG_FOUNTAINS	) strcat(buf,"[&+BFountains&N] "	);
	if( flags & WFLAG_LOADALL	) strcat(buf,"[&+YLoadall&N] "		);

	if( flags & WFLAG_SAME_CLASS	) strcat(buf,"[&+MSame Class&N] "	);
	if( flags & WFLAG_EQ_SCATTERED	) strcat(buf,"[&+CEq Scattered&N] "	);
	if( flags & WFLAG_RANDOM_EQ	) strcat(buf,"[&+cRandom Eq&N] "	);
	if( flags & WFLAG_NO_NUKE	) strcat(buf,"[&+CNo Nuke&N] "		);
	if( flags & WFLAG_HUNTERS	) strcat(buf,"[&+GHunters&N] "		);
	if( flags & WFLAG_WRATH		) strcat(buf,"[&+LWrath&N] "		);
	if( flags & WFLAG_NO_MOBS	) strcat(buf,"[&+gNo Mobs&N] "		);
	if( flags & WFLAG_RANDOM_CLASS	) strcat(buf,"[&+mRandom Class&N] "	);
	if( flags & WFLAG_NO_PKING	) strcat(buf,"[&=rRNo Pking&N] "	);

	return buf[0] == '\0' ? "None." : buf;
}


char *get_war_name( int war )
{
	switch( war )
	{
	case WAR_PIT:		return "Pit";
	case WAR_TEAM:		return "Team";
	case WAR_CAPTURE_FLAG:	return "Capture-The-Flag";
	case WAR_ARTIFACT:	return "Artifact";
	case WAR_ALL_VS_ONE:	return "All-Vs-One";
	case WAR_CITY:		return "City";
	case WAR_FITTEST:	return "Battle-Of-The-Fittest";
	case WAR_BORG:		return "Borg";
	case WAR_HOT_SKULL:	return "Hot-Skull";
	case WAR_GOD:		return "God";
	default:		return "Hell-If-I-Know";
	}
	return "Hell-If-I-Know";
}

char * team_name( int team )
{
	switch( team )
	{
	case 0:  return "&+g(Green)&n ";
	case 1:  return "&+L(Black)&n ";
	case 2:  return "&+y(Brown)&n ";
	case 3:  return "&+r(Red)&n ";
	case 4:  return "&+W(White)&n ";
	case 5:  return "&+b(Blue)&n ";
	case 6:  return "&+m(Magenta)&n ";
	case 7:  return "&+c(Cyan)&n ";
	case 8:  return "&+Y(Yellow)&n ";
	case 9:  return "&+C(Bright Cyan)&n ";
	case 10: return "&+B(Bright Blue)&n ";
	case 11: return "&+R(Bright Red)&n ";
	case 12: return "&+G(Bright Green)&n ";
	case 13: return "&+M(Bright Magenta)&n ";
	}
	return "Nasty";
}

int team_num( char *str )
{
	if( !str_cmp(str,"green"	)) return 0;
	if( !str_cmp(str,"black"	)) return 1;
	if( !str_cmp(str,"brown"	)) return 2;
	if( !str_cmp(str,"red"		)) return 3;
	if( !str_cmp(str,"white"	)) return 4;
	if( !str_cmp(str,"blue"		)) return 5;
	if( !str_cmp(str,"magenta"	)) return 6;
	if( !str_cmp(str,"cyan"		)) return 7;
	if( !str_cmp(str,"yellow"	)) return 8;
	if( !str_cmp(str,"bcyan"	)) return 9;
	if( !str_cmp(str,"bblue"	)) return 10;
	if( !str_cmp(str,"bred"		)) return 11;
	if( !str_cmp(str,"bgreen"	)) return 12;
	if( !str_cmp(str,"bmagenta"	)) return 13;
	return 0;
}

int get_war_num( char *str )
{
	if( !str_cmp(str, "pit"		)) return WAR_PIT;
	if( !str_cmp(str, "team"	)) return WAR_TEAM;
	if( !str_cmp(str, "capture"	)) return WAR_CAPTURE_FLAG;
	if( !str_cmp(str, "artifact"	)) return WAR_ARTIFACT;
	if( !str_cmp(str, "allvsone"	)) return WAR_ALL_VS_ONE;
	if( !str_cmp(str, "city"	)) return WAR_CITY;
	if( !str_cmp(str, "fittest"	)) return WAR_FITTEST;
	if( !str_cmp(str, "skull"	)) return WAR_HOT_SKULL;
	if( !str_cmp(str, "god"		)) return WAR_GOD;
	if( !str_cmp(str, "borg"	)) return WAR_BORG;
					   return -1;
}


int get_war_flag_num( char *str )
{


	if( !str_cmp(str, "eq_scattered"	)) return WFLAG_EQ_SCATTERED;
	if( !str_cmp(str, "random_eq"		)) return WFLAG_RANDOM_EQ;
	if( !str_cmp(str, "loadall"		)) return WFLAG_LOADALL;
	if( !str_cmp(str, "no_pking"		)) return WFLAG_NO_PKING;
	if( !str_cmp(str, "same_class"		)) return WFLAG_SAME_CLASS;
	if( !str_cmp(str, "no_nuke"		)) return WFLAG_NO_NUKE;
	if( !str_cmp(str, "bloods"		)) return WFLAG_BLOODS;
	if( !str_cmp(str, "fountains"		)) return WFLAG_FOUNTAINS;
	if( !str_cmp(str, "hunters"		)) return WFLAG_HUNTERS;
	if( !str_cmp(str, "wrath"		)) return WFLAG_WRATH;
	if( !str_cmp(str, "no_mobs"		)) return WFLAG_NO_MOBS;
	if( !str_cmp(str, "random_class"	)) return WFLAG_RANDOM_CLASS;

	return -1;
}


int calculate_load_percent( OBJ_INDEX_DATA *obj )
{
	int percent = 100;

	if( IS_SET(war_flags, WFLAG_LOADALL) )
		return 100;

	if( !obj )
	{
		bug("Calculate_load_percent: called with null object!", 0 );
		return 0;
	}

	switch( obj->item_type )
	{
	case ITEM_LIGHT:
	case ITEM_KEY:
	case ITEM_BOAT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_FURNITURE:
		return 100;

	case ITEM_SCROLL:
	case ITEM_WAND:
	case ITEM_STAFF:
	case ITEM_PILL:
		percent -= obj->value[0];
		percent -= obj->value[1] > 0 ? 10 : 0;
		percent -= obj->value[2] > 0 ? 20 : 0;
		percent -= obj->value[3] > 0 ? 30 : 0;
		return percent;

	case ITEM_WEAPON:
		percent -= obj->value[1]*10;
		percent -= obj->value[2]*5;
		percent += (10 - obj->weight) > 0 ? (10 - obj->weight) : 0;
		return percent;

	case ITEM_ARMOR:
		percent -= obj->value[0]*8;
		return percent;

	case ITEM_FOOD:
		return number_range(0,100);

	case ITEM_DRINK_CON:
		percent -= liq_table[obj->value[2]].percent_deduction;
		return percent;

	case ITEM_FOUNTAIN:
		if( IS_SET(war_flags, WFLAG_FOUNTAINS) )
			return 100;
		return 0;
	default:
		return 100;
	}
	return 100;
}


int calculate_obj_cost( OBJ_DATA *obj )
{
	switch( obj->item_type )
	{
	default:
		return 1000;
	}
	return 1000;
}


int find_number_of_players( void )
{
	DESCRIPTOR_DATA *d;

	/* Modifies local variable players */
	/* Called at begin_war( ) */
	players = 0;

	for( d = descriptor_list; d != NULL; d = d->next )
	{
		if( can_war(d) == NULL
		||  d->character->pcdata->in_war == FALSE )
			continue;

		players++;
	}
	return players;
}


bool participated_this_war( CHAR_DATA *ch )
{
	if( (int)ch->pcdata->last_war >= (int)war_start_time )
		return TRUE;

	return FALSE;
}


CHAR_DATA *can_war( DESCRIPTOR_DATA *d )
{
	if( d->connected > CON_PLAYING )
		return NULL;
	if( !d->character )
		return NULL;
	if( d->character->pcdata->noplay )
		return NULL;
	if( IS_IMMORTAL(d->character) )
		return NULL;
	return d->character;
}



/***************************EOF***************************/




