/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// cvar.c -- dynamic variable tracking

#ifdef QUAKE1
#include "quakedef.h"
#else
#ifdef SERVERONLY 
#include "qwsvdef.h"
#else
#include "quakedef.h"
#endif
#endif // QUAKE1

cvar_t	*cvar_vars;
cvar_t	*developer;

void Cvar_ParseDeveloperFlags (void); /* FS: Special stuff for showing all the dev flags */

/*
============
Cvar_InfoValidate
============
*/
static qboolean Cvar_InfoValidate (char *s)
{
	if (strchr (s, '\\'))
		return false;
	if (strchr (s, '\"'))
		return false;
	if (strchr (s, ';'))
		return false;
	return true;
}

static int cmpr_cvars (const void *a, const void *b)
{
	cvar_t *aa = *(cvar_t **)a;
	cvar_t *bb = *(cvar_t **)b;

	return strcmp(aa->name, bb->name);
}

static int GetCVARCount (void)
{
	int i = 0;
	cvar_t* cvar;

	for (cvar = cvar_vars; cvar; cvar = cvar->next)
	{
		i++;
	}

	return i;
}

/*
============
Cvar_List_f

============
*/
void Cvar_List_f (void)
{
	cvar_t **sorted_cvars = NULL; /* FS: Sort by name. */
	cvar_t	*head = &cvar_vars[0];
	cvar_t	*var;
	const char *search_filter = NULL;
	int		i = 0, j = 0, q = 0, args = 0, search_filter_len = 0, cvar_count = 0;

	args = Cmd_Argc();

	if (args > 1) /* FS */
	{
		search_filter = Cmd_Argv(1);
		if (search_filter != NULL)
		{
			Com_SafePrintf("Listing matches for '%s'...\n", search_filter);

			if (args > 2)
			{
				search_filter_len = strlen(search_filter);
			}
		}
	}

	cvar_count = GetCVARCount();

	sorted_cvars = (cvar_t **)malloc(sizeof(cvar_t*)*cvar_count);
	if (!sorted_cvars)
	{
		Sys_Error ("Cvar_List_f: Failed to allocate memory.");
		return;
	}

	for (q = 0; q < cvar_count; q++)
	{
		sorted_cvars[q] = cvar_vars;
		cvar_vars = cvar_vars->next;
	}

	qsort(sorted_cvars, cvar_count, sizeof(cvar_t*), &cmpr_cvars);

	for (i = 0; i < cvar_count; i++)
	{
		var = sorted_cvars[i];
		if (!var)
		{
			break;
		}

		if (search_filter) /* FS */
		{
			if (!strstr(var->name, search_filter))
				continue;

			if ((args > 2) && (strncmp(var->name, search_filter, search_filter_len)))
				continue;

			j++;
		}

		if (var->flags & CVAR_ARCHIVE)
			Com_SafePrintf("*");
		else
			Com_SafePrintf(" ");
		if (var->flags & CVAR_USERINFO)
			Com_SafePrintf("U");
		else
			Com_SafePrintf(" ");
		if (var->flags & CVAR_SERVERINFO)
			Com_SafePrintf("S");
		else
			Com_SafePrintf(" ");
		if (var->flags & CVAR_NOSET)
			Com_SafePrintf("-");
		else if (var->flags & CVAR_LATCH)
			Com_SafePrintf("L");
		else
			Com_SafePrintf(" ");
		if (var->description)
			Com_SafePrintf("D");
		else
			Com_SafePrintf(" ");

		if ( (var->flags & CVAR_LATCH) && var->latched_string)
			Com_SafePrintf("\"%s\" is \"%s\", Default: \"%s\", Latched to: \"%s\"\n", var->name, var->string, var->defaultString, var->latched_string);
		else
			Com_SafePrintf(" %s \"%s\", Default: \"%s\"\n", var->name, var->string, var->defaultString);
	}

	Com_SafePrintf("Legend: * Archive. U Userinfo. S Serverinfo. - Write Protected. L Latched. D Containts a Help Description.\n"); /* FS: Added a legend */
	Com_SafePrintf("%d cvars\n", search_filter ? j : i);

	free(sorted_cvars);
	sorted_cvars = NULL;

	cvar_vars = (cvar_t *)head;
}

/*
============
Cvar_FindVar
============
*/
cvar_t *Cvar_FindVar (const char *var_name)
{
	cvar_t	*var;
	
	for (var=cvar_vars ; var ; var=var->next)
		if (!Q_strcmp ((char *)var_name, var->name))
			return var;

	return NULL;
}

/*
============
Cvar_VariableValue
============
*/
float	Cvar_VariableValue (char *var_name)
{
	cvar_t	*var;
	
	var = Cvar_FindVar (var_name);
	if (!var)
		return 0;
	return Q_atof (var->string);
}


/*
============
Cvar_VariableString
============
*/
char *Cvar_VariableString (char *var_name)
{
	cvar_t *var;
	
	var = Cvar_FindVar (var_name);
	if (!var)
		return "";
	return var->string;
}


/*
============
Cvar_CompleteVariable
============
*/
char *Cvar_CompleteVariable (char *partial)
{
	cvar_t		*cvar;
	int			len;
	
	len = Q_strlen(partial);
	
	if (!len)
		return NULL;

	// check exact match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!strcmp (partial,cvar->name))
			return cvar->name;

	// check partial match
	for (cvar=cvar_vars ; cvar ; cvar=cvar->next)
		if (!Q_strncmp (partial,cvar->name, len))
			return cvar->name;

	return NULL;
}


/*
============
Cvar_Get

If the variable already exists, the value will not be set
The flags will be or'ed in if the variable exists.
============
*/
cvar_t *Cvar_Get (char *var_name, char *var_value, int flags)
{
	cvar_t	*var;

	if (flags & CVAR_SERVERINFO)
	{
		if (!Cvar_InfoValidate (var_name))
		{
			Com_Printf("invalid info cvar name\n");
			return NULL;
		}
	}

	var = Cvar_FindVar (var_name);
	if (var)
	{
		var->flags |= flags;
		// Knightmare- change default value if this is called again
		if (var->defaultString)
		{
			free(var->defaultString);
		}
		if (!var_value)
			var->defaultString = strdup("0");
		else
			var->defaultString = strdup(var_value);
		var->defaultFlags |= flags; /* FS: Ditto */

		return var;
	}

	if (!var_value)
		return NULL;

	if (flags & CVAR_SERVERINFO)
	{
		if (!Cvar_InfoValidate (var_value))
		{
			Com_Printf("invalid info cvar value\n");
			return NULL;
		}
	}

	var = malloc (sizeof(cvar_t));
	if (var == NULL)
	{
		return NULL;
	}

	var->name = strdup (var_name);
	var->string = strdup (var_value);
	var->latched_string = NULL;
	var->modified = true;
	var->value = atof (var->string);
	var->intValue = atoi(var->string); /* FS: So we don't need to cast shit all the time */
	var->defaultString = strdup(var_value); /* FS: Find out what it was initially */
	var->defaultFlags = flags; /* FS: Default flags for resetcvar */
	var->description = NULL; /* FS: Init it first, d'oh */

	// link the variable in
	var->next = cvar_vars;
	cvar_vars = var;

	var->flags = flags;

	return var;
}

/*
============
Cvar_Set2
============
*/
cvar_t *Cvar_Set2 (char *var_name, char *value, qboolean force)
{
	cvar_t	*var;
	
	var = Cvar_FindVar (var_name);
	if (!var)
	{	// create it
		return Cvar_Get (var_name, value, 0);
	}

	if (var->flags & CVAR_SERVERINFO)
	{
		if (!Cvar_InfoValidate (value))
		{
			Com_Printf("invalid info cvar value\n");
			return var;
		}
	}

	if (!force)
	{
		if (var->flags & CVAR_NOSET)
		{
			Com_Printf ("%s is write protected.\n", var_name);
			return var;
		}

		if (var->flags & CVAR_LATCH)
		{
			if (var->latched_string)
			{
				if (strcmp(value, var->latched_string) == 0)
					return var;
				free (var->latched_string);
			}
			else
			{
				if (strcmp(value, var->string) == 0)
					return var;
			}

#ifdef QUAKE1
			if (sv.active)
#else
			if (cls.state == ca_active)
#endif
			{
				Com_Printf ("%s will be changed for next map.\n", var_name);
				var->latched_string = strdup(value);
			}
			else
			{
				var->string = strdup(value);
				var->value = atof (var->string);
				var->intValue = atoi(var->string); /* FS: So we don't need to cast shit all the time */
			}
			return var;
		}
	}
	else
	{
		if (var->latched_string)
		{
			free (var->latched_string);
			var->latched_string = NULL;
		}
	}

#ifdef QUAKEWORLD
#ifdef SERVERONLY
	if (var->flags & CVAR_SERVERINFO)
	{
		Info_SetValueForKey (svs.info, var_name, value, MAX_SERVERINFO_STRING);
		SV_SendServerInfoChange(var_name, value);
	}
#else
	if (var->flags & CVAR_USERINFO)
	{
		Info_SetValueForKey (cls.userinfo, var_name, value, MAX_INFO_STRING);
		if (cls.state >= ca_connected)
		{
			MSG_WriteByte (&cls.netchan.message, clc_stringcmd);
			SZ_Print (&cls.netchan.message, va("setinfo \"%s\" \"%s\"\n", var_name, value));
		}
	}
#endif // SERVERONLY
#endif // QUAKEWORLD

	if (!strcmp(value, var->string))
		return var;		// not changed

	var->modified = true; /* FS: Added */

	free (var->string);	// free the old value string

	var->string = strdup(value);
	var->value = atof (var->string);
	var->intValue = atoi(var->string); /* FS: So we don't need to cast shit all the time */

#ifdef QUAKE1
	if (var->flags & CVAR_SERVERINFO)
	{
		if (sv.active)
			SV_BroadcastPrintf ("\"%s\" changed to \"%s\"\n", var->name, var->string);
	}
#endif

	return var;
}

/*
============
Cvar_ForceSet
============
*/
cvar_t *Cvar_ForceSet (char *var_name, char *value)
{
	return Cvar_Set2 (var_name, value, true);
}

/*
============
Cvar_Set
============
*/
cvar_t *Cvar_Set (char *var_name, char *value)
{
	return Cvar_Set2 (var_name, value, false);
}

/*
============
Cvar_FullSet
============
*/
cvar_t *Cvar_FullSet (char *var_name, char *value, int flags)
{
	cvar_t	*var;

	var = Cvar_FindVar (var_name);
	if (!var)
	{	// create it
		return Cvar_Get (var_name, value, flags);
	}

	var->modified = true;

	free (var->string);	// free the old value string

	var->string = strdup(value);
	var->value = atof (var->string);
	var->intValue = atoi(var->string); /* FS: So we don't need to cast shit all the time */
	var->flags = flags;

	return var;
}

/*
============
Cvar_SetValue
============
*/
void Cvar_SetValue (char *var_name, float value)
{
	char	val[32];

	if (value == (int)value) /* FS: Weird zeros fix from QIP */
		Com_sprintf(val, sizeof(val), "%d", (int)value);
	else
		Com_sprintf(val, sizeof(val), "%f", value);

	Cvar_Set (var_name, val);
}

/*
============
Cvar_GetLatchedVars

Any variables with latched values will now be updated
============
*/
void Cvar_GetLatchedVars (void)
{
	cvar_t	*var;

	for (var = cvar_vars ; var ; var = var->next)
	{
		if (!var->latched_string)
			continue;
		free (var->string);
		var->string = var->latched_string;
		var->latched_string = NULL;
		var->value = atof(var->string);
		var->intValue = atoi(var->string); /* FS: So we don't need to cast shit all the time */
	}
}

/*
============
Cvar_Command

Handles variable inspection and changing from the console
============
*/
qboolean	Cvar_Command (void)
{
	cvar_t			*v;

// check variables
	v = Cvar_FindVar (Cmd_Argv(0));
	if (!v)
		return false;

	if (!Q_strcmp(v->name, "developer") && con_show_dev_flags->intValue) /* FS: Special case for showing enabled flags */
	{
		if(Q_strlen(Cmd_Argv(1)) > 0)
			Cvar_Set(developer->name, Cmd_Argv(1));
		Cvar_ParseDeveloperFlags();
		return true;
	}

// perform a variable print or set
	if (Cmd_Argc() == 1)
	{
		if ( (v->flags & CVAR_LATCH) && v->latched_string)
			Com_Printf ("\"%s\" is \"%s\", Default: \"%s\", Latched to: \"%s\"\n", v->name, v->string, v->defaultString, v->latched_string);
		else
			Com_Printf ("\"%s\" is \"%s\",  Default: \"%s\".\n", v->name, v->string, v->defaultString);

		/* FS: cvar descriptions */
		/* FS: Always show it for con_show_description so we know what it does */
		if (v->description) {
		    if (con_show_description->intValue || v == con_show_description)
			Com_Printf("Description: %s\n", v->description);
		}

		return true;
	}

	Cvar_Set (v->name, Cmd_Argv(1));
	return true;
}


/*
============
Cvar_Set_f

Allows setting and defining of arbitrary cvars from console
============
*/
void Cvar_Set_f (void)
{
	int		c;
	int		flags;

	c = Cmd_Argc();
	if (c != 3 && c != 4)
	{
		Com_Printf ("usage: set <variable> <value> [s]\n");
		return;
	}

	if (c == 4)
	{
		if (!strcmp(Cmd_Argv(3), "s"))
			flags = CVAR_SERVERINFO;
		else
		{
			Com_Printf ("flags can only be 's'\n");
			return;
		}
		Cvar_FullSet (Cmd_Argv(1), Cmd_Argv(2), flags);
	}
	else
		Cvar_Set (Cmd_Argv(1), Cmd_Argv(2));
}


/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void Cvar_WriteVariables (const char *path)
{
	cvar_t	*var;
	char	buffer[1024];
	FILE	*f;
	
	f = fopen (path, "a");
	for (var = cvar_vars ; var ; var = var->next)
	{
		if (var->flags & CVAR_ARCHIVE)
		{
			Com_sprintf (buffer, sizeof(buffer), "set %s \"%s\"\n", var->name, var->string);
			fprintf (f, "%s", buffer);
		}
	}
	fclose (f);
}

void Cvar_Init (void) /* FS: from fitzquake */
{
#ifdef QUAKE1
	developer = Cvar_Get("developer","0", 0);
	Cvar_Set_Description("developer", "Enable the use of developer messages. \nAvailable flags:\n  * All flags except verbose msgs - 1\n  * Standard msgs - 2\n  * Sound msgs - 4\n  * Network msgs - 8\n  * File IO msgs - 16\n  * Graphics renderer msgs - 32\n  * CD Player msgs - 64\n  * Memory management msgs - 128\n  * Server msgs - 256\n  * Progs msgs - 512\n  * Physics msgs - 2048\n  * Entity msgs - 16384\n  * Save/Restore msgs - 32768\n  * Extremely verbose msgs - 65536\n  * Extremely verbose gamespy msgs - 131072\n");
#else
	developer = Cvar_Get("developer","0", 0);
	Cvar_Set_Description("developer", "Enable the use of developer messages. \nAvailable flags:\n  * All flags except verbose msgs - 1\n  * Standard msgs - 2\n  * Sound msgs - 4\n  * Network msgs - 8\n  * File IO msgs - 16\n  * Graphics renderer msgs - 32\n  * CD Player msgs - 64\n  * Memory management msgs - 128\n  * Physics msgs - 2048\n  * Entity msgs - 16384\n  * Extremely verbose msgs - 65536\n  * Extremely verbose gamespy msgs - 131072\n");
#endif

	Cmd_AddCommand ("set", Cvar_Set_f);
	Cmd_AddCommand ("cvarlist", Cvar_List_f);
}

void Cvar_Set_Description (const char *var_name, const char *description) /* FS: Added */
{
	cvar_t	*var;
	var = Cvar_FindVar (var_name);
	if (!var)
	{
		Com_DPrintf(DEVELOPER_MSG_STANDARD, "Error: Can't set description for %s!\n", var_name);
		return;
	}

	if (var->description)
	{
		free(var->description);
	}
	var->description = strdup(description);
}

void Cvar_ParseDeveloperFlags (void) /* FS: Special stuff for showing all the dev flags */
{
	Com_Printf("\"%s\" is \"%s\", Default: \"%s\"\n", developer->name, developer->string, developer->defaultString);
	if(developer->intValue > 0)
	{
		unsigned long devFlags = 0;
		if(developer->intValue == 1)
			devFlags = 65534;
		else
			devFlags = (unsigned long)developer->value;
		Com_Printf("Toggled flags:\n");
		if(devFlags & DEVELOPER_MSG_STANDARD)
			Com_Printf(" * Standard messages - 2\n");
		if(devFlags & DEVELOPER_MSG_SOUND)
			Com_Printf(" * Sound messages - 4\n");
		if(devFlags & DEVELOPER_MSG_NET)
			Com_Printf(" * Network messages - 8\n");
		if(devFlags & DEVELOPER_MSG_IO)
			Com_Printf(" * File IO messages - 16\n");
		if(devFlags & DEVELOPER_MSG_VIDEO)
			Com_Printf(" * Graphics Renderer messages - 32\n");
		if(devFlags & DEVELOPER_MSG_CD)
			Com_Printf(" * CD Player messages - 64\n");
		if(devFlags & DEVELOPER_MSG_MEM)
			Com_Printf(" * Memory messages - 128\n");
#ifdef QUAKE1
		if(devFlags & DEVELOPER_MSG_SERVER)
			Com_Printf(" * Server messages - 256\n");
		if(devFlags & DEVELOPER_MSG_PROGS)
			Com_Printf(" * Prog messages - 512\n");
#endif
//		if(devFlags & DEVELOPER_MSG_WORLD)
//			Com_Printf(" * World.dll messages - 1024\n");
		if(devFlags & DEVELOPER_MSG_PHYSICS)
			Com_Printf(" * Physics messages - 2048\n");
//		if(devFlags & DEVELOPER_MSG_WEAPONS)
//			Com_Printf(" * Weapons.dll messages - 4096\n");
//		if(devFlags & DEVELOPER_MSG_GCE)
//			Com_Printf(" * GCE.dll messages - 8192\n");
		if(devFlags & DEVELOPER_MSG_ENTITY)
			Com_Printf(" * Entity messages - 16384\n");
#ifdef QUAKE1
		if(devFlags & DEVELOPER_MSG_SAVE)
			Com_Printf(" * Save/Restore messages - 32768\n");
#endif
		if(devFlags & DEVELOPER_MSG_VERBOSE)
			Com_Printf(" * Extremely Verbose messages - 65536\n");
		if(devFlags & DEVELOPER_MSG_GAMESPY)
			Com_Printf(" * Extremely Verbose GameSpy messages - 131072\n");
	}
	else
	{
		if (developer->description && con_show_description->intValue)
			Com_Printf("Description: %s\n", developer->description);
	}
}
