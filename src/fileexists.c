
/* fileexists.c
 *
 * WMAppl - Window Maker Application Launcher
 * Copyright (C) 1999-2002 Casey Harkins
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

#ifndef __FILEEXISTS_H__

#include "fileexists.h"

/* fileexists
 *	PARAMETERS:
 *      char *filename    : the file to check to see if it exists
 *                          and is readable
 *	RETURN:
 *      int               : returns 1 if file exists, 0 if not
 */
int fileexists(char *filename)
{
	int             fd;

	fd = open(filename, O_RDONLY);

	if(fd < 0) {
		return 0;
	}

	close(fd);
	return 1;
}

#endif /* not __FILEEXISTS_H__ */
