/* options.c
 *
 * WMAppl - Window Maker Application Launcher
 * Copyright (C) 1999-2004 Casey Harkins
 *
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "options.h"

/* opt_create_iconpath
 *  PARAMETERS:
 *      char * : colon separated path
 *  RETURN:
 *      IconPath *  : the icon path as a structure 
 */
IconPath *opt_create_iconpath(char *colonpath) {

	int             i, j, k;
	char           *directory;
	IconPath       *return_path;

	/* if path is empty, return NULL */
	if(strlen(colonpath) < 1)
		return (IconPath *) NULL;

	/* get path count */
	for(i = 0, j = 1; i < strlen(colonpath); i++) {
		if(colonpath[i] == PATHSEP)
			j++;
	}

	/* allocate new IconPath structure, set count and allocate path array */
	return_path = (IconPath *) malloc(sizeof(IconPath));
	return_path->count = j;
	return_path->path = (char **) malloc(sizeof(char *) * j);

	/* set dirallocsize to the length of the path + 2 (for trailing / and NULL) */
	return_path->dirallocsize = sizeof(char) * (strlen(colonpath) + 2);

	/* allocate a string for directory */
	directory = (char *) malloc(return_path->dirallocsize);

	/* iterate through path separating directories */
	for(i = 0, j = 0, k = 0; i < strlen(colonpath); i++, j++) {

		/* are we at a path separator */
		if(colonpath[i] == PATHSEP) {

			/* do we need to add the trailing directory separator */
			if(i > 0 && directory[j - 1] != DIRSEP) {
				directory[j] = DIRSEP;
				j++;
			}

			/* terminate the directory with a NULL */
			directory[j] = '\0';

			/* reallocate directory to it's proper size */
			directory = (char *) realloc(directory, sizeof(char) * (strlen(directory) + 1));
			if(!directory) {
				fprintf(stderr, "wmappl.c: Couldn't reallocate string.\n");
				exit(1);
			}

			/* store the directory in the IconPath structure */
			return_path->path[k] = directory;

			/* allocate new directory string and manage counters */
			directory = (char *) malloc(return_path->dirallocsize);
			k++;
			j = -1;

		}
		/* if not at path separator, add character to directory string */
		else {
			directory[j] = colonpath[i];
		}
	}

	/* do we need to add the trailing directory separator */
	if(i > 0 && directory[j - 1] != DIRSEP) {
		directory[j] = DIRSEP;
		j++;
	}

	/* terminate the last directory */
	directory[j] = '\0';

	/* reallocate directory to it's proper size */
	directory = (char *) realloc(directory, sizeof(char) * (strlen(directory) + 1));
	if(!directory) {
		fprintf(stderr, "wmappl.c: Couldn't reallocate string.\n");
		exit(1);
	}

	/* store the directory in the IconPath structure */
	return_path->path[k] = directory;

	return return_path;
}

/* opt_free_iconpath
 *  PARAMETERS:
 *      IconPath * : IconPath structure to free
 */
void opt_free_iconpath(IconPath *ip) {
	int i;
	if(ip==NULL) return;
	for(i=0; i<ip->count; i++) {
		free(ip->path[i]);
	}
	free(ip->path);
	free(ip);
}

/* opt_create_options
 *  RETURN:
 *      wmappl_opt * : the new wmappl_opt structure
 */
wmappl_opt *opt_create_options() {
	wmappl_opt *o = (wmappl_opt *)malloc(sizeof(wmappl_opt));
	
	/* set default values */
	o->withdrawn=-1;
	o->debugmode=-1;
	o->name=NULL;
	o->colonpath=NULL;
	o->bgcolor=NULL;
	o->bgicon=NULL;
#ifdef USE_TOOLTIPS
	o->tooltipDelay=-1;
	o->tooltipfg=NULL;
	o->tooltipbg=NULL;
	o->tooltipfont=NULL;
#endif

	return o;
}

/* opt_free_options
 *  PARAMETERS:
 *      wmappl_opt * : wmappl_opt structure to free
 */
void opt_free_options(wmappl_opt *opts) {
	if(opts==NULL) return;
	if(opts->name!=NULL) free(opts->name);
	if(opts->colonpath!=NULL) free(opts->colonpath);
	if(opts->bgcolor!=NULL) free(opts->bgcolor);
	if(opts->bgicon!=NULL) free(opts->bgicon);
#ifdef USE_TOOLTIPS
	if(opts->tooltipfg) free(opts->tooltipfg);
	if(opts->tooltipbg) free(opts->tooltipbg);
	if(opts->tooltipfont) free(opts->tooltipfont);
#endif
	free(opts);
}
