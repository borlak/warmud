
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

/* Fille added by Borlak.  This file is for room specials, similar to
 * Mob specials, but, obviously, for rooms instead :)
 * In this file, unlike the other special functions, the boolean DOES
 * matter, if someone is moving and it returns FALSE, it will not let
 * them move.  you have to return TRUE to let players move in rooms
 * these functinos are set.  
 * Another addition. In the spec update in update.c if it returns false
 * on its final call (ROOM_END) then it will keep going for another tick.
 * so return TRUE to kill the function.
 *
 * these functions should never be called with a null to_room or from_room
 */

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

/* Rooms */
DECLARE_SPEC_FUN(	spec_wall		);
DECLARE_SPEC_FUN(	spec_vines		);
DECLARE_SPEC_FUN(	spec_tornado		);
DECLARE_SPEC_FUN(	spec_alarm		);
/* Objects */
DECLARE_SPEC_FUN(	spec_artifact		);
DECLARE_SPEC_FUN(	spec_flag		);
DECLARE_SPEC_FUN(	spec_hot_skull		);
/* Mobs */
DECLARE_SPEC_FUN(	spec_borg		);
DECLARE_SPEC_FUN(	spec_elemental_fire	);
DECLARE_SPEC_FUN(	spec_elemental_water	);
DECLARE_SPEC_FUN(	spec_elemental_earth	);
DECLARE_SPEC_FUN(	spec_elemental_air	);
DECLARE_SPEC_FUN(	spec_elemental_greater	);
DECLARE_SPEC_FUN(	spec_assassin		);
DECLARE_SPEC_FUN(	spec_warrior		);
DECLARE_SPEC_FUN(	spec_cleric		);
DECLARE_SPEC_FUN(	spec_mage		);
DECLARE_SPEC_FUN(	spec_sunray		);
DECLARE_SPEC_FUN(	spec_reflect		);
DECLARE_SPEC_FUN(	spec_death		);
DECLARE_SPEC_FUN(	spec_animate_dead	);
DECLARE_SPEC_FUN(	spec_quivering_palm	);

SPEC_FUN *spec_lookup( const char *name )
{
	/* Rooms */
	if( !str_cmp( name, "spec_wall"			) ) return spec_wall;
	if( !str_cmp( name, "spec_vines"		) ) return spec_vines;
	if( !str_cmp( name, "spec_tornado"		) ) return spec_tornado;
	if( !str_cmp( name, "spec_alarm"		) ) return spec_alarm;
	/* Objects */
	if( !str_cmp( name, "spec_artifact"		) ) return spec_artifact;
	if( !str_cmp( name, "spec_flag"			) ) return spec_flag;
	if( !str_cmp( name, "spec_hot_skull"		) ) return spec_hot_skull;
	/* Mobiles */
	if( !str_cmp( name, "spec_borg"			) ) return spec_borg;
	if( !str_cmp( name, "spec_elemental_fire"	) ) return spec_elemental_fire;
	if( !str_cmp( name, "spec_elemental_water"	) ) return spec_elemental_water;
	if( !str_cmp( name, "spec_elemental_earth"	) ) return spec_elemental_earth;
	if( !str_cmp( name, "spec_elemental_air"	) ) return spec_elemental_air;
	if( !str_cmp( name, "spec_elemental_greater"	) ) return spec_elemental_greater;
	if( !str_cmp( name, "spec_assassin"		) ) return spec_assassin;
	if( !str_cmp( name, "spec_warrior"		) ) return spec_warrior;
	if( !str_cmp( name, "spec_cleric"		) ) return spec_cleric;
	if( !str_cmp( name, "spec_mage"			) ) return spec_mage;
	if( !str_cmp( name, "spec_sunray"		) ) return spec_sunray;
	if( !str_cmp( name, "spec_reflect"		) ) return spec_reflect;
	if( !str_cmp( name, "spec_death"		) ) return spec_death;
	if( !str_cmp( name, "spec_animate_dead"		) ) return spec_animate_dead;
	if( !str_cmp( name, "spec_quivering_palm"	) ) return spec_quivering_palm;
	return 0;
}

char *spec_name_lookup( SPEC_FUN *spec )
{
	/* Rooms */
	if( spec == spec_wall			) return "spec_wall";
	if( spec == spec_vines			) return "spec_vines";
	if( spec == spec_tornado		) return "spec_tornado";
	if( spec == spec_alarm			) return "spec_alarm";
	/* Objects */
	if( spec == spec_artifact		) return "spec_artifact";
	if( spec == spec_flag			) return "spec_flag";
	if( spec == spec_hot_skull		) return "spec_hot_skull";
	/* Mobiles */
	if( spec == spec_borg			) return "spec_borg";
	if( spec == spec_elemental_fire		) return "spec_elemental_fire";
	if( spec == spec_elemental_water	) return "spec_elemental_water";
	if( spec == spec_elemental_earth	) return "spec_elemental_earth";
	if( spec == spec_elemental_air		) return "spec_elemental_air";
	if( spec == spec_elemental_greater	) return "spec_elemental_greater";
	if( spec == spec_assassin		) return "spec_assassin";
	if( spec == spec_warrior		) return "spec_warrior";
	if( spec == spec_cleric			) return "spec_cleric";
	if( spec == spec_mage			) return "spec_mage";
	if( spec == spec_sunray			) return "spec_sunray";
	if( spec == spec_reflect		) return "spec_reflect";
	if( spec == spec_death			) return "spec_death";
	if( spec == spec_animate_dead		) return "spec_animate_dead";
	if( spec == spec_quivering_palm		) return "spec_quivering_palm";
						  return "none";
}


bool has_spec( char *str, SPEC_OBJ_DATA *list )
{
	SPEC_OBJ_DATA *spec;

	for( spec = list; spec; spec = spec->next )
	{
		if( !str_cmp(spec_name_lookup(spec->spec_fun), str) )
			return TRUE;
	}

	return FALSE;
}



SPEC_OBJ_DATA *new_spec_obj( void )
{
	SPEC_OBJ_DATA *spec_obj;

	if( spec_obj_free == NULL )
	{
		spec_obj	= alloc_perm( sizeof(*spec_obj) );
	}
	else
	{
		spec_obj	= spec_obj_free;
		spec_obj_free	= spec_obj->next;
		spec_obj->next	= NULL;
	}
	return spec_obj;
}

SPEC_OBJ_DATA *add_spec( const char *spec_name, ROOM_INDEX_DATA *room, CHAR_DATA *ch, OBJ_DATA *obj )
{
	SPEC_OBJ_DATA *spec_obj;
	SPEC_FUN *spec_fun;
	ROOM_INDEX_DATA *r_test;
	OBJ_DATA *o_test;
	CHAR_DATA *m_test;
	bool found = FALSE;

	if( (!room && !ch && !obj)
	||  (spec_fun = spec_lookup(spec_name)) == NULL )
		return NULL;

	spec_obj		= new_spec_obj();
	spec_obj->spec_fun	= spec_fun;
	spec_obj->room		= room;
	spec_obj->ch		= ch;
	spec_obj->vch		= NULL;
	spec_obj->obj		= obj;
	spec_obj->timer		= -1;
	spec_obj->value[0]	= 0;
	spec_obj->value[1]	= 0;
	spec_obj->value[2]	= 0;
	spec_obj->value[3]	= 0;

	if( room )
	{
		spec_obj->next		= room->spec_obj;
		room->spec_obj		= spec_obj;

		/* test to see if room in list already */
		for( r_test = room_spec_list; r_test; r_test = r_test->next_spec )
		{
			if( r_test == room )
				found = TRUE;
		}
		if( !found )
		{
			room->next_spec		= room_spec_list;
			room_spec_list		= room;
		}
	}
	else if( obj )
	{
		spec_obj->next		= obj->spec_obj;
		obj->spec_obj		= spec_obj;

		/* obj in list already? */
		for( o_test = obj_spec_list; o_test; o_test = o_test->next_spec )
		{
			if( o_test == obj )
				found = TRUE;
		}
		if( !found )
		{
			obj->next_spec		= obj_spec_list;
			obj_spec_list		= obj;
		}
	}
	else if( ch )
	{
		spec_obj->next		= ch->spec_obj;
		ch->spec_obj		= spec_obj;

		/* mob in list already? */
		for( m_test = mob_spec_list; m_test; m_test = m_test->next_spec )
		{
			if( m_test == ch )
				found = TRUE;
		}
		if( !found )
		{
			ch->next_spec		= mob_spec_list;
			mob_spec_list		= ch;
		}
	}

	return spec_obj;
}

/* this function shouldn't be called by other procedues, mainly
 * used for special functions that need to "move" specs
 */
SPEC_OBJ_DATA *rem_spec( SPEC_OBJ_DATA *spec, int type )
{
	SPEC_OBJ_DATA *iterator = NULL;
	SPEC_OBJ_DATA *beginning = spec;
	bool remove = FALSE;

	if( (type == UPDATE_ROOM && !spec->room)
	||  (type == UPDATE_OBJ  && !spec->obj)
	||  (type == UPDATE_MOB  && !spec->ch) )
	{
		bug("Rem_spec: Incorrect type %d!", type );
		exit( 1 );
	}

	switch( type )
	{
	case UPDATE_ROOM: 
		if( (iterator = spec->room->spec_obj) == spec )
		{
			spec->room->spec_obj = spec->next;
			remove = TRUE;
		}
		break;

	case UPDATE_OBJ:  
		if( (iterator = spec->obj->spec_obj) == spec )
		{
			spec->obj->spec_obj = spec->next;
			remove = TRUE;
		}
		break;

	case UPDATE_MOB:
		if( (iterator = spec->ch->spec_obj) == spec )
		{
			spec->ch->spec_obj = spec->next;
			remove = TRUE;
		}
		break;
	}

	if( remove )
	{
		beginning	= spec->next;
		spec->next	= spec_obj_free;
		spec_obj_free	= spec;
		return beginning;
	}

	beginning	= iterator;
	remove		= FALSE;

	for( ; iterator; iterator = iterator->next )
	{
		if( iterator->next == spec )
		{
			remove		= TRUE;
			iterator->next	= spec->next;
			break;
		}
	}

	if( remove )
	{
		spec->next	= spec_obj_free;
		spec_obj_free	= spec;
	}

	return beginning;
}



/* ok this is kind of tricky.  some updates will
 * return soon as they are FALSE, but keep going
 * on if they are true.. and some will return
 * right off as soon as they are activated, etc.
 */
/* what = what type of structure is calling this function, UPDATE_ROOM, etc.
 * type = the update.. UPDATE_ENTERED, etc.
 * data1, data2 = the datas to pass on to the special functions
 * order is: room updated first, then objects on the mob, then the mob(s), then objects in room
 */
/* definitions:
 * ROOMS: will return if exited/entered and FALSE from spec fun
 *
 */

int update_specs( ROOM_INDEX_DATA *room, void *data1, void *data2, void *data3, int what, int type )
{
	SPEC_OBJ_DATA *spec;
	CHAR_DATA *ch = NULL;
	CHAR_DATA *ch_next;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int flag = FALSE;

	if( !room )
	{
		bug("Update_specs: Called with null room!  Type = %d", type );
		return TRUE;
	}

	if( what == UPDATE_ALL || what == UPDATE_ROOM )
	{
		for( spec = room->spec_obj; spec; spec = spec->next )
		{
			flag = (*spec->spec_fun)( spec, data1, data2, data3, type );
	
			if( what == UPDATE_ROOM || what == UPDATE_ALL )
			{
				switch(type)
				{
				case UPDATE_EXITED: /* move_char calls these, if FALSE, dont move char */
				case UPDATE_ENTERED:
					if( flag == FALSE )
						return FALSE;
					break;
				default:
					break;
				}
			}
		}
	}

	if( what != UPDATE_ROOM )
	{
		for( ch = room->people; ch; ch = ch_next )
		{
			ch_next = ch->next_in_room;
	
			if( what == UPDATE_ALL || what == UPDATE_OBJ )
			{
				for( obj = ch->carrying; obj; obj = obj_next )
				{
					obj_next = obj->next_content;

					for( spec = obj->spec_obj; spec; spec = spec->next )
						(*spec->spec_fun)( spec, data1, data2, data3, type );
				}
			}

			if( what == UPDATE_ALL || what == UPDATE_MOB )
			{
				for( spec = ch->spec_obj; spec; spec = spec->next )
					(*spec->spec_fun)( spec, data1, data2, data3, type );
			}
		}
	}

	if( what == UPDATE_ALL || what == UPDATE_OBJ )
	{
		for( obj = room->contents; obj; obj = obj_next )
		{
			obj_next = obj->next_content;

			for( spec = obj->spec_obj; spec; spec = spec->next )
				(*spec->spec_fun)( spec, data1, data2, data3, type );
		}
	}
	return TRUE;
}


/* most rooms will use these */
extern const char * dir_name[];
extern const sh_int rev_dir[];



/*************************************************************************/
/*                                ROOMS                                  */
/*************************************************************************/

/* walls, for spells */
/* if fire wall, person gets burned twice, but only messaged once */
int spec_wall( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = NULL;
	int direction = 0;

	if( !spec->room )
		return TRUE;

	switch( update )
	{
	case UPDATE_LOOK:
		ch = (CHAR_DATA *)data1;
		if( spec->value[1] == FALSE )
		{
			act("&+yThere is a giant earthen wall up on the $tern wall.&N\n\r",
				ch, dir_name[spec->value[0]], NULL, TO_CHAR );
		}
		else
		{
			act("&+RThere is a giant flaming wall up on the $tern wall.&N\n\r", 
				ch, dir_name[spec->value[0]], NULL, TO_CHAR );
		}
		return TRUE;

	case UPDATE_ENTERED:
		direction = (int)data2;
		direction = rev_dir[direction];
	case UPDATE_EXITED:
		ch = (CHAR_DATA *)data1;

		if( update != UPDATE_ENTERED ) 
			direction = (int)data2;

		switch( direction )
		{
		case DIR_NORTH:	if( spec->value[0] != DIR_NORTH ) return TRUE; break;
		case DIR_SOUTH:	if( spec->value[0] != DIR_SOUTH ) return TRUE; break;	
		case DIR_EAST:	if( spec->value[0] != DIR_EAST )  return TRUE; break;
		case DIR_WEST:	if( spec->value[0] != DIR_WEST )  return TRUE; break;
		default:					  return TRUE;
		}
		break;

	case UPDATE_END:
		{
			CHAR_DATA *ppl;

			if( (ppl = spec->room->people) == NULL )
				return TRUE;

			if( spec->value[1] == 0 )
			{
				act("&+YThe wall of earth crumbles to the ground.&N", ppl, NULL, NULL, TO_ROOM );
				act("&+YThe wall of earth crumbles to the ground.&N", ppl, NULL, NULL, TO_CHAR );
			}
			else
			{
				act("&+rThe wall of fire crackles down to it's last flame.&N", ppl, NULL, NULL, TO_ROOM );
				act("&+rThe wall of fire crackles down to it's last flame.&N", ppl, NULL, NULL, TO_CHAR );
			}
			return TRUE;
		}
		break;

	default:
		return TRUE;
	}

	/* dont want to double message them so only message victim if called_by is exiting */
	if( spec->value[1] == 1 ) /* A Fire Wall */
	{
		bool mount = FALSE;
		/* if mount gets burned and dies, dont want the other one to luck out of the burn too */

		/* burn a mounter? mount gets kill if mounter dies :P (and no creator) */
		if( ch->mounted_by )
		{
			mount = TRUE;

			act("&+RYou get torched as your mount rides into the flames!&N",
				ch->mounted_by, NULL, NULL, TO_CHAR );

			act("&+R$n gets torched as $s mount rides straight into the flames!&N",
				ch->mounted_by, NULL, NULL, TO_ROOM );

			damage( spec->ch ? spec->ch : ch, ch->mounted_by, dice(3,5), TYPE_FIRE );
		}

		/* do damage */
		act("&+RYou get torched as you walk through the $tern wall of fire!&N",
			ch, dir_name[direction], NULL, TO_CHAR );

		act("&+R$n gets torched as $e walks through the $tern wall of fire!&N",
			ch, dir_name[direction], NULL, TO_ROOM );

		damage( spec->ch ? spec->ch : ch, ch, dice(3,5), gsn_wall_of_fire );

		if(!ch || (mount && !ch->mounted_by) ) /* guy died! dont want move_char to deal with a null char */
			return FALSE;

		return TRUE;
	}

	act("You walk straight into a wall. *thump*", ch, NULL, NULL, TO_CHAR );
	act("$n thumps into a wall as $e walks $t.", ch, dir_name[direction], NULL, TO_ROOM );
	return FALSE;
}


int spec_vines( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;

	if( !ch || !spec->room )
		return TRUE;

	switch( update )
	{
	case UPDATE_LOOK:
		ch = (CHAR_DATA *)data1;

		send_to_char("&+ySome entangling vines are in the room here, thrashing around wildly.&N\n\r", ch );
		return TRUE;

	case UPDATE_FIGHTING:
		if( ch->in_room != spec->room )
			return TRUE;

		if( number_percent() > 65 + get_curr_dex(ch) )
		{
			act("&+yThe entangling vines reach out and grasp your arms and body!&N",
				ch, NULL, NULL, TO_CHAR );
			act("&+yThe entangling vines reach out and grasp $n's arms and body!&N",
				ch, NULL, NULL, TO_ROOM );
			WAIT_STATE(ch,12);
		}
		return TRUE;

	case UPDATE_END:
		if( !(ch = spec->room->people) )
			return TRUE;

		act("&+YThe entangling vines crack and break into many twigs.",
			ch, NULL, NULL, TO_ROOM );
		act("&+YThe entangling vines crack and break into many twigs.",
			ch, NULL, NULL, TO_CHAR );
		return TRUE;

	default:
		return TRUE;
	}

	return TRUE;
}


/* timer: damage someone, or move, or do nothing */
int spec_tornado( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *vch = NULL;
	CHAR_DATA *vch_next = NULL;

	if( !spec->room )
		return TRUE;

	switch( update )
	{
	case UPDATE_LOOK:
		vch = (CHAR_DATA *)data1;

		send_to_char("&+WA giant swirling tornado is here blasting everything out of its way!&N\n\r", vch);
		return TRUE;

	case UPDATE_TIMER:
		/* move first so it doesnt act like a damage monger */
		if( number_percent() > 85 )
		{
			SPEC_OBJ_DATA *newspec = NULL;
			ROOM_INDEX_DATA *room = spec->room;
			int i, x = 0;

			do
			{
				i = number_door( );
				x++; /*what if in noexit room?*/
			} while( room->exit[i] == NULL && x < 5 );

			if( room->exit[i] == NULL
			||  room->exit[i]->to_room == NULL )
				return TRUE;

			if( room->people )
			{
				act("A tornado blows out of the room $tward!",
					room->people, dir_name[i], NULL, TO_ROOM );
				act("A tornado blows out of the room $tward!",
					room->people, dir_name[i], NULL, TO_CHAR );
			}

			newspec		= add_spec( "spec_tornado", room->exit[i]->to_room, spec->ch, NULL );
			newspec->timer	= spec->timer;
			room->spec_obj	= rem_spec( spec, UPDATE_ROOM );

			if( newspec->room->people )
			{
				i = rev_dir[i];
				act("A tornado blows in the room from the $t!",
					newspec->room->people, dir_name[i], NULL, TO_ROOM );
				act("A tornado blows in the room from the $t!",
					newspec->room->people, dir_name[i], NULL, TO_CHAR );
			}
			return FALSE;
		}
		/* check to see if it can whack someone! */
		/* what damage should they do, hmm.. how about 5-25, 5d5 */
		if( number_percent() > 30 )
		{
			for( vch = spec->room->people; vch; vch = vch_next )
			{
				vch_next = vch->next_in_room;

				if( !IN_WAR(vch)
				||  IS_IMMORTAL(vch)
				||  vch == spec->ch
				||  saves_spell(vch) ) /* hey, its a spell! */
					continue;
	
				act("&+WA tornado thrashes you to bits!&N",
					vch, NULL, NULL, TO_CHAR );
				act("&+WA tornado thrashes $n to bits!&N",
					vch, NULL, NULL, TO_ROOM );
	
				damage( spec->ch ? spec->ch : vch, vch, dice(5,5), gsn_tornado );
			}
		}
		break;

	case UPDATE_END:
		if( !(vch = spec->room->people) )
			return TRUE;

		act("&+WThe tornado begins to rotate more slowly until it finally dies out.&N",
			vch, NULL, NULL, TO_ROOM );
		act("&+WThe tornado begins to rotate more slowly until it finally dies out.&N",
			vch, NULL, NULL, TO_CHAR );
		return TRUE;

	default:
		return TRUE;
	}
	return TRUE;
}



int spec_alarm( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;
	CHAR_DATA *vch = NULL;
	char buf[120];

	if( !ch || !spec->room || ch->in_room == spec->room )
		return TRUE;

	switch( update )
	{
	case UPDATE_ENTERED:
		vch = (CHAR_DATA *)data1;

		if( IS_NPC(vch) || vch == ch )
			return TRUE;

		sprintf(buf,"Your vision blurs and you see %s arrive at %s.\n\r",
			can_see(ch,vch) ? vch->name : "someone",
			spec->room->name );

		send_to_char( buf, ch );
		send_to_char( "You feel you are being watched...\n\r", vch );
		return TRUE;

	default:
		break;
	}

	return TRUE;
}



/*************************************************************************/
/*                              OBJECTS                                  */
/*************************************************************************/



int spec_artifact( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch;
	OBJ_DATA *obj = spec->obj;

	if( !obj )
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		{
			int i = 0;
			const static struct artifact_type
			{
				char *name;
				char *short_descr;
				int die1;
				int die2;
				int type;
				int flags;
			} artifact_table [5] =
			{
				{
					"ball strange",
					"A perfectly round shiny ball",
					5, 5, 6, ITEM_GLOW|ITEM_HUM
				},
				{
					"staff bladed wicked",
					"A wicked 8-bladed staff",
					6, 5, 8, ITEM_HUM
				},
				{
					"sword diamond",
					"A diamond-bladed sword",
					6, 6, 1, ITEM_GLOW
				},
				{
					"dagger pronged",
					"A three-pronged emerald dagger",
					5, 5, 11, ITEM_GLOW
				},
				{
					"chain ball",
					"A five-foot length ball and chain",
					7, 4, 7, 0
				}
			};

			if( (obj = spec->obj) == NULL )
				return FALSE;

			if( (ch = obj->carried_by) == NULL
			||  IS_NPC(ch))
				return FALSE;

			if( !IN_WAR(ch) )
			{
				obj_from_char(obj);
				obj_to_room(obj,ch->in_room);
				act("$p leaps from your hands!", ch, obj, NULL, TO_CHAR );
				act("$p leaps from $n's hands!", ch, obj, NULL, TO_ROOM );
				return TRUE;
			}

			if( number_percent() <= 3 )
			{
				act("$p changes form!",
					ch, obj, NULL, TO_ROOM );
				act("$p changes form!",
					ch, obj, NULL, TO_CHAR );
				free_string(obj->name);
				free_string(obj->short_descr);
	
				i			= number_range(0,4);
				obj->name		= str_dup( artifact_table[i].name );
				obj->short_descr	= str_dup( artifact_table[i].short_descr );
				obj->value[1]		= artifact_table[i].die1;
				obj->value[2]		= artifact_table[i].die2;
				obj->value[3]		= artifact_table[i].type;
				obj->extra_flags	= artifact_table[i].flags;
				return TRUE;
			}

			if( ch->fighting )
			{
				switch( dice(1,10) )
				{
				case 1:
					act("&+W$p flies from your hands and smites $N!&N",
						ch, obj, ch->fighting, TO_CHAR );
					act("&+W$p flies from $n's hands and smites $N!&N",
						ch, obj, ch->fighting, TO_NOTVICT );
					act("&+W$p flies from $n's hands and smites you!&N",
						ch, obj, ch->fighting, TO_VICT );
					damage(ch,ch->fighting,dice(10,5),TYPE_UNDEFINED );
					break;
				case 2:
					act("&+B$p knocks $N prone!&N",
						ch, obj, ch->fighting, TO_ROOM );
					act("&+B$p knocks you prone!&N",
						ch->fighting, obj, NULL, TO_CHAR );
					WAIT_STATE(ch->fighting, 12);
				default:
					break;
				}
				if( number_percent() > 75 )
				{
					act("&=rR$p grows in power.&N",
						ch, obj, NULL, TO_CHAR );
					act("&=rR$p grows in power.&N",
						ch, obj, NULL, TO_ROOM );
					obj->value[1]++;
				}
				return TRUE;
			}

			switch( dice(1,25) )
			{
			case 1:
			case 2:
				if( ch->hit < ch->max_hit
				&&  obj->value[1] > 5 )
				{
					act("&=cC$p is drained as it heals you.&N",
						ch, obj, NULL, TO_CHAR );
					act("&=cC$p is drained as it heals $n.&N",
						ch, obj, NULL, TO_ROOM );
					ch->hit = ch->max_hit;
					obj->value[1]--;
					return TRUE;
				}
				break;
			case 3:
			case 4:
				if( ch->mana < ch->max_mana
				&&  obj->value[1] > 5 )
				{
					act("&=bB$p is drained as it energizes you.&N",
						ch, obj, NULL, TO_CHAR );
					act("&=bB$p is drained as it energizes $n.&N",
						ch, obj, NULL, TO_ROOM );
					ch->mana = ch->max_mana;
					obj->value[1]--;
					return TRUE;
				}
				break;
			case 5:
				act("$p says 'Come on, $n, lets get to killing!'",
					ch, obj, NULL, TO_ROOM );
				act("$p says 'Come on, lets get to killing!'",
					ch, obj, NULL, TO_CHAR );
				break;
			case 6:
				act("$p says 'What's taking $n so long?'",
					ch, obj, NULL, TO_ROOM );
				act("$p says 'What's taking you so long?'",
					ch, obj, NULL, TO_CHAR );
				break;
			case 7:
				act("$p says '$n, I'm bored, let's do something!'",
					ch, obj, NULL, TO_ROOM );
				act("$p says 'I'm bored, let's do something!'",
					ch, obj, NULL, TO_CHAR );
				break;
			default:
				break;
			}
		}
	default:
		return TRUE;
	}

	return TRUE;
}
			
			


/* value0 = what team flag belonds to -> should be either 0 or 1
 * value1 = start room
 * value2 = what room needs to end in
 */

int spec_flag( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	char buf[200];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch;
	OBJ_DATA *obj = spec->obj;

	if( !obj )
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:

		/* is flag where its supposed to be? */
		if( obj->in_room && obj->in_room->vnum == obj->value[1] )
			return TRUE;

		/* first check to see if someone got the flag back */
		if( (ch = obj->carried_by) != NULL
		&&   !IS_NPC(ch)
		&&   ch->pcdata->team == obj->value[0] 
		&&   ch->in_room->vnum != obj->value[1] )
		{
			act("$n dematerializes.", ch, NULL, NULL, TO_ROOM );
			act("You dematerialize.", ch, NULL, NULL, TO_CHAR );
			char_from_room(ch);
			char_to_room(ch, get_room_index(obj->value[1]) );
			act("$n rematerializes.",			ch, NULL, NULL, TO_ROOM );
			act("You rematerialize back at home base.",	ch, NULL, NULL, TO_CHAR );
			return TRUE;
		}

		/* not in the right room! randomly notify opposite team of action */
		if( number_percent() > 75 )
		{
			for( d = descriptor_list; d; d = d->next )
			{
				if( d->connected > CON_PLAYING
				|| (ch = d->character) == NULL
				||  !IN_WAR(ch)
				||  IS_IMMORTAL(ch)
				||  ch->pcdata->team != obj->value[0] )
					continue;
		
				/* player might give obj to a mob to throw them off */
				if( obj->carried_by )
				{
					sprintf(buf,"&+m%s has your flag at &+M[%s]&N&+m !!&N\n\r",
						IS_NPC(obj->carried_by) ? obj->carried_by->short_descr : obj->carried_by->name,
						obj->carried_by->in_room->name );
					send_to_char( buf, ch );
					continue;
				}
				else if( obj->in_room )
				{
					sprintf(buf,"&+mYour flag is at &+M[%s]&N&+m !!&N\n\r",
						obj->in_room->name );
					send_to_char(buf,ch);
				}
				else if( obj->in_obj )
				{
					sprintf(buf, "&+m%s is in &+M[%s]&N&+m !!&N\n\r",
						obj->short_descr,
						obj->in_obj->short_descr );
					send_to_char( buf, ch );
				}
				else
				{
					send_to_char("&=MMYour flag is lost!&N\n\r", ch );
				}
			}
		}

		/* check to see if opposite team has it at the goal spot */
		if( ((ch = obj->carried_by) != NULL
		&&  !IS_NPC(ch)
		&&   ch->in_room->vnum == obj->value[2])
		||  (obj->in_room && obj->in_room->vnum == obj->value[2]) )
		{
			CHAR_DATA *winner = NULL;
			ROOM_INDEX_DATA *room;
			OBJ_DATA *obj_next;
			int start_team_0 = 0;
			int start_team_1 = 0;
			int team = 0;
			int kills = 0;
			int exp = 0;
			int i = 0;

			sprintf(buf,"The %steam has successfully captured the flag!",
				team_name((obj->value[0] == 0 ? 1 : 0)) );
			do_warmessage( NULL, buf );

			for( d = descriptor_list; d; d = d->next )
			{
				if( d->connected > CON_PLAYING
				|| (ch = d->character) == NULL
				||  !IN_WAR(ch)
				||  IS_IMMORTAL(ch) )
					continue;

				if( ch->pcdata->team != obj->value[0] )
				{
					send_to_char("You won the capture-the-flag war!  You are awarded a win.\n\r", ch );
					ch->pcdata->wars_won[WAR_CAPTURE_FLAG]++;
				}
				else
				{
					send_to_char("Bummer, they caught your flag...\n\r", ch );
					ch->pcdata->wars_lost[WAR_CAPTURE_FLAG]++;
				}
				if( ch->pcdata->kills_this_war > kills )
				{
					winner = ch;
					kills  = ch->pcdata->kills_this_war;
				}
			}
			if( winner )
			{
				exp = winner->level/2;
				sprintf(buf,"%s gets %d experience for having the most kills!", winner->name, exp );
				do_warmessage(NULL,buf);
				sprintf(buf,"You get awarded %d experience for having the most kills.", exp );
				send_to_char(buf,winner);
				gain_exp(winner,exp);
			}
	
			for( obj = object_list; obj; obj = obj_next )
			{
				obj_next = obj->next;

				if( obj->spec_obj && obj->spec_obj->spec_fun == spec_flag )
					extract_obj(obj);
			}
			/* obj is null now */
	
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
	
			for( d = descriptor_list; d != NULL; d = d->next )
			{
				if( (ch = can_war(d)) == NULL )
					continue;
	
				char_from_room(ch);
	
				if( ++team % 2 == 0 )
				{
					ch->pcdata->team = 1;
					char_to_room(ch,get_room_index(start_team_1));
				}
				else
				{
					ch->pcdata->team = 0;
					char_to_room(ch,get_room_index(start_team_0));
				}
			
				sprintf(buf,"You are part of the %steam, find the %sflag!\n\r",
					ch->pcdata->team == 0 ? team_name(0) : team_name(1),
					ch->pcdata->team == 0 ? team_name(1) : team_name(0) );
				send_to_char(buf,ch);
			}
	
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
			return TRUE;
		}
		return TRUE;

	default:
		return TRUE;
	}
	return FALSE;
}


int spec_hot_skull( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch;
	OBJ_DATA *obj = spec->obj;
	char buf[MAX_INPUT_LENGTH];
	int i = 0;

	if( !obj )
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !obj->carried_by )
		{
			DESCRIPTOR_DATA *d;
			int num = find_number_of_players( );

			if( num <= 1 )
				return TRUE;

			num = number_range( 1, num );	

			for( d = descriptor_list; d; d = d->next )
			{
				if( d->connected > CON_PLAYING
				||  (ch = d->character) == NULL
				||  !IN_WAR(ch)
				||  IS_IMMORTAL(ch) )
					continue;
	
				if( ++i == num )
				{
					sprintf(buf,"%s hops over to %s!", obj->short_descr, ch->name );
					do_warmessage( NULL, buf );
					send_to_char("Oh Shit! Now you got the skull!\n\r", ch );
					obj_to_char(obj,ch);
					break;
				}
			}
			return TRUE;
		}
	
		if( --obj->value[0] == 0 )
		{
			ROOM_INDEX_DATA *orig;
			OBJ_DATA *bpart;
	
			ch		= obj->carried_by;
			orig		= ch->in_room;
			obj->value[0]	= number_range(10,30);
	
			send_to_char("You blink, and...\n\r", ch );
			send_to_char("\n\r", ch );
			send_to_char("                            .xm*f''??T?@hc.\n\r", ch );
			send_to_char("                          z@'` '~((!!!!!!!?*m.\n\r", ch );
			send_to_char("                        z$$$K   ~~(/!!!!!!!!!Mh\n\r", ch );
			send_to_char("                      .f` '#$k'`~~|!!!!!!!!!!!MMc\n\r", ch );
			send_to_char("                     :'     f*! ~:~(!!!!!!!!!!XHMk\n\r", ch );
			send_to_char("                     f      ' %n:~(!!!!!!!!!!!HMMM.\n\r", ch );
			send_to_char("                    d          X~!~(!!!!!!!X!X!SMMR\n\r", ch );
			send_to_char("                    M :   x::  :~~!>!!!!!!MNWXMMM@R\n\r", ch );
			send_to_char(" n                  E ' *  ueeeeiu(!!XUWWWWWXMRHMMM>                :.\n\r", ch );
			send_to_char(" E%                 E  8 .$$$$$$$$K!!$$$$$$$$&M$RMM>               :'5\n\r", ch );
			send_to_char("z  %                3  $ 4$$$$$$$$!~!*$$$$$$$$!$MM$               :' `\n\r", ch );
			send_to_char("K   ':              ?> # '#$$$$$#~!!!!TR$$$$$R?@MME              z   R\n\r", ch );
			send_to_char("?     %.             5     ^'''~~~:XW!!!!T?T!XSMMM~            :^    J\n\r", ch );
			send_to_char(" '.    ^s             ?.       ~~d$X$NX!!!!!!M!MM             f     :~\n\r", ch );
			send_to_char("  '+.    #L            *c:.    .~'?!??!!!!!XX@M@~           z'    .*\n\r", ch );
			send_to_char("    '+     %L           #c`'!+~~~!/!!!!!!@*TM8M           z'    .~\n\r", ch );
			send_to_char("      ':    '%.         'C*X  .!~!~!!!!!X!!!@RF         .#     +\n\r", ch );
			send_to_char("        ':    ^%.        9-MX!X!!X~H!!M!N!X$MM        .#`    +'\n\r", ch );
			send_to_char("          #:    'n       'L'!~M~)H!M!XX!$!XMXF      .+`   .z'\n\r", ch );
			send_to_char("            #:    ':      R *H$@@$H$*@$@$@$%M~     z`    +'\n\r", ch );
			send_to_char("              %:   `*L    'k' M!~M~X!!$!@H!tF    z'    z'\n\r", ch );
			send_to_char("                *:   ^*L   'k ~~~!~!!!!!M!X*   z*   .+'\n\r", ch );
			send_to_char("                  's   ^*L  '%:.~~~:!!!!XH'  z#   .*'\n\r", ch );
			send_to_char("                    #s   ^%L  ^'#4@UU@##'  z#   .*'\n\r", ch );
			send_to_char("                      #s   ^%L           z#   .r'\n\r", ch );
			send_to_char("                        #s   ^%.       u#   .r'\n\r", ch );
			send_to_char("                          #i   '%.   u#   .@'\n\r", ch );
			send_to_char("                            #s   ^%u#   .@'\n\r", ch );
			send_to_char("                              #s x#   .*'\n\r", ch );
			send_to_char("                               x#`  .@%.\n\r", ch );
			send_to_char("                             x#`  .d'  '%.\n\r", ch );
			send_to_char("                           xf~  .r' #s   '%.\n\r", ch );
			send_to_char("                     u   x*`  .r'     #s   '%.  x.\n\r", ch );
			send_to_char("                     %Mu*`  x*'         #m.  '%zX'\n\r", ch );
			send_to_char("                     :R(h x*              'h..*dN.\n\r", ch );
			send_to_char("                   u@NM5e#>                 7?dMRMh.\n\r", ch );
			send_to_char("                 z$@M@$#'#'                 *''*@MM$hL\n\r", ch );
			send_to_char("               u@@MM8*                          '*$M@Mh.\n\r", ch );
			send_to_char("             z$RRM8F'                             'N8@M$bL\n\r", ch );
			send_to_char("            5`RM$#                                  'R88f)R\n\r", ch );
			send_to_char("            'h.$'                                     #$x*\n\r", ch );
			send_to_char("\n\r\n\r", ch );
	
			sprintf(buf,"%s explodes in %s's hands!", obj->short_descr, ch->name );
			do_warmessage( NULL, buf );
			sprintf(buf,"%s's body parts are thrown all over the realm.", ch->name );
			do_warmessage( NULL, buf );
			
			if( !IS_NPC(ch) )
			{
				ch->pcdata->hot_skulls++;
				ch->pcdata->in_war = 0;
				ch->pcdata->deaths++;
			}
	
			for( i = 0; i < 5; i++ )
			{
				/* who will know? */
				bpart = create_object( get_obj_index(OBJ_VNUM_MUSHROOM) );
	
				switch(i)
				{
				case 0:
					bpart->name		= str_dup("leg");
					sprintf(buf,"%s's leg", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->short_descr	= str_dup(buf);
					sprintf(buf,"%s's leg is on the ground here, rotting.", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->description	= str_dup(buf);
					break;
				case 1:
					bpart->name		= str_dup("arm");
					sprintf(buf,"%s's arm", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->short_descr	= str_dup(buf);
					sprintf(buf,"%s's arm is on the ground here, rotting.", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->description	= str_dup(buf);
					break;
				case 2:
					bpart->name		= str_dup("head");
					sprintf(buf,"%s's head", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->short_descr	= str_dup(buf);
					sprintf(buf,"%s's head is staring at you.", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->description	= str_dup(buf);
					break;
				case 3:
					bpart->name		= str_dup("torso");
					sprintf(buf,"%s's torso", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->short_descr	= str_dup(buf);
					sprintf(buf,"%s's torso is lying flat on the ground, no limbs attached.", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->description	= str_dup(buf);
					break;
				case 4:
					bpart->name		= str_dup("pelvis");
					sprintf(buf,"%s's pelvis", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->short_descr	= str_dup(buf);
					sprintf(buf,"%s's pelvis is on the ground here.", IS_NPC(ch) ? ch->short_descr : ch->name );
					bpart->description	= str_dup(buf);
					break;
				default: break;
				}
	
				obj_to_room( bpart, get_room_index(number_range(30409, 30433)) );
				char_from_room(ch);
				char_to_room(ch,bpart->in_room);
				act("$p lands in the room with a squelchy thud.", ch, bpart, NULL, TO_ROOM );
			}
			char_from_room(ch);
			char_to_room(ch,orig);
			obj_from_char(obj);
			obj_to_room( obj, get_room_index(number_range(30409, 30433)) );
			return TRUE;
		}
		return TRUE;

	default:
		return TRUE;
	}
	return TRUE;
}
	
	
/*************************************************************************/
/*                              MOBILES                                  */
/*************************************************************************/


int spec_borg( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;
	CHAR_DATA *vch;
	char buf[MAX_INPUT_LENGTH];

	if( !ch )
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !ch->hunting )
		{
			for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
			{
				if( IS_NPC(vch)
				|| IS_IMMORTAL(vch)
				|| !IN_WAR(vch)
				|| IS_BORG(vch) )
					continue;
	
				ch->hunting = vch;
				act("$n pauses for a moment, then turns slowly to you.",
					ch, NULL, vch, TO_VICT );
				act("$n pauses for a moment, then turns slowly to $N.",
					ch, NULL, vch, TO_NOTVICT );
				sprintf(buf,"%s will now join the &+Wcollective&N.",
					vch->name );
				do_chat(ch,buf);
				multi_hit(ch, vch, TYPE_UNDEFINED);
				break;
			}
			return TRUE;
		}

		if( !IS_AFFECTED(ch,AFF_PASS_DOOR) )
		{
			act("&+c$n opens up a metal plate on $s arm and presses a button on it.&n", 
				ch, NULL, NULL, TO_ROOM );
			act("&+c$n begins to waver in front of your eyes.&n", 
				ch, NULL, NULL, TO_ROOM );
			SET_BIT(ch->affected_by,AFF_PASS_DOOR);
			return TRUE;
		}
		if( !IS_AFFECTED(ch,AFF_DETECT_INVIS) )
		{
			act("&+c$n opens up a metal plate on the side of $s head and presses a button on it.&n",
				ch, NULL, NULL, TO_ROOM );
			act("&+c$n's eyes are surrounded by a red aura.&n",
				ch, NULL, NULL, TO_ROOM );
			SET_BIT(ch->affected_by,AFF_DETECT_INVIS);
			return TRUE;
		}
		if( !IS_AFFECTED(ch,AFF_DETECT_HIDDEN) )
		{
			act("&+c$n opens up a metal plate on the side of $s head and presses a button on it.&n",
				ch, NULL, NULL, TO_ROOM);
			act("&+c$n's eyes are surrounded by a black aura.&n",
				ch, NULL, NULL, TO_ROOM );
			SET_BIT(ch->affected_by,AFF_DETECT_HIDDEN);
			return TRUE;
		}
		if( !IS_AFFECTED(ch,AFF_FLYING) )
		{
			act("&+c$n reaches behind $s back and adjusts a control button.&n",
				ch, NULL, NULL, TO_ROOM );
			act("&+c$n begins to hover off the ground slightly.&n",
				ch, NULL, NULL, TO_ROOM );
			SET_BIT(ch->affected_by,AFF_FLYING);
			return TRUE;
		}
		break;

	case UPDATE_FIGHTING:
		switch( ch->position )
		{
		case POS_RESTING:
		case POS_SLEEPING:
			do_stand(ch,"");
			return TRUE;
	
		case POS_FIGHTING:
			if( !ch->fighting )
			{
				ch->position = POS_STANDING;
				return TRUE;
			}
	
			switch( number_range(0, 35) )
			{
			case 0:
			case 1:
			case 2:
				do_bash(ch,ch->fighting->name);
				return TRUE;
			case 3:
			case 4:
			case 5:
				do_kick(ch,ch->fighting->name);
				return TRUE;
			case 6:
				do_chat(ch,"Resistance is futile.");
				return TRUE;
			case 7:
				do_chat(ch,"You shall be assimilated into the collective.");
				return TRUE;
			case 8:
				do_chat(ch,"All hail Locutus of Borg.");
				return TRUE;
			default:
				return TRUE;
			}
		default:
			return TRUE;
		}
		break;

	default:
		break;
	}
	return TRUE;
}


int spec_elemental_fire( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !ch->fighting )
		{
			switch(dice(1,20))
			{
			case 1: do_say(ch,"Give me something to burn!");		break;
			case 2: do_say(ch,"Is it hot in here, or is it just me?");	break;
			case 3: do_say(ch,"I think I'm having a hot flash...");		break;
			default:
				break;
			}
		}
		break;

	case UPDATE_FIGHTING:
		switch(dice(1,30))
		{
		case 1:
			act("$n extends $s arm out and singes $N!", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n extends $s arm out and singes you!", ch, NULL, ch->fighting, TO_VICT );
			damage( ch, ch->fighting, 5, TYPE_UNDEFINED );
			break;
		case 2:
			act("$n throws a minor fireball at $N!", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n throws a minor fireball at you!", ch, NULL, ch->fighting, TO_VICT );
			damage( ch, ch->fighting, 10, TYPE_UNDEFINED );
			break;
		case 3:
			act("$n envelops $N.", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n envelops you in flames!", ch, NULL, ch->fighting, TO_VICT );
			damage( ch, ch->fighting, 15, TYPE_UNDEFINED );
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	return TRUE;
}

int spec_elemental_water( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !ch->fighting )
		{
			switch(dice(1,20))
			{
			case 1: do_say(ch,"You thirsty?");				break;
			case 2: do_say(ch,"I'm sweating...I think.");			break;
			case 3: do_say(ch,"I need a drink..Oh wait, nevermind..");	break;
			default:
				break;
			}
		}
		break;

	case UPDATE_FIGHTING:
		switch(dice(1,30))
		{
		case 1:
			act("$n pelts $N with a high powered stream of water.", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n pelts you with a high powered stream of water!", ch, NULL, ch->fighting, TO_VICT );
			damage( ch, ch->fighting, 5, TYPE_UNDEFINED );
			break;
		case 2:
		case 3:
			if( ch->master )
			{
				act("$n touches $N and $E is somewhat healed.", ch, NULL, ch->master, TO_NOTVICT );
				act("$n touches you and you are somewhat healed.", ch, NULL, ch->master, TO_VICT );
				heal_char(ch->master, 10);
			}
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	return TRUE;
}

int spec_elemental_earth( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !ch->fighting )
		{
			switch(dice(1,20))
			{
			case 1: do_say(ch,"Man this soil is heavy.");				break;
			case 2: do_say(ch,"I wanna beat up something, come on, hurry up!");	break;
			case 3: do_say(ch,"My legs are tired, can you fly me or something?");	break;
			default:
				break;
			}
		}
		break;

	case UPDATE_FIGHTING:
		switch(dice(1,30))
		{
		case 1:
			act("$n's punch slams right into $N's chest!", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n's punch slams right into your chest!", ch, NULL, ch->fighting, TO_VICT );
			damage( ch, ch->fighting, 5, TYPE_UNDEFINED );
			break;
		case 2:
			act("$n trips $N.", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n trips you.", ch, NULL, ch->fighting, TO_VICT );
			ch->fighting->position = POS_RESTING;
			break;
		case 3:
			act("$n knocks $N prone!", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n knocks you prone!", ch, NULL, ch->fighting, TO_VICT );
			WAIT_STATE(ch->fighting, 12);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	return TRUE;
}

int spec_elemental_air( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !ch->fighting )
		{
			switch(dice(1,20))
			{
			case 1: do_say(ch,"He went to the east!.. No wait, hmm.. I don't know, I'm such an airhead sometimes");	break;
			case 2: do_say(ch,"That damn guy ran right through me!");	break;
			case 3: do_emote(ch,"whistles a happy tune");			break;
			default:
				break;
			}
		}
		break;

	case UPDATE_FIGHTING:
		switch(dice(1,30))
		{
		case 1:
			act("$n forms a terrible wind and knocks $N to the ground.", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n forms a terrible wind and knocks you to the ground!", ch, NULL, ch->fighting, TO_VICT );
			damage( ch, ch->fighting, 10, TYPE_UNDEFINED );
			ch->fighting->position = POS_RESTING;
			break;
		case 2:
			{
			CHAR_DATA *vch = ch->fighting; 
			act("$n tries to blow $N out of the room!", ch, NULL, ch->fighting, TO_NOTVICT );
			act("$n tries to blow you out of the room!", ch, NULL, ch->fighting, TO_VICT );
			stop_fighting(ch,TRUE);
			move_char(vch,number_door());
			}
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	return TRUE;
}

int spec_elemental_greater( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !ch->fighting )
		{
			switch(dice(1,20))
			{
			case 1: do_say(ch,"Hurry up you flap!  I must get to killing!");	break;
			case 2: do_say(ch,"DIE!  DIE!  DIE!");					break;
			case 3: do_emote(ch,"points to you and says 'You're next'");		break;
			default:
				break;
			}
		}
		break;

	case UPDATE_FIGHTING:
		switch(dice(1,30))
		{
		case 1:
		case 2:
			(*spell_conjure_elemental)(gsn_conjure_elemental, ch->level, ch, NULL );
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	return TRUE;
}

/* neede for class specs */
void set_class( CHAR_DATA *ch, char *argument )
{
	extern int CLASSES;
	int i;

	for( i = 0; i < CLASSES; i++ )
	{
		if( class_table[i].name == NULL || class_table[i].name[0] == '\0' )
			break;

		if( !str_cmp(class_table[i].name, argument) )
		{
			ch->class = i;
			return;
		}
	}
}



int spec_assassin( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;
	CHAR_DATA *vch;
	OBJ_DATA *obj;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !IS_AFFECTED(ch,AFF_SNEAK) )
		{
			set_class(ch,"assassin");
			SET_BIT(ch->affected_by, AFF_SNEAK);
			return TRUE;
		}

		if( (obj = get_eq_char(ch,WEAR_WIELD)) == NULL )
		{
			act("$n pulls a dagger out of $s jacket.", ch, NULL, NULL, TO_ROOM );
			obj = create_object( get_obj_index( 30326 ) );
			obj_to_char( obj, ch );
			do_wear( ch, obj->name );
			return TRUE;
		}

		if( !ch->hunting )
		{
			for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
			{
				if( IS_NPC(vch)
				|| IS_IMMORTAL(vch)
				|| !IN_WAR(vch) )
					continue;
	
				ch->hunting = vch;
				act("You notice $n for a second, then $e slips into the shadows...",
					ch, NULL, vch, TO_VICT );
				act("$n looks squarely at $N, grins, and moves into the shadows...",
					ch, NULL, vch, TO_NOTVICT );
				do_backstab(ch, vch->name);
				break;
			}
			return TRUE;
		}
		break;

	case UPDATE_FIGHTING:
		if( ch->wait <= 0 )
			do_flee(ch,"");
		break;

	case UPDATE_ENTERED:
		if( !ch->fighting && ch->in_room == ch->hunting->in_room )
		{
			do_backstab( ch, ch->hunting->name );
			return TRUE;
		}
		break;

	default:
		break;
	}
	return TRUE;
}


int spec_warrior(  SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;
	CHAR_DATA *vch;
	OBJ_DATA *obj;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !IS_AFFECTED(ch,AFF_PROTECT) )
		{
			SET_BIT(ch->affected_by, AFF_PROTECT);
			set_class(ch,"warrior");
			return TRUE;
		}

		if( (obj = get_eq_char(ch,WEAR_SHIELD)) == NULL )
		{
			act("$n whips out a shield from $s backpack.", ch, NULL, NULL, TO_ROOM );
			obj = create_object( get_obj_index( 30327 ) );
			obj_to_char( obj, ch );
			do_wear(ch,obj->name);
			return TRUE;
		}
		if( (obj = get_eq_char(ch,WEAR_WIELD)) == NULL )
		{
			act("$n whips out a sword from $s backpack.", ch, NULL, NULL, TO_ROOM );
			obj = create_object( get_obj_index( 30328 ) );
			obj_to_char( obj, ch );
			do_wear(ch,obj->name);
			return TRUE;
		}

		if( !ch->hunting )
		{
			for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
			{
				if( IS_NPC(vch)
				|| IS_IMMORTAL(vch)
				|| !IN_WAR(vch) )
					continue;
	
				ch->hunting = vch;
				act("$n gives you a solid look and yells 'CHARGE!'",
					ch, NULL, vch, TO_VICT );
				act("$n gives $N a solid look and yells 'CHARGE!'",
					ch, NULL, vch, TO_NOTVICT );
				do_bash(ch, vch->name);
				break;
			}
			return TRUE;
		}
		break;

	case UPDATE_FIGHTING:
		if( ch->fighting )
		{
			switch( number_range(0,10) )
			{
			case 5:
				do_bash(ch,ch->fighting->name);
				return TRUE;
			case 8:
				do_kick(ch,ch->fighting->name);
				return TRUE;
			default:
				return TRUE;
			}
		}
		break;

	default:
		break;
	}
	return TRUE;
}


int spec_cleric(  SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = spec->ch;
	CHAR_DATA *vch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !IS_AFFECTED(ch, AFF_SANCTUARY) )
		{
			set_class(ch,"cleric");
			do_cast(ch,"sanctuary");
			return TRUE;
		}

		if( !ch->hunting )
		{
			for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
			{
				if( IS_NPC(vch)
				|| IS_IMMORTAL(vch)
				|| !IN_WAR(vch) )
					continue;
	
				ch->hunting = vch;
				act("$n makes a prayer, and a red aura surrounds you...",
					ch, NULL, vch, TO_VICT );
				act("$n makes a prayer, and $N is surrounded by a red aura...",
					ch, NULL, vch, TO_NOTVICT );
				do_kill(ch, vch->name);
				break;
			}
			return TRUE;
		}
		break;

	case UPDATE_FIGHTING:
		if( ch->fighting )
		{
			if( ch->hit < ch->max_hit/2 )
			{
				do_cast(ch,"heal");
				return TRUE;
			}

			switch( number_range(0,20) )
			{
			case 5:
				do_cast(ch,"flamestrike");
				return TRUE;
			case 10:
				do_cast(ch,"dispel evil");
				return TRUE;
			default:
				return TRUE;
			}
		}
		break;

	default:
		break;
	}
	return TRUE;
}


int spec_mage(  SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	char buf[50];
	CHAR_DATA *ch = spec->ch;
	CHAR_DATA *vch;

	if(!ch)
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( !IS_AFFECTED(ch,AFF_FIRESHIELD) )
		{
			set_class(ch,"mage");
			do_cast(ch,"fireshield");
			return TRUE;
		}
	
		if( !ch->hunting )
		{
	
			for( vch = ch->in_room->people; vch; vch = vch->next_in_room )
			{
				if( IS_NPC(vch)
				|| IS_IMMORTAL(vch)
				|| !IN_WAR(vch) )
					continue;
	
				sprintf(buf,"fireball %s", vch->name );
	
				ch->hunting = vch;
				act("$n winks at you.",
					ch, NULL, vch, TO_VICT );
				act("$n winks at $N.",
					ch, NULL, vch, TO_NOTVICT );
				do_cast(ch,buf);
				break;
			}
			return TRUE;
		}
		break;

	case UPDATE_FIGHTING:
		if( ch->fighting )
		{
			switch( number_range(0,20) )
			{
			case 5:
			case 15:
				do_cast(ch,"fireball");
				return TRUE;
			case 10:
				do_cast(ch,"lightning bolt");
				return TRUE;
			default:
				return TRUE;
			}
		}
		break;

	default:
		break;
	}
	return TRUE;
}



/* this spec is like a delayed spell blast, very powerful area affect delayed spell */
/* spec->vch = caster :: spec->ch = victim */
int spec_sunray( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *vch = NULL;
	CHAR_DATA *vch_next = NULL;
	int dam = 0;

	switch( update )
	{
	case UPDATE_LOOK:
		vch = (CHAR_DATA *)data1;

		send_to_char("A &+Wbright&N light is heading this way.\n\r", vch );
		return TRUE;

	case UPDATE_END:
		if( !spec->ch )
			return TRUE;

		for( vch = spec->ch->in_room->people; vch; vch = vch_next )
		{
			vch_next = vch->next_in_room;

			if( IS_IMMORTAL(vch)
			||  !IN_WAR(vch)
			||  vch == spec->vch
			||  is_same_group(vch,spec->vch) )
				continue;

			act("The powerful &+Wsunray&N arrives and you are surrounded by a white light as it blasts you to bits!&N",
				vch, NULL, NULL, TO_CHAR );
			act("The powerful &+Wsunray&N arrives and $n is surrounded by a white light as it blasts $m to bits!&N",
				vch, NULL, NULL, TO_ROOM );

			dam = dice(40,2) * (strstr(vch->name,"undead") != NULL ? 2 : 1);

			damage( spec->vch ? spec->vch : vch, vch, dam, gsn_sunray );
		}
		return TRUE;
	}
	return TRUE;
}


int spec_reflect( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *vch;
	int type;

	if( !spec->ch )
		return TRUE;

	switch( update )
	{
	case UPDATE_DAMAGE:
		type = skill_table[(int)data3].dam_type;

		if( (int)data3 < 0
		||  (int)data3 >= MAX_SKILL
		||  type == TYPE_MYSTICAL
		||  type == TYPE_BLUNT
		||  type == TYPE_PIERCE
		||  type == TYPE_SLASH
		|| (vch = (CHAR_DATA *)data1) == NULL
		||  !spec->ch->fighting
		||  vch == spec->ch
		||  vch->in_room != spec->ch->in_room
		||  number_percent() > 50 )
			break;

		type = (int)data3;

		act("&+CThe $t is reflected off of $n's magical shield and hits you!&N",
			spec->ch, skill_table[type].name, vch, TO_VICT );
		act("&+CThe $t is reflected off of your magical shield and hits $N!&N",
			spec->ch, skill_table[type].name, vch, TO_CHAR );
		act("&+CThe $t is reflected off of $n's magical shield and hits $N!&N",
			spec->ch, skill_table[type].name, vch, TO_NOTVICT );

		spec->ch->hit += (int)data2;
		damage( spec->ch, vch, (int)data2, (int)data3 );
		return TRUE;
	
	case UPDATE_END:
		send_to_char("Your magical sphere collapses.\n\r", spec->ch );
		act("$n's magical sphere collapses.", spec->ch, NULL, NULL, TO_ROOM );
		break;

	default:
		break;
	}

	return TRUE;
}
		

int spec_death( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch = NULL;
	char buf[100];

	if( !spec->ch )
		return TRUE;

	if( !spec->vch )
	{
		send_to_char("Your spirit is saved.\n\r", spec->ch );
		spec->timer = 1;
		spec->value[0]++;
		return TRUE;
	}

	switch( update )
	{
	case UPDATE_EXITED:
		ch = (CHAR_DATA *)data1;

		if( ch == spec->vch )
		{
			send_to_char("Your spirit seems to shiver in pain.\n\r", spec->ch );
		}
		break;

	case UPDATE_ENTERED:
		ch = (CHAR_DATA *)data1;

		if( ch == spec->vch )
		{
			send_to_char("Your spirit shivers in excitement.\n\r", spec->vch );
			act("Your spirit goes mad and you leap to attack $N!",
				spec->ch, NULL, spec->vch, TO_CHAR );
			multi_hit( spec->ch, spec->vch, TYPE_UNDEFINED );
		}
		break;

	case UPDATE_TIMER:
		if( IS_NPC(spec->vch)
		||  (!IS_NPC(spec->vch) && !IN_WAR(spec->vch)) )
		{
			send_to_char("Your spirit is saved.\n\r", spec->ch );
			spec->timer = 1;
			spec->value[0]++;
			break;
		}

		if( spec->vch->in_room
		&&  spec->vch->in_room != spec->ch->in_room )
		{
			sprintf(buf, "Doom overcomes you.\n\r%s is at %s.\n\r",
				spec->vch->name,
				spec->vch->in_room->name );
			send_to_char( buf, spec->ch );

			send_to_char( "Your spirit shivers.\n\r", spec->vch );
			WAIT_STATE( spec->vch, 6 );
		}
		break;

	case UPDATE_END:
		/* he can save his life if they are in same room */
		if( spec->vch->in_room == spec->ch->in_room )
			return FALSE;

		if( spec->value[0] == 0 )
		{
			act("$N's spirit overcomes you and your spirit is shattered.\n\r", 
				spec->ch, NULL, spec->vch, TO_CHAR );
			raw_kill( spec->vch, spec->ch );
			send_to_char("Your spirit is healed.\n\r", spec->vch );
		}
		break;
	}
	return TRUE;
}


int spec_animate_dead( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next = NULL;
	int num = 0;
	AFFECT_DATA af;

	if( !spec->ch || !spec->vch )
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		switch( spec->value[0] )
		{
		case 0:
			for( ch = spec->ch->in_room->people; ch; ch = ch->next_in_room )
			{
				if( ch->master == spec->vch
				&&  strstr(ch->name, "undead") != NULL )
					num += ch->level;
			}			
	
			if( num >= 18 )
			{
				for( ch = spec->ch->in_room->people; ch; ch = ch->next_in_room )
				{
					if( ch->master == spec->vch
					&&  strstr(ch->name, "undead") != NULL )
					{
						act("$n falls down into a pile of broken bones.",
							ch, NULL, NULL, TO_ROOM );
						ch->leader	= NULL;
						ch->position	= POS_RESTING;
					}
				}
				spec->value[0]++;
			}
			break;

		case 1:
			act("The bones begin to glow with awesome power.\n\rYou can hear a strange humming sound come from the pile of bones.",
				spec->ch, NULL, NULL, TO_ROOM );
			spec->value[0]++;
			break;

		case 2:
			act("All of the bones begin to float up in the air and forming a giant skeleton.",
				spec->ch, NULL, NULL, TO_ROOM );
			act("The skeleton begins to glow with power, two red eyes glow within its skull.",
				spec->ch, NULL, NULL, TO_ROOM );
			act("Suddenly it begins to morph forming a new shape....a dracolich!",
				spec->ch, NULL, NULL, TO_ROOM );
			act("The dracolich &+WRRRRROOOOOAAAAAAARRRRRSSSSS&N with power!",
				spec->ch, NULL, NULL, TO_ROOM );


			ch			= create_mobile( get_mob_index( MOB_VNUM_DRACOLICH ) );
			ch->hit = ch->max_hit	= 210;
			ch->master		= spec->vch;
			ch->leader		= spec->vch;
			ch->level		= 25;
			SET_BIT( ch->affected_by, AFF_BLACKMANTLE );
			SET_BIT( ch->act, ACT_SERVANT );
			SET_BIT( ch->act, ACT_SENTINEL );
			af.type			= skill_lookup("animate dead");
			af.duration		= get_curr_int(spec->vch);
			af.modifier		= 0;
			af.location		= 0;
			af.bitvector		= AFF_SUMMONED;
			affect_to_char( ch, &af );

			char_to_room( ch, spec->ch->in_room );

			for( ch = spec->ch->in_room->people; ch; ch = ch_next )
			{
				ch_next = ch->next_in_room;

				if( ch->master == spec->vch
				&&  strstr(ch->name, "undead") != NULL
				&&  strstr(ch->name, "dracolich") == NULL )
					extract_char( ch, TRUE );
			}
			spec->value[0]++;

		default:
			break;
		}
		break;

	case UPDATE_FIGHTING:
	case UPDATE_DAMAGE:
	case UPDATE_GIVE:
		spec->value[0] = 0;
		break;

	default:
		break;
	}

	return TRUE;
}


int spec_quivering_palm( SPEC_OBJ_DATA *spec, void *data1, void *data2, void *data3, int update )
{
	if( !spec->ch || !spec->vch )
		return TRUE;

	switch( update )
	{
	case UPDATE_TIMER:
		if( number_percent() > 50 )
		{
			act("$n quivers spasmodically.", spec->ch, NULL, NULL, TO_ROOM );
			send_to_char("You quiver spasmodically.\n\r", spec->ch );
			damage( spec->vch, spec->ch, dice(5,5), gsn_quivering_palm );
		}
		break;

	case UPDATE_END:
		send_to_char("Your body seems to relax a little.\n\r", spec->ch );
		break;
	}
	return TRUE;
}
















