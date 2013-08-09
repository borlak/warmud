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

/* File added by Borlak */
/* This file is for the dynamic class structure. */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

#define CLASS_FILE "../area/classes.txt"

struct	class_type	*class_table;
int	CLASSES;

void	clear_class		args( ( int num ) );
void	extend_class_table	args( ( void ) );

int	get_class		args( ( char *str ) );

void read_classes( void )
{
	OBJ_INDEX_DATA *obj;
	char buf[MAX_INPUT_LENGTH];
	FILE *fp;
	char *word;
	int num;
	int i;
	int sn = 0;
	bool end = FALSE;

	if( (fp = fopen(CLASS_FILE, "r")) == NULL )
	{
		perror( CLASS_FILE );
		exit( 1 );
	}

	num		= fread_number(fp);

	while( num % 10 != 0 ) /* Create the array in 10 size blocks */
		num++;

	CLASSES = num;

	class_table	= alloc_mem( num * sizeof(struct class_type) );
	
	for( i = -1; ; )
	{
		if( end )
			break;

		word = fread_word(fp);

		switch(word[0])
		{
		case 'b':
			if( !str_cmp(word,"base"))
			{class_table[i].base		= fread_number(fp);		break;}
			break;

		case 'c':
			if( !str_cmp(word,"class"))
			{i++;class_table[i].name	= fread_string(fp);		break;}

			if( !str_cmp(word,"con"))
			{class_table[i].stat_con	= fread_number(fp);		break;}
			break;

		case 'd':
			if( !str_cmp(word,"dex"))
			{class_table[i].stat_dex	= fread_number(fp);		break;}
			break;

		case 'e':
			if( !str_cmp(word,"enabled"))
			{class_table[i].enabled		= fread_number(fp);		break;}
			if( !str_cmp(word,"endfile"))
			{end = TRUE; break;}
			break;

		case 'f':
			if( !str_cmp(word,"flags"))
			{class_table[i].flags		= fread_number(fp);		break;}
			break;

		case 'h':
			if( !str_cmp(word,"hp"))
			{class_table[i].hp		= fread_number(fp);		break;}
			break;

		case 'i':
			if( !str_cmp(word,"int"))
			{class_table[i].stat_int	= fread_number(fp);		break;}
			break;

		case 'm':
			if( !str_cmp(word,"mana"))
			{class_table[i].mana		= fread_number(fp);		break;}
			break;

		case 'o':
			if( !str_cmp(word,"object"))
			{
				sn = fread_number( fp );

				if( (obj = get_obj_index(sn)) == NULL )
				{
					sprintf(buf,"Read_classes: Bad object vnum '%d' for class '%s'",
						sn, class_table[i].name );
					bug(buf,0);
					exit( 1 );
				}

				for( sn = 0; sn < 5; sn++ )
				{
					if( class_table[i].obj_list[sn] == NULL )
					{
						class_table[i].obj_list[sn] = obj;
						break;
					}
				}
				break;
			}
			break;

		case 's':
			if( !str_cmp(word,"short"))
			{class_table[i].short_name	= fread_string(fp);		break;}
			
			if( !str_cmp(word,"str"))
			{class_table[i].stat_str	= fread_number(fp);		break;}

			if( !str_cmp(word,"skills"))
			{
				for( ; ; )
				{
					if( end )
						break;

					word = fread_word(fp);

					switch(word[0])
					{
					case 'n':
						if(!str_cmp(word,"name"))
						{
							word = fread_word(fp);
							if( (sn = skill_lookup(word)) < 0 )
							{
								sprintf(buf,"Read_classes: Bad skill '%s' for class '%s'",
									word, class_table[i].name );
								bug(buf,0);
								exit( 1 );
							}
							class_table[i].skills[sn][SKILL_PRAC] = fread_number(fp);
							break;
						}
						break;
					case 'b':
						if(!str_cmp(word,"beats"))
						{class_table[i].skills[sn][SKILL_BEAT] = fread_number(fp); break;}
						break;
					case 'e':
						if( !str_cmp(word,"endskills"))
						{end = TRUE; break;}
						break;
					case 'm':
						if( !str_cmp(word,"mana"))
						{class_table[i].skills[sn][SKILL_MANA] = fread_number(fp); break;}
						break;
					default:
						sprintf(buf,"Read_classes: Bad word[%s] for skill %s for class %s.",
							word, skill_table[sn].name, class_table[i].name );
						bug(buf,0);
						exit( 1 );
					} /* switch */
				} /* for loop */
				end = FALSE;
				break;
			} /* if statement */	
			break;
		case 't':
			if( !str_cmp(word,"thacO"))
			{class_table[i].thac0		= fread_number(fp);		break;}
			break;

		case 'w':
			if( !str_cmp(word,"wis"))
			{class_table[i].stat_wis	= fread_number(fp);		break;}
			break;

		default:
			sprintf(buf,"Read_classes: Bad word[%s] for class %s.",
				word, class_table[i].name );
			bug(buf,0);
			exit( 1 );
		}
	}
	fclose(fp);
	log_string("Classes read successfully.");
}


void write_classes( void )
{
	FILE *fp;
	int sn;
	int i;

	if( (fp = fopen(CLASS_FILE, "w")) == NULL )
	{
		perror( CLASS_FILE );
		return;
	}

	fprintf( fp, "%d\n",	CLASSES );

	for( i = 0; class_table[i].name != NULL && i < CLASSES; i++ )
	{
		if( !str_cmp(class_table[i].name, "newclass") )
			continue;

		fprintf( fp, "class	%s~\n", class_table[i].name	);
		fprintf( fp, "short	%s~\n", class_table[i].short_name);
		fprintf( fp, "base	%d\n", class_table[i].base	);
		fprintf( fp, "enabled	%d\n", class_table[i].enabled	);
		fprintf( fp, "hp	%d\n", class_table[i].hp	);
		fprintf( fp, "mana	%d\n", class_table[i].mana	);
		fprintf( fp, "thacO	%d\n", class_table[i].thac0	);
		fprintf( fp, "str	%d\n", class_table[i].stat_str	);
		fprintf( fp, "int	%d\n", class_table[i].stat_int	);
		fprintf( fp, "wis	%d\n", class_table[i].stat_wis	);
		fprintf( fp, "dex	%d\n", class_table[i].stat_dex	);
		fprintf( fp, "con	%d\n", class_table[i].stat_con	);
		fprintf( fp, "flags	%d\n", class_table[i].flags	);

		for( sn = 0; sn < 5; sn++ )
			if( class_table[i].obj_list[sn] != NULL )
		fprintf( fp, "object	%d\n", class_table[i].obj_list[sn]->vnum );

		fprintf( fp, "skills\n" );
		for( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if( class_table[i].skills[sn][SKILL_PRAC] == 0 )
				continue;

		fprintf( fp, "	name    '%s' %d\n", skill_table[sn].name,
					class_table[i].skills[sn][SKILL_PRAC] );
		fprintf( fp, "	beats	%d\n", class_table[i].skills[sn][SKILL_BEAT] );
		fprintf( fp, "	mana	%d\n\n", class_table[i].skills[sn][SKILL_MANA] );
		}
		fprintf( fp, "endskills\n\n\n" );
	}

	fprintf( fp, "endfile\n" );
	fclose(fp);
}


int add_class( void )
{
	int i;

	if( class_table[CLASSES-1].name != NULL
	&&  class_table[CLASSES-1].name[0] != '\0' )
	{
		extend_class_table();
	}

	/* CLASSES is now CLASSES + 10 */

	for( i = 0; i < CLASSES; i++ )
	{
		if( class_table[i].name == NULL 
		||  class_table[i].name[0] == '\0' )
		{
			clear_class(i);
			break;
		}
	}
	return i;
}



void clear_class( int num )
{
	int i;

	if( class_table[num].name != NULL )
		free_string(class_table[num].name);
	if( class_table[num].short_name != NULL )
		free_string(class_table[num].short_name);

	class_table[num].name		= str_dup("newclass");
	class_table[num].short_name	= str_dup("new");
	class_table[num].base		= 0;
	class_table[num].hp		= 1;
	class_table[num].mana		= 1;
	class_table[num].thac0		= 20;
	class_table[num].stat_str	= 1;
	class_table[num].stat_int	= 1;
	class_table[num].stat_wis	= 1;
	class_table[num].stat_dex	= 1;
	class_table[num].stat_con	= 1;
	class_table[num].flags		= 0;

	for( i = 0; i < 5; i++ )
		class_table[num].obj_list[i] = NULL;
	
	for( i = 0; i < MAX_SKILL; i++ )
	{
		class_table[num].skills[i][SKILL_PRAC]	= 0;
		class_table[num].skills[i][SKILL_BEAT]	= 0;
		class_table[num].skills[i][SKILL_MANA]	= 0;
	}
}


void extend_class_table( void )
{
	struct class_type *new_table;
	int i;
	int sn;

	new_table	= alloc_mem( (CLASSES+10) * sizeof(struct class_type) );
	
	for( i = 0; i < CLASSES; i++ )
	{
		new_table[i].name	= class_table[i].name;
		new_table[i].short_name	= class_table[i].short_name;
		new_table[i].base	= class_table[i].base;
		new_table[i].hp		= class_table[i].hp;
		new_table[i].mana	= class_table[i].mana;
		new_table[i].thac0	= class_table[i].thac0;
		new_table[i].stat_str	= class_table[i].stat_str;
		new_table[i].stat_int	= class_table[i].stat_int;
		new_table[i].stat_wis	= class_table[i].stat_wis;
		new_table[i].stat_dex	= class_table[i].stat_dex;
		new_table[i].stat_con	= class_table[i].stat_con;
		new_table[i].flags	= class_table[i].flags;
		new_table[i].enabled	= class_table[i].enabled;

		for( sn = 0; sn < 5; sn++ )
			new_table[i].obj_list[sn] = class_table[i].obj_list[sn];

		for( sn = 0; sn < MAX_SKILL; sn++ )
		{
			new_table[i].skills[sn][SKILL_PRAC] = class_table[i].skills[sn][SKILL_PRAC];
			new_table[i].skills[sn][SKILL_BEAT] = class_table[i].skills[sn][SKILL_PRAC];
			new_table[i].skills[sn][SKILL_MANA] = class_table[i].skills[sn][SKILL_PRAC];
		}
	}

	free_mem(class_table, CLASSES * sizeof(struct class_type));
	class_table	= new_table;
	CLASSES		+= 10;
}


char *class_flag_name( int flags )
{
	static char buf[200];

	buf[0] = '\0';

	if( flags & CLS_NO_REGEN_HP	) strcat(buf,"[No Regen Hp] "		);
	if( flags & CLS_NO_REGEN_MANA	) strcat(buf,"[No Regen Mana] "		);
	if( flags & CLS_INFRAVISION	) strcat(buf,"[Infravision] "		);
	if( flags & CLS_SLOW		) strcat(buf,"[Slow] "			);
	if( flags & CLS_ULTRAVISION	) strcat(buf,"[Ultravision] "		);
	if( flags & CLS_NO_EQUIP	) strcat(buf,"[No Equip] "		);
	if( flags & CLS_NO_MAGIC	) strcat(buf,"[No Magic] "		);
	if( flags & CLS_DEADLY_BACKSTAB ) strcat(buf,"[Deadly Backstab] "	);
	if( flags & CLS_MARTIAL_ARTIST	) strcat(buf,"[Martial Artist] "	);

	return buf[0] == '\0' ? "None" : buf;
}


int get_class_flag_num( char *str )
{
	if( !str_cmp(str,"no_regen_hp"		)) return CLS_NO_REGEN_HP;
	if( !str_cmp(str,"no_regen_mana"	)) return CLS_NO_REGEN_MANA;
	if( !str_cmp(str,"infravision"		)) return CLS_INFRAVISION;
	if( !str_cmp(str,"slow"			)) return CLS_SLOW;
	if( !str_cmp(str,"ultravision"		)) return CLS_ULTRAVISION;
	if( !str_cmp(str,"no_equip"		)) return CLS_NO_EQUIP;
	if( !str_cmp(str,"no_magic"		)) return CLS_NO_MAGIC;
	if( !str_cmp(str,"deadly_backstab"	)) return CLS_DEADLY_BACKSTAB;
	if( !str_cmp(str,"martial_artist"	)) return CLS_MARTIAL_ARTIST;
	return -1;
}


int get_class( char *str )
{
	int i;
	for( i = 0; i < CLASSES; i++ )
	{
		if( class_table[i].name == NULL )
			return -1;
		if( !str_cmp(class_table[i].name,       str)
		||  !str_cmp(class_table[i].short_name, str) )
			return i;
	}
	return -1;
}


/*****************Character Commands*******************/


void do_cview( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH*2];
	char arg[MAX_INPUT_LENGTH];
	int col = 0;
	int i;
	int sn;
	int base;
	bool found = FALSE;

	one_argument( argument, arg );

	buf[0] = '\0';
	if( arg[0] == '\0' )
	{
		send_to_char("\n\r",ch);

		for( base = 0; base < 5; base++ )
		{
			col = 0;

			switch(base)
			{
			case CLASS_WAR: sprintf(buf+strlen(buf),"\n\rWarrior classes:\n\r"	); break;
			case CLASS_THI: sprintf(buf+strlen(buf),"\n\rThief classes:\n\r"	); break;
			case CLASS_CLE: sprintf(buf+strlen(buf),"\n\rPriest classes:\n\r"	); break;
			case CLASS_MAG: sprintf(buf+strlen(buf),"\n\rMagic-user classes:\n\r"	); break;
			case CLASS_SPE: sprintf(buf+strlen(buf),"\n\rSpecial classes:\n\r"	); break;
			}

			for( i = 0; i < CLASSES; i++ )
			{
				if( class_table[i].name == NULL
				|| (!str_cmp(class_table[i].name, "newclass") && !IS_IMMORTAL(ch))
				||  class_table[i].base != base )
					continue;
	
				sprintf(buf+strlen(buf),"%c%s%-15s ", '\t',
					class_table[i].enabled ? "" : "!",
					class_table[i].name );

				if( ++col % 3 == 0 )
					sprintf(buf+strlen(buf),"\n\r" );
			}
			if( col % 3 != 0 )
				sprintf(buf+strlen(buf),"\n\r" );
			
		}
		send_to_char(buf,ch);
		send_to_char("\n\r", ch );
		return;
	}

	for( i = 0; i < CLASSES; i++ )
	{
		if( class_table[i].name == NULL || class_table[i].name[0] == '\0' )
			break;

		if( !str_cmp(arg, class_table[i].name)
		||  !str_cmp(arg, class_table[i].short_name) )
		{
			found = TRUE;
			break;
		}
	}

	if( !found )
	{
		send_to_char("There is no such class.\n\r", ch );
		return;
	}

	sprintf(buf+strlen(buf),"\n\rClass: &+C%s&N(&+C%s&N).  %s\n\r\n\r",
		class_table[i].name,
		class_table[i].short_name,
		class_table[i].enabled ? "&+WEnabled&N" : "&+LDisabled&N" );

	sprintf(buf+strlen(buf),"&+CStr:&N &+W%-2d&N  &+CInt:&N &+W%-2d&N     &+CHp:&N &+W%-2d&N\n\r",
		class_table[i].stat_str,
		class_table[i].stat_int,
		class_table[i].hp	);
		
	sprintf(buf+strlen(buf),"&+CDex:&N &+W%-2d&N  &+CWis:&N &+W%-2d&N   &+CMana:&N &+W%-2d&N\n\r",
		class_table[i].stat_dex,
		class_table[i].stat_wis,
		class_table[i].mana	);

	sprintf(buf+strlen(buf),"&+CCon:&N &+W%-2d&N            &+CThac0:&N &+W%-2d&N\n\r\n\r",
		class_table[i].stat_con,
		class_table[i].thac0	);

	sprintf(buf+strlen(buf),"Flags: %s\n\r", 
		class_flag_name(class_table[i].flags) );

	for( sn = 0; sn < 5; sn++ )
	{
		if( class_table[i].obj_list[sn] == NULL )
			continue;

		sprintf(buf+strlen(buf),"&+YLoads:&N &+y%s&N.  %s %d.\n\r",
			class_table[i].obj_list[sn]->short_descr,
			IS_IMMORTAL(ch) ? "Vnum:" : "Num:",
			IS_IMMORTAL(ch) ? class_table[i].obj_list[sn]->vnum : 1 );
	}
	
	col = 0;

	sprintf(buf+strlen(buf),"&+W                   Name %%  Lg M                    Name %%  Lg M&N\n\r" );
	for( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if( class_table[i].skills[sn][SKILL_PRAC] <= 0 )
			continue;

		sprintf(buf+strlen(buf),"&+C%c%15s&N&+W %-2d %-2d %-3d&N", '\t',
			skill_table[sn].name,
			class_table[i].skills[sn][SKILL_PRAC],
			class_table[i].skills[sn][SKILL_BEAT],
			class_table[i].skills[sn][SKILL_MANA] );
		
		if( ++col % 2 == 0 )
			sprintf(buf+strlen(buf),"\n\r" );
	}

	send_to_char(buf,ch);
	send_to_char("\n\r",ch);
}


void do_changeclass( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int i;
	bool found = FALSE;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if( IS_NPC(ch) ) return;

	if( arg[0] == '\0' )
	{
		sprintf(buf,"You are currently a %s(%s).\n\r",
			class_table[ch->class].name,
			class_table[ch->class].short_name );
		send_to_char(buf,ch);
		return;
	}

	if( IN_WAR(ch) && !IS_IMMORTAL(ch) )
	{
		send_to_char("Yea right.\n\r", ch );
		return;
	}

	for( i = 0; i <= CLASSES; i++ )
	{
		if( class_table[i].name == NULL
		||  !str_cmp(class_table[i].name, "newclass") )
			continue;

		if( !str_cmp(class_table[i].name, arg)
		||  !str_cmp(class_table[i].short_name, arg) )
		{
			found = TRUE;
			break;
		}
	}

	if( !found )
	{
		send_to_char("What class?\n\r", ch );
		return;
	}
	if( class_table[i].enabled != TRUE )
	{
		send_to_char("That class is not enabled at the moment.\n\r", ch );
		return;
	}
	if( class_table[i].base == CLASS_SPE 
	&& arg2[0] != '\0'
	&& !str_cmp(arg2,"a1s2d3f4") && !IS_IMMORTAL(ch) )
	{
		send_to_char("You cannot change to special classes.\n\r", ch );
		return;
	}

	ch->hit			= 1;
	ch->mana		= 1;
	ch->class		= i;
	ch->max_hit		= class_table[i].hp;
	ch->max_mana		= class_table[i].mana;

	sprintf(buf,"You are now a %s.\n\r", class_table[i].name );
	send_to_char(buf,ch);

}

void do_cset( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	int value, prac, beat, mana;
	int i, sn;
	int mod;

	if( IS_NPC(ch) ) return;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	strcpy( arg4, argument );

	if( arg1[0] == '\0' )
	{
		send_to_char("Syntax: cset new\n\r",				ch );
		send_to_char("or:     cset delete <class>\n\r",			ch );
		send_to_char("or:     cset save\n\r",				ch );
		send_to_char("or:     cset <class to begin modifying>\n\r",	ch );
		send_to_char("then:   cset <field> <value>\n\r",		ch );
		send_to_char("or:     cset enabled\n\r",			ch );
		send_to_char("\n\r",						ch );
		send_to_char("Field being one of:\n\r",				ch );
		send_to_char("  hp mana str int wis dex con thac0\n\r",		ch );
		send_to_char("  name short flag base object\n\r",		ch );
		send_to_char("\n\r",						ch );
		send_to_char("To set or delete a skill:\n\r",			ch );
		send_to_char("  cset <skill> <percent> <beats> <mana>\n\r",	ch );
		send_to_char("  cset <skill> delete\n\r",			ch );
		return;
	}
	if( arg2[0] == '\0' )
	{
		if( !str_cmp( arg1, "new" ) )
		{
			ch->pcdata->modifying = add_class();
			send_to_char("Class added to list.\n\r", ch );
			send_to_char("You begin modifying the new class.\n\r", ch );
			return;
		}

		if( !str_cmp( arg1, "save") )
		{
			write_classes( );
			send_to_char("Classes saved!\n\r", ch );
			return;
		}

		if( !str_cmp( arg1, "enabled") )
		{
			if( ch->pcdata->modifying < 0 || ch->pcdata->modifying >= CLASSES )
			{
				send_to_char("Your modifer is out of range.\n\r", ch );
				return;
			}
			if( class_table[ch->pcdata->modifying].enabled )
			{
				class_table[ch->pcdata->modifying].enabled = FALSE;
				send_to_char("Class disabled.\n\r", ch );
				return;
			}

			class_table[ch->pcdata->modifying].enabled = TRUE;
			send_to_char("Class enabled.\n\r", ch );
			return;
		}				

		if( (i = get_class(arg1)) < 0 )
		{
			send_to_char("That's not a class.\n\r", ch );
			return;
		}

		sprintf(buf,"You begin to modify the %s class.\n\r",
			class_table[i].name );
		send_to_char(buf,ch);
		ch->pcdata->modifying = i;
		return;
	}
	if( arg3[0] == '\0' || arg4[0] == '\0' )
	{
		if( !str_cmp(arg1, "delete") )
		{
			if( (i = get_class(arg2)) < 0 )
			{
				send_to_char("No such class!\n\r", ch );
				return;
			}

			clear_class(i);
			send_to_char("Class cleared.\n\r", ch );
			return;
		}

		if( (sn = skill_lookup(arg1)) >= 0 && str_cmp(arg1,"con") && str_cmp(arg2,"delete") )
		{
			send_to_char("Skills require 3 arguments: prac beat mana.\n\r", ch );
			return;
		}
	}

	if( (mod = ch->pcdata->modifying) > CLASSES
	||   mod < 0 )
	{
		send_to_char("You are modifying a null class!  Modify a new one.\n\r", ch );
		return;
	}

	if( (sn = skill_lookup(arg1)) >= 0 && str_cmp(arg1,"con") )
	{
		if( !str_cmp(arg2,"delete") )
		{
			send_to_char("Skill removed.\n\r", ch );
			class_table[mod].skills[sn][SKILL_PRAC] = 0;
			return;
		}

		if( (prac = atoi(arg2)) < 1 )
		{
			send_to_char("Practice percent must be above 0.\n\r", ch );
			return;
		}
		if( (beat = atoi(arg3)) < 0 )
		{
			send_to_char("Beat(s) must be 0 or above.\n\r", ch );
			return;
		}
		if( (mana = atoi(arg4)) < 0 )
		{
			send_to_char("Mana must be 0 or above.\n\r", ch );
			return;
		}
		class_table[mod].skills[sn][SKILL_PRAC] = prac;
		class_table[mod].skills[sn][SKILL_BEAT] = beat;
		class_table[mod].skills[sn][SKILL_MANA] = mana;

		sprintf(buf,"%s's '%s' = Percent:%d.  Beat:%d.  Mana:%d.\n\r",
			class_table[mod].name,
			skill_table[sn].name,
			prac,
			beat,
			mana );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp( arg1, "object") )
	{
		OBJ_INDEX_DATA *obj;
		int num;
		int vnum;
		bool del = FALSE;

		if( !is_number(arg2) )
		{
			send_to_char("Syntax: cset object <vnum>\n\r", ch );
			send_to_char("    or: cset object <vnum> delete\n\r", ch );
			return;
		}

		vnum = atoi(arg2);

		if( !str_cmp(arg3,"delete") )
			del = TRUE;

		if( (obj = get_obj_index(vnum)) == NULL )
		{
			send_to_char("That vnum doesn't exist.\n\r", ch );
			return;
		}

		for( num = 0; num < 5; num++ )
		{
			if( del )
			{
				if( class_table[mod].obj_list[num] == NULL )
					continue;

				if( class_table[mod].obj_list[num]->vnum == vnum )
				{
					send_to_char("Ok.\n\r", ch );
					class_table[mod].obj_list[num] = NULL;
					break;
				}
			}
			if( class_table[mod].obj_list[num] == NULL )
			{
				send_to_char("Ok.\n\r", ch );
				class_table[mod].obj_list[num] = obj;
				break;
			}
		}

		send_to_char("If you did not get an 'Ok.' then the object was not found.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "base") )
	{
		if( !str_prefix(arg2,"warrior") )
		{
			class_table[mod].base = CLASS_WAR;
			send_to_char("Ok.\n\r", ch );
			return;
		}
		if( !str_prefix(arg2,"thief") )
		{
			class_table[mod].base = CLASS_THI;
			send_to_char("Ok.\n\r", ch );
			return;
		}
		if( !str_prefix(arg2,"cleric") )
		{
			class_table[mod].base = CLASS_CLE;
			send_to_char("Ok.\n\r", ch );
			return;
		}
		if( !str_prefix(arg2,"mage") )
		{
			class_table[mod].base = CLASS_MAG;
			send_to_char("Ok.\n\r", ch );
			return;
		}
		if( !str_prefix(arg2,"special") )
		{
			class_table[mod].base = CLASS_SPE;
			send_to_char("Ok.\n\r", ch );
			return;
		}
		send_to_char("Base classes are: warrior thief cleric mage special\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "flag") )
	{
		if( (value = get_class_flag_num(arg2)) < 0 )
		{
			send_to_char("Thats not a flag.\n\r",			ch );
			send_to_char("Choices are:\n\r",			ch );
			send_to_char("  slow  no_regen_hp  no_regen_mana\n\r",	ch );
			send_to_char("  ultravision  infravision  no_equip\n\r",ch );
			send_to_char("  no_magic deadly_backstab\n\r",		ch ); 
			return;
		}

		if( IS_SET(class_table[mod].flags, value) )
		{
			REMOVE_BIT(class_table[mod].flags,value);
			send_to_char("Flag removed.\n\r", ch );
			return;
		}
		SET_BIT(class_table[mod].flags,value);
		send_to_char("Flag added.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "name") )
	{
		free_string(class_table[mod].name);
		class_table[mod].name = str_dup(arg2);
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "short") )
	{
		free_string(class_table[mod].short_name);
		class_table[mod].short_name = str_dup(arg2);
		send_to_char("Ok.\n\r", ch );
		return;
	}

	value = atoi(arg2);

	if( !str_cmp( arg1, "hp" ) )
	{
		if( value < 1 )
		{
			send_to_char("Hp must be above 0.\n\r", ch );
			return;
		}
		class_table[mod].hp = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "mana" ) )
	{
		if( value < 0 )
		{
			send_to_char("Mana must be 0 or above.\n\r", ch );
			return;
		}
		class_table[mod].mana = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "str" ) )
	{
		if( value < 3 || value > 25 )
		{
			send_to_char("Range is from 3 to 25.\n\r", ch );
			return;
		}
		class_table[mod].stat_str = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "int" ) )
	{
		if( value < 3 || value > 25 )
		{
			send_to_char("Range is from 3 to 25.\n\r", ch );
			return;
		}
		class_table[mod].stat_int = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "wis" ) )
	{
		if( value < 3 || value > 25 )
		{
			send_to_char("Range is from 3 to 25.\n\r", ch );
			return;
		}
		class_table[mod].stat_wis = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "dex" ) )
	{
		if( value < 3 || value > 25 )
		{
			send_to_char("Range is from 3 to 25.\n\r", ch );
			return;
		}
		class_table[mod].stat_dex = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "con" ) )
	{
		if( value < 3 || value > 25 )
		{
			send_to_char("Range is from 3 to 25.\n\r", ch );
			return;
		}
		class_table[mod].stat_con = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	if( !str_cmp( arg1, "thac0" ) )
	{
		if( value < 0 || value > 25 )
		{
			send_to_char("Range is from 0 to 25.\n\r", ch );
			return;
		}
		class_table[mod].thac0 = value;
		send_to_char("Ok.\n\r", ch );
		return;
	}

	do_cset(ch,"");
	return;
}
