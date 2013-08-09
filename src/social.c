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
/* For dynamic social table */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"

#define SOCIAL_FILE "../area/socials.txt"

/**********Local Vars**************/
int	SOCIALS		= 0;
struct	social_type	*social_table;

/**********Local Funcs*************/
void	clear_social		args( ( int num ) );
void	extend_social_table	args( ( void ) );

int	get_social		args( ( char *str ) );

void read_socials( void )
{
	char buf[MAX_INPUT_LENGTH];
	FILE *fp;
	char *word;
	int num;
	int i;
	bool end = FALSE;

	if( (fp = fopen(SOCIAL_FILE, "r")) == NULL )
	{
		perror( SOCIAL_FILE );
		exit( 1 );
	}

	num		= fread_number(fp);

	while( num % 10 != 0 )
		num++;

	SOCIALS = num;

	social_table	= alloc_mem( num * sizeof(struct social_type) );

	for( i = -1; ; )
	{
		if( end )
			break;

		word = fread_word(fp);

		if( !str_cmp( word, "Social" ) )
		{
			++i;
			social_table[i].name		= fread_string(fp);
			social_table[i].char_no_arg	= fread_string(fp);
			social_table[i].others_no_arg	= fread_string(fp);
			social_table[i].char_found	= fread_string(fp);
			social_table[i].others_found	= fread_string(fp);
			social_table[i].vict_found	= fread_string(fp);
			social_table[i].char_auto	= fread_string(fp);
			social_table[i].others_auto	= fread_string(fp);

			if( !str_cmp(social_table[i].char_no_arg,"(null)") )
				social_table[i].char_no_arg = NULL;
			if( !str_cmp(social_table[i].others_no_arg,"(null)") )
				social_table[i].others_no_arg = NULL;
			if( !str_cmp(social_table[i].char_found,"(null)") )
				social_table[i].char_found = NULL;
			if( !str_cmp(social_table[i].others_found,"(null)") )
				social_table[i].others_found = NULL;
			if( !str_cmp(social_table[i].vict_found,"(null)") )
				social_table[i].vict_found = NULL;
			if( !str_cmp(social_table[i].char_auto,"(null)") )
				social_table[i].char_auto = NULL;
			if( !str_cmp(social_table[i].others_auto,"(null)") )
				social_table[i].others_auto = NULL;

			continue;
		}
		if( !str_cmp( word, "Endfile" ) )
		{
			end = TRUE;
			continue;
		}
		sprintf(buf,"Read_socials: bad word: '%s'", word );
		bug( buf, 0 );
		exit( 1 );
	}
	fclose(fp);
	log_string("Socials read successfully.");
}


void write_socials( void )
{
	FILE *fp;
	int i;

	if( (fp = fopen(SOCIAL_FILE, "w")) == NULL )
	{
		perror( SOCIAL_FILE );
		return;
	}

	fprintf( fp, "%d\n",	SOCIALS );

	for( i = 0; social_table[i].name != NULL && i < SOCIALS; i++ )
	{
		if( !str_cmp(social_table[i].name, "newsocial") )
			continue;

		fprintf( fp, "Social	%s~\n",	social_table[i].name		);
		fprintf( fp, "%s~\n",		social_table[i].char_no_arg	);
		fprintf( fp, "%s~\n",		social_table[i].others_no_arg	);
		fprintf( fp, "%s~\n",		social_table[i].char_found	);
		fprintf( fp, "%s~\n",		social_table[i].others_found	);
		fprintf( fp, "%s~\n",		social_table[i].vict_found	);
		fprintf( fp, "%s~\n",		social_table[i].char_auto	);
		fprintf( fp, "%s~\n\n\n",	social_table[i].others_auto	);
	}
	fprintf( fp, "Endfile\n" );
	fclose(fp);
}




int add_social( void )
{
	int i;

	if( social_table[SOCIALS-1].name != NULL
	&&  social_table[SOCIALS-1].name[0] != '\0' )
	{
		extend_social_table( );
	}

	for( i = 0; i < SOCIALS; i++ )
	{
		if( social_table[i].name == NULL 
		||  social_table[i].name[0] == '\0' )
		{
			clear_social(i);
			break;
		}
	}
	return i;
}

void clear_social( int num )
{
	if( social_table[num].name				!= NULL		)
		free_string(social_table[num].name				);
	if( social_table[num].char_no_arg			!= NULL		)
		free_string(social_table[num].char_no_arg			);
	if( social_table[num].others_no_arg			!= NULL		)
		free_string(social_table[num].others_no_arg			);
	if( social_table[num].char_found			!= NULL		)
		free_string(social_table[num].char_found			);
	if( social_table[num].others_found			!= NULL		)
		free_string(social_table[num].others_found			);
	if( social_table[num].vict_found			!= NULL		)
		free_string(social_table[num].vict_found			);
	if( social_table[num].char_auto				!= NULL		)
		free_string(social_table[num].char_auto				);
	if( social_table[num].others_auto			!= NULL		)
		free_string(social_table[num].others_auto			);

	social_table[num].name		= str_dup("newsocial");
	social_table[num].char_no_arg	= NULL;
	social_table[num].others_no_arg	= NULL;
	social_table[num].char_found	= NULL;
	social_table[num].others_found	= NULL;
	social_table[num].vict_found	= NULL;
	social_table[num].char_auto	= NULL;
	social_table[num].others_auto	= NULL;
}

void extend_social_table( void )
{
	struct social_type *new_table;
	int i;

	new_table	= alloc_mem( (SOCIALS+10) * sizeof(struct social_type) );

	for( i = 0; i < SOCIALS; i++ )
	{
		new_table[i].name		= social_table[i].name;
		new_table[i].char_no_arg	= social_table[i].char_no_arg;
		new_table[i].others_no_arg	= social_table[i].others_no_arg;
		new_table[i].char_found		= social_table[i].char_found;
		new_table[i].others_found	= social_table[i].others_found;
		new_table[i].vict_found		= social_table[i].vict_found;
		new_table[i].char_auto		= social_table[i].char_auto;
		new_table[i].others_auto	= social_table[i].others_auto;
	}

	free_mem(social_table, SOCIALS * sizeof(struct social_type));
	social_table	= new_table;
	SOCIALS		+= 10;
}


bool check_social( CHAR_DATA *ch, char *argument, char *command )
{
	ROOM_INDEX_DATA *prev;
	CHAR_DATA *victim;
	char buf[MAX_INPUT_LENGTH];
	int i;
	bool found = FALSE;

	for( i = 0; i < SOCIALS; i++ )
	{
		if( social_table[i].name == NULL )
			return FALSE;

		if( !str_prefix(argument,social_table[i].name) )
		{
			found = TRUE;
			break;
		}
	}
	if( !found )
		return FALSE;
		
	if( (victim = get_char_room(ch,command)) == NULL 
	&&  command[0] != '\0' )
	{
		if( (victim = get_char_world(ch,command)) != NULL )
		{
			prev = ch->in_room;
			char_from_room(ch);
			char_to_room(ch,victim->in_room);
			sprintf(buf,"From afar, %s", social_table[i].vict_found );
			act(buf,ch,NULL,victim,TO_VICT);
			sprintf(buf,"From afar, %s", social_table[i].char_found );
			act(buf,ch,NULL,victim,TO_CHAR);
			char_from_room(ch);
			char_to_room(ch,prev);
			return TRUE;
		}
		sprintf(buf,"%s at whom?\n\r", argument);
		send_to_char(buf,ch);
		return FALSE;
	}
	else if( !victim )
	{
		act( social_table[i].char_no_arg,	ch, NULL, NULL, TO_CHAR );
		act( social_table[i].others_no_arg,	ch, NULL, NULL, TO_ROOM );
	}
	else if( victim == ch )
	{
		act( social_table[i].char_auto,		ch, NULL, victim, TO_CHAR );
		act( social_table[i].others_auto,	ch, NULL, victim, TO_ROOM );
	}
	else
	{
		act( social_table[i].char_found,	ch, NULL, victim, TO_CHAR );
		act( social_table[i].others_found,	ch, NULL, victim, TO_NOTVICT );
		act( social_table[i].vict_found,	ch, NULL, victim, TO_VICT );
	}
	return TRUE;
}



int get_social( char *str )
{
	int i;

	for( i = 0; i < SOCIALS; i++ )
	{
		if( social_table[i].name == NULL )
			return -1;

		if( !str_cmp(social_table[i].name, str) )
			return i;
	}
	return -1;
}


void do_sset( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int i;

	if( IS_NPC(ch) ) return;

	smash_tilde(argument);

	argument = one_argument(argument,arg1);
	strcpy(arg2,argument);

	if( arg1[0] == '\0' )
	{
		send_to_char("Syntax: sset new\n\r",				ch );
		send_to_char("or:     sset save\n\r",				ch );
		send_to_char("or:     sset help\n\r",				ch );
		send_to_char("or:     sset delete <social>\n\r",		ch );
		send_to_char("or:     sset <social to begin modifying>\n\r",	ch );
		send_to_char("then:   sset <field> <string>\n\r",		ch );
		send_to_char("or:     sset <field> null\n\r",			ch );
		send_to_char("or:     sset show\n\r",				ch );
		send_to_char("\n\r",						ch );
		send_to_char("Field being one of:\n\r",				ch );
		send_to_char("  name\n\r",					ch );
		send_to_char("  char_no_arg others_no_arg\n\r",			ch );
		send_to_char("  char_found  others_found  vict_found\n\r",	ch );
		send_to_char("  char_auto others_auto\n\r",			ch );
		return;
	}
	if( arg2[0] == '\0' )
	{
		if( !str_cmp( arg1, "new" ) )
		{
			ch->pcdata->modifying = add_social( );
			send_to_char("Social added to list.\n\r", ch );
			send_to_char("You begin editing the new social.\n\r", ch );
			return;
		}
		if( !str_cmp( arg1, "save") )
		{
			write_socials( );
			send_to_char("Socials saved.\n\r", ch );
			return;
		}
		if( !str_cmp( arg1, "help") )
		{
			send_to_char("You must use certain triggers for socials to work properly.\n\r", ch );
			send_to_char("Make sure not to use a trigger when the victim/performer is\n\r",	ch );
			send_to_char("not present.  IE. dont use $M on the char_no_arg variable,\n\r",	ch );
			send_to_char("\n\r",								ch );
			send_to_char("Use capital triggers for victim.  IE. $m is for the\n\r",		ch );
			send_to_char("performer, $M is for the victim.\n\r",				ch );
			send_to_char("Also, you provide the punctuation, see sample below.\n\r",	ch );
			send_to_char("Here is a list of those triggers and a short definition.\n\r",	ch );
			send_to_char("\n\r",								ch );
			send_to_char("$n - The performer of the command. $n grins.\n\r",		ch );
			send_to_char("$m - him/her.  You grin at $m.\n\r",				ch );
			send_to_char("$s - his/her.  You grin at $s face.\n\r",				ch );
			send_to_char("$e - he/she.   You grin because $e asked you too.\n\r",		ch );
			send_to_char("\n\r",								ch );
			send_to_char("Here is a sample social.\n\r",					ch );
			send_to_char("\n\r",								ch );
			send_to_char("         name: grin\n\r",						ch );
			send_to_char("  char_no_arg: You grin evilly.\n\r",				ch );
			send_to_char("others_no_arg: $n grins evilly.\n\r",				ch );
			send_to_char("   char_found: You grin evilly at $N.\n\r",			ch );
			send_to_char(" others_found: $n grins evilly at $N.\n\r",			ch );
			send_to_char("   vict_found: $n grins evilly at you.\n\r",			ch );
			send_to_char("    char_auto: You grin evilly at yourself.\n\r",			ch );
			send_to_char("  others_auto: $n grins evilly at $mself.\n\r",			ch );
			send_to_char("\n\r",								ch );
			return;
		}

		if( !str_cmp(arg1, "show") )
		{
			if( ch->pcdata->modifying < 0 || ch->pcdata->modifying >= SOCIALS )
			{
				send_to_char("Your modifier value is out of range, begin editing the social again.\n\r", ch );
				return;
			}

			sprintf( buf,            "         name: %s\n\r", social_table[ch->pcdata->modifying].name		);
			sprintf( buf+strlen(buf),"  char_no_arg: %s\n\r", social_table[ch->pcdata->modifying].char_no_arg	);
			sprintf( buf+strlen(buf),"others_no_arg: %s\n\r", social_table[ch->pcdata->modifying].others_no_arg	);
			sprintf( buf+strlen(buf),"   char_found: %s\n\r", social_table[ch->pcdata->modifying].char_found	);
			sprintf( buf+strlen(buf)," others_found: %s\n\r", social_table[ch->pcdata->modifying].others_found	);
			sprintf( buf+strlen(buf),"   vict_found: %s\n\r", social_table[ch->pcdata->modifying].vict_found	);
			sprintf( buf+strlen(buf),"    char_auto: %s\n\r", social_table[ch->pcdata->modifying].char_auto		);
			sprintf( buf+strlen(buf),"  others_auto: %s\n\r", social_table[ch->pcdata->modifying].others_auto	);
		
			send_to_char(buf,ch);
			return;
		}

		if( (i = get_social(arg1)) < 0 )
		{
			do_sset(ch,"");
			return;
		}

		sprintf(buf,"You begin to modify the %s social.\n\r",
			social_table[i].name );
		send_to_char(buf,ch);
		ch->pcdata->modifying = i;
		return;
	}

	if( !str_cmp(arg1,"delete"))
	{
		if( (i = get_social(arg2)) < 0 )
		{
			send_to_char("No such social!\n\r", ch );
			return;
		}
		clear_social(i);
		send_to_char("Social cleared.\n\r", ch );
		return;
	}

	if( ch->pcdata->modifying < 0 || ch->pcdata->modifying >= SOCIALS )
	{
		send_to_char("Your modifier is out of range, re-edit the social.\n\r", ch );
		return;
	}

	if( !str_cmp(arg1, "name") )
	{
		sprintf(buf,"Social Edit: %s created a social named %s.",
			ch->name, arg2 );
		log_string(buf);

		if( social_table[ch->pcdata->modifying].name != NULL )
			free_string(social_table[ch->pcdata->modifying].name);

		social_table[ch->pcdata->modifying].name = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "char_no_arg") )
	{
		if( social_table[ch->pcdata->modifying].char_no_arg != NULL )
			free_string(social_table[ch->pcdata->modifying].char_no_arg );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].char_no_arg = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].char_no_arg = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "others_no_arg") )
	{
		if( social_table[ch->pcdata->modifying].others_no_arg != NULL )
			free_string(social_table[ch->pcdata->modifying].others_no_arg );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].others_no_arg = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].others_no_arg= str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "char_found") )
	{
		if( social_table[ch->pcdata->modifying].char_found != NULL )
			free_string(social_table[ch->pcdata->modifying].char_found );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].char_found = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].char_found = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "others_found") )
	{
		if( social_table[ch->pcdata->modifying].others_found != NULL )
			free_string(social_table[ch->pcdata->modifying].others_found );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].others_found = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].others_found = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "vict_found") )
	{
		if( social_table[ch->pcdata->modifying].vict_found != NULL )
			free_string(social_table[ch->pcdata->modifying].vict_found );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].vict_found = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].vict_found = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "char_auto") )
	{
		if( social_table[ch->pcdata->modifying].char_auto != NULL )
			free_string(social_table[ch->pcdata->modifying].char_auto );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].char_auto = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].char_auto = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	if( !str_cmp(arg1, "others_auto") )
	{
		if( social_table[ch->pcdata->modifying].others_auto != NULL )
			free_string(social_table[ch->pcdata->modifying].others_auto );

		if( !str_cmp(arg2,"null") )
		{
			social_table[ch->pcdata->modifying].others_auto = NULL;
			sprintf(buf,"%s: NULL (will do nothing)",arg1);
			return;
		}

		social_table[ch->pcdata->modifying].others_auto = str_dup(arg2);
		sprintf(buf,"%s: %s\n\r", arg1, arg2 );
		send_to_char(buf,ch);
		return;
	}

	do_sset(ch,"");
	return;
}









