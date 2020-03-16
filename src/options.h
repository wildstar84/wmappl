/* options.h
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

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include "../config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MACRO defining the path separator */
#define PATHSEP     ':'
/* MACRO defining the directory separator (separating each individual directory) */
#define DIRSEP      '/'

/* icon path structure */
typedef struct _IconPath_ {
    int count;
    size_t dirallocsize;
    char **path;
} IconPath;


/* wmappl_opt structure */
typedef struct _wmappl_opt_ {
	int withdrawn;
	int debugmode;
	char *name;
	char *colonpath;
	char *bgcolor;
	char *bgicon;
#ifdef USE_TOOLTIPS
	int   tooltipDelay;
	char *tooltipfg;
	char *tooltipbg;
	char *tooltipfont;
#endif
} wmappl_opt;


/* opt_create_iconpath
 *  PARAMETERS:
 *      char * : colon separated path
 * RETURN:
 *      IconPath *  : the icon path as a structure 
*/
IconPath *opt_create_iconpath(char *colonpath);

/* opt_free_iconpath
 *  PARAMETERS:
 *      IconPath * : IconPath structure to free
 */
void opt_free_iconpath(IconPath *ip);

/* opt_create_options
 *  RETURN:
 *      wmappl_opt * : the new wmappl_opt structure
 */
wmappl_opt *opt_create_options();


/* opt_free_options
 *  PARAMETERS:
 *      wmappl_opt * : wmappl_opt structure to free
 */
void opt_free_options(wmappl_opt *opts);


#endif /* end __OPTIONS_H__ */
