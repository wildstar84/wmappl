/* xpmbutton.h
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

#ifndef __XPMBUTTON_H__
#define __XPMBUTTON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/xpm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct _XpmButton_ {
	Pixmap pixmap;
	Pixmap pixmask;
	int pressed;
	char *command;
	char *tooltip;
	int x, y, width, height;
	struct _XpmButton_ *last;
	struct _XpmButton_ *next;
} XpmButton;

/* create_xpmbutton
 *  PARAMETERS:
 *      char *filename : xpm filename to load
 *      Display *display: X11 display
 *  RETURN:
 *      XpmButton* : an XpmButton structure
 */
XpmButton*
create_xpmbutton(char *filename, Display *display);

/* create_include_xpmbutton
 *  PARAMETERS:
 *      char **data : xpm character array to load
 *      Display *display: X11 display
 *  RETURN:
 *      XpmButton* : an XpmButton structure
 */
XpmButton*
create_include_xpmbutton(char **data, Display *display);

/* set_xpmbutton_command
 *  PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set command for
 *      char *command        : command to set in xpmbutton
 *  RETURN:
 *      none
 */
void
set_xpmbutton_command(XpmButton *xpmbutton, char *command);

/* set_xpmbutton_tooltip
 *  PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set tooltip for
 *      char *tooltip        : tooltip text for xpmbutton
 *  RETURN:
 *      none
 */
void
set_xpmbutton_tooltip(XpmButton *xpmbutton, char *tooltip);

/* set_xpmbutton_last
 *  PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set list pointer for
 *      XpmButton *last      : the previous XpmButton in the list
 *  RETURN:
 *      none
 */
void
set_xpmbutton_last(XpmButton *xpmbutton, XpmButton *last);

/* set_xpmbutton_next
 *  PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set list pointer for
 *      XpmButton *next      : the next XpmButton in the list
 *  RETURN:
 *      none
 */
void
set_xpmbutton_next(XpmButton *xpmbutton, XpmButton *next);

/* set_xpmbutton_position
 *  PARAMETERS:
 *		XpmButton *xpmbutton : XpmButton to set position for
 *      int x  : x coordinate of xpmbutton
 *      int y  : y coordinate of xpmbutton
 *  RETURN:
 *      none
 */
void
set_xpmbutton_position(XpmButton *xpmbutton, int x, int y);

/* set_xpmbutton_pressed
 *  PARAMETERS:
 *		XpmButton *xpmbutton : XpmButton to set state for
 *      int pressed  : set to nonzero if button should be
 *                     drawn pressed
 *  RETURN:
 *      none
 */
void
set_xpmbutton_pressed(XpmButton *xpmbutton, int pressed);

/* is_xpmbutton_at
 *  PARAMETERS:
 *		XpmButton *xpmbutton : XpmButton to check
 *      int x  : x coordinate to check
 *      int y  : y coordinate to check
 *  RETURN:
 *      int : returns nonzero if x,y is within the coordinates
 *            of xpmbutton
 */
int
is_xpmbutton_at(XpmButton *xpmbutton, int x, int y);

/* draw_xpmbutton
 *  PARAMETERS:
 *		XpmButton *xpmbutton : XpmButton to draw
 *      Display *display     : display to draw on
 *      Drawable drawable : drawable to draw into
 *      GC gc             : graphics context to use for drawing
 *  RETURN:
 *      none
 */
void
draw_xpmbutton(XpmButton *xpmbutton, Display *display,
    Drawable drawable, GC gc);

#endif /* not __XPMBUTTON_H__ */
