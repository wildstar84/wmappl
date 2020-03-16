/* args.h
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

#ifndef __ARGS_H__
#define __ARGS_H__

#include "../config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "options.h"

#ifdef HAVE_GETOPT_LONG
  #include <getopt.h>
#endif


/* parse_args
 *	PARAMETERS:
 *      int argc          : number of command line parameters
 *      char **argv       : array of command line parameters
 *      wmappl_opt*       : wmappl_opt structure to store parameters in
 *	RETURN:
 *		char *            : returns rcfile name if specified on command line
 *                          or NULL if none was specified
 */
char *parse_args(int argc, char **argv, wmappl_opt *opt);

/* show_version
 *	PARAMETERS:
 *		none
 *	RETURN:
 *		none
 */
void show_version();

/* show_usage
 *	PARAMETERS:
 *		none
 *	RETURN:
 *		none
 */
void show_usage();

#endif /* not __ARGS_H__ */
