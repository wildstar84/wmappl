/* pixmap.h
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

#ifndef __PIXMAP_H__
#define __PIXMAP_H__ "Read or build the pixmap."

#include "../config.h"

#include <X11/xpm.h>
#include <X11/Xlib.h>

/* create_button_pixmap
 *	PARAMETERS:
 *          char *filename : filename to load ( XPM, PNG... )
 *	    Display *display: X11 display
 *
 *	RETURN:
 *		XpmButton* : an XpmButton structure
 */
int create_button_pixmap ( Display        *display,
			   char           *filename,
			   Pixmap         *pixmap,
			   Pixmap         *pixmask,
			   XpmAttributes  *xpmattributes );

#endif
